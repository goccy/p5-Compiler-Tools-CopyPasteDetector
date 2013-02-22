package CopyPasteDetector;
use Mojo::Base 'Mojolicious';
use CopyPasteDetector::Router;
use CopyPasteDetector::Model;
use Mojolicious::Plugin::Mount;

# This method will run once at server start
__PACKAGE__->attr(model => sub {
    my $host = '';
    my $port = '';
    my $user = 'root';
    my $pass = '';
    my $dbname = 'copy_and_paste_record';
    return CopyPasteDetector::Model->new(
        {
            host => $host,
            port => $port,
            user => $user,
            pass => $pass,
            dbname => $dbname
        }
    );
});

sub startup {
  my $self = shift;
  # Documentation browser under "/perldoc"
  push(@{$self->app->static->paths}, 'detail/public');
  $self->plugin(Mount => {
      '/detail' => 'detail/start.psgi'
  });
  $self->plugin('PODRenderer');
  #$self->plugin('xslate_renderer');
  $self->routes->namespace('CopyPasteDetector::Controller');
  CopyPasteDetector::Router::dispatch($self->routes);
}

1;
