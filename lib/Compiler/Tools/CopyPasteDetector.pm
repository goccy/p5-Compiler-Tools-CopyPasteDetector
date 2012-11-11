package CopyPasteDetector;

use 5.012004;
use strict;
use warnings;

require Exporter;

our @ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use CopyPasteDetector ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	
);

our $VERSION = '0.01';

require XSLoader;
XSLoader::load('CopyPasteDetector', $VERSION);

# Preloaded methods go here.

use B::Deparse;
use MIME::Base64;
use Digest::MD5 qw(md5 md5_hex md5_base64);
use Compiler::Lexer;
use HTML::Template;

sub new {
    my $class = shift;
    my $self = {};
    return bless($self, $class);
}

sub _get_script {
    my ($filename) = @_;
    my $script = "";
    open(FP, "<", $filename) or die("Error");
    $script .= $_ foreach (<FP>);
    close(FP);
    return $script;
}

sub _get_stmt_data {
    my ($self, $filename, $script) = @_;
    my $stmts = Lexer::get_stmt_codes($filename, $script);
    my @deparsed_stmts;
    foreach my $stmt (@$$stmts) {
        open(FP, ">", "tmp.pl");
        print FP $stmt->{src};
        close(FP);
        my $code = `perl -MList::Util -MCarp -MO=Deparse tmp.pl 2> /dev/null`;
        chomp($code);
        next if ($code eq "" || $code eq ";\n");
        if ($code ne "'???';") { # B::Deparse's BUG [1; => '???';]
            my $start_line = $stmt->{start_line};
            my $end_line = $stmt->{end_line};
            my $line_num = $end_line - $start_line;
            my $deparsed_stmt = {hash => md5_base64($code),
                                 src => encode_base64($code, ""),
                                 orig => $code,
                                 file => $filename,
                                 lines => ($line_num > 0) ? $line_num : 1,
                                 start_line => $start_line,
                                 end_line => $end_line};
            my @tmp_deparsed_stmts = ();
            foreach (@deparsed_stmts) {
                if (($_->{end_line}     == $deparsed_stmt->{start_line}) ||
                    ($_->{end_line} + 1 == $deparsed_stmt->{start_line})) {
                    my $src = $_->{orig} . "\n" . $deparsed_stmt->{orig};
                    $start_line = $_->{start_line};
                    $end_line = $deparsed_stmt->{end_line};
                    $line_num = $end_line - $start_line;
                    my $added_stmt = {hash => md5_base64($src), #$src
                                      src => encode_base64($src, ""),
                                      orig => $src,
                                      file => $filename,
                                      lines => ($line_num > 0) ? $line_num : 1,
                                      start_line => $start_line,
                                      end_line => $end_line};
                    push(@tmp_deparsed_stmts, $added_stmt);
                }
            }
            push(@deparsed_stmts, $deparsed_stmt);
            push(@deparsed_stmts, @tmp_deparsed_stmts);
        }
    }
    return \@deparsed_stmts;
}

sub detect {
    my ($self, $files) = @_;
    my @stmts;
    foreach my $file (@$files) {
        my $stmt_data = $self->_get_stmt_data($file, _get_script($file));
        push(@stmts, @$stmt_data);
    }
    unlink("tmp.pl");
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
        my @value = @$_;
        my $hit = $#value;
        if ($hit > 0) {
            my $score = $hit * $value[0]->{lines};
            push(@ret, {score => $score, results => $_});
        }
    }
    return \@ret;
}

sub display {
    my ($self, $scores) = @_;
    my @sorted_data = sort { $b->{score} <=> $a->{score} } @$scores;
    foreach my $data (@sorted_data) {
        print "
    score    : $data->{score}
    location : [";
        my $results = $data->{results};
        foreach (@$results) {
            print "$_->{file}, ($_->{start_line} ~ $_->{end_line}), ";
        }
        my $src = decode_base64($results->[0]->{src});
        print "]
    src      : ${src}
";
    }
}

use File::Copy::Recursive qw(rcopy);

sub gen_html {
    my ($self, $scores) = @_;
    my @sorted_data = sort { $b->{score} <=> $a->{score} } @$scores;
    my @cpd_main_table;
    foreach my $data (@sorted_data) {
        my @locations;
        my $results = $data->{results};
        foreach (@$results) {
            push(@locations, {
                file       => $_->{file},
                start_line => $_->{start_line},
                end_line   => $_->{end_line}
            });
        }
        my $score = $data->{score};
        my $src = decode_base64($results->[0]->{src});
        push(@cpd_main_table, {
            score => $score,
            location => \@locations,
            src => $src
        });
    }
    my $library_path = $INC{"Compiler/Tools/CopyPasteDetector.pm"};
    $library_path =~ s/\.pm//;
    my $output_dir = "copy_paste_detector_output";
    mkdir($output_dir);
    rcopy($library_path . "/js", $output_dir . "/js");
    rcopy($library_path . "/css", $output_dir . "/css");
    rcopy($library_path . "/images", $output_dir . "/images");
    open(FP, ">", "${output_dir}/index.html");
    my $tmpl = HTML::Template->new(filename => "$library_path/index.tmpl");
    $tmpl->param({cpd_main_table => \@cpd_main_table});
    print FP $tmpl->output;
    close(FP);
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
