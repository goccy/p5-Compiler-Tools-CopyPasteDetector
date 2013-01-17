#!/usr/bin/env perl
use strict;
use warnings;
use Compiler::Tools::CopyPasteDetector;
use Getopt::Long;
use Pod::Usage;

my $options = +{};
GetOptions(
    'i|ignore-variable-name' => \$options->{ignore_variable_name},
    'j|jobs=s'               => \$options->{jobs},
    't|min-token-num=s'      => \$options->{min_token_num},
    'l|min-line-num=s'       => \$options->{min_line_num},
    'e|encoding=s'           => \$options->{encoding},
    'order-by=s'             => \$options->{order_by},
    'help'                   => \$options->{help}
);

pod2usage(1) if $options->{help};
my $project_root = $ARGV[0];
my $detector = Compiler::Tools::CopyPasteDetector->new($options);
my $files = $detector->get_target_files_by_project_root($project_root);
my $data = $detector->detect($files);
my $score = $detector->get_score($data);
$detector->display($score);
$detector->gen_html($score);

=head1 SYNOPSIS

./example/simple/simple-detector.pl [OPTIONS] project_root_name

=head2 OPTIONS

=over 4

=item -i, --ignore-variable-name

ignore orthographic variation of variable name (default: none)

=item -j, --jobs=<jobs number>

detect by using multiple cores (default: 1)

=item -t, --min-token-num=<minimal token number>

change threshold to detect code clones (default: 30)

=item -l, --min-line-num=<minimal line number>

change threshold to detect code clones (default: 4)

=item -e, --encoding=<encoding style>

set encoding of target files for visualizer (default: none)

=item --order-by=<length|population|radius|nif>

set default's order name for code clone set metrics (default: length)

=back
