package CopyPasteDetector::Extension::RoutineExecutor;
use strict;
use warnings;
use Compiler::Tools::CopyPasteDetector;
use List::MoreUtils qw(before after);
use Data::Dumper;

sub new {
    my ($class, $options) = @_;
    my $self = {
        version_system => 'git',
        remote_repository => 'origin',
        remote_branch => 'master'
    };
    return bless($self, $class);
}

sub set_observe_namespaces {
    my ($self, $namespaces) = @_;
    my $self->{namespaces} = $namespaces;
}

sub fetch_latest_data_from_remote_repository {
    my ($self, $repo, $branch) = @_;
    return (__exec_command_with_message("git fetch $repo $branch",
                                        "...fetching from $repo $branch") == 0) ? 1 : 0;
}

sub __exec_command_with_message {
    my ($cmd, $msg) = @_;
    print $msg, "\n";
    return system($cmd);
}

sub get_modified_filenames {
    my ($self, $rev) = @_;
    my @modified_files = map { chomp($_); $_; } `git show --name-only --pretty=format:"%f" $rev`;
    shift(@modified_files); # exclude HEAD because commit title
    return \@modified_files;
}

sub get_current_revision {
    chomp(my $rev = `git log --pretty=format:"%H" --max-count=1`);
    return $rev;
}

sub get_remote_all_revisions {
    my ($self) = @_;
    return [map { chomp($_) if ($_ =~ /\n$/); $_; } `git rev-list $remote_repository/$remote_branch`];
}

sub get_next_revisions {
    my $rev = get_current_revision();
    my $all_revs = get_remote_all_revisions();
    return [reverse before { $rev eq $_ } @$all_revs];
}

sub dump_current_database {
    my ($self, $rev) = @_;
    my $time = $self->get_timestamp_by_revision($rev);
    `mysqldump --host $host -u $user $database_name > dump_${time}_${rev}.sql`;
}

sub get_timestamp_by_revision {
    my ($self, $rev) = @_;
    my @splitted = split(/\s/, `git log --max-count=1 --pretty=format:"%ci" $rev`);
    return join(' ', @splitted[0 .. 1]);
}

sub get_metrics_results {
    my ($self, $directory_score, $type) = @_;
    return [ sort {
        $b->{score} <=> $a->{score};
    } map {
        {
            name    => $_,
            score   => $directory_score->{$_}->{metrics}->{$type},
            metrics => $directory_score->{$_}->{metrics}
        }
    } grep {
        exists $observe_namespaces->{$_};
    } keys %$directory_score ];
}

sub exec_copy_paste_detector {
    my ($self, $files, $options) = @_;
    my $detector = Compiler::Tools::CopyPasteDetector->new($options);
    my $record = $detector->detect($files);
    $detector->insert_record($record);
    my $score = $detector->get_score($record);
    my $directory_score = $score->{directory_score};
    my @metrics_types = qw(coverage self_similarity another_directories_similarity neighbor);
    my $results = +{};
    foreach my $type (@metrics_types) {
        $results->{$type} = $self->get_metrics_results($directory_score, $type);
    }
    $score->gen_html($score);
    #system("@$files|xargs $perl @includes $detector --host $host --jobs $jobs");
}

sub run {
    my ($self) = @_;
    $self->fetch_latest_data_from_remote_repository() ||
        die "cannot fetch from $remote_repository $remote_branch\n";
    my $next_revisions = get_next_revisions();
    foreach my $rev (@$next_revisions) {
        my $files = $self->get_modified_filenames($rev);
        $self->dump_current_database($rev);
        $self->exec_copy_paste_detector($files, $rev);
    }
}

1;
