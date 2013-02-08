package CopyPasteDetector;
use strict;
use warnings;
use base 'Compiler::Tools::CopyPasteDetector';
use CopyPasteDetector::DB;
use Data::Dumper;
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
    my $db = CopyPasteDetector::DB->new({dsn => $dsn, username => $user, password => $pass});
    my $sql = do { local $/; <DATA> };
    $db->dbh->do($sql);
    $self->{db} = $db;
    return bless($self, $class);
}

sub exists_record {
    my ($self, $filename) = @_;
    my $count = $self->{db}->count('copy_and_paste_record', 'file', { file => $filename });
    return ($count > 0) ? 1 : 0;
}

sub insert_record {
    my ($self, $all_data) = @_;
    my $rev = '';
    foreach my $data (@$all_data) {
        $self->{db}->insert('copy_and_paste_record', {
            file       => $data->{file},
            lines      => $data->{lines},
            start_line => $data->{start_line},
            end_line   => $data->{end_line},
            token_num  => $data->{token_num},
            src        => $data->{src},
            hash       => $data->{hash},
            parents    => encode_json($data->{parents}),
            revision   => ''
        });
    }
}

sub read_record {
    my ($self, $filename) = @_;
    my @rows = $self->{db}->search_named(q{SELECT * FROM copy_and_paste_record WHERE file = :file},
                                         {file => $filename});
    #my @rows = $self->{db}->search('copy_and_paste_record', { file => $filename });
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

1;

__DATA__
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
