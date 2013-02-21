package CopyPasteDetector::Controller::Home;
use strict;
use warnings;
use Mojo::Base 'Mojolicious::Controller';
use HTML::Template;
use utf8;
use Encode;
use Data::Dumper;

sub index {
    my ($self) = @_;
    my $assign = +{
        ranking_contents => $self->app->model->{namespace}->recent_ranking,
        namespace_list   => $self->app->model->{namespace}->list
    };
    print Dumper $assign;
    my $tmpl = HTML::Template->new(filename => 'tmpl/index.tmpl');
    $tmpl->param($assign);
    $self->render(text => decode('utf-8', $tmpl->output()));
}

1;
