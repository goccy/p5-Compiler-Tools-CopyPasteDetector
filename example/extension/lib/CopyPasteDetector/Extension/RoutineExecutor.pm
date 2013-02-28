package CopyPasteDetector::Extension::RoutineExecutor;
use strict;
use warnings;
use Compiler::Tools::CopyPasteDetector;
use CopyPasteDetector::Extension::RoutineExecutor::DB;
use Data::Section::Simple qw(get_data_section);
use List::MoreUtils qw(any before_incl after);
use Data::Dumper;
use JSON::XS;

sub new {
    my ($class, $options) = @_;
    my $host = $options->{host} ||= 'localhost';
    my $port = $options->{port} ||= '';
    my $user = $options->{user} ||= 'root';
    my $pass = $options->{pass} ||= '';
    my $dbname = $options->{database} ||= 'copy_and_paste_record';
    my $table_name = $dbname;
    my $dsn = sprintf('DBI:mysql:%s:%s:%s', $dbname, $host, $port);
    my $db = CopyPasteDetector::Extension::RoutineExecutor::DB->new(
        {dsn => $dsn, username => $user, password => $pass});
    my $copy_and_paste_record_table = get_data_section('copy_and_paste_record_table');
    my $routine_record_table = get_data_section('routine_record_table');
    $db->dbh->do($copy_and_paste_record_table);
    $db->dbh->do($routine_record_table);
    my $self = {
        version_system => 'git',
        remote_repository => 'origin',
        remote_branch => 'master',
        options => $options,
        db => $db
    };
    return bless($self, $class);
}

sub set_root {
    my ($self, $root) = @_;
    $self->{root} = $root;
}

sub set_observe_namespaces {
    my ($self, $namespaces) = @_;
    $self->{namespaces} = {};
    $self->{namespaces}->{$_}++ foreach (@$namespaces);
}

sub set_observe_revision_range {
    my ($self, $range) = @_;
    $self->{range} = $range;
}

sub fetch_latest_data_from_remote_repository {
    my ($self) = @_;
    my $repo = $self->{remote_repository};
    my $branch = $self->{remote_branch};
    return (__exec_command_with_message("git fetch $repo $branch",
                                        "...fetching from $repo $branch") == 0) ? 1 : 0;
}

sub exists_record {
    my ($self, $filename) = @_;
    my $count = $self->{db}->count('copy_and_paste_record', 'file', { file => $filename });
    return ($count > 0) ? 1 : 0;
}

sub update_record {
    my ($self, $all_data) = @_;
    my $rev = '';
    foreach my $data (@$all_data) {
        eval {
            $self->{db}->update('copy_and_paste_record', {
                file       => $data->{file},
                lines      => $data->{lines},
                start_line => $data->{start_line},
                end_line   => $data->{end_line},
                token_num  => $data->{token_num},
                src        => $data->{src},
                hash       => $data->{hash},
                parents    => encode_json($data->{parents}),
            });
        };
        print $@ if ($@);
    }
}

sub read_record {
    my ($self, $filename) = @_;
    my @rows = $self->{db}->search_named(q{SELECT * FROM copy_and_paste_record WHERE file = :file},
                                         {file => $filename});
    my @records = ();
    foreach my $row (@rows) {
        push(@records, {
            file       => $row->file,
            lines      => $row->lines,
            start_line => $row->start_line,
            end_line   => $row->end_line,
            token_num  => $row->token_num,
            src        => $row->src,
            hash       => $row->hash,
            parents    => decode_json($row->parents)
        });
    }
    return \@records;
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
    my $repo = $self->{remote_repository};
    my $branch = $self->{remote_branch};
    return [map { chomp($_) if ($_ =~ /\n$/); $_; } `git rev-list ${repo}/${branch}`];
}

sub get_next_revisions {
    my ($self) = @_;
    my $rev = (defined $self->{range}) ? $self->{range}->{from} : $self->get_current_revision();
    my $all_revs = $self->get_remote_all_revisions();
    return [reverse before_incl { $rev eq $_ } @$all_revs];
}

sub switch_revision {
    my ($self, $rev) = @_;
    return system("git checkout -b $rev $rev");
}

sub dump_current_database {
    my ($self, $rev) = @_;
    my $host = $self->{options}->{host};
    my $user = $self->{options}->{user};
    my $dbname = $self->{options}->{database};
    my $time = $self->get_timestamp_by_revision($rev);
    $time =~ s/\s/_/g;
    my $dump_sql = "dump_${time}_${rev}.sql";
    print "mysqldump --host $host -u $user $dbname > $dump_sql\n";
    `mysqldump --host $host -u $user $dbname > $dump_sql`;
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
        }
    } grep {
        exists $self->{namespaces}->{$_};
    } keys %$directory_score ];
}

sub exec_copy_paste_detector {
    my ($self, $files, $rev) = @_;
    my $created_at = $self->get_timestamp_by_revision($rev);
    my $time = $created_at;
    $time =~ s/\s/_/g;
    $self->{options}->{output_dirname} = "${time}_${rev}";
    my $detector = Compiler::Tools::CopyPasteDetector->new($self->{options});
    my $all_files = $detector->get_target_files_by_project_root($self->{root});
    my (@data, %not_evaluated_files);
    foreach my $file (@$all_files) {
        if (any { $_ =~ $file } @$files) {
            $not_evaluated_files{$file}++;
        } elsif ($self->exists_record($file)) {
            push(@data, @{$self->read_record($file)});
        } else {
            $not_evaluated_files{$file}++;
        }
    }
    my @names = keys %not_evaluated_files;
    if (@names) {
        my $record = $detector->detect(\@names);
        $self->update_record($record);
        push(@data, @$record);
    }
    my $score = $detector->get_score(\@data);
    my $directory_score = $score->{directory_score};
    my @observe_names = grep {
        exists $self->{namespaces}->{$_};
    } keys %$directory_score;
    foreach my $name (@observe_names) {
        my $metrics = $directory_score->{$name}->{metrics};
        eval {
            $self->{db}->insert('routine_record', {
                namespace  => $name,
                coverage   => $metrics->{coverage},
                another_directories_similarity => $metrics->{another_directories_similarity},
                self_similarity => $metrics->{self_similarity},
                neighbor => $metrics->{neighbor},
                created_at => $created_at,
                revision => $rev
            });
        };
        print $@ if ($@);
    }
    $detector->gen_html($score);
    #system("@$files|xargs $perl @includes $detector --host $host --jobs $jobs");
}

sub run {
    my ($self) = @_;
    $self->fetch_latest_data_from_remote_repository() or
        die "cannot fetch from $self->{remote_repository} $self->{remote_branch}\n";
    my $next_revisions = $self->get_next_revisions();
    die 'Already up-to-date' unless (@$next_revisions);
    my $end_revision = $self->{range}->{to};
    foreach my $rev (@$next_revisions) {
        my $files = $self->get_modified_filenames($rev);
        $self->switch_revision($rev);
        #$self->dump_current_database($rev);
        $self->exec_copy_paste_detector($files, $rev);
        print $rev, "\n";
        last if (defined $end_revision && $end_revision eq $rev);
    }
}

1;

__DATA__
@@ copy_and_paste_record_table
CREATE TABLE IF NOT EXISTS `copy_and_paste_record` (
    `file` varchar(255) NOT NULL,
    `lines` int(10) NOT NULL DEFAULT '0',
    `start_line` int(10) NOT NULL DEFAULT '0',
    `end_line` int(10) NOT NULL DEFAULT '0',
    `src` text NOT NULL,
    `hash` varchar(255) NOT NULL,
    `parents` text NOT NULL,
    `revision` varchar(255) NOT NULL,
    `token_num` int(10) NOT NULL DEFAULT '0',
    PRIMARY KEY(`file`, `start_line`, `end_line`, `hash`)
) ENGINE=InnoDB;

@@ routine_record_table
CREATE TABLE IF NOT EXISTS `routine_record` (
    `namespace` varchar(255) NOT NULL,
    `coverage` float(7,6) NOT NULL DEFAULT '0',
    `another_directories_similarity` float(7,6) NOT NULL DEFAULT '0',
    `self_similarity` float(7,6) NOT NULL DEFAULT '0',
    `neighbor` float(7,6) NOT NULL DEFAULT '0',
    `created_at` datetime NOT NULL,
    `revision` varchar(255) NOT NULL,
    PRIMARY KEY(`namespace`, `revision`)
) ENGINE=InnoDB;
