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
    #push(@{$self->app->static->paths}, 'data');
    #my $success = $self->app->static->serve(Mojolicious::Controller->new, 'data/2013-02-19_14:08:36_3ce073fe9be96bb6a11795c198651987d6e19f68/index.html');
    my $tmpl = HTML::Template->new(filename => 'tmpl/index.tmpl');
    #my $tmpl = HTML::Template->new(filename => 'data/2013-02-19_14:08:36_3ce073fe9be96bb6a11795c198651987d6e19f68/index.html');
    $tmpl->param($assign);
    $self->render(text => decode('utf-8', $tmpl->output()));
}

1;
