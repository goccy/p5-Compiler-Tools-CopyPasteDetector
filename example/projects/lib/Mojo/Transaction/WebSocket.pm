package Mojo::Transaction::WebSocket;
use Mojo::Base 'Mojo::Transaction';

# "I'm not calling you a liar but...
#  I can't think of a way to finish that sentence."
use Config;
use Mojo::Transaction::HTTP;
use Mojo::Util qw/b64_encode decode encode sha1_bytes/;

use constant DEBUG => $ENV{MOJO_WEBSOCKET_DEBUG} || 0;

# Unique value from the spec
use constant GUID => '258EAFA5-E914-47DA-95CA-C5AB0DC85B11';

# Opcodes
use constant {
  CONTINUATION => 0,
  TEXT         => 1,
  BINARY       => 2,
  CLOSE        => 8,
  PING         => 9,
  PONG         => 10
};

has handshake => sub { Mojo::Transaction::HTTP->new };
has 'masked';
has max_websocket_size => sub { $ENV{MOJO_MAX_WEBSOCKET_SIZE} || 262144 };

sub build_frame {
  my ($self, $fin, $op, $payload) = @_;
  warn "BUILDING FRAME\n" if DEBUG;

  # Head
  my $frame = 0;
  vec($frame, 0, 8) = $op | 0b10000000 if $fin;

  # Mask payload
  warn "PAYLOAD: $payload\n" if DEBUG;
  my $masked = $self->masked;
  if ($masked) {
    warn "MASKING PAYLOAD\n" if DEBUG;
    my $mask = pack 'N', int(rand 9999999);
    $payload = $mask . _xor_mask($payload, $mask);
  }

  # Length
  my $len = length $payload;
  $len -= 4 if $masked;

  # Empty prefix
  my $prefix = 0;

  # Small payload
  if ($len < 126) {
    vec($prefix, 0, 8) = $masked ? ($len | 0b10000000) : $len;
    $frame .= $prefix;
  }

  # Extended payload (16bit)
  elsif ($len < 65536) {
    vec($prefix, 0, 8) = $masked ? (126 | 0b10000000) : 126;
    $frame .= $prefix;
    $frame .= pack 'n', $len;
  }

  # Extended payload (64bit)
  else {
    vec($prefix, 0, 8) = $masked ? (127 | 0b10000000) : 127;
    $frame .= $prefix;
    $frame .=
      $Config{ivsize} > 4
      ? pack('Q>', $len)
      : pack('NN', $len >> 32, $len & 0xFFFFFFFF);
  }

  if (DEBUG) {
    warn 'HEAD: ', unpack('B*', $frame), "\n";
    warn "OPCODE: $op\n";
  }

  # Payload
  $frame .= $payload;

  return $frame;
}

sub client_challenge {
  my $self = shift;

  # Solve WebSocket challenge
  my $solution = $self->_challenge($self->req->headers->sec_websocket_key);
  return unless $solution eq $self->res->headers->sec_websocket_accept;
  return 1;
}

sub client_handshake {
  my $self = shift;

  # Default headers
  my $headers = $self->req->headers;
  $headers->upgrade('websocket')  unless $headers->upgrade;
  $headers->connection('Upgrade') unless $headers->connection;
  $headers->sec_websocket_protocol('mojo')
    unless $headers->sec_websocket_protocol;
  $headers->sec_websocket_version(13) unless $headers->sec_websocket_version;

  # Generate WebSocket challenge
  $headers->sec_websocket_key(b64_encode(pack('N*', int(rand 9999999)), ''))
    unless $headers->sec_websocket_key;

  return $self;
}

sub client_read  { shift->server_read(@_) }
sub client_write { shift->server_write(@_) }
sub connection   { shift->handshake->connection(@_) }

sub finish {
  my $self = shift;
  $self->send_frame(1, CLOSE, '');
  $self->{finished} = 1;
  return $self;
}

sub is_websocket {1}

sub local_address { shift->handshake->local_address }
sub local_port    { shift->handshake->local_port }

sub parse_frame {
  my ($self, $buffer) = @_;
  warn "PARSING FRAME\n" if DEBUG;

  # Head
  my $clone = $$buffer;
  return unless length $clone > 2;
  my $head = substr $clone, 0, 2;
  warn 'HEAD: ' . unpack('B*', $head) . "\n" if DEBUG;

  # FIN
  my $fin = (vec($head, 0, 8) & 0b10000000) == 0b10000000 ? 1 : 0;
  warn "FIN: $fin\n" if DEBUG;

  # Opcode
  my $op = vec($head, 0, 8) & 0b00001111;
  warn "OPCODE: $op\n" if DEBUG;

  # Length
  my $len = vec($head, 1, 8) & 0b01111111;
  warn "LENGTH: $len\n" if DEBUG;

  # No payload
  my $hlen = 2;
  if ($len == 0) { warn "NOTHING\n" if DEBUG }

  # Small payload
  elsif ($len < 126) { warn "SMALL\n" if DEBUG }

  # Extended payload (16bit)
  elsif ($len == 126) {
    return unless length $clone > 4;
    $hlen = 4;
    my $ext = substr $clone, 2, 2;
    $len = unpack 'n', $ext;
    warn "EXTENDED (16bit): $len\n" if DEBUG;
  }

  # Extended payload (64bit)
  elsif ($len == 127) {
    return unless length $clone > 10;
    $hlen = 10;
    my $ext = substr $clone, 2, 8;
    $len =
      $Config{ivsize} > 4
      ? unpack('Q>', $ext)
      : unpack('N', substr($ext, 4, 4));
    warn "EXTENDED (64bit): $len\n" if DEBUG;
  }

  # Check message size
  $self->finish and return if $len > $self->max_websocket_size;

  # Check if whole packet has arrived
  my $masked = vec($head, 1, 8) & 0b10000000;
  return if length $clone < ($len + $hlen + ($masked ? 4 : 0));
  substr $clone, 0, $hlen, '';

  # Payload
  $len += 4 if $masked;
  return if length $clone < $len;
  my $payload = $len ? substr($clone, 0, $len, '') : '';

  # Unmask payload
  if ($masked) {
    warn "UNMASKING PAYLOAD\n" if DEBUG;
    $payload = _xor_mask($payload, substr($payload, 0, 4, ''));
  }
  warn "PAYLOAD: $payload\n" if DEBUG;
  $$buffer = $clone;

  return [$fin, $op, $payload];
}

sub remote_address { shift->handshake->remote_address }
sub remote_port    { shift->handshake->remote_port }
sub req            { shift->handshake->req(@_) }
sub res            { shift->handshake->res(@_) }

sub resume {
  my $self = shift;
  $self->handshake->resume;
  return $self;
}

sub send_frame {
  my ($self, $fin, $type, $payload, $cb) = @_;

  # Prepare frame
  $self->{drain} = $cb if $cb;
  $self->{write} //= '';
  $self->{write} .= $self->build_frame($fin, $type, $payload);
  $self->{state} = 'write';

  # Resume
  $self->emit('resume');
}

sub send_message {
  my ($self, $m, $cb) = @_;
  $m //= '';

  # Binary or raw text
  return $self->send_frame(1, $m->[0] eq 'text' ? TEXT : BINARY, $m->[1], $cb)
    if ref $m;

  # Text
  $self->send_frame(1, TEXT, encode('UTF-8', $m), $cb);
}

sub server_handshake {
  my $self = shift;

  # WebSocket handshake
  my $res         = $self->res;
  my $res_headers = $res->headers;
  $res->code(101);
  $res_headers->upgrade('websocket');
  $res_headers->connection('Upgrade');
  my $req_headers = $self->req->headers;
  my $protocol = $req_headers->sec_websocket_protocol || '';
  $protocol =~ /^\s*([^\,]+)/;
  $res_headers->sec_websocket_protocol($1) if $1;
  $res_headers->sec_websocket_accept(
    $self->_challenge($req_headers->sec_websocket_key));

  return $self;
}

sub server_read {
  my ($self, $chunk) = @_;

  # Parse frames
  $self->{read} //= '';
  $self->{read} .= $chunk if defined $chunk;
  $self->{message} //= '';
  while (my $frame = $self->parse_frame(\$self->{read})) {
    $self->emit(frame => $frame);
    my $op = $frame->[1] || CONTINUATION;

    # Ping/Pong
    $self->send_frame(1, PONG, $frame->[2]) and next if $op == PING;
    next if $op == PONG;

    # Close
    $self->finish and next if $op == CLOSE;

    # Append chunk and check message size
    next unless $self->has_subscribers('message');
    $self->{op} = $op unless exists $self->{op};
    $self->{message} .= $frame->[2];
    $self->finish and last
      if length $self->{message} > $self->max_websocket_size;

    # No FIN bit (Continuation)
    next unless $frame->[0];

    # Message
    my $message = $self->{message};
    $self->{message} = '';
    $message = decode 'UTF-8', $message
      if $message && delete $self->{op} == TEXT;
    $self->emit(message => $message);
  }

  # Resume
  return $self->emit('resume');
}

sub server_write {
  my $self = shift;

  # Drain
  $self->{write} //= '';
  unless (length $self->{write}) {
    $self->{state} = $self->{finished} ? 'finished' : 'read';
    my $cb = delete $self->{drain};
    $self->$cb if $cb;
  }

  # Empty buffer
  my $write = $self->{write};
  $self->{write} = '';

  return $write;
}

sub _challenge { b64_encode(sha1_bytes((pop() || '') . GUID), '') }

sub _xor_mask {
  my ($input, $mask) = @_;

  # 512 byte mask
  $mask = $mask x 128;
  my $output = '';
  $output .= $_ ^ $mask while length($_ = substr($input, 0, 512, '')) == 512;
  $output .= $_ ^ substr($mask, 0, length, '');

  return $output;
}

1;
__END__

=head1 NAME

Mojo::Transaction::WebSocket - WebSocket transaction container

=head1 SYNOPSIS

  use Mojo::Transaction::WebSocket;

  my $ws = Mojo::Transaction::WebSocket->new;

=head1 DESCRIPTION

L<Mojo::Transaction::WebSocket> is a container for WebSocket transactions as
described in RFC 6455. Note that this module is EXPERIMENTAL and might change
without warning!

=head1 EVENTS

L<Mojo::Transaction::WebSocket> inherits all events from L<Mojo::Transaction>
and can emit the following new ones.

=head2 C<frame>

  $ws->on(frame => sub {
    my ($ws, $frame) = @_;
    ...
  });

Emitted when a WebSocket frame has been received.

  $ws->on(frame => sub {
    my ($ws, $frame) = @_;
    say 'Fin: ',     $frame->[0];
    say 'Type: ',    $frame->[1];
    say 'Payload: ', $frame->[2];
  });

=head2 C<message>

  $ws->on(message => sub {
    my ($ws, $message) = @_;
    ...
  });

Emitted when a complete WebSocket message has been received.

  $ws->on(message => sub {
    my ($ws, $message) = @_;
    say "Message: $message";
  });

=head1 ATTRIBUTES

L<Mojo::Transaction::WebSocket> inherits all attributes from
L<Mojo::Transaction> and implements the following new ones.

=head2 C<handshake>

  my $handshake = $ws->handshake;
  $ws           = $ws->handshake(Mojo::Transaction::HTTP->new);

The original handshake transaction, defaults to a L<Mojo::Transaction::HTTP>
object.

=head2 C<masked>

  my $masked = $ws->masked;
  $ws        = $ws->masked(1);

Mask outgoing frames with XOR cipher and a random 32bit key.

=head2 C<max_websocket_size>

  my $size = $ws->max_websocket_size;
  $ws      = $ws->max_websocket_size(1024);

Maximum WebSocket message size in bytes, defaults to the value of
C<MOJO_MAX_WEBSOCKET_SIZE> or C<262144>.

=head1 METHODS

L<Mojo::Transaction::WebSocket> inherits all methods from
L<Mojo::Transaction> and implements the following new ones.

=head2 C<build_frame>

  my $bytes = $ws->build_frame($fin, $op, $payload);

Build WebSocket frame.

=head2 C<client_challenge>

  my $success = $ws->client_challenge;

Check WebSocket handshake challenge.

=head2 C<client_handshake>

  $ws->client_handshake;

WebSocket handshake.

=head2 C<client_read>

  $ws->client_read($data);

Read raw WebSocket data.

=head2 C<client_write>

  my $chunk = $ws->client_write;

Raw WebSocket data to write.

=head2 C<connection>

  my $connection = $ws->connection;

Alias for L<Mojo::Transaction/"connection">.

=head2 C<finish>

  $ws = $ws->finish;

Finish the WebSocket connection gracefully.

=head2 C<is_websocket>

  my $true = $ws->is_websocket;

True.

=head2 C<local_address>

  my $local_address = $ws->local_address;

Alias for L<Mojo::Transaction/"local_address">.

=head2 C<local_port>

  my $local_port = $ws->local_port;

Alias for L<Mojo::Transaction/"local_port">.

=head2 C<parse_frame>

  my $frame = $ws->parse_frame(\$bytes);

Parse WebSocket frame.

=head2 C<remote_address>

  my $remote_address = $ws->remote_address;

Alias for L<Mojo::Transaction/"remote_address">.

=head2 C<remote_port>

  my $remote_port = $ws->remote_port;

Alias for L<Mojo::Transaction/"remote_port">.

=head2 C<req>

  my $req = $ws->req;

Alias for L<Mojo::Transaction/"req">.

  $ws->req->headers->header('X-Bender' => 'Bite my shiny metal ass!');

=head2 C<res>

  my $res = $ws->res;

Alias for L<Mojo::Transaction/"res">.

  $ws->res->headers->header('X-Bender' => 'Bite my shiny metal ass!');

=head2 C<resume>

  $ws = $ws->resume;

Alias for L<Mojo::Transaction/"resume">.

=head2 C<send_frame>

  $ws->send_frame($fin, $op, $payload);
  $ws->send_frame($fin, $op, $payload, sub {...});

Send a single frame non-blocking via WebSocket, the optional drain callback
will be invoked once all data has been written.

=head2 C<send_message>

  $ws->send_message([binary => $bytes]);
  $ws->send_message([text   => $bytes]);
  $ws->send_message('Hi there!');
  $ws->send_message('Hi there!', sub {...});

Send a message non-blocking via WebSocket, the optional drain callback will
be invoked once all data has been written.

=head2 C<server_handshake>

  $ws->server_handshake;

WebSocket handshake.

=head2 C<server_read>

  $ws->server_read($data);

Read raw WebSocket data.

=head2 C<server_write>

  my $chunk = $ws->server_write;

Raw WebSocket data to write.

=head1 DEBUGGING

You can set the C<MOJO_WEBSOCKET_DEBUG> environment variable to get some
advanced diagnostics information printed to C<STDERR>.

  MOJO_WEBSOCKET_DEBUG=1

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
