package Mojo::Server::PSGI;
use Mojo::Base 'Mojo::Server';

use constant CHUNK_SIZE => $ENV{MOJO_CHUNK_SIZE} || 131072;

# "Things aren't as happy as they used to be down here at the unemployment
#  office.
#  Joblessness is no longer just for philosophy majors.
#  Useful people are starting to feel the pinch."
sub run {
  my ($self, $env) = @_;

  # Environment
  my $tx  = $self->build_tx;
  my $req = $tx->req;
  $req->parse($env);

  # Store connection information
  $tx->remote_address($env->{REMOTE_ADDR});
  $tx->local_port($env->{SERVER_PORT});

  # Request body
  my $len = $env->{CONTENT_LENGTH};
  while (!$req->is_finished) {
    my $chunk = ($len && $len < CHUNK_SIZE) ? $len : CHUNK_SIZE;
    my $read = $env->{'psgi.input'}->read(my $buffer, $chunk, 0);
    last unless $read;
    $req->parse($buffer);
    $len -= $read;
    last if $len <= 0;
  }

  # Handle
  $self->emit(request => $tx);

  # Response headers
  my $res = $tx->res;
  $res->fix_headers;
  my $headers = $res->content->headers;
  my @headers;
  for my $name (@{$headers->names}) {
    for my $values ($headers->header($name)) {
      push @headers, $name => $_ for @$values;
    }
  }

  # PSGI response
  return [$res->code || 404,
    \@headers, Mojo::Server::PSGI::_IO->new(tx => $tx)];
}

# "Wow! Homer must have got one of those robot cars!
#  *Car crashes in background*
#  Yeah, one of those AMERICAN robot cars."
package Mojo::Server::PSGI::_IO;
use Mojo::Base -base;

sub close { shift->{tx}->server_close }

sub getline {
  my $self = shift;

  # Blocking read
  my $res = $self->{tx}->res;
  while (1) {
    my $chunk = $res->get_body_chunk($self->{offset} //= 0);

    # No content yet, try again
    sleep 1 and next unless defined $chunk;

    # End of content
    return unless length $chunk;

    # Content
    $self->{offset} += length $chunk;
    return $chunk;
  }
}

1;
__END__

=head1 NAME

Mojo::Server::PSGI - PSGI server

=head1 SYNOPSIS

  use Mojo::Server::PSGI;

  my $psgi = Mojo::Server::PSGI->new;
  $psgi->unsubscribe('request');
  $psgi->on(request => sub {
    my ($psgi, $tx) = @_;

    # Request
    my $method = $tx->req->method;
    my $path   = $tx->req->url->path;

    # Response
    $tx->res->code(200);
    $tx->res->headers->content_type('text/plain');
    $tx->res->body("$method request for $path!");

    # Resume transaction
    $tx->resume;
  });
  my $app = sub { $psgi->run(@_) };

=head1 DESCRIPTION

L<Mojo::Server::PSGI> allows L<Mojo> applications to run on all PSGI
compatible servers.

See L<Mojolicious::Guides::Cookbook> for deployment recipes.

=head1 EVENTS

L<Mojo::Server::PSGI> inherits all events from L<Mojo::Server>.

=head1 METHODS

L<Mojo::Server::PSGI> inherits all methods from L<Mojo::Server> and
implements the following new ones.

=head2 C<run>

  my $res = $psgi->run($env);

Start PSGI.

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
