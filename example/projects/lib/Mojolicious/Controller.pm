package Mojolicious::Controller;
use Mojo::Base -base;

use Mojo::ByteStream;
use Mojo::Cookie::Response;
use Mojo::Exception;
use Mojo::Home;
use Mojo::Transaction::HTTP;
use Mojo::URL;
use Mojo::Util;
use Mojolicious;
use Mojolicious::Routes::Match;

require Carp;
require Scalar::Util;

# "Scalpel... blood bucket... priest."
has app => sub { Mojolicious->new };
has match => sub {
  Mojolicious::Routes::Match->new(get => '/')->root(shift->app->routes);
};
has tx => sub { Mojo::Transaction::HTTP->new };

# Bundled files
our $H = Mojo::Home->new;
$H->parse($H->parse($H->mojo_lib_dir)->rel_dir('Mojolicious/templates'));
our $EXCEPTION     = $H->slurp_rel_file('exception.html.ep');
our $DEV_EXCEPTION = $H->slurp_rel_file('exception.development.html.ep');
our $MOJOBAR       = $H->slurp_rel_file('mojobar.html.ep');
our $NOT_FOUND     = $H->slurp_rel_file('not_found.html.ep');
our $DEV_NOT_FOUND = $H->slurp_rel_file('not_found.development.html.ep');

# Reserved stash values
my @RESERVED = (
  qw/action app cb class controller data exception extends format handler/,
  qw/json layout method namespace partial path status template text/
);
my %RESERVED;
$RESERVED{$_}++ for @RESERVED;

# "Is all the work done by the children?
#  No, not the whipping."
sub AUTOLOAD {
  my $self = shift;

  # Method
  my ($package, $method) = our $AUTOLOAD =~ /^([\w\:]+)\:\:(\w+)$/;
  Carp::croak(qq/Undefined subroutine &${package}::$method called/)
    unless Scalar::Util::blessed($self) && $self->isa(__PACKAGE__);

  # Call helper
  Carp::croak(qq/Can't locate object method "$method" via package "$package"/)
    unless my $helper = $self->app->renderer->helpers->{$method};
  return $self->$helper(@_);
}

sub DESTROY { }

# "For the last time, I don't like lilacs!
#  Your first wife was the one who liked lilacs!
#  She also liked to shut up!"
sub cookie {
  my ($self, $name, $value, $options) = @_;
  return unless $name;

  # Response cookie
  if (defined $value) {

    # Cookie too big
    $self->app->log->error(qq/Cookie "$name" is bigger than 4096 bytes./)
      if length $value > 4096;

    # Create new cookie
    my $cookie = Mojo::Cookie::Response->new(
      name  => $name,
      value => $value,
      %{$options || {}}
    );
    $self->res->cookies($cookie);
    return $self;
  }

  # Request cookie
  unless (wantarray) {
    return unless my $cookie = $self->req->cookie($name);
    return $cookie->value;
  }

  # Request cookies
  my @cookies = $self->req->cookie($name);
  return map { $_->value } @cookies;
}

# "Something's wrong, she's not responding to my poking stick."
sub finish {
  my ($self, $chunk) = @_;

  # WebSocket
  my $tx = $self->tx;
  return $tx->finish if $tx->is_websocket;

  # Chunked stream
  if ($tx->res->is_chunked) {
    $self->write_chunk($chunk) if defined $chunk;
    return $self->write_chunk('');
  }

  # Normal stream
  $self->write($chunk) if defined $chunk;
  $self->write('');
}

# "You two make me ashamed to call myself an idiot."
sub flash {
  my $self = shift;

  # Get
  my $session = $self->stash->{'mojo.session'};
  if ($_[0] && !defined $_[1] && !ref $_[0]) {
    return unless $session && ref $session eq 'HASH';
    return unless my $flash = $session->{flash};
    return unless ref $flash eq 'HASH';
    return $flash->{$_[0]};
  }

  # Initialize
  $session = $self->session;
  my $flash = $session->{new_flash};
  $flash = {} unless $flash && ref $flash eq 'HASH';
  $session->{new_flash} = $flash;

  # Set
  my $values = @_ > 1 ? {@_} : $_[0];
  $session->{new_flash} = {%$flash, %$values};

  return $self;
}

# "My parents may be evil, but at least they're stupid."
sub on {
  my ($self, $name, $cb) = @_;
  my $tx = $self->tx;
  $self->rendered(101) if $tx->is_websocket;
  $tx->on($name => sub { shift and $self->$cb(@_) });
}

# DEPRECATED in Leaf Fluttering In Wind!
sub on_finish {
  warn <<EOF;
Mojolicious::Controller->on_finish is DEPRECATED in favor of
Mojolicious::Controller->on!
EOF
  shift->on(finish => @_);
}

# "Just make a simple cake. And this time, if someone's going to jump out of
#  it make sure to put them in *after* you cook it."
sub param {
  my ($self, $name) = (shift, shift);

  # List
  my $p = $self->stash->{'mojo.captures'} || {};
  unless (defined $name) {
    my %seen;
    my @keys = grep { !$seen{$_}++ } $self->req->param;
    push @keys, grep { !$RESERVED{$_} && !$seen{$_}++ } keys %$p;
    return sort @keys;
  }

  # Override value
  if (@_) {
    $p->{$name} = $_[0];
    return $self;
  }

  # Captured unreserved value
  return $p->{$name} if !$RESERVED{$name} && exists $p->{$name};

  # Param value
  return $self->req->param($name);
}

# "Is there an app for kissing my shiny metal ass?
#  Several!
#  Oooh!"
sub redirect_to {
  my $self = shift;

  # Don't override 3xx status
  my $res     = $self->res;
  my $headers = $res->headers;
  $headers->location($self->url_for(@_)->to_abs);
  $self->rendered($res->is_status_class(300) ? undef : 302);

  return $self;
}

# "Mamma Mia! The cruel meatball of war has rolled onto our laps and ruined
#  our white pants of peace!"
sub render {
  my $self = shift;

  # Template may be first argument
  my $template;
  $template = shift if @_ % 2 && !ref $_[0];
  my $args = ref $_[0] ? $_[0] : {@_};

  # Template
  my $stash = $self->stash;
  $args->{template} = $template if $template;
  unless ($stash->{template} || $args->{template}) {

    # Default template
    my $controller = $args->{controller} || $stash->{controller};
    my $action     = $args->{action}     || $stash->{action};

    # Normal default template
    if ($controller && $action) {
      $self->stash->{template} = join('/', split(/-/, $controller), $action);
    }

    # Try the route name if we don't have controller and action
    elsif ($self->match->endpoint) {
      $self->stash->{template} = $self->match->endpoint->name;
    }
  }

  # Render
  my ($output, $type) = $self->app->renderer->render($self, $args);
  return unless defined $output;
  return Mojo::ByteStream->new($output) if $args->{partial};

  # Prepare response
  my $res = $self->res;
  $res->body($output) unless $res->body;
  my $headers = $res->headers;
  $headers->content_type($type) unless $headers->content_type;
  $self->rendered($stash->{status});

  return 1;
}

# "She's built like a steakhouse, but she handles like a bistro!"
sub render_content {
  my $self    = shift;
  my $name    = shift || 'content';
  my $content = pop;

  # Set
  my $stash = $self->stash;
  my $c = $stash->{'mojo.content'} ||= {};
  if (defined $content) {

    # Reset with multiple values
    if (@_) {
      $c->{$name} =
        join('', map({ref $_ eq 'CODE' ? $_->() : $_} @_, $content));
    }

    # First come
    else { $c->{$name} ||= ref $content eq 'CODE' ? $content->() : $content }
  }

  # Get
  $content = $c->{$name} // '';
  return Mojo::ByteStream->new("$content");
}

sub render_data { shift->render(data => shift, @_) }

# "The path to robot hell is paved with human flesh.
#  Neat."
sub render_exception {
  my ($self, $e) = @_;
  $e = Mojo::Exception->new($e);
  my $app = $self->app;
  $app->log->error($e);

  # Recursion
  return if $self->stash->{'mojo.exception'};

  # Filtered stash snapshot
  my $snapshot = {};
  my $stash    = $self->stash;
  for my $key (keys %$stash) {
    next if $key =~ /^mojo\./;
    next unless defined(my $value = $stash->{$key});
    $snapshot->{$key} = $value;
  }

  # Render with fallbacks
  my $mode    = $app->mode;
  my $options = {
    template         => "exception.$mode",
    format           => $stash->{format} || 'html',
    handler          => undef,
    status           => 500,
    snapshot         => $snapshot,
    exception        => $e,
    'mojo.exception' => 1
  };
  my $inline = $mode eq 'development' ? $DEV_EXCEPTION : $EXCEPTION;
  return if $self->_render_fallbacks($options, 'exception', $inline);
  $options->{format} = 'html';
  $self->_render_fallbacks($options, 'exception', $inline);
}

# "If you hate intolerance and being punched in the face by me,
#  please support Proposition Infinity."
sub render_json {
  my ($self, $json) = (shift, shift);
  my $args = ref $_[0] ? $_[0] : {@_};
  $args->{json} = $json;
  return $self->render($args);
}

sub render_later { shift->stash->{'mojo.rendered'}++ }

# "Excuse me, sir, you're snowboarding off the trail.
#  Lick my frozen metal ass."
sub render_not_found {
  my ($self, $resource) = @_;
  $self->app->log->debug(qq/Resource "$resource" not found./) if $resource;

  # Recursion
  my $stash = $self->stash;
  return if $stash->{'mojo.exception'};
  return if $stash->{'mojo.not_found'};

  # Render with fallbacks
  my $mode    = $self->app->mode;
  my $options = {
    template         => "not_found.$mode",
    format           => $stash->{format} || 'html',
    status           => 404,
    'mojo.not_found' => 1
  };
  my $inline = $mode eq 'development' ? $DEV_NOT_FOUND : $NOT_FOUND;
  return if $self->_render_fallbacks($options, 'not_found', $inline);
  $options->{format} = 'html';
  $self->_render_fallbacks($options, 'not_found', $inline);
}

# "You called my thesis a fat sack of barf, and then you stole it?
#  Welcome to academia."
sub render_partial {
  my $self     = shift;
  my $template = @_ % 2 ? shift : undef;
  my $args     = {@_, partial => 1};
  $args->{template} = $template if defined $template;
  return $self->render($args);
}

sub render_static {
  my ($self, $file) = @_;

  my $app = $self->app;
  unless ($app->static->serve($self, $file)) {
    $app->log->debug(
      qq/Static file "$file" not found, public directory missing?/);
    return;
  }
  $self->rendered;

  return 1;
}

sub render_text { shift->render(text => shift, @_) }

sub rendered {
  my ($self, $status) = @_;

  # Disable auto rendering
  $self->render_later;

  # Make sure we have a status
  my $res = $self->res;
  $res->code($status) if $status;

  # Finish transaction
  my $stash = $self->stash;
  unless ($stash->{'mojo.finished'}++) {
    $res->code(200) unless $res->code;
    my $app = $self->app;
    $app->plugins->emit_hook_reverse(after_dispatch => $self);
    $app->sessions->store($self);
  }
  $self->tx->resume;

  return $self;
}

# "A three month calendar? What is this, Mercury?"
sub req { shift->tx->req }
sub res { shift->tx->res }

sub respond_to {
  my $self = shift;
  my $args = ref $_[0] ? $_[0] : {@_};

  # Detect formats
  my @formats;
  my $app = $self->app;
  push @formats, @{$app->types->detect($self->req->headers->accept)};
  my $stash = $self->stash;
  unless (@formats) {
    if (my $format = $stash->{format} || $self->req->param('format')) {
      push @formats, $format;
    }
    else { push @formats, $app->renderer->default_format }
  }

  # Find target
  my $target;
  for my $format (@formats) {
    if ($target = $args->{$format}) {
      $stash->{format} = $format;
      last;
    }
  }

  # Fallback
  unless ($target) {
    return $self->rendered(204) unless $target = $args->{any};
    delete $stash->{format};
  }

  # Dispatch
  ref $target eq 'CODE' ? $target->($self) : $self->render($target);
}

sub send_message {
  my ($self, $message, $cb) = @_;

  my $tx = $self->tx;
  Carp::croak('No WebSocket connection to send message to')
    unless $tx->is_websocket;
  $tx->send_message($message, sub { shift and $self->$cb(@_) if $cb });
  $self->rendered(101);

  return $self;
}

# "Why am I sticky and naked? Did I miss something fun?"
sub session {
  my $self = shift;

  # Get
  my $stash   = $self->stash;
  my $session = $stash->{'mojo.session'};
  if ($_[0] && !defined $_[1] && !ref $_[0]) {
    return unless $session && ref $session eq 'HASH';
    return $session->{$_[0]};
  }

  # Hash
  $session = {} unless $session && ref $session eq 'HASH';
  $stash->{'mojo.session'} = $session;
  return $session unless @_;

  # Set
  my $values = @_ > 1 ? {@_} : $_[0];
  $stash->{'mojo.session'} = {%$session, %$values};

  return $self;
}

sub signed_cookie {
  my ($self, $name, $value, $options) = @_;
  return unless $name;

  # Response cookie
  my $secret = $self->app->secret;
  if (defined $value) {

    # Sign value
    my $sig = Mojo::Util::hmac_md5_sum $value, $secret;
    $value = $value .= "--$sig";

    # Create cookie
    my $cookie = $self->cookie($name, $value, $options);
    return $cookie;
  }

  # Request cookies
  my @values = $self->cookie($name);
  my @results;
  for my $value (@values) {

    # Check signature
    if ($value =~ s/\-\-([^\-]+)$//) {
      my $sig = $1;
      my $check = Mojo::Util::hmac_md5_sum $value, $secret;

      # Verified
      if (Mojo::Util::secure_compare $sig, $check) { push @results, $value }

      # Bad cookie
      else {
        $self->app->log->debug(
          qq/Bad signed cookie "$name", possible hacking attempt./);
      }
    }

    # Not signed
    else { $self->app->log->debug(qq/Cookie "$name" not signed./) }
  }

  return wantarray ? @results : $results[0];
}

# "All this knowledge is giving me a raging brainer."
sub stash {
  my $self = shift;

  # Hash
  $self->{stash} ||= {};
  return $self->{stash} unless @_;

  # Get
  return $self->{stash}->{$_[0]} unless @_ > 1 || ref $_[0];

  # Set
  my $values = ref $_[0] ? $_[0] : {@_};
  for my $key (keys %$values) {
    $self->app->log->debug(qq/Careful, "$key" is a reserved stash value./)
      if $RESERVED{$key};
    $self->{stash}->{$key} = $values->{$key};
  }

  return $self;
}

sub ua { shift->app->ua }

sub url_for {
  my $self = shift;
  my $target = shift || '';

  # Absolute URL
  return Mojo::URL->new($target) if $target =~ m#^\w+\://#;

  # Base
  my $url = Mojo::URL->new;
  my $req = $self->req;
  $url->base($req->url->base->clone);
  my $base = $url->base;
  $base->userinfo(undef);

  # Relative URL
  my $path = $url->path;
  if ($target =~ m#^/#) {
    if (my $e = $self->stash->{path}) {
      my $real = Mojo::Util::url_unescape($req->url->path->to_abs_string);
      $real = Mojo::Util::decode('UTF-8', $real) // $real;
      $real =~ s|/?$e$|$target|;
      $target = $real;
    }
    $url->parse($target);
  }

  # Route
  else {
    my ($p, $ws) = $self->match->path_for($target, @_);
    $path->parse($p) if $p;

    # Fix trailing slash
    $path->trailing_slash(1)
      if (!$target || $target eq 'current')
      && $req->url->path->trailing_slash;

    # Fix scheme for WebSockets
    $base->scheme(($base->scheme || '') eq 'https' ? 'wss' : 'ws') if $ws;
  }

  # Make path absolute
  my $base_path = $base->path;
  unshift @{$path->parts}, @{$base_path->parts};
  $base_path->parts([])->trailing_slash(0);

  return $url;
}

sub write {
  my ($self, $chunk, $cb) = @_;

  if (ref $chunk && ref $chunk eq 'CODE') {
    $cb    = $chunk;
    $chunk = undef;
  }
  $self->res->write($chunk, sub { shift and $self->$cb(@_) if $cb });
  $self->rendered;

  return $self;
}

sub write_chunk {
  my ($self, $chunk, $cb) = @_;

  if (ref $chunk && ref $chunk eq 'CODE') {
    $cb    = $chunk;
    $chunk = undef;
  }
  $self->res->write_chunk($chunk, sub { shift and $self->$cb(@_) if $cb });
  $self->rendered;

  return $self;
}

sub _render_fallbacks {
  my ($self, $options, $template, $inline) = @_;

  # Mode specific template
  unless ($self->render($options)) {

    # Template
    $options->{template} = $template;
    unless ($self->render($options)) {

      # Inline template
      my $stash = $self->stash;
      return unless $stash->{format} eq 'html';
      delete $stash->{layout};
      delete $stash->{extends};
      delete $options->{template};
      $options->{inline}  = $inline;
      $options->{handler} = 'ep';
      return $self->render($options);
    }
  }
}

1;
__END__

=head1 NAME

Mojolicious::Controller - Controller base class

=head1 SYNOPSIS

  use Mojo::Base 'Mojolicious::Controller';

=head1 DESCRIPTION

L<Mojolicious::Controller> is the base class for your L<Mojolicious>
controllers. It is also the default controller class for L<Mojolicious>
unless you set C<controller_class> in your application.

=head1 ATTRIBUTES

L<Mojolicious::Controller> inherits all attributes from L<Mojo::Base> and
implements the following new ones.

=head2 C<app>

  my $app = $c->app;
  $c      = $c->app(Mojolicious->new);

A reference back to the L<Mojolicious> application that dispatched to this
controller, defaults to a L<Mojolicious> object.

  $c->app->log->debug('Hello Mojo!');

=head2 C<match>

  my $m = $c->match;
  $c    = $c->match(Mojolicious::Routes::Match->new);

Routes dispatcher results for the current request, defaults to a
L<Mojolicious::Routes::Match> object.

  my $name = $c->match->endpoint->name;

=head2 C<tx>

  my $tx = $c->tx;
  $c     = $c->tx(Mojo::Transaction::HTTP->new);

The transaction that is currently being processed, usually a
L<Mojo::Transaction::HTTP> or L<Mojo::Transaction::WebSocket> object.

  my $address = $c->tx->remote_address;

=head1 METHODS

L<Mojolicious::Controller> inherits all methods from L<Mojo::Base> and
implements the following new ones.

=head2 C<cookie>

  $c         = $c->cookie(foo => 'bar');
  $c         = $c->cookie(foo => 'bar', {path => '/'});
  my $value  = $c->cookie('foo');
  my @values = $c->cookie('foo');

Access request cookie values and create new response cookies.

  my $foo = $c->cookie('foo')->value;

=head2 C<finish>

  $c->finish;
  $c->finish('Bye!');

Gracefully end WebSocket connection or long poll stream.

=head2 C<flash>

  my $foo   = $c->flash('foo');
  $c        = $c->flash({foo => 'bar'});
  $c        = $c->flash(foo => 'bar');

Data storage persistent only for the next request, stored in the C<session>.

  $c->flash(message => 'User created successfully!');
  $c->redirect_to('show_user', id => 23);

=head2 C<on>

  my $cb = $c->on(finish => sub {...});

Subscribe to events of C<tx>, which is usually a L<Mojo::Transaction::HTTP>
or L<Mojo::Transaction::WebSocket> object.

  # Emitted when the transaction has been finished
  $c->on(finish => sub {
    my $c = shift;
    say 'We are done!';
  });

  # Emitted when new WebSocket messages arrive
  $c->on(message => sub {
    my ($c, $message) = @_;
    say "Message: $message";
  });

=head2 C<param>

  my @names = $c->param;
  my $foo   = $c->param('foo');
  my @foo   = $c->param('foo');
  $c        = $c->param(foo => 'ba;r');

Access GET/POST parameters and route captures that are not reserved stash
values.

  # Only GET parameters
  my $foo = $c->req->url->query->param('foo');

  # Only GET and POST parameters
  my $foo = $c->req->param('foo');

=head2 C<redirect_to>

  $c = $c->redirect_to('named');
  $c = $c->redirect_to('named', foo => 'bar');
  $c = $c->redirect_to('/path');
  $c = $c->redirect_to('http://127.0.0.1/foo/bar');

Prepare a C<302> redirect response, takes the exact same arguments as
C<url_for>.

  # Conditional redirect
  return $c->redirect_to('login') unless $c->session('user');

  # Moved permanently
  $c->res->code(301);
  $c->redirect_to('some_route');

=head2 C<render>

  $c->render;
  $c->render(controller => 'foo', action => 'bar');
  $c->render({controller => 'foo', action => 'bar'});
  $c->render(text => 'Hello!');
  $c->render(template => 'index');
  $c->render(template => 'foo/index');
  $c->render(template => 'index', format => 'html', handler => 'epl');
  $c->render(handler => 'something');
  $c->render('foo/bar');
  $c->render('foo/bar', format => 'html');

This is a wrapper around L<Mojolicious::Renderer> exposing pretty much all
functionality provided by it. It will set a default template to use based on
the controller and action name or fall back to the route name. You can call
it with a hash of options which can be preceded by an optional template name.

=head2 C<render_content>

  my $output = $c->render_content;
  my $output = $c->render_content('header');
  my $output = $c->render_content(header => 'Hello world!');
  my $output = $c->render_content(header => sub { 'Hello world!' });

Contains partial rendered templates, used for the renderers C<layout> and
C<extends> features.

=head2 C<render_data>

  $c->render_data($bytes);
  $c->render_data($bytes, format => 'png');

Render the given content as raw bytes, similar to C<render_text> but data
will not be encoded.

=head2 C<render_exception>

  $c->render_exception('Oops!');
  $c->render_exception(Mojo::Exception->new('Oops!'));

Render the exception template C<exception.$mode.$format.*> or
C<exception.$format.*> and set the response status code to C<500>.

=head2 C<render_json>

  $c->render_json({foo => 'bar'});
  $c->render_json([1, 2, -3], status => 201);

Render a data structure as JSON.

=head2 C<render_later>

  $c->render_later;

Disable automatic rendering, especially for long polling this can be quite
useful.

  $c->render_later;
  Mojo::IOLoop->timer(2 => sub {
    $c->render(text => 'Delayed by 2 seconds!');
  });

=head2 C<render_not_found>

  $c->render_not_found;
  $c->render_not_found('some_resource');

Render the not found template C<not_found.$mode.$format.*> or
C<not_found.$format.*> and set the response status code to C<404>.

=head2 C<render_partial>

  my $output = $c->render_partial('menubar');
  my $output = $c->render_partial('menubar', format => 'txt');

Same as C<render> but returns the rendered result.

=head2 C<render_static>

  my $success = $c->render_static('images/logo.png');
  my $success = $c->render_static('../lib/MyApp.pm');

Render a static file using L<Mojolicious::Static> relative to the
C<public> directory of your application.

=head2 C<render_text>

  $c->render_text('Hello World!');
  $c->render_text('Hello World', layout => 'green');

Render the given content as Perl characters, which will be encoded to bytes.
See C<render_data> for an alternative without encoding. Note that this does
not change the content type of the response, which is
C<text/html;charset=UTF-8> by default.

  $c->render_text('Hello World!', format => 'txt');

=head2 C<rendered>

  $c = $c->rendered;
  $c = $c->rendered(302);

Finalize response and run C<after_dispatch> plugin hook.

  # Stream content directly from file
  $c->res->content->asset(Mojo::Asset::File->new(path => '/etc/passwd'));
  $c->res->headers->content_type('text/plain');
  $c->rendered(200);

=head2 C<req>

  my $req = $c->req;

Alias for C<$c-E<gt>tx-E<gt>req>. Usually refers to a
L<Mojo::Message::Request> object.

  $c->render_json({url => $c->req->url->to_abs->to_string});

=head2 C<res>

  my $res = $c->res;

Alias for C<$c-E<gt>tx-E<gt>res>. Usually refers to a
L<Mojo::Message::Response> object.

  $c->res->headers->content_disposition('attachment; filename=foo.png;');

=head2 C<respond_to>

  $c->respond_to(
    json => sub {...},
    xml  => {text => 'hello!'},
    any  => sub {...}
  );

Automatically select best possible representation for resource from C<Accept>
request header, C<format> stash value or C<format> GET/POST parameter,
defaults to rendering an empty C<204> response.

  $c->respond_to(
    json => sub { $c->render_json({just => 'works'}) },
    xml  => {text => '<just>works</just>'},
    any  => {data => '', status => 204}
  );

=head2 C<send_message>

  $c = $c->send_message([binary => $bytes]);
  $c = $c->send_message([text   => $bytes]);
  $c = $c->send_message('Hi there!');
  $c = $c->send_message('Hi there!', sub {...});

Send a message non-blocking via WebSocket, the optional drain callback will
be invoked once all data has been written. Note that this method is
EXPERIMENTAL and might change without warning!

  # Send JSON object as text frame
  $c->send_message([text => Mojo::JSON->new->encode({hello => 'world'})]);

=head2 C<session>

  my $session = $c->session;
  my $foo     = $c->session('foo');
  $c          = $c->session({foo => 'bar'});
  $c          = $c->session(foo => 'bar');

Persistent data storage, stored C<JSON> serialized in a signed cookie. Note
that cookies are generally limited to 4096 bytes of data.

  $c->session->{foo} = 'bar';
  my $foo = $c->session->{foo};
  delete $c->session->{foo};

=head2 C<signed_cookie>

  $c         = $c->signed_cookie(foo => 'bar');
  $c         = $c->signed_cookie(foo => 'bar', {path => '/'});
  my $value  = $c->signed_cookie('foo');
  my @values = $c->signed_cookie('foo');

Access signed request cookie values and create new signed response cookies.
Cookies failing signature verification will be automatically discarded.

=head2 C<stash>

  my $stash = $c->stash;
  my $foo   = $c->stash('foo');
  $c        = $c->stash({foo => 'bar'});
  $c        = $c->stash(foo => 'bar');

Non persistent data storage and exchange, application wide default values can
be set with L<Mojolicious/"defaults">.

  $c->stash->{foo} = 'bar';
  my $foo = $c->stash->{foo};
  delete $c->stash->{foo};

=head2 C<ua>

  my $ua = $c->ua;

Alias for C<$c-E<gt>app-E<gt>ua>. Usually refers to a L<Mojo::UserAgent>
object.

  # Blocking
  my $tx = $c->ua->get('http://mojolicio.us');
  my $tx = $c->ua->post_form('http://kraih.com/login' => {user => 'mojo'});

  # Non-blocking
  $c->ua->get('http://mojolicio.us' => sub {
    my ($ua, $tx) = @_;
    $c->render_data($tx->res->body);
  });

  # Parallel non-blocking
  my $delay = Mojo::IOLoop->delay(sub {
    my ($delay, @titles) = @_;
    $c->render_json(\@titles);
  });
  for my $url ('http://mojolicio.us', 'https://metacpan.org') {
    $delay->begin;
    $c->ua->get($url => sub {
      my ($ua, $tx) = @_;
      $delay->end($tx->res->dom->html->head->title->text);
    });
  }

=head2 C<url_for>

  my $url = $c->url_for;
  my $url = $c->url_for(name => 'sebastian');
  my $url = $c->url_for('test', name => 'sebastian');
  my $url = $c->url_for('/perldoc');
  my $url = $c->url_for('http://mojolicio.us/perldoc');

Generate a portable L<Mojo::URL> object with base for a route, path or URL.

  # "/perldoc?foo=bar" if application is deployed under "/"
  $c->url_for('/perldoc')->query(foo => 'bar');

  # "/myapp/perldoc?foo=bar" if application is deployed under "/myapp"
  $c->url_for('/perldoc')->query(foo => 'bar');

You can also use L<Mojolicious::Plugin::DefaultHelpers/"url_with"> to inherit
query parameters from the current request.

  # "/list?q=mojo&page=2" if current request was for "/list?q=mojo&page=1"
  $c->url_with->query([page => 2]);

=head2 C<write>

  $c->write;
  $c->write('Hello!');
  $c->write(sub {...});
  $c->write('Hello!', sub {...});

Write dynamic content non-blocking, the optional drain callback will be
invoked once all data has been written.

  # Keep connection alive (with Content-Length header)
  $c->res->headers->content_length(6);
  $c->write('Hel', sub {
    my $c = shift;
    $c->write('lo!')
  });

  # Close connection when finished (without Content-Length header)
  $c->write('Hel', sub {
    my $c = shift;
    $c->write('lo!', sub {
      my $c = shift;
      $c->finish;
    });
  });

For Comet (C<long polling>) you might also want to increase the connection
timeout, which usually defaults to C<15> seconds.

  # Increase timeout for current connection to 300 seconds
  Mojo::IOLoop->stream($c->tx->connection)->timeout(300);

=head2 C<write_chunk>

  $c->write_chunk;
  $c->write_chunk('Hello!');
  $c->write_chunk(sub {...});
  $c->write_chunk('Hello!', sub {...});

Write dynamic content non-blocking with C<chunked> transfer encoding, the
optional drain callback will be invoked once all data has been written.

  $c->write_chunk('He', sub {
    my $c = shift;
    $c->write_chunk('ll', sub {
      my $c = shift;
      $c->finish('o!');
    });
  });

You can call C<finish> at any time to end the stream.

  2
  He
  2
  ll
  2
  o!
  0

=head1 HELPERS

In addition to the attributes and methods above you can also call helpers on
instances of L<Mojolicious::Controller>. This includes all helpers from
L<Mojolicious::Plugin::DefaultHelpers> and
L<Mojolicious::Plugin::TagHelpers>.

  $c->layout('green');
  $c->title('Welcome!');

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
