package CopyPasteDetector;

use 5.012004;
use strict;
use warnings;

### =================== Exporter ======================== ###
require Exporter;
our @ISA = qw(Exporter);
our %EXPORT_TAGS = ( 'all' => [ qw(
) ] );
our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );
our @EXPORT = qw(
);
our $VERSION = '0.01';
require XSLoader;
XSLoader::load('CopyPasteDetector', $VERSION);

### ============== Dependency Modules =================== ###

use B::Deparse;
use MIME::Base64;
use Digest::MD5 qw(md5 md5_hex md5_base64);
use Compiler::Lexer;
use Data::Dumper;

### ================ Public Methods ===================== ###

sub new {
    my $class = shift;
    my $self = {tmp => q{__copy_paste_detector.tmp}};
    return bless($self, $class);
}

sub detect {
    my ($self, $files) = @_;
    my @stmts;
    foreach my $file (@$files) {
        my $stmt_data = $self->__get_stmt_data($file, __get_script($file));
        push(@stmts, @$stmt_data);
    }
    return \@stmts;
}

sub get_score {
    my ($self, $stmts) = @_;
    my @deparsed_stmts = @$stmts;
    my $results = {};
    foreach my $stmt (@deparsed_stmts) {
        push(@{$results->{$stmt->{hash}}}, $stmt);
    }
    my @ret;
    foreach (values %$results) {
        my @matched_values = @$_;
        my $hit = $#matched_values;
        if ($hit > 0) {
            next if ($self->__is_exists_parent(\@matched_values));
            my $score = $hit * $matched_values[0]->{lines};
            push(@ret, {score => $score, results => $_}) if ($matched_values[0]->{lines} > 2);
        }
    }
    return \@ret;
}

sub display {
    my ($self, $scores) = @_;
    my @sorted_data = sort { $b->{score} <=> $a->{score} } @$scores;
    foreach my $data (@sorted_data) {
        print "\n\tscore    : $data->{score}\n\tlocation : [";
        my $results = $data->{results};
        foreach (@$results) {
            print "$_->{file}, ($_->{start_line} ~ $_->{end_line}), ";
        }
        my $src = decode_base64($results->[0]->{src});
        print "]\n\tsrc      : ${src}\n";
    }
}

### ================ Private Methods ===================== ###
sub __get_script {
    my ($filename) = @_;
    my $script = "";
    open(FP, "<", $filename) or die("Error");
    $script .= $_ foreach (<FP>);
    close(FP);
    return $script;
}

sub __get_stmt_data {
    my ($self, $filename, $script) = @_;
    my $stmts = Lexer::get_stmt_codes($filename, $script);
    my @deparsed_stmts;
    my $tmp_file = $self->{tmp};
    foreach my $stmt (@$$stmts) {
        open(FP, ">", $tmp_file);
        print FP $stmt->{src};
        close(FP);
        my $code = `perl -MList::Util -MCarp -MO=Deparse $tmp_file 2> /dev/null`;
        chomp($code);
        next if ($code eq "" || $code eq ";\n");
        if ($code ne "'???';") { # B::Deparse's BUG [1; => '???';]
            my $start_line = $stmt->{start_line};
            my $end_line = $stmt->{end_line};
            my $line_num = $end_line - $start_line;
            my $deparsed_stmt = {
                hash => md5_base64($code),
                src => encode_base64($code, ""),
                orig => $code,
                file => $filename,
                lines => ($line_num > 0) ? $line_num : 1,
                start_line => $start_line,
                end_line => $end_line,
                parent => []
            };
            my @tmp_deparsed_stmts = ();
            foreach (@deparsed_stmts) {
                if (($_->{end_line}     == $deparsed_stmt->{start_line}) ||
                    ($_->{end_line} + 1 == $deparsed_stmt->{start_line})) {
                    my $src = $_->{orig} . "\n" . $deparsed_stmt->{orig};
                    $start_line = $_->{start_line};
                    $end_line = $deparsed_stmt->{end_line};
                    $line_num = $end_line - $start_line;
                    my $new_hash = md5_base64($src);
                    my $parent = $_->{parent};
                    push(@$parent, $new_hash);
                    my $added_stmt = {
                        hash => $new_hash,
                        src => encode_base64($src, ""),
                        orig => $src,
                        file => $filename,
                        lines => ($line_num > 0) ? $line_num : 1,
                        start_line => $start_line,
                        end_line => $end_line,
                        parent => []
                    };
                    push(@tmp_deparsed_stmts, $added_stmt);
                }
            }
            push(@deparsed_stmts, $deparsed_stmt);
            push(@deparsed_stmts, @tmp_deparsed_stmts);
        }
    }

    foreach my $stmt (@deparsed_stmts) {
        my $start_line = $stmt->{start_line};
        my $lines = $stmt->{lines};
        my @parents = grep { $_->{start_line} == $start_line - ($_->{lines} - $lines) } @deparsed_stmts;
        @parents = grep { $_->{lines} > $stmt->{lines} } @parents;
        push(@{$stmt->{parent}}, $_->{hash}) foreach (@parents);
    }
    unlink($tmp_file);
    return \@deparsed_stmts;
}

sub __match_parent_hash {
    my ($self, $from_parent, $to_parent) = @_;
    my $ret = 0;
    foreach my $from (@$from_parent) {
        if (grep { $from eq $_ } @$to_parent) {
            $ret = 1;
            last;
        }
    }
    return $ret;
}

sub __is_exists_parent {
    my ($self, $matched_values) = @_;
    my @matched_values = @$matched_values;
    my @copied_matched_values = @matched_values;
    my $ret = 1;
    foreach my $v (@matched_values) {
        foreach (@copied_matched_values) {
            my @from_parent = @{$v->{parent}};
            my @to_parent = @{$_->{parent}};
            next if ($v == $_);
            unless ($self->__match_parent_hash(\@from_parent, \@to_parent)) {
                $ret = 0;
            }
        }
    }
    return $ret;
}

1;
__END__

=head1 NAME

Compiler::Tools::CopyPasteDetector - detect Copy and Paste of Perl5 Codes.

=head1 VERSION

This document describes Compiler::Tools::CopyPasteDetector version 1.0000.

=head1 SYNOPSIS

use Compiler::Tools::CopyPasteDetector;

my @files = qw(file1.pl file2.pl file3.pl);

my $detector = CopyPasteDetector->new();
my $data = $detector->detect(\@files); #for Caching
my $score = $detector->get_score($data);
$detector->display($score);

=head1 AUTHOR

Masaaki, Goshima (goccy) E<lt>goccy54(at)cpan.orgE<gt>

=head1 LICENSE AND COPYRIGHT

Copyright (c) 2012, Masaaki, Goshima (goccy). All rights reserved.

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself.

=cut
