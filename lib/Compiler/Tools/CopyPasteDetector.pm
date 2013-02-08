package Compiler::Tools::CopyPasteDetector;

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
XSLoader::load(__PACKAGE__, $VERSION);

### ============== Dependency Modules =================== ###

use B::Deparse;
use MIME::Base64;
use Digest::MD5 qw(md5 md5_hex md5_base64);
use HTML::Template;
use File::Copy::Recursive qw(rcopy);
use File::Basename qw/dirname basename/;
use File::Path;
use JSON::XS;
use Data::Dumper;
use Module::CoreList;
use Compiler::Lexer;
use Compiler::Tools::CopyPasteDetector::CloneSetMetrics;
use Compiler::Tools::CopyPasteDetector::FileMetrics;
use Compiler::Tools::CopyPasteDetector::DirectoryMetrics;
use Compiler::Tools::CopyPasteDetector::Scattergram;
use constant DEBUG => 1;
### ================== Constants ======================== ###

# B::Deparse's BUG [1; => '???';]
my $DEPARSE_ERROR_MESSAGE = "'???';";
my $MAX_ROW_NUM_PER_PAGE = 100;
my $DEFAULT_MIN_LINE_NUM = 4;
my $DEFAULT_MIN_TOKEN_NUM = 30;
my $DEFAULT_ORDER_NAME = 'length';
### ================ Public Methods ===================== ###

sub new {
    my $class = shift;
    my $options = shift;
    my $tk_n    = $options->{min_token_num};
    my $line_n  = $options->{min_line_num};
    my $order   = $options->{order_by};
    my $jobs    = $options->{jobs};
    my $ignore  = $options->{ignore_variable_name};
    my $encoding = $options->{encoding};
    my $output_dirname = $options->{output_dirname} || 'copy_paste_detector_output';
    my @order_by_list = qw(length population kind_of_token radius nif);
    my $checked_order = $order if (defined $order && grep {$_ eq $order} @order_by_list);
    my $self = {
        tmp => q{__copy_paste_detector.tmp},
        stmt_num_manager     => +{},
        min_token_num        => $tk_n || $DEFAULT_MIN_TOKEN_NUM,
        min_line_num         => $line_n || $DEFAULT_MIN_LINE_NUM,
        jobs                 => $jobs || 1,
        ignore_variable_name => $ignore || 0,
        order_by             => $checked_order || $DEFAULT_ORDER_NAME,
        encoding             => $encoding,
        output_dirname       => $output_dirname
    };
    return bless($self, $class);
}

sub get_target_files_by_project_root {
    my ($self, $root) = @_;
    my @cur_files = glob("$root/*");
    my @names;
    foreach (@cur_files) {
        if (-d $_) {
            push(@names, @{$self->get_target_files_by_project_root($_)});
        } else {
            push(@names, $_) if ($_ =~ /\.p[ml]$/);
        }
    }
    my @ret = map { $_ =~ s|//|/|g; $_; } @names;
    return \@ret;
}

sub detect {
    my ($self, $files) = @_;
    return $self->__parallel_detect($files) if ($self->{jobs} > 1);
    return $self->__detect($files);
}

sub get_score {
    my ($self, $stmts) = @_;
    my $min_token_num = $self->{min_token_num};
    my $min_line_num  = $self->{min_line_num};
    my $order_by      = $self->{order_by};
    my @deparsed_stmts = @$stmts;
    my $clone_set_map = +{};
    my $filemap = +{};
    my @clone_set_results;
    foreach my $stmt (@deparsed_stmts) {
        push(@{$clone_set_map->{$stmt->{hash}}}, $stmt);
    }
    foreach my $clone_set (values %$clone_set_map) {
        my @clones = @$clone_set;
        my $hit = scalar @$clone_set;
        next if ($hit <= 0 || $self->__is_exists_parents($clone_set));
        my $first_clone = $clone_set->[0];
        next unless ($first_clone->{lines} + 1 >= $min_line_num && $first_clone->{token_num} > $min_token_num);
        $self->__set_neighbor_name($clone_set);
        my $token_num = $first_clone->{token_num};
        my $clone_metrics = Compiler::Tools::CopyPasteDetector::CloneSetMetrics->new($clone_set)->get_score();
        push(@clone_set_results, { metrics => $clone_metrics, set => $clone_set });
        foreach my $clone (@$clone_set) {
            my $filename = $clone->{file};
            my $hash = $clone->{hash};
            $filemap->{$filename} = +{ clone => +{} } unless (exists $filemap->{$filename});
            my $file_point = $filemap->{$filename};
            unless (exists $file_point->{all_token_num}) {
                my $all_tokens = Compiler::Lexer->new($filename)->tokenize($self->__get_script($filename));
                $file_point->{token_num} = scalar @$$all_tokens;
            }
            $file_point->{clone}->{$hash} = +{} if (!exists $file_point->{clone}->{$hash});
            my $clone_point = $file_point->{clone}->{$hash};
            $clone_point->{count}++;
            $clone_point->{token_num} = $token_num;
            $clone_point->{parents} = $clone->{parents};
            push(@{$clone_point->{start_line}}, $clone->{start_line});
            push(@{$clone_point->{end_line}}, $clone->{end_line});
            $clone_point->{from_names} = $clone->{from_names};
            my $node = $self->__make_node($filename);
            push(@{$node->{children}}, $file_point->{clone})
                unless (grep { $_ =~ $file_point->{clone} } @{$node->{children}});
        }
    }
    foreach my $filename (keys %$filemap) {
        my $file_metrics = Compiler::Tools::CopyPasteDetector::FileMetrics->new(
            {
                name => $filename,
                clones => $filemap->{$filename}->{clone},
                all_token_num => $filemap->{$filename}->{token_num}
            })->get_score();
        $filemap->{$filename}->{metrics} = $file_metrics;
    }
    my $dm = Compiler::Tools::CopyPasteDetector::DirectoryMetrics->new($self->{root}->{root}, $filemap);
    my $dirmap = $dm->get_directory_map();
    my $directory_score = +{};
    foreach my $dirname (keys %$dirmap) {
        $directory_score->{$dirname}->{metrics} = $dm->get_score($dirname);
    }
    delete $self->{diretory_map};
    #default property to sort is length.
    my @sorted_clone_set_results = sort {
        $b->{score} <=> $a->{score};
    } map {
        $_->{score} = $_->{metrics}->{$order_by}; $_;
    } @clone_set_results;
    return {
        file_score      => $filemap,
        clone_set_score => \@sorted_clone_set_results,
        directory_score => $directory_score
    };
}

sub display {
    my ($self, $score) = @_;
    my $clone_set_score = $score->{clone_set_score};
    foreach my $data (@$clone_set_score) {
        my $clone_set = $data->{set};
        print "\n\tscore    : $data->{score}\n\tlocation : [";
        foreach (@$clone_set) {
            print "$_->{file}, ($_->{start_line} ~ $_->{end_line}), ";
        }
        my $src = decode_base64($clone_set->[0]->{src});
        print "]\n\tsrc      : ${src}\n";
    }
}

sub gen_html {
    my ($self, $score) = @_;
    my $library_path = $INC{"Compiler/Tools/CopyPasteDetector.pm"};
    $library_path =~ s/\.pm//;
    my $output_dir = $self->{output_dirname};
    mkdir($output_dir);
    rcopy($library_path . "/HTML", $output_dir);
    my $file_score = $score->{file_score};
    my $directory_score = $score->{directory_score};
    my @file_data = sort {
        $b->{score} <=> $a->{score};
    } map {
        {
            name    => $_,
            score   => $file_score->{$_}->{metrics}->{coverage},
            metrics => $file_score->{$_}->{metrics}
        };
    } keys %$file_score;
    my @directory_data = sort {
        $b->{score} <=> $a->{score};
    } map {
        {
            name    => $_,
            score   => $directory_score->{$_}->{metrics}->{coverage},
            metrics => $directory_score->{$_}->{metrics}
        }
    } keys %$directory_score;
    my $json = JSON::XS->new();
    open(my $fh, '>', "$output_dir/js/file_data.json");
    print $fh $json->encode(\@file_data);
    close($fh);

    open($fh, '>', "$output_dir/js/directory_data.json");
    print $fh $json->encode(\@directory_data);
    close($fh);

    delete $file_score->{$_}->{metrics} foreach (keys %$file_score);
    $self->__output_clone_data($score->{file_score}, $output_dir);
    foreach (@{$score->{clone_set_score}}) {
        my $set = $_->{set};
        for (my $i = 0; $i < scalar @$set; $i++) {# result (@{$_->{set}}) {
            my $result = $set->[$i];
            if ($i == 0) {
                $result->{src} = decode_base64($result->{src});
            } else {
                delete $result->{src};
                delete $result->{hash};
            }
            delete $result->{parents};
            delete $result->{token_num};
            delete $result->{lines};
            delete $result->{from_names};
            delete $result->{indent};
            delete $result->{block_id};
            delete $result->{stmt_num};
            delete $result->{orig};
        }
    }
    $json = JSON::XS->new();
    open($fh, '>', "$output_dir/js/clone_set_data.json");
    print $fh $json->encode($score->{clone_set_score});
    close($fh);
}

### ================ Private Methods ===================== ###

sub __set_neighbor_name {
    my ($self, $matched_values) = @_;
    foreach my $v (@$matched_values) {
        my @names = map { $_->{file}; } grep { $v !~ $_ } @$matched_values;
        $v->{from_names}->{$_}++ foreach (@names);
    }
}

sub __get_parents_node {
    my ($self, $dirname) = @_;
    $dirname =~ m|(.*)/.*|;
    my $parents_dir = $1;
    if ($parents_dir !~ m|/|) {
        my $root = {name => $parents_dir, children => []};
        $self->{root}->{$parents_dir} = $root;
        $self->{root}->{root} = $root;
    }
    return (!exists $self->{root}->{$parents_dir}) ?
        $self->__make_node($parents_dir) : $self->{root}->{$parents_dir};
}

sub __make_node {
    my ($self, $dirname) = @_;
    my $parents_node = $self->__get_parents_node($dirname);
    my $node = {name => $dirname, children => []};
    $self->{root}->{$dirname} = $node;
    unless (grep { $_->{name} eq $dirname } @{$parents_node->{children}}) {
        push(@{$parents_node->{children}}, $node);
    }
    return $node;
}

sub __output_clone_data {
    my ($self, $filemap, $output_dir) = @_;
    foreach my $filepath (keys %$filemap) {
        my $clones = $filemap->{$filepath}->{clone};
        open(my $fp, "<", $filepath);
        binmode($fp, sprintf(":encoding(%s)", $self->{encoding})) if ($self->{encoding});
        my $line_number = 1;
        my %start_line_nums;
        my %end_line_nums;
        foreach my $hash (keys %$clones) {
            push(@{$start_line_nums{$_}}, $hash) foreach (@{$clones->{$hash}->{start_line}});
            push(@{$end_line_nums{$_}}, $hash) foreach (@{$clones->{$hash}->{end_line}});
        }
        my $output_data = "";
        my %clone_area_flags;
        foreach my $line (<$fp>) {
            if (exists $start_line_nums{$line_number}) {
                $output_data .= sprintf("<div class='code-clone-start %s'></div>", $_)
                    foreach (@{$start_line_nums{$line_number}});
            }
            $output_data .= "$line";
            if (exists $end_line_nums{$line_number}) {
                $output_data .= sprintf("<div class='code-clone-end %s'></div>", $_)
                    foreach (@{$end_line_nums{$line_number}});
            }
            $line_number++;
        }
        close($fp);
        my $filename = basename($filepath);
        my $dirname = "$output_dir/data/" . dirname($filepath);
        mkpath($dirname);
        open($fp, ">", "${dirname}/${filename}");
        binmode($fp, ":utf8");
        print $fp $output_data;
        close($fp);
    }
    open(my $fp, '>', "$output_dir/scattergram.html");
    print $fp Compiler::Tools::CopyPasteDetector::Scattergram->new($self->{root}->{root})->render();
    close($fp);
    my $json = JSON::XS->new();
    open($fp, '>', "$output_dir/js/output.json");
    print $fp $json->encode($self->{root}->{root}) if defined $self->{root}->{root};
    close($fp);
}

sub __gen_file {
    my $args = shift;
    my $tmpl = HTML::Template->new(filename => $args->{from});
    $tmpl->param($args->{data});
    open(FP, ">", $args->{to});
    print FP $tmpl->output;
    close(FP);
}

sub __get_script {
    my ($self, $filename) = @_;
    my $script = "";
    open(FP, "<", $filename) or die("Error");
    $script .= $_ foreach (<FP>);
    close(FP);
    return $script;
}

sub __ignore_orthographic_variation_of_variable_name {
    my ($self, $tokens) = @_;
    my @variables = (
        Compiler::Lexer::TokenType::T_Var,
        Compiler::Lexer::TokenType::T_CodeVar,
        Compiler::Lexer::TokenType::T_ArrayVar,
        Compiler::Lexer::TokenType::T_HashVar,
        Compiler::Lexer::TokenType::T_LocalVar,
        Compiler::Lexer::TokenType::T_LocalArrayVar,
        Compiler::Lexer::TokenType::T_LocalHashVar,
        Compiler::Lexer::TokenType::T_GlobalVar,
        Compiler::Lexer::TokenType::T_GlobalArrayVar,
        Compiler::Lexer::TokenType::T_GlobalHashVar
    );
    foreach my $token (@$$tokens) {
        if (grep { $_ == $token->{type} } @variables) {
            $token->{data} = substr($token->{data}, 0, 1) . "v";
        }
    }
}

sub __detect {
    my ($self, $files) = @_;
    print "normal_detecting\n";
    my @stmts;
    foreach my $file (@$files) {
        my $stmt_data = $self->__get_stmt_data($file, $self->__get_script($file));
        push(@stmts, @$stmt_data);
    }
    return \@stmts;
}

sub __make_command {
    my ($self, $modules) = @_;
    my $core_modules = $Module::CoreList::version{$]};
    eval("package main; use $_->{name}; 1;") foreach (@$modules);
    my @error_modules = grep {
        !exists $INC{$_} && !exists $core_modules->{$_}
    } map {
        my $tmp = $_->{name}; $tmp =~ s|::|/|g; "$tmp.pm";
    } grep {
        $_->{name} !~ /^[\d|\.]+/
    } @$modules;
    if (@error_modules) {
        warn sprintf("Can't locate %s\n", join(', ', @error_modules));
        die;
    }
    push(@$modules, { name => 'Compiler::Tools::CopyPasteDetector::DeparseHooker', args => undef});
    my $perl = $^X;
    my $include_dirs .= join(' ', map { "-I$_"; } @INC);
    my $preload_option = join(' ', map {
        if (defined $_->{args}) {
            my $args = $_->{args};
            $args =~ s/'/'\\''/g;
            sprintf("'-M%s %s' -M-strict", $_->{name}, $args);
        } else {
            sprintf("-M%s -M-strict", $_->{name});
        }
    } @$modules);
    return {
        normal => "$perl -MO=Deparse",
        full   => "$perl $include_dirs $preload_option -MO=Deparse"
    };
}

sub __parallel_detect {
    my ($self, $files) = @_;
    print "parallel_detecting\n";
    my @prepare;
    foreach my $filename (@$files) {
        my $script = $self->__get_script($filename);
        my $lexer = Compiler::Lexer->new($filename);
        my $tokens = $lexer->tokenize($script);
        if ($self->{ignore_variable_name}) {
            $self->__ignore_orthographic_variation_of_variable_name($tokens);
        }
        my $stmts = $lexer->get_groups_by_syntax_level($$tokens, Compiler::Lexer::SyntaxType::T_Stmt);
        my $modules = $lexer->get_used_modules($script);
        my $cmd = $self->__make_command($modules);
        foreach my $stmt (@$$stmts) {
            $stmt->{src} =~ s/'/'\\''/g;
        }
        push(@prepare, {filename => $filename, stmts => $$stmts, command => $cmd});
    }
    print "detecting...\n";
    my $deparsed_stmts = get_deparsed_stmts_by_xs_parallel(\@prepare, $self->{jobs});
    return $$deparsed_stmts;
}

sub __get_stmt_data {
    my ($self, $filename, $script) = @_;
    my $lexer = Compiler::Lexer->new($filename);
    my $tokens = $lexer->tokenize($script);
    if ($self->{ignore_variable_name}) {
        $self->__ignore_orthographic_variation_of_variable_name($tokens);
    }
    my $stmts = $lexer->get_groups_by_syntax_level($$tokens, Compiler::Lexer::SyntaxType::T_Stmt);
    my $modules = $lexer->get_used_modules($script);
    my $cmd = $self->__make_command($modules);
    my @deparsed_stmts;
    my $tmp_file = $self->{tmp};
    my @stmts = @$$stmts;
    $self->{stmt_num_manager} = +{};
    foreach my $stmt (@stmts) {
        my $src = $stmt->{src};
        my @splitted = split(/^\s+(else|elsif)/, $src);
        $src = $splitted[-1] if (scalar @splitted > 1);
        $src =~ s/'/'\\''/g;
        my $cmd = ($stmt->{has_warnings}) ? $cmd->{full} : $cmd->{normal};
        my $code = `$cmd -e '$src' 2> /dev/null`;
        chomp($code);
        $code = "$splitted[1] $code" if (scalar @splitted > 1);
        if ($code eq "" || $code eq ";\n" || $code eq $DEPARSE_ERROR_MESSAGE) {
            if (DEBUG) {
                #print sprintf("%s : %s : %s\n", `$cmd`, $cmd, $filename);
                print "orig : [ $stmt->{src} ]\n";
            }
            next;
        }
        $self->__add_stmt(\@deparsed_stmts, $stmt, $code, $filename);
    }
    foreach my $stmt (@deparsed_stmts) {
        my $start_line = $stmt->{start_line};
        my $lines = $stmt->{lines};
        my @parents = grep { $_->{start_line} == $start_line - ($_->{lines} - $lines) } @deparsed_stmts;
        @parents = grep { $_->{lines} > $stmt->{lines} } @parents;
        push(@{$stmt->{parents}}, $_->{hash}) foreach (@parents);
    }
    unlink($tmp_file);
    return \@deparsed_stmts;
}

sub __add_stmt {
    my ($self, $deparsed_stmts, $stmt, $code, $filename) = @_;
    my $start_line = $stmt->{start_line};
    my $end_line = $stmt->{end_line};
    my $token_num = $stmt->{token_num};
    my $indent = $stmt->{indent};
    my $block_id = $stmt->{block_id};
    my $line_num = $end_line - $start_line;
    my $stmt_num = (defined($self->{stmt_num_manager}->{"${indent}_${block_id}"})) ?
        $self->{stmt_num_manager}->{"${indent}_${block_id}"} : 0;
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
        parents     => []
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
            my $parents = $prev_stmt->{parents};
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
                parents     => []
            };
            push(@{$added_stmt->{parents}}, @$parents);
            push(@$parents, $new_hash);
            push(@tmp_deparsed_stmts, $added_stmt);
        } elsif ($indent - 1 == $prev_stmt->{indent} &&
                 $start_line - 1 == $prev_stmt->{start_line}) {
            # prev_stmt is sub f {} or if () {} or for () {} and so on
            push(@{$deparsed_stmt->{parents}}, $prev_stmt->{hash});
        }
    }
    $self->{stmt_num_manager}->{"${indent}_${block_id}"}++;
    push(@$deparsed_stmts, $deparsed_stmt);
    push(@$deparsed_stmts, @tmp_deparsed_stmts);
}

sub __match_parents_hash {
    my ($self, $from_parents, $to_parents) = @_;
    my $ret = 0;
    foreach my $from (@$from_parents) {
        if (grep { $from eq $_ } @$to_parents) {
            $ret = 1;
            last;
        }
    }
    return $ret;
}

sub __is_exists_parents {
    my ($self, $matched_values) = @_;
    my @matched_values = @$matched_values;
    my @copied_matched_values = @matched_values;
    my $ret = 1;
    foreach my $v (@matched_values) {
        foreach (@copied_matched_values) {
            my @from_parents = @{$v->{parents}};
            my @to_parents = @{$_->{parents}};
            next if ($v == $_);
            $ret = 0 unless ($self->__match_parents_hash(\@from_parents, \@to_parents));
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
my $options = {
    jobs => 1, # detect by using multi thread
    min_token_num => 30,
    min_line_num  => 4
};
my $detector = Compiler::Tools::CopyPasteDetector->new($options);
my $data = $detector->detect(\@files);
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
