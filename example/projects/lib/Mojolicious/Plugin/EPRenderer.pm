package Mojolicious::Plugin::EPRenderer;
use Mojo::Base 'Mojolicious::Plugin';

use Mojo::Template;
use Mojo::Util qw/encode md5_sum/;

# "What do you want?
#  I'm here to kick your ass!
#  Wishful thinking. We have long since evolved beyond the need for asses."
sub register {
  my ($self, $app, $conf) = @_;
  $conf ||= {};

  # Config
  my $name     = $conf->{name}     || 'ep';
  my $template = $conf->{template} || {};

  # Custom sandbox
  $template->{namespace} //=
    'Mojo::Template::SandBox::' . md5_sum(($ENV{MOJO_EXE} || ref $app) . $$);

  # Auto escape by default to prevent XSS attacks
  $template->{auto_escape} //= 1;

  # Add "ep" handler
  $app->renderer->add_handler(
    $name => sub {
      my ($r, $c, $output, $options) = @_;

      # Generate name
      my $path = $options->{inline} || $r->template_path($options);
      return unless defined $path;
      my $id = encode 'UTF-8', join(', ', $path, sort keys %{$c->stash});
      my $key = $options->{cache} = md5_sum $id;

      # Cache
      my $cache = $r->cache;
      unless ($cache->get($key)) {
        my $mt = Mojo::Template->new($template);

        # Be a bit more relaxed for helpers
        my $prepend = q/my $self = shift; use Scalar::Util 'weaken';/
          . q/weaken $self; no strict 'refs'; no warnings 'redefine';/;

        # Helpers
        $prepend .= 'my $_H = $self->app->renderer->helpers;';
        for my $name (sort keys %{$r->helpers}) {
          next unless $name =~ /^\w+$/;
          $prepend .= "sub $name; *$name = sub { ";
          $prepend .= "\$_H->{'$name'}->(\$self, \@_) };";
        }

        # Be less relaxed for everything else
        $prepend .= 'use strict;';

        # Stash
        $prepend .= 'my $_S = $self->stash;';
        for my $var (keys %{$c->stash}) {
          next unless $var =~ /^\w+$/;
          $prepend .= " my \$$var = \$_S->{'$var'};";
        }

        # Prepend generated code
        $mt->prepend($prepend);

        # Cache
        $cache->set($key => $mt);
      }

      # Render with epl
      return $r->handlers->{epl}->($r, $c, $output, $options);
    }
  );

  # Set default handler
  $app->renderer->default_handler('ep');
}

1;
__END__

=head1 NAME

Mojolicious::Plugin::EPRenderer - Embedded Perl renderer plugin

=head1 SYNOPSIS

  # Mojolicious
  $self->plugin('EPRenderer');
  $self->plugin(EPRenderer => {name => 'foo'});
  $self->plugin(EPRenderer => {template => {line_start => '.'}});

  # Mojolicious::Lite
  plugin 'EPRenderer';
  plugin EPRenderer => {name => 'foo'};
  plugin EPRenderer => {template => {line_start => '.'}};

=head1 DESCRIPTION

L<Mojolicious::Plugin::EPRenderer> is a renderer for C<ep> templates.

=head1 TEMPLATES

C<ep> or C<Embedded Perl> is a simple template format where you embed perl
code into documents. It is based on L<Mojo::Template>, but extends it with
some convenient syntax sugar designed specifically for L<Mojolicious>. It
supports L<Mojolicious> template helpers and exposes the stash directly as
Perl variables. This is a core plugin, that means it is always enabled and
its code a good example for learning to build new plugins.

=head1 OPTIONS

L<Mojolicious::Plugin::EPRenderer> supports the following options.

=head2 C<name>

  # Mojolicious::Lite
  plugin EPRenderer => {name => 'foo'};

Handler name.

=head2 C<template>

  # Mojolicious::Lite
  plugin EPRenderer => {template => {line_start => '.'}};

Template options.

=head1 METHODS

L<Mojolicious::Plugin::EPRenderer> inherits all methods from
L<Mojolicious::Plugin> and implements the following new ones.

=head2 C<register>

  $plugin->register;

Register renderer in L<Mojolicious> application.

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
