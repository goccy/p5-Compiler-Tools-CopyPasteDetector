package CopyPasteDetector::Controller::Namespace;
use strict;
use warnings;
use Mojo::Base 'Mojolicious::Controller';
use HTML::Template;
use utf8;
use Encode;
use Data::Dumper;
use DateTime;
use JSON::XS;

sub convert_to_utc {
    my ($time) = @_;
    $time =~ /^(\d+)-(\d+)-(\d+)\s(\d+):(\d+):(\d+)$/;
    my $day = DateTime->new(
        time_zone => 'Asia/Tokyo',
        year => $1, month  => $2, day  => $3,
        hour => $4, minute => $5, second => $6
        );
    $day->set_time_zone("UTC");
    return $day->epoch() * 1000;
}

sub get_graph_data {
    my ($self) = @_;
    my $namespace = $self->param('namespace');
    my $rows = $self->app->model->{namespace}->all_data($namespace);
    my (@coverage, @ss, @ads, @neighbor, @time, @utc, @rev);
    foreach my $row (@$rows) {
        my $created_at = $row->created_at;
        next if ($created_at =~ /0000-00-00 00:00:00/);
        push(@coverage, $row->coverage);
        push(@ads, $row->another_directories_similarity);
        push(@neighbor, $row->neighbor);
        push(@ss, $row->self_similarity);
        push(@rev, $row->revision);
        push(@time, $created_at);
        push(@utc, convert_to_utc($created_at));
    }
    $self->render(text => encode_json([
        {
            name => $namespace,
            coverage => \@coverage,
            ads      => \@ads,
            ss       => \@ss,
            neighbor => \@neighbor,
            rev      => \@rev,
            time     => \@time,
            utc      => \@utc
        }
    ]));
}

sub index {
    my ($self) = @_;
    my $namespace = $self->param('namespace');
    my $assign = +{
        namespace_list => $self->app->model->{namespace}->list,
        name => $namespace
    };
    my $tmpl = HTML::Template->new(filename => 'tmpl/namespace.tmpl');
    $tmpl->param($assign);
    $self->render(text => decode('utf-8', $tmpl->output()));
}

1;
