package Mojo::Headers;
use Mojo::Base -base;

use Mojo::Util 'get_line';

has max_line_size => sub { $ENV{MOJO_MAX_LINE_SIZE} || 10240 };

# Common headers
my @HEADERS = (
  qw/Accept Accept-Language Accept-Ranges Authorization Connection/,
  qw/Cache-Control Content-Disposition Content-Length Content-Range/,
  qw/Content-Transfer-Encoding Content-Type Cookie DNT Date ETag Expect/,
  qw/Expires Host If-Modified-Since Last-Modified Location/,
  qw/Proxy-Authenticate Proxy-Authorization Range Sec-WebSocket-Accept/,
  qw/Sec-WebSocket-Key Sec-WebSocket-Origin Sec-WebSocket-Protocol/,
  qw/Sec-WebSocket-Version Server Set-Cookie Status Trailer/,
  qw/Transfer-Encoding Upgrade User-Agent WWW-Authenticate X-Forwarded-For/,
);
{
  no strict 'refs';
  for my $header (@HEADERS) {
    my $name = lc $header;
    $name =~ s/-/_/g;
    *{__PACKAGE__ . "::$name"} = sub { scalar shift->header($header => @_) };
  }
}

# Lower case headers
my %NORMALCASE_HEADERS;
for my $name (@HEADERS) {
  my $lowercase = lc $name;
  $NORMALCASE_HEADERS{$lowercase} = $name;
}

sub add {
  my ($self, $name) = (shift, shift);

  # Make sure we have a normal case entry for name
  my $lcname = lc $name;
  $NORMALCASE_HEADERS{$lcname} = $name
    unless exists $NORMALCASE_HEADERS{$lcname};
  $name = $lcname;

  # Add lines
  push @{$self->{headers}->{$name}}, (ref $_ || '') eq 'ARRAY' ? $_ : [$_]
    for @_;

  return $self;
}

sub clone {
  my $self  = shift;
  my $clone = $self->new;
  $clone->{headers}->{$_} = [@{$self->{headers}->{$_}}]
    for keys %{$self->{headers}};
  return $clone;
}

sub from_hash {
  my ($self, $hash) = (shift, shift);

  # Empty hash deletes all headers
  if (keys %{$hash} == 0) {
    $self->{headers} = {};
    return $self;
  }

  # Merge
  while (my ($header, $value) = each %$hash) {
    $self->add($header => ref $value eq 'ARRAY' ? @$value : $value);
  }

  return $self;
}

# "Will you be my mommy? You smell like dead bunnies..."
sub header {
  my ($self, $name) = (shift, shift);

  # Replace
  return $self->remove($name)->add($name, @_) if @_;

  # String
  return unless my $headers = $self->{headers}->{lc $name};
  return join ', ', map { join ', ', @$_ } @$headers unless wantarray;

  # Array
  return @$headers;
}

# DEPRECATED in Leaf Fluttering In Wind!
sub is_done {
  warn <<EOF;
Mojo::Headers->is_done is DEPRECATED in favor of Mojo::Headers->is_finished!
EOF
  shift->is_finished;
}

sub is_finished { (shift->{state} || '') eq 'finished' }

sub is_limit_exceeded { shift->{limit} }

sub leftovers { delete shift->{buffer} }

sub names {
  my @headers;
  push @headers, $NORMALCASE_HEADERS{$_} || $_ for keys %{shift->{headers}};
  return \@headers;
}

sub parse {
  my ($self, $chunk) = @_;

  # Parse headers with size limit
  $self->{state} = 'headers';
  $self->{buffer} //= '';
  $self->{buffer} .= $chunk if defined $chunk;
  my $headers = $self->{cache} ||= [];
  my $max = $self->max_line_size;
  while (defined(my $line = get_line \$self->{buffer})) {

    # Check line size limit
    if (length $line > $max) {
      $self->{state} = 'finished';
      $self->{limit} = 1;
      return $self;
    }

    # New header
    if ($line =~ /^(\S+)\s*:\s*(.*)$/) { push @$headers, $1, $2 }

    # Multiline
    elsif (@$headers && $line =~ s/^\s+//) { $headers->[-1] .= " $line" }

    # Empty line
    else {
      $self->add(splice @$headers, 0, 2) while @$headers;
      $self->{state} = 'finished';
      return $self;
    }
  }

  # Check line size limit
  if (length $self->{buffer} > $max) {
    $self->{state} = 'finished';
    $self->{limit} = 1;
  }

  return $self;
}

sub referrer { scalar shift->header(Referer => @_) }

sub remove {
  my ($self, $name) = @_;
  delete $self->{headers}->{lc $name};
  return $self;
}

sub to_hash {
  my $self   = shift;
  my %params = @_;

  # Build
  my $hash = {};
  foreach my $header (@{$self->names}) {
    my @headers = $self->header($header);

    # Nested arrayrefs
    if ($params{arrayref}) { $hash->{$header} = [@headers] }

    # Flat arrayref
    else {

      # Turn single value arrayrefs into strings
      foreach my $h (@headers) { $h = $h->[0] if @$h == 1 }
      $hash->{$header} = @headers > 1 ? [@headers] : $headers[0];
    }
  }

  return $hash;
}

sub to_string {
  my $self = shift;

  # Format multiline values
  my @headers;
  for my $name (@{$self->names}) {
    push @headers, "$name: " . join("\x0d\x0a ", @$_)
      for $self->header($name);
  }

  # Format headers
  return join "\x0d\x0a", @headers;
}

1;
__END__

=head1 NAME

Mojo::Headers - Headers

=head1 SYNOPSIS

  use Mojo::Headers;

  my $headers = Mojo::Headers->new;
  $headers->content_type('text/plain');
  $headers->parse("Content-Type: text/html\n\n");

=head1 DESCRIPTION

L<Mojo::Headers> is a container and parser for HTTP headers.

=head1 ATTRIBUTES

L<Mojo::Headers> implements the following attributes.

=head2 C<max_line_size>

  my $size = $headers->max_line_size;
  $headers = $headers->max_line_size(1024);

Maximum line size in bytes, defaults to the value of C<MOJO_MAX_LINE_SIZE> or
C<10240>. Note that this attribute is EXPERIMENTAL and might change without
warning!

=head1 METHODS

L<Mojo::Headers> inherits all methods from L<Mojo::Base> and implements the
following new ones.

=head2 C<accept>

  my $accept = $headers->accept;
  $headers   = $headers->accept('application/json');

Shortcut for the C<Accept> header.

=head2 C<accept_language>

  my $accept_language = $headers->accept_language;
  $headers            = $headers->accept_language('de, en');

Shortcut for the C<Accept-Language> header.

=head2 C<accept_ranges>

  my $ranges = $headers->accept_ranges;
  $headers   = $headers->accept_ranges('bytes');

Shortcut for the C<Accept-Ranges> header.

=head2 C<add>

  $headers = $headers->add('Content-Type', 'text/plain');

Add one or more header lines.

=head2 C<authorization>

  my $authorization = $headers->authorization;
  $headers          = $headers->authorization('Basic Zm9vOmJhcg==');

Shortcut for the C<Authorization> header.

=head2 C<cache_control>

  my $cache_control = $headers->cache_control;
  $headers          = $headers->cache_control('max-age=1, no-cache');

Shortcut for the C<Cache-Control> header.

=head2 C<clone>

  my $clone = $headers->clone;

Clone headers. Note that this method is EXPERIMENTAL and might change without
warning!

=head2 C<connection>

  my $connection = $headers->connection;
  $headers       = $headers->connection('close');

Shortcut for the C<Connection> header.

=head2 C<content_disposition>

  my $content_disposition = $headers->content_disposition;
  $headers                = $headers->content_disposition('foo');

Shortcut for the C<Content-Disposition> header.

=head2 C<content_length>

  my $content_length = $headers->content_length;
  $headers           = $headers->content_length(4000);

Shortcut for the C<Content-Length> header.

=head2 C<content_range>

  my $range = $headers->content_range;
  $headers  = $headers->content_range('bytes 2-8/100');

Shortcut for the C<Content-Range> header.

=head2 C<content_transfer_encoding>

  my $encoding = $headers->content_transfer_encoding;
  $headers     = $headers->content_transfer_encoding('foo');

Shortcut for the C<Content-Transfer-Encoding> header.

=head2 C<content_type>

  my $content_type = $headers->content_type;
  $headers         = $headers->content_type('text/plain');

Shortcut for the C<Content-Type> header.

=head2 C<cookie>

  my $cookie = $headers->cookie;
  $headers   = $headers->cookie('$Version=1; f=b; $Path=/');

Shortcut for the C<Cookie> header.

=head2 C<date>

  my $date = $headers->date;
  $headers = $headers->date('Sun, 17 Aug 2008 16:27:35 GMT');

Shortcut for the C<Date> header.

=head2 C<dnt>

  my $dnt  = $headers->dnt;
  $headers = $headers->dnt(1);

Shortcut for the C<DNT> (Do Not Track) header.

=head2 C<etag>

  my $etag = $headers->etag;
  $headers = $headers->etag('abc321');

Shortcut for the C<ETag> header.

=head2 C<expect>

  my $expect = $headers->expect;
  $headers   = $headers->expect('100-continue');

Shortcut for the C<Expect> header.

=head2 C<expires>

  my $expires = $headers->expires;
  $headers    = $headers->expires('Thu, 01 Dec 1994 16:00:00 GMT');

Shortcut for the C<Expires> header.

=head2 C<from_hash>

  $headers = $headers->from_hash({'Content-Type' => 'text/html'});

Parse headers from a hash.

=head2 C<header>

  my $string = $headers->header('Content-Type');
  my @lines  = $headers->header('Content-Type');
  $headers   = $headers->header('Content-Type' => 'text/plain');

Get or replace the current header values.

  # Multiple headers with the same name
  for my $header ($headers->header('Set-Cookie')) {
    say 'Set-Cookie:';

    # Each header contains an array of lines
    for my $line (@$header) {
      say $line;
    }
  }

=head2 C<host>

  my $host = $headers->host;
  $headers = $headers->host('127.0.0.1');

Shortcut for the C<Host> header.

=head2 C<if_modified_since>

  my $m    = $headers->if_modified_since;
  $headers = $headers->if_modified_since('Sun, 17 Aug 2008 16:27:35 GMT');

Shortcut for the C<If-Modified-Since> header.

=head2 C<is_finished>

  my $success = $headers->is_finished;

Check if header parser is finished.

=head2 C<is_limit_exceeded>

  my $success = $headers->is_limit_exceeded;

Check if a header has exceeded C<max_line_size>. Note that this method is
EXPERIMENTAL and might change without warning!

=head2 C<last_modified>

  my $m    = $headers->last_modified;
  $headers = $headers->last_modified('Sun, 17 Aug 2008 16:27:35 GMT');

Shortcut for the C<Last-Modified> header.

=head2 C<leftovers>

  my $leftovers = $headers->leftovers;

Leftovers.

=head2 C<location>

  my $location = $headers->location;
  $headers     = $headers->location('http://127.0.0.1/foo');

Shortcut for the C<Location> header.

=head2 C<names>

  my $names = $headers->names;

Generate a list of all currently defined headers.

=head2 C<parse>

  $headers = $headers->parse("Content-Type: text/foo\n\n");

Parse formatted headers.

=head2 C<proxy_authenticate>

  my $authenticate = $headers->proxy_authenticate;
  $headers         = $headers->proxy_authenticate('Basic "realm"');

Shortcut for the C<Proxy-Authenticate> header.

=head2 C<proxy_authorization>

  my $proxy_authorization = $headers->proxy_authorization;
  $headers = $headers->proxy_authorization('Basic Zm9vOmJhcg==');

Shortcut for the C<Proxy-Authorization> header.

=head2 C<range>

  my $range = $headers->range;
  $headers  = $headers->range('bytes=2-8');

Shortcut for the C<Range> header.

=head2 C<referrer>

  my $referrer = $headers->referrer;
  $headers     = $headers->referrer('http://mojolicio.us');

Shortcut for the C<Referer> header, there was a typo in RFC 2068 which
resulted in C<Referer> becoming an official header.

=head2 C<remove>

  $headers = $headers->remove('Content-Type');

Remove a header.

=head2 C<sec_websocket_accept>

  my $accept = $headers->sec_websocket_accept;
  $headers   = $headers->sec_websocket_accept('s3pPLMBiTxaQ9kYGzzhZRbK+xOo=');

Shortcut for the C<Sec-WebSocket-Accept> header.

=head2 C<sec_websocket_key>

  my $key  = $headers->sec_websocket_key;
  $headers = $headers->sec_websocket_key('dGhlIHNhbXBsZSBub25jZQ==');

Shortcut for the C<Sec-WebSocket-Key> header.

=head2 C<sec_websocket_origin>

  my $origin = $headers->sec_websocket_origin;
  $headers   = $headers->sec_websocket_origin('http://example.com');

Shortcut for the C<Sec-WebSocket-Origin> header.

=head2 C<sec_websocket_protocol>

  my $protocol = $headers->sec_websocket_protocol;
  $headers     = $headers->sec_websocket_protocol('sample');

Shortcut for the C<Sec-WebSocket-Protocol> header.

=head2 C<sec_websocket_version>

  my $version = $headers->sec_websocket_version;
  $headers    = $headers->sec_websocket_version(13);

Shortcut for the C<Sec-WebSocket-Version> header.

=head2 C<server>

  my $server = $headers->server;
  $headers   = $headers->server('Mojo');

Shortcut for the C<Server> header.

=head2 C<set_cookie>

  my $set_cookie = $headers->set_cookie;
  $headers       = $headers->set_cookie('f=b; Version=1; Path=/');

Shortcut for the C<Set-Cookie> header.

=head2 C<status>

  my $status = $headers->status;
  $headers   = $headers->status('200 OK');

Shortcut for the C<Status> header.

=head2 C<to_hash>

  my $hash = $headers->to_hash;
  my $hash = $headers->to_hash(arrayref => 1);

Format headers as a hash. Nested arrayrefs to represent multi line values are
optional.

=head2 C<to_string>

  my $string = $headers->to_string;

Format headers suitable for HTTP 1.1 messages.

=head2 C<trailer>

  my $trailer = $headers->trailer;
  $headers    = $headers->trailer('X-Foo');

Shortcut for the C<Trailer> header.

=head2 C<transfer_encoding>

  my $transfer_encoding = $headers->transfer_encoding;
  $headers              = $headers->transfer_encoding('chunked');

Shortcut for the C<Transfer-Encoding> header.

=head2 C<upgrade>

  my $upgrade = $headers->upgrade;
  $headers    = $headers->upgrade('websocket');

Shortcut for the C<Upgrade> header.

=head2 C<user_agent>

  my $user_agent = $headers->user_agent;
  $headers       = $headers->user_agent('Mojo/1.0');

Shortcut for the C<User-Agent> header.

=head2 C<www_authenticate>

  my $authenticate = $headers->www_authenticate;
  $headers         = $headers->www_authenticate('Basic realm="realm"');

Shortcut for the C<WWW-Authenticate> header.

=head2 C<x_forwarded_for>

  my $x_forwarded_for = $headers->x_forwarded_for;
  $headers            = $headers->x_forwarded_for('127.0.0.1');

Shortcut for the C<X-Forwarded-For> header.

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
