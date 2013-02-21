package CopyPasteDetector::Model::Namespace;
use strict;
use warnings;
use base 'CopyPasteDetector::Extension::RoutineExecutor';

sub new {
    my $class = shift;
    my $self = $class->SUPER::new(@_);
    $self->{recent_rev} = $self->__get_recent_rev();
    return bless($self, $class);
}

sub __get_recent_rev {
    my ($self) = @_;
    my @row = $self->{db}->search_by_sql(q{
        SELECT revision FROM routine_record WHERE created_at ORDER BY created_at desc limit 1}
    );
    return $row[0]->revision;
}

sub __get_ranking_data {
    my ($self, $metrics_type) = @_;
    my $rank = 1;
    my @ret = map {
        { rank => $rank++, namespace => $_->{name}, score => $_->{score} };
    } sort { $b->{score} <=> $a->{score} } @$metrics_type;
    return \@ret;
}

sub recent_ranking {
    my ($self) = @_;
    my @rows = $self->{db}->search_named(q{
        SELECT * FROM routine_record WHERE revision = :revision}, {revision => $self->{recent_rev}});
    my (@coverage, @ss, @ads, @neighbor);
    foreach my $row (@rows) {
        push(@coverage, { name => $row->namespace, score => $row->coverage });
        push(@ads, { name => $row->namespace, score => $row->another_directories_similarity });
        push(@neighbor, { name => $row->namespace, score => $row->neighbor });
        push(@ss, { name => $row->namespace, score => $row->self_similarity });
    }
    my @ret;
    push(@ret, { metrics_type => 'coverage', ranking => $self->__get_ranking_data(\@coverage) });
    push(@ret, { metrics_type => 'another_directories_similarity', ranking => $self->__get_ranking_data(\@ads) });
    push(@ret, { metrics_type => 'self_similarity', ranking => $self->__get_ranking_data(\@ss) });
    push(@ret, { metrics_type => 'neighbor', ranking => $self->__get_ranking_data(\@neighbor) });
    return \@ret;
}

sub list {
    my ($self) = @_;
    my @rows = $self->{db}->search_named(q{
        SELECT * FROM routine_record WHERE revision = :revision}, {revision => $self->{recent_rev}});
    my @ret;
    foreach my $row (@rows) {
        my $name = $row->namespace;
        my @splitted = split(m|/|, $name);
        push(@ret, { name => $name });#, last_name => $splitted[-1] });
    }
    return \@ret;
}

sub get_type_data_by_time {
    my ($self, $time, $type) = @_;
    $type = 'another_directories_similarity' if ($type eq 'ads');
    $type = 'self_similarity' if ($type eq 'ss');
    my @rows = $self->{db}->search_named(qq{
        SELECT namespace, $type FROM routine_record WHERE created_at = :time
    }, { time => $time });
    my @ret;
    foreach my $row (@rows) {
        push(@ret, {
            namespace => $row->namespace,
            score     => $row->$type() + 0.0
        });
    }
    return \@ret;
}

sub all_data {
    my ($self, $namespace) = @_;
    my @ret = $self->{db}->search_named(q{
        SELECT * FROM routine_record WHERE namespace = :namespace ORDER BY created_at desc
    }, { namespace => $namespace });
    return \@ret;
}

1;
