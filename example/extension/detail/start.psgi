use Mojolicious::Lite;

get '/' => sub {
    my $self = shift;
    $self->render('index');
};

app->start;
