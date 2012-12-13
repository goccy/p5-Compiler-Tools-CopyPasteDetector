package CopyPasteDetector;

use 5.012004;
use strict;
use warnings;

### =================== Exporter ======================== ###
require Exporter;
our @ISA = qw(Exporter);
our %EXPORT_TAGS = ( 'all' => [ qw() ] );
our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );
our @EXPORT = qw();
our $VERSION = '0.01';
require XSLoader;
XSLoader::load('CopyPasteDetector', $VERSION);

### ============== Dependency Modules =================== ###

use B::Deparse;
use MIME::Base64;
use Digest::MD5 qw(md5 md5_hex md5_base64);
use HTML::Template;
use File::Copy::Recursive qw(rcopy);
use Data::Dumper;
use Compiler::Lexer;

### ================== Constants ======================== ###

# B::Deparse's BUG [1; => '???';]
my $DEPARSE_ERROR_MESSAGE = "'???';";
my $MAX_ROW_NUM_PER_PAGE = 100;

### ================ Public Methods ===================== ###

sub new {
    my $class = shift;
    my $options = shift;
    my $self = {tmp => q{__copy_paste_detector.tmp}, options => $options};
    return bless($self, $class);
}

sub detect {
    my ($self, $files) = @_;
    return $self->__parallel_detect($files) if ($self->{options}->{job} > 1);
    print "normal_detect\n";
    return $self->__detect($files);
}

sub get_score {
    my ($self, $stmts) = @_;
    my $min_token_num = defined($self->{options}->{min_token_num}) ?
        $self->{options}->{min_token_num} : 40;
    my $min_line_num = defined($self->{options}->{min_line_num}) ?
        $self->{options}->{min_line_num} : 4;
    my @deparsed_stmts = @$stmts;
    my $results = {};
    foreach my $stmt (@deparsed_stmts) {
        push(@{$results->{$stmt->{hash}}}, $stmt);
    }
    my @ret;
    foreach (values %$results) {
        my @matched_values = @$_;
        my $hit = $#matched_values;
        next if ($hit <= 0 || $self->__is_exists_parent(\@matched_values));
        my $first_value = $matched_values[0];
        next unless ($first_value->{lines} + 1 >= $min_line_num && $first_value->{token_num} > $min_token_num);
        my $score = $hit * $first_value->{token_num};
        push(@ret, {score => $score, results => $_})
    }
    return \@ret;
}

sub display {
    my ($self, $scores) = @_;
    my @sorted_data = sort { $b->{score} <=> $a->{score} } @$scores;
    foreach my $data (@sorted_data) {
        my $results = $data->{results};
        print "\n\tscore    : $data->{score}\n\tlocation : [";
        foreach (@$results) {
            print "$_->{file}, ($_->{start_line} ~ $_->{end_line}), ";
        }
        my $src = decode_base64($results->[0]->{src});
        print "]\n\tsrc      : ${src}\n";
    }
}

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
        if ($#locations > 3) {
            my @short_location = @locations[0 .. 3];
            push(@cpd_main_table, {
                score => $score,
                capacity_over => 1,
                short_location => \@short_location,
                location => \@locations,
                src => $src
            });
        } else {
            push(@cpd_main_table, {
                score => $score,
                capacity_over => 0,
                location => \@locations,
                src => $src
            });
        }
    }

    my $library_path = $INC{"Compiler/Tools/CopyPasteDetector.pm"};
    $library_path =~ s/\.pm//;
    my $output_dir = "copy_paste_detector_output";
    mkdir($output_dir);
    my $main_table_size = $#cpd_main_table;
    my $sub_contents_num = $main_table_size / $MAX_ROW_NUM_PER_PAGE;
    my @cpd_contents;
    for (my $i = 1; $i < $sub_contents_num; $i++) {
        my $start = $i * $MAX_ROW_NUM_PER_PAGE + 1;
        my $size = ($i + 1) * $MAX_ROW_NUM_PER_PAGE;
        my $end = ($size > $main_table_size) ? $main_table_size : $size;
        my $name = "sub_contents$i.html";
        push(@cpd_contents, {name => $name});
        $name =~ s/.html$//;
        my @cpd_sub_table = map { $_->{class} = $name; $_; } @cpd_main_table[$start .. $end];
        __gen_file({
            from => "$library_path/tmpl/sub.tmpl",
            to   => "$output_dir/${name}.html",
            data => {cpd_sub_table => \@cpd_sub_table}
        });
    }

    if ($main_table_size > $MAX_ROW_NUM_PER_PAGE) {
        @cpd_main_table = @cpd_main_table[0 .. $MAX_ROW_NUM_PER_PAGE];
    }

    $_->{class} = "" foreach (@cpd_main_table);

    __gen_file({
        from => "$library_path/tmpl/index.tmpl",
        to   => "$output_dir/index.html",
        data => {cpd_main_table => \@cpd_main_table}
    });

    rcopy($library_path . "/js",     $output_dir . "/js");
    rcopy($library_path . "/css",    $output_dir . "/css");
    rcopy($library_path . "/images", $output_dir . "/images");

    __gen_file({
        from => "$library_path/tmpl/js.tmpl",
        to   => "$output_dir/js/cpd.js",
        data => {cpd_contents => \@cpd_contents}
    });

}

### ================ Private Methods ===================== ###

sub __gen_file {
    my $args = shift;
    my $tmpl = HTML::Template->new(filename => $args->{from});
    $tmpl->param($args->{data});
    open(FP, ">", $args->{to});
    print FP $tmpl->output;
    close(FP);
}

sub __get_script {
    my ($filename) = @_;
    my $script = "";
    open(FP, "<", $filename) or die("Error");
    $script .= $_ foreach (<FP>);
    close(FP);
    return $script;
}

sub __autoflush {
    my ($self, $flushed) = @_;
    my $old_fh = select $flushed;
    $| = 1;
    select $old_fh;
}

sub __parallel_detect {
    my ($self, $files) = @_;
    print "parallel_detecting\n";
    my @prepare;
    foreach my $filename (@$files) {
        my $script = __get_script($filename);
        my $stmts = Lexer::get_stmt_codes($filename, $script);
        push(@prepare, {filename => $filename, stmts => $$stmts});
    }
    my $deparsed_stmts = get_deparsed_stmts_by_xs_parallel(\@prepare, $self->{options}->{job});
    return $$deparsed_stmts;
}

sub __detect {
    my ($self, $files) = @_;
    my @stmts;
    foreach my $file (@$files) {
        my $stmt_data = $self->__get_stmt_data($file, __get_script($file));
        push(@stmts, @$stmt_data);
    }
    return \@stmts;
}

use Data::Dumper;
sub __get_stmt_data {
    my ($self, $filename, $script) = @_;
    my $stmts = Lexer::get_stmt_codes($filename, $script);
    my @deparsed_stmts;
    my $tmp_file = $self->{tmp};
    my $stmt_num = 0;
    my @stmts = @$$stmts;
    foreach my $stmt (@stmts) {
        my $str = $stmt->{src};
        open(FP, ">", $tmp_file);
        print FP $stmt->{src};
        close(FP);
        my $code = `perl -MList::Util -MCarp -MO=Deparse $tmp_file 2> /dev/null`;
        chomp($code);
        next if ($code eq "" || $code eq ";\n");
        if ($code ne $DEPARSE_ERROR_MESSAGE) {
            __add_stmt(\@deparsed_stmts, $stmt, $code, $filename, $stmt_num);
            $stmt_num++;
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

sub __add_stmt {
    my ($deparsed_stmts, $stmt, $code, $filename, $stmt_num) = @_;
    my $start_line = $stmt->{start_line};
    my $end_line = $stmt->{end_line};
    my $token_num = $stmt->{token_num};
    my $indent = $stmt->{indent};
    my $block_id = $stmt->{block_id};
    my $line_num = $end_line - $start_line;
    my $deparsed_stmt = {
        hash       => md5_base64($code),
        src        => encode_base64($code, ""),
        orig       => $code,
        file       => $filename,
        lines      => ($line_num > 0) ? $line_num : 1,
        start_line => $start_line,
        end_line   => $end_line,
        indent     => $indent,
        block_id   => $block_id,
        stmt_num   => $stmt_num,
        token_num  => $token_num,
        parent     => []
    };
    my @tmp_deparsed_stmts = ();
    foreach my $prev_stmt (@$deparsed_stmts) {
        if ($prev_stmt->{stmt_num} + 1 == $stmt_num &&
            $prev_stmt->{indent} == $indent &&
            $prev_stmt->{block_id} == $block_id) {
            my $src = $prev_stmt->{orig} . "\n" . $deparsed_stmt->{orig};
            $start_line = $prev_stmt->{start_line};
            $line_num = $end_line - $start_line;
            my $new_hash = md5_base64($src);
            my $parent = $prev_stmt->{parent};
            push(@$parent, $new_hash);
            my $added_stmt = {
                hash       => $new_hash,
                src        => encode_base64($src, ""),
                orig       => $src,
                file       => $filename,
                lines      => ($line_num > 0) ? $line_num : 1,
                start_line => $start_line,
                end_line   => $end_line,
                indent     => $indent,
                block_id   => $block_id,
                stmt_num   => $stmt_num,
                token_num  => $prev_stmt->{token_num} + $token_num,
                parent     => []
            };
            push(@tmp_deparsed_stmts, $added_stmt);
        }
    }
    push(@$deparsed_stmts, $deparsed_stmt);
    push(@$deparsed_stmts, @tmp_deparsed_stmts);
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
            $ret = 0 unless ($self->__match_parent_hash(\@from_parent, \@to_parent));
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
$detector->display($score); #output to console
$detector->gen_html($score); #generates html to 'copy_paste_detector_output' dir.

=head1 AUTHOR

Masaaki, Goshima (goccy) E<lt>goccy54(at)cpan.orgE<gt>

=head1 LICENSE AND COPYRIGHT

Copyright (c) 2012, Masaaki, Goshima (goccy). All rights reserved.

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself.

=cut
