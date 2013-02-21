package CopyPasteDetector::Controller::Compare;
use strict;
use warnings;
use Mojo::Base 'Mojolicious::Controller';
use HTML::Template;
use utf8;
use Encode;
use Data::Dumper;
use JSON::XS;

sub get_diff_data {
    my ($self, $data, $prev_data) = @_;
    my @ret;
    my $size = scalar @$data;
    for (my $i = 0; $i < $size; $i++) {
        my $score = $data->[$i]->{score};
        my $prev_score = (defined $prev_data->[$i]) ? $prev_data->[$i]->{score} : 0;
        push(@ret, $score - $prev_score);
    }
    return \@ret;
}

sub get_compare_data {
    my ($self) = @_;
    my $time = $self->param('time');
    my $type = $self->param('type');
    my $model_namespace = $self->app->model->{namespace};
    my $data = $model_namespace->get_type_data_by_time($time, $type);
    my $prev_time = $self->app->model->{timestamp}->get_prev_time($time);
    print "prev_time = $prev_time\n" if (defined $prev_time);
    my @sorted_data = sort { $b->{score} <=> $a->{score} } @$data;
    my @name;
    my @score;
    foreach (@sorted_data) {
        push(@name, $_->{namespace});
        push(@score, $_->{score});
    }
    my $diff = (defined $prev_time) ?
        $self->get_diff_data($data, $model_namespace->get_type_data_by_time($prev_time, $type))
        : undef;
    return $self->render(text => encode_json([
        {
            diff => $diff,
            name => \@name,
            score => \@score
        }
    ]));
}

sub get_recent_time {
    my ($self) = @_;
    $self->render(text => $self->app->model->{timestamp}->recent_time);
}

sub coverage {
    my ($self) = @_;
    $self->display('coverage');
}

sub self_similarity {
    my ($self) = @_;
    $self->display('ss');
}

sub another_directories_similarity {
    my ($self) = @_;
    $self->display('ads');
}

sub neighbor {
    my ($self) = @_;
    $self->display('neighbor');
}

sub display {
    my ($self, $type) = @_;
    my $tmpl = HTML::Template->new(filename => 'tmpl/compare.tmpl');
    my $assign = +{
        timestamp_list => $self->app->model->{timestamp}->list,
        type => $type
    };
    $tmpl->param($assign);
    $self->render(text => decode('utf-8', $tmpl->output()));
}

1;
