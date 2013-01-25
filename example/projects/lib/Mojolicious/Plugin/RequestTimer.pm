package Mojolicious::Plugin::RequestTimer;
use Mojo::Base 'Mojolicious::Plugin';

use Time::HiRes qw/gettimeofday tv_interval/;

# "I don't trust that doctor.
#  I bet I've lost more patients than he's even treated."
sub register {
  my ($self, $app) = @_;

  # Start timer
  $app->hook(
    after_static_dispatch => sub {
      my $self = shift;

      # Ignore static files
      my $stash = $self->stash;
      return if $stash->{'mojo.static'} || $stash->{'mojo.started'};
      my $req    = $self->req;
      my $method = $req->method;
      my $path   = $req->url->path->to_abs_string;
      my $ua     = $req->headers->user_agent || 'Anonymojo';
      $self->app->log->debug("$method $path ($ua).");
      $stash->{'mojo.started'} = [Time::HiRes::gettimeofday()];
    }
  );

  # End timer
  $app->hook(
    after_dispatch => sub {
      my $self = shift;

      # Ignore static files
      my $stash = $self->stash;
      return unless my $started = delete $stash->{'mojo.started'};
      return if $stash->{'mojo.static'};
      my $elapsed = sprintf '%f',
        Time::HiRes::tv_interval($started, [Time::HiRes::gettimeofday()]);
      my $rps     = $elapsed == 0 ? '??' : sprintf '%.3f', 1 / $elapsed;
      my $res     = $self->res;
      my $code    = $res->code || 200;
      my $message = $res->message || $res->default_message($code);
      $self->app->log->debug("$code $message (${elapsed}s, $rps/s).");
    }
  );
}

1;
__END__

=head1 NAME

Mojolicious::Plugin::RequestTimer - Request timer plugin

=head1 SYNOPSIS

  # Mojolicious
  $self->plugin('RequestTimer');

  # Mojolicious::Lite
  plugin 'RequestTimer';

=head1 DESCRIPTION

L<Mojolicious::Plugin::RequestTimer> is a plugin to gather and log request
timing information. This is a core plugin, that means it is always enabled
and its code a good example for learning to build new plugins.

=head1 METHODS

L<Mojolicious::Plugin::RequestTimer> inherits all methods from
L<Mojolicious::Plugin> and implements the following new ones.

=head2 C<register>

  $plugin->register;

Register plugin hooks in L<Mojolicious> application.

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
