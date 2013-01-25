package Mojolicious::Plugin::PoweredBy;
use Mojo::Base 'Mojolicious::Plugin';

# "It's just like the story of the grasshopper and the octopus.
#  All year long, the grasshopper kept burying acorns for the winter,
#  while the octopus mooched off his girlfriend and watched TV.
#  But then the winter came, and the grasshopper died,
#  and the octopus ate all his acorns.
#  And also he got a racecar. Is any of this getting through to you?"
sub register {
  my ($self, $app, $args) = @_;
  my $name = $args->{name} || 'Mojolicious (Perl)';
  $app->hook(before_dispatch =>
      sub { shift->res->headers->header('X-Powered-By' => $name) });
}

1;
__END__

=head1 NAME

Mojolicious::Plugin::PoweredBy - Powered by plugin

=head1 SYNOPSIS

  # Mojolicious
  $self->plugin('PoweredBy');
  $self->plugin(PoweredBy => (name => 'MyApp 1.0'));

  # Mojolicious::Lite
  plugin 'PoweredBy';
  plugin PoweredBy => (name => 'MyApp 1.0');

=head1 DESCRIPTION

L<Mojolicious::Plugin::PoweredBy> is a plugin that adds an C<X-Powered-By>
header which defaults to C<Mojolicious (Perl)>. This is a core plugin, that
means it is always enabled and its code a good example for learning to build
new plugins.

=head1 OPTIONS

L<Mojolicious::Plugin::PoweredBy> supports the following options.

=head2 C<powered_by>

  plugin PoweredBy => (name => 'MyApp 1.0');

Value for C<X-Powered-By> header.

=head1 METHODS

L<Mojolicious::Plugin::PoweredBy> inherits all methods from
L<Mojolicious::Plugin> and implements the following new ones.

=head2 C<register>

  $plugin->register;

Register plugin hooks in L<Mojolicious> application.

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
