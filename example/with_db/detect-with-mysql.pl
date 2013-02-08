#! /usr/bin/env perl
use strict;
use warnings;
use FindBin::libs;
use CopyPasteDetector;
use Getopt::Long;
use Pod::Usage;

my $options = +{};
GetOptions(
    'i|ignore-variable-name' => \$options->{ignore_variable_name},
    'j|jobs=s'               => \$options->{jobs},
    't|min-token-num=s'      => \$options->{min_token_num},
    'l|min-line-num=s'       => \$options->{min_line_num},
    'e|encoding=s'           => \$options->{encoding},
    'o|output-dir=s'         => \$options->{output_dirname},
    'order-by=s'             => \$options->{order_by},
    'h|host=s'               => \$options->{host},
    'P|port=s'               => \$options->{port},
    'u|user=s'               => \$options->{user},
    'p|password=s'           => \$options->{pass},
    'd|database=s'           => \$options->{database},
    'help'                   => \$options->{help}
);

pod2usage(1) if $options->{help};
my $project_root = $ARGV[0];
defined $project_root or die "please define 'project root'\n";
my $detector = CopyPasteDetector->new($options);
my $files = $detector->get_target_files_by_project_root($project_root);
my (@data, @not_evaluated_files);
foreach my $file (@$files) {
    if ($detector->exists_record($file)) {
        push(@data, @{$detector->read_record($file)});
    } else {
        push(@not_evaluated_files, $file);
    }
}
if (@not_evaluated_files) {
    my $record = $detector->detect(\@not_evaluated_files);
    $detector->insert_record($record);
    push(@data, @$record);
}
my $score = $detector->get_score(\@data);
$detector->display($score);
$detector->gen_html($score);

=head1 SYNOPSIS

./example/with_db/detect-with-db.pl [OPTIONS] project_root_name

=head2 OPTIONS

=over 4

=item -h, --host=<hostname>

set hostname for mysql server (default: 'localhost')

=item -P, --port=<port number>

set port number for mysql server (default: '')

=item -u, --user=<username>

set username for mysql server (default: 'root')

=item -p,  --password=<password>

set password for mysql server (default: '')

=item -d, --database=<database name>

set database name for mysql (default: 'copy_and_paste_record')

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

=item -o, --output-dir=<directory's name>

change the output directory (default: copy_paste_detector_output)

=item --order-by=<length|population|radius|nif>

set default's order name for code clone set metrics (default: length)

=back
