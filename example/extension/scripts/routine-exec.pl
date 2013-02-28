#! /usr/bin/env perl
use strict;
use warnings;
use FindBin::libs;
use CopyPasteDetector::Extension::RoutineExecutor;
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

sub get_observe_namespaces {
    return [
        'example/projects/lib/Mojolicious',
        'example/projects/lib/Catalyst',
        'example/projects/lib/Mojo'
    ];
}

my $observe_revision_range = {
    from => '9f4ec75333799c95a8435f5f738570b87a44452a',
    to   => '8363245bce7a690f1fc6ed53da284f32c12dc3e8'
};

sub main {
    my $executor = CopyPasteDetector::Extension::RoutineExecutor->new($options);
    $executor->set_root('example/projects/lib/');
    $executor->set_observe_namespaces(get_observe_namespaces());
    $executor->set_observe_revision_range($observe_revision_range);
    $executor->run();
}

main unless caller(0);

=head1 SYNOPSIS

./example/extension/routine-exec.pl [OPTIONS]

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
