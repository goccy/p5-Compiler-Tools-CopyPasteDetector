#! /usr/bin/env perl
use strict;
use warnings;
use CopyPasteDetector::Extension::Initializer;
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
    'h|host=s'               => \$options->{host},
    'P|port=s'               => \$options->{port},
    'u|user=s'               => \$options->{user},
    'p|password=s'           => \$options->{pass},
    'd|database=s'           => \$options->{database},
    'version-system=s'       => \$options->{version_system},
    'remote-repos=s'         => \$options->{remote_repos},
    'remote-branch=s'        => \$options->{remote_branch},
    'project-dir=s'          => \$options->{project_dir},
    'help'                   => \$options->{help}
);

pod2usage(1) if $options->{help};

sub main {
    my $initializer = CopyPasteDetector::Extension::Initializer->new();
    $initializer->run();
}

main unless caller(0);

=head1 SYNOPSIS

./example/extension/initialize.pl [OPTIONS]

=head2 OPTIONS

=over 4

=item -h, --host=<hostname>

set hostname for mysql server (default: 'localhost')

=item -p, --port=<port number>

set port number for mysql server (default: '')

=item -u, --user=<username>

set username for mysql server (default: 'root')

=item --pass=<password>

set password for mysql server (default: '')

=item -d, --database=<database name>

set database name for mysql (default: 'copy_and_paste_record')
