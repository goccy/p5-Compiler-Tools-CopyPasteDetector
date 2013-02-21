package CopyPasteDetector::Model::Timestamp;
use strict;
use warnings;
use base 'CopyPasteDetector::Extension::RoutineExecutor';

sub new {
    my $class = shift;
    my $self = $class->SUPER::new(@_);
    return bless($self, $class);
}

sub get_prev_time {
    my ($self, $time) = @_;
    my @rows = $self->{db}->search_named(q{
        SELECT created_at FROM routine_record WHERE created_at < :time order by created_at desc
    }, { time => $time });
    return (@rows) ? $rows[0]->created_at : undef;
}

sub list {
    my ($self) = @_;
    my @rows = $self->{db}->search_by_sql(q{
        SELECT DISTINCT created_at FROM routine_record WHERE created_at order by created_at desc
    });
    my @ret;
    foreach my $row (@rows) {
        my $time = $row->created_at;
        push(@ret, { time => $time });
    }
    return \@ret;
}

sub recent_time {
    my ($self) = @_;
    my @row = $self->{db}->search_by_sql(q{
        SELECT created_at FROM routine_record WHERE created_at ORDER BY created_at desc limit 1}
    );
    return $row[0]->created_at;
}

1;
