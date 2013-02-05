package DB::CopyPasteDetector;
use strict;
use warnings;
use base 'Compiler::Tools::CopyPasteDetector';
use Data::Dumper;
use DBI;
use JSON::XS;

sub new {
    my ($class, $options) = @_;
    my $self = $class->SUPER::new($options);
    my $host = $options->{host} || 'localhost';
    my $port = $options->{port} || '';
    my $user = $options->{user} || 'root';
    my $pass = $options->{pass} || '';
    my $dbname = $options->{database} || 'copy_and_paste_record';
    my $table_name = $dbname;
    my $dsn = sprintf('DBI:mysql:%s:%s:%s', $dbname, $host, $port);
    $self->{dbh} = DBI->connect($dsn, $user, $pass);
    $self->{table} = $table_name;
    return bless($self, $class);
}

sub exists_record {
    my ($self, $filename) = @_;
    my $sql = sprintf("SELECT COUNT(file) from %s where file = ?", $self->{table});
    my $sth = $self->{dbh}->prepare($sql);
    $sth->execute($filename);
    my $res = $sth->fetchrow_arrayref;
    my $count = $res->[0];
    return ($count > 0) ? 1 : 0;
}

sub insert_record {
    my ($self, $all_data) = @_;
    my $rev = '';
    my $sth = $self->{dbh}->prepare(sprintf(qq{
        INSERT INTO
           %s
        SET
           `file`       = ? ,
           `lines`      = ? ,
           `start_line` = ? ,
           `end_line`   = ? ,
           `token_num`  = ? ,
           `src`        = ? ,
           `hash`       = ? ,
           `parents`    = ? ,
           `revision`   = ?
    }, $self->{table}));
    foreach my $data (@$all_data) {
        my @record = (
            $data->{file},
            $data->{lines},
            $data->{start_line},
            $data->{end_line},
            $data->{token_num},
            $data->{src},
            $data->{hash},
            encode_json($data->{parents}),
            $rev || ""
        );
        $sth->execute(@record);
    }
}

sub read_record {
    my ($self, $filename) = @_;
    my $sql = sprintf("SELECT * from %s where file = ?", $self->{table});
    my $sth = $self->{dbh}->prepare($sql);
    $sth->execute($filename);
    my @record = ();
    while (my $res = $sth->fetchrow_hashref) {
        push(@record, {
            file       => $res->{file},
            lines      => $res->{lines},
            start_line => $res->{start_line},
            end_line   => $res->{end_line},
            token_num  => $res->{token_num},
            src        => $res->{src},
            hash       => $res->{hash},
            parents    => decode_json($res->{parents})
        });
    }
    return \@record;
}

1;


=Schema

CREATE TABLE `copy_and_paste_record` (
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
) ENGINE=InnoDB DEFAULT CHARSET=utf-8;

=cut
