package Mojolicious;
use Mojo::Base 'Mojo';

use Carp 'croak';
use Mojolicious::Commands;
use Mojolicious::Controller;
use Mojolicious::Plugins;
use Mojolicious::Renderer;
use Mojolicious::Routes;
use Mojolicious::Sessions;
use Mojolicious::Static;
use Mojolicious::Types;
use Scalar::Util qw/blessed weaken/;

# "Robots don't have any emotions, and sometimes that makes me very sad."
has controller_class => 'Mojolicious::Controller';
has mode => sub { ($ENV{MOJO_MODE} || 'development') };
has plugins  => sub { Mojolicious::Plugins->new };
has renderer => sub { Mojolicious::Renderer->new };
has routes   => sub { Mojolicious::Routes->new };
has secret   => sub {
  my $self = shift;

  # Warn developers about unsecure default
  $self->log->debug('Your secret passphrase needs to be changed!!!');

  # Default to application name
  return ref $self;
};
has sessions => sub { Mojolicious::Sessions->new };
has static   => sub { Mojolicious::Static->new };
has types    => sub { Mojolicious::Types->new };

our $CODENAME = 'Leaf Fluttering In Wind';
our $VERSION  = '2.46';

# "These old doomsday devices are dangerously unstable.
#  I'll rest easier not knowing where they are."
sub AUTOLOAD {
  my $self = shift;

  # Method
  my ($package, $method) = our $AUTOLOAD =~ /^([\w\:]+)\:\:(\w+)$/;
  croak qq/Undefined subroutine &${package}::$method called/
    unless blessed $self && $self->isa(__PACKAGE__);

  # Check for helper
  croak qq/Can't locate object method "$method" via package "$package"/
    unless my $helper = $self->renderer->helpers->{$method};

  # Call helper with fresh controller
  return $self->controller_class->new(app => $self)->$helper(@_);
}

sub DESTROY { }

# "I personalized each of your meals.
#  For example, Amy: you're cute, so I baked you a pony."
sub new {
  my $self = shift->SUPER::new(@_);

  # Root directories
  my $home = $self->home;
  $self->renderer->root($home->rel_dir('templates'));
  $self->static->root($home->rel_dir('public'));

  # Default to application namespace
  my $r = $self->routes;
  $r->namespace(ref $self);

  # Hide own controller methods
  $r->hide(qw/AUTOLOAD DESTROY app cookie finish flash handler on on_finish/);
  $r->hide(qw/param redirect_to render render_content render_data/);
  $r->hide(qw/render_exception render_json render_not_found render_partial/);
  $r->hide(qw/render_static render_text rendered req res respond_to/);
  $r->hide(qw/send_message session signed_cookie stash tx ua url_for write/);
  $r->hide('write_chunk');

  # Prepare log
  my $mode = $self->mode;
  $self->log->path($home->rel_file("log/$mode.log"))
    if -w $home->rel_file('log');

  # Load default plugins
  $self->plugin('HeaderCondition');
  $self->plugin('DefaultHelpers');
  $self->plugin('TagHelpers');
  $self->plugin('EPLRenderer');
  $self->plugin('EPRenderer');
  $self->plugin('RequestTimer');
  $self->plugin('PoweredBy');

  # Reduced log output outside of development mode
  $self->log->level('info') unless $mode eq 'development';

  # Run mode
  $mode = $mode . '_mode';
  $self->$mode(@_) if $self->can($mode);

  # Startup
  $self->startup(@_);

  return $self;
}

sub build_tx {
  my $self = shift;
  my $tx   = Mojo::Transaction::HTTP->new;
  $self->plugins->emit_hook(after_build_tx => $tx, $self);
  return $tx;
}

sub defaults {
  my $self = shift;

  # Hash
  $self->{defaults} ||= {};
  return $self->{defaults} unless @_;

  # Get
  return $self->{defaults}->{$_[0]} unless @_ > 1 || ref $_[0];

  # Set
  my $values = ref $_[0] ? $_[0] : {@_};
  for my $key (keys %$values) {
    $self->{defaults}->{$key} = $values->{$key};
  }

  return $self;
}

sub dispatch {
  my ($self, $c) = @_;

  # Prepare transaction
  my $tx = $c->tx;
  $c->res->code(undef) if $tx->is_websocket;
  $self->sessions->load($c);
  my $plugins = $self->plugins;
  $plugins->emit_hook(before_dispatch => $c);

  # Try to find a static file
  $self->static->dispatch($c);
  $plugins->emit_hook_reverse(after_static_dispatch => $c);

  # Routes
  my $res = $tx->res;
  return if $res->code;
  if (my $code = ($tx->req->error)[1]) { $res->code($code) }
  elsif ($tx->is_websocket) { $res->code(426) }
  unless ($self->routes->dispatch($c)) {
    $c->render_not_found
      unless $res->code;
  }
}

# "Bite my shiny metal ass!"
sub handler {
  my ($self, $tx) = @_;

  # Embedded application
  my $stash = {};
  if ($tx->can('stash')) {
    $stash = $tx->stash;
    $tx    = $tx->tx;
  }

  # Build default controller
  my $defaults = $self->defaults;
  @{$stash}{keys %$defaults} = values %$defaults;
  my $c =
    $self->controller_class->new(app => $self, stash => $stash, tx => $tx);
  weaken $c->{app};
  weaken $c->{tx};

  # Dispatcher
  unless ($self->{dispatcher}) {
    $self->hook(
      around_dispatch => sub {
        my ($next, $c) = @_;

        # DEPRECATED in Leaf Fluttering In Wind!
        $self->on_process->($c->app, $c);
      }
    );
    $self->{dispatcher}++;
  }

  # Process
  unless (eval { $self->plugins->emit_chain(around_dispatch => $c) }) {
    $self->log->fatal("Processing request failed: $@");
    $tx->res->code(500);
    $tx->resume;
  }

  # Delayed
  $self->log->debug('Nothing has been rendered, assuming delayed response.')
    unless $stash->{'mojo.rendered'} || $tx->is_writing;
}

sub helper {
  my ($self, $name) = (shift, shift);
  my $r = $self->renderer;
  $self->log->debug(qq/Helper "$name" already exists, replacing./)
    if exists $r->helpers->{$name};
  $r->add_helper($name, @_);
}

# "He knows when you are sleeping.
#  He knows when you're on the can.
#  He'll hunt you down and blast your ass, from here to Pakistan.
#  Oh...
#  You better not breathe, you better not move.
#  You're better off dead, I'm tellin' you, dude.
#  Santa Claus is gunning you down!"
sub hook { shift->plugins->on(@_) }

# DEPRECATED in Leaf Fluttering In Wind!
sub on_process {
  my ($self, $cb) = @_;
  return $self->{on_process} ||= sub { shift->dispatch(@_) }
    unless $cb;
  warn <<EOF;
Mojolicious->on_process is DEPRECATED in favor of the around_dispatch hook!
EOF
  $self->{on_process} = $cb;
  return $self;
}

sub plugin {
  my $self = shift;
  $self->plugins->register_plugin(shift, $self, @_);
}

sub start {
  my $class = shift;

  # Executable
  $ENV{MOJO_EXE} ||= (caller)[1];

  # We are the application
  $ENV{MOJO_APP} = ref $class ? $class : $class->new;

  # Start!
  Mojolicious::Commands->start(@_);
}

sub startup { }

1;
__END__

=head1 NAME

Mojolicious - Real-time web framework

=head1 SYNOPSIS

  # Application
  package MyApp;
  use Mojo::Base 'Mojolicious';

  # Route
  sub startup {
    my $self = shift;
    $self->routes->get('/hello')->to('foo#hello');
  }

  # Controller
  package MyApp::Foo;
  use Mojo::Base 'Mojolicious::Controller';

  # Action
  sub hello {
    my $self = shift;
    $self->render_text('Hello World!');
  }

=head1 DESCRIPTION

Take a look at our excellent documentation in L<Mojolicious::Guides>!

=head1 ATTRIBUTES

L<Mojolicious> inherits all attributes from L<Mojo> and implements the
following new ones.

=head2 C<controller_class>

  my $class = $app->controller_class;
  $app      = $app->controller_class('Mojolicious::Controller');

Class to be used for the default controller, defaults to
L<Mojolicious::Controller>.

=head2 C<mode>

  my $mode = $app->mode;
  $app     = $app->mode('production');

The operating mode for your application, defaults to the value of the
C<MOJO_MODE> environment variable or C<development>. You can also add per
mode logic to your application by defining methods named C<${mode}_mode> in
the application class, which will be called right before C<startup>.

  sub development_mode {
    my $self = shift;
    ...
  }

  sub production_mode {
    my $self = shift;
    ...
  }

Right before calling C<startup> and mode specific methods, L<Mojolicious>
will pick up the current mode, name the log file after it and raise the log
level from C<debug> to C<info> if it has a value other than C<development>.

=head2 C<plugins>

  my $plugins = $app->plugins;
  $app        = $app->plugins(Mojolicious::Plugins->new);

The plugin loader, defaults to a L<Mojolicious::Plugins> object. You can
usually leave this alone, see L<Mojolicious::Plugin> if you want to write a
plugin or the C<plugin> method below if you want to load a plugin.

=head2 C<renderer>

  my $renderer = $app->renderer;
  $app         = $app->renderer(Mojolicious::Renderer->new);

Used in your application to render content, defaults to a
L<Mojolicious::Renderer> object. The two main renderer plugins
L<Mojolicious::Plugin::EPRenderer> and L<Mojolicious::Plugin::EPLRenderer>
contain more information.

=head2 C<routes>

  my $routes = $app->routes;
  $app       = $app->routes(Mojolicious::Routes->new);

The routes dispatcher, defaults to a L<Mojolicious::Routes> object. You use
this in your startup method to define the url endpoints for your application.

  sub startup {
    my $self = shift;

    my $r = $self->routes;
    $r->route('/:controller/:action')->to('test#welcome');
  }

=head2 C<secret>

  my $secret = $app->secret;
  $app       = $app->secret('passw0rd');

A secret passphrase used for signed cookies and the like, defaults to the
application name which is not very secure, so you should change it!!! As long
as you are using the unsecure default there will be debug messages in the log
file reminding you to change your passphrase.

=head2 C<sessions>

  my $sessions = $app->sessions;
  $app         = $app->sessions(Mojolicious::Sessions->new);

Simple signed cookie based sessions, defaults to a L<Mojolicious::Sessions>
object. You can usually leave this alone, see
L<Mojolicious::Controller/"session"> for more information about working with
session data.

=head2 C<static>

  my $static = $app->static;
  $app       = $app->static(Mojolicious::Static->new);

For serving static assets from your C<public> directory, defaults to a
L<Mojolicious::Static> object.

=head2 C<types>

  my $types = $app->types;
  $app      = $app->types(Mojolicious::Types->new);

Responsible for connecting file extensions with MIME types, defaults to a
L<Mojolicious::Types> object.

  $app->types->type(twt => 'text/tweet');

=head1 METHODS

L<Mojolicious> inherits all methods from L<Mojo> and implements the following
new ones.

=head2 C<new>

  my $app = Mojolicious->new;

Construct a new L<Mojolicious> application. Will automatically detect your
home directory and set up logging based on your current operating mode. Also
sets up the renderer, static dispatcher and a default set of plugins.

=head2 C<build_tx>

  my $tx = $app->build_tx;

Transaction builder, defaults to building a L<Mojo::Transaction::HTTP>
object.

=head2 C<defaults>

  my $defaults = $app->defaults;
  my $foo      = $app->defaults('foo');
  $app         = $app->defaults({foo => 'bar'});
  $app         = $app->defaults(foo => 'bar');

Default values for the stash, assigned for every new request.

  $app->defaults->{foo} = 'bar';
  my $foo = $app->defaults->{foo};
  delete $app->defaults->{foo};

=head2 C<dispatch>

  $app->dispatch($c);

The heart of every Mojolicious application, calls the static and routes
dispatchers for every request and passes them a L<Mojolicious::Controller>
object.

=head2 C<handler>

  $tx = $app->handler($tx);

Sets up the default controller and calls process for every request.

=head2 C<helper>

  $app->helper(foo => sub {...});

Add a new helper that will be available as a method of the controller object
and the application object, as well as a function in C<ep> templates.

  # Helper
  $app->helper(add => sub { $_[1] + $_[2] });

  # Controller/Application
  my $result = $self->add(2, 3);

  # Template
  %= add 2, 3

=head2 C<hook>

  $app->hook(after_dispatch => sub {...});

Extend L<Mojolicious> with hooks.

  # Dispatchers will not run if there's already a response code defined
  $app->hook(before_dispatch => sub {
    my $c = shift;
    $c->render(text => 'Skipped dispatchers!')
      if $c->req->url->path->contains('/do_not_dispatch');
  });

These hooks are currently available and are emitted in the listed order:

=over 2

=item B<after_build_tx>

Emitted right after the transaction is built and before the HTTP request gets
parsed.

  $app->hook(after_build_tx => sub {
    my ($tx, $app) = @_;
  });

This is a very powerful hook and should not be used lightly, it makes some
rather advanced features such as upload progress bars possible, just note
that it will not work for embedded applications. (Passed the transaction and
application instances)

=item B<before_dispatch>

Emitted right before the static and routes dispatchers start their work.

  $app->hook(before_dispatch => sub {
    my $c = shift;
  });

Very useful for rewriting incoming requests and other preprocessing tasks.
(Passed the default controller instance)

=item B<after_static_dispatch>

Emitted in reverse order after the static dispatcher determined if a static
file should be served and before the routes dispatcher starts its work.

  $app->hook(after_static_dispatch => sub {
    my $c = shift;
  });

Mostly used for custom dispatchers and postprocessing static file responses.
(Passed the default controller instance)

=item B<after_dispatch>

Emitted in reverse order after a response has been rendered. Note that this
hook can trigger before C<after_static_dispatch> due to its dynamic nature.

  $app->hook(after_dispatch => sub {
    my $c = shift;
  });

Useful for all kinds of postprocessing tasks. (Passed the current controller
instance)

=item B<around_dispatch>

Emitted right before the C<before_dispatch> hook and wraps around the whole
dispatch process, so you have to manually forward to the next hook if you
want to continue the chain. Note that this hook is EXPERIMENTAL and might
change without warning!

  $app->hook(around_dispatch => sub {
    my ($next, $c) = @_;
    $next->();
  });

This is a very powerful hook and should not be used lightly, consider it the
sledgehammer in your toolbox. (Passed a closure leading to the next hook and
the current controller instance)

=back

=head2 C<plugin>

  $app->plugin('some_thing');
  $app->plugin('some_thing', foo => 23);
  $app->plugin('some_thing', {foo => 23});
  $app->plugin('SomeThing');
  $app->plugin('SomeThing', foo => 23);
  $app->plugin('SomeThing', {foo => 23});
  $app->plugin('MyApp::Plugin::SomeThing');
  $app->plugin('MyApp::Plugin::SomeThing', foo => 23);
  $app->plugin('MyApp::Plugin::SomeThing', {foo => 23});

Load a plugin with L<Mojolicious::Plugins/"register_plugin">.

These plugins are included in the L<Mojolicious> distribution as examples:

=over 2

=item L<Mojolicious::Plugin::Charset>

Change the application charset.

=item L<Mojolicious::Plugin::Config>

Perl-ish configuration files.

=item L<Mojolicious::Plugin::DefaultHelpers>

General purpose helper collection.

=item L<Mojolicious::Plugin::EPLRenderer>

Renderer for plain embedded Perl templates.

=item L<Mojolicious::Plugin::EPRenderer>

Renderer for more sophisiticated embedded Perl templates.

=item L<Mojolicious::Plugin::HeaderCondition>

Route condition for all kinds of headers.

=item L<Mojolicious::Plugin::I18N>

Internationalization helpers.

=item L<Mojolicious::Plugin::JSONConfig>

JSON configuration files.

=item L<Mojolicious::Plugin::Mount>

Mount whole L<Mojolicious> applications.

=item L<Mojolicious::Plugin::PODRenderer>

Renderer for POD files and documentation browser.

=item L<Mojolicious::Plugin::PoweredBy>

Add an C<X-Powered-By> header to outgoing responses.

=item L<Mojolicious::Plugin::RequestTimer>

Log timing information.

=item L<Mojolicious::Plugin::TagHelpers>

Template specific helper collection.

=back

=head2 C<start>

  Mojolicious->start;
  Mojolicious->start('daemon');

Start the L<Mojolicious::Commands> command line interface for your
application.

=head2 C<startup>

  $app->startup;

This is your main hook into the application, it will be called at application
startup.

  sub startup {
    my $self = shift;
  }

=head1 HELPERS

In addition to the attributes and methods above you can also call helpers on
instances of L<Mojolicious>. This includes all helpers from
L<Mojolicious::Plugin::DefaultHelpers> and
L<Mojolicious::Plugin::TagHelpers>. Note that application helpers are always
called with a new C<controller_class> instance, so they can't depend on or
change controller state, which includes request, response and stash.

  $app->log->debug($app->dumper({foo => 'bar'}));

=head1 SUPPORT

=head2 Web

L<http://mojolicio.us>

=head2 IRC

C<#mojo> on C<irc.perl.org>

=head2 Mailing-List

L<http://groups.google.com/group/mojolicious>

=head1 DEVELOPMENT

=head2 Repository

L<http://github.com/kraih/mojo>

=head1 BUNDLED FILES

L<Mojolicious> ships with a few popular static files bundled in the C<public>
directory.

=head2 Mojolicious Artwork

  Copyright (C) 2010-2012, Sebastian Riedel.

Licensed under the CC-SA License, Version 3.0
L<http://creativecommons.org/licenses/by-sa/3.0>.

=head2 jQuery

  Copyright 2011, John Resig.

Licensed under the MIT License, L<http://creativecommons.org/licenses/MIT>.

=head2 prettify.js

  Copyright (C) 2006, Google Inc.

Licensed under the Apache License, Version 2.0
L<http://www.apache.org/licenses/LICENSE-2.0>.

=head1 CODE NAMES

Every major release of L<Mojolicious> has a code name, these are the ones
that have been used in the past.

2.0, C<Leaf Fluttering In Wind> (u1F343)

1.4, C<Smiling Face With Sunglasses> (u1F60E)

1.3, C<Tropical Drink> (u1F379)

1.1, C<Smiling Cat Face With Heart-Shaped Eyes> (u1F63B)

1.0, C<Snowflake> (u2744)

0.999930, C<Hot Beverage> (u2615)

0.999927, C<Comet> (u2604)

0.999920, C<Snowman> (u2603)

=head1 AUTHOR

Sebastian Riedel, C<sri@cpan.org>

=head1 CORE DEVELOPERS

Current members of the core team in alphabetical order:

=over 4

Abhijit Menon-Sen, C<ams@cpan.org>

Glen Hinkle, C<tempire@cpan.org>

Marcus Ramberg, C<mramberg@cpan.org>

=back

=head1 CREDITS

In alphabetical order:

=over 2

Adam Kennedy

Adriano Ferreira

Al Newkirk

Alex Salimon

Alexey Likhatskiy

Anatoly Sharifulin

Andre Vieth

Andreas Jaekel

Andrew Fresh

Andreas Koenig

Andy Grundman

Aristotle Pagaltzis

Ashley Dev

Ask Bjoern Hansen

Audrey Tang

Ben van Staveren

Benjamin Erhart

Bernhard Graf

Breno G. de Oliveira

Brian Duggan

Burak Gursoy

Ch Lamprecht

Charlie Brady

Chas. J. Owens IV

Christian Hansen

chromatic

Curt Tilmes

Daniel Kimsey

Danijel Tasov

David Davis

Diego Kuperman

Dmitriy Shalashov

Dmitry Konstantinov

Douglas Christopher Wilson

Eugene Toropov

Gisle Aas

Graham Barr

Henry Tang

Hideki Yamamura

James Duncan

Jan Jona Javorsek

Jaroslav Muhin

Jesse Vincent

Johannes Plunien

John Kingsley

Jonathan Yu

Kazuhiro Shibuya

Kevin Old

KITAMURA Akatsuki

Lars Balker Rasmussen

Leon Brocard

Magnus Holm

Maik Fischer

Mark Stosberg

Matthew Lineen

Maksym Komar

Maxim Vuets

Michael Harris

Mirko Westermeier

Mons Anderson

Moritz Lenz

Neil Watkiss

Nils Diewald

Oleg Zhelo

Pascal Gaudette

Paul Tomlin

Pedro Melo

Peter Edwards

Pierre-Yves Ritschard

Quentin Carbonneaux

Rafal Pocztarski

Randal Schwartz

Robert Hicks

Robin Lee

Roland Lammel

Ryan Jendoubi

Sascha Kiefer

Sergey Zasenko

Simon Bertrang

Simone Tampieri

Shu Cho

Skye Shaw

Stanis Trendelenburg

Tatsuhiko Miyagawa

Terrence Brannon

The Perl Foundation

Tomas Znamenacek

Ulrich Habel

Ulrich Kautz

Uwe Voelker

Viacheslav Tykhanovskyi

Victor Engmark

Viliam Pucik

Wes Cravens

Yaroslav Korshak

Yuki Kimoto

Zak B. Elep

=back

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2008-2012, Sebastian Riedel.

This program is free software, you can redistribute it and/or modify it under
the terms of the Artistic License version 2.0.

=cut
