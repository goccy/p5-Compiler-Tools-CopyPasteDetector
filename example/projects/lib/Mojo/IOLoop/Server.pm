package Mojo::IOLoop::Server;
use Mojo::Base 'Mojo::EventEmitter';

use Carp 'croak';
use File::Spec;
use IO::File;
use IO::Socket::INET;
use Scalar::Util 'weaken';
use Socket qw/IPPROTO_TCP TCP_NODELAY/;

# IPv6 support requires IO::Socket::IP
use constant IPV6 => $ENV{MOJO_NO_IPV6}
  ? 0
  : eval 'use IO::Socket::IP 0.06 (); 1';

# TLS support requires IO::Socket::SSL
use constant TLS => $ENV{MOJO_NO_TLS}
  ? 0
  : eval 'use IO::Socket::SSL 1.37 "inet4"; 1';
use constant TLS_READ  => TLS ? IO::Socket::SSL::SSL_WANT_READ()  : 0;
use constant TLS_WRITE => TLS ? IO::Socket::SSL::SSL_WANT_WRITE() : 0;

# Default TLS cert (20.03.2010)
# (openssl req -new -x509 -keyout cakey.pem -out cacert.pem -nodes -days 7300)
use constant CERT => <<EOF;
-----BEGIN CERTIFICATE-----
MIIDbzCCAtigAwIBAgIJAM+kFv1MwalmMA0GCSqGSIb3DQEBBQUAMIGCMQswCQYD
VQQGEwJERTEWMBQGA1UECBMNTmllZGVyc2FjaHNlbjESMBAGA1UEBxMJSGFtYmVy
Z2VuMRQwEgYDVQQKEwtNb2pvbGljaW91czESMBAGA1UEAxMJbG9jYWxob3N0MR0w
GwYJKoZIhvcNAQkBFg5rcmFpaEBjcGFuLm9yZzAeFw0xMDAzMjAwMDQ1MDFaFw0z
MDAzMTUwMDQ1MDFaMIGCMQswCQYDVQQGEwJERTEWMBQGA1UECBMNTmllZGVyc2Fj
aHNlbjESMBAGA1UEBxMJSGFtYmVyZ2VuMRQwEgYDVQQKEwtNb2pvbGljaW91czES
MBAGA1UEAxMJbG9jYWxob3N0MR0wGwYJKoZIhvcNAQkBFg5rcmFpaEBjcGFuLm9y
ZzCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAzu9mOiyUJB2NBuf1lZxViNM2
VISqRAoaXXGOBa6RgUoVfA/n81RQlgvVA0qCSQHC534DdYRk3CdyJR9UGPuxF8k4
CckOaHWgcJJsd8H0/q73PjbA5ItIpGTTJNh8WVpFDjHTJmQ5ihwddap4/offJxZD
dPrMFtw1ZHBRug5tHUECAwEAAaOB6jCB5zAdBgNVHQ4EFgQUo+Re5wuuzVFqH/zV
cxRGXL0j5K4wgbcGA1UdIwSBrzCBrIAUo+Re5wuuzVFqH/zVcxRGXL0j5K6hgYik
gYUwgYIxCzAJBgNVBAYTAkRFMRYwFAYDVQQIEw1OaWVkZXJzYWNoc2VuMRIwEAYD
VQQHEwlIYW1iZXJnZW4xFDASBgNVBAoTC01vam9saWNpb3VzMRIwEAYDVQQDEwls
b2NhbGhvc3QxHTAbBgkqhkiG9w0BCQEWDmtyYWloQGNwYW4ub3JnggkAz6QW/UzB
qWYwDAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQUFAAOBgQCZZcOeAobctD9wtPtO
40CKHpiGYEM3rh7VvBhjTcVnX6XlLvffIg3uTrVRhzmlEQCZz3O5TsBzfMAVnjYz
llhwgRF6Xn8ict9L8yKDoGSbw0Q7HaCb8/kOe0uKhcSDUd3PjJU0ZWgc20zcGFA9
R65bABoJ2vU1rlQFmjs0RT4UcQ==
-----END CERTIFICATE-----
EOF

# Default TLS key (20.03.2010)
# (openssl req -new -x509 -keyout cakey.pem -out cacert.pem -nodes -days 7300)
use constant KEY => <<EOF;
-----BEGIN RSA PRIVATE KEY-----
MIICXAIBAAKBgQDO72Y6LJQkHY0G5/WVnFWI0zZUhKpEChpdcY4FrpGBShV8D+fz
VFCWC9UDSoJJAcLnfgN1hGTcJ3IlH1QY+7EXyTgJyQ5odaBwkmx3wfT+rvc+NsDk
i0ikZNMk2HxZWkUOMdMmZDmKHB11qnj+h98nFkN0+swW3DVkcFG6Dm0dQQIDAQAB
AoGAeLmd8C51tqQu1GqbEc+E7zAZsDE9jDhArWdELfhsFvt7kUdOUN1Nrlv0x9i+
LY2Dgb44kmTM2suAgjvGulSMOYBGosZcM0w3ES76nmeAVJ1NBFhbZTCJqo9svoD/
NKdctRflUuvFSWimoui+vj9D5p/4lvAMdBHUWj5FlQsYiOECQQD/FRXtsDetptFu
Vp8Kw+6bZ5+efcjVfciTp7fQKI2xZ2n1QyloaV4zYXgDC2y3fMYuRigCGrX9XeFX
oGHGMyYFAkEAz635I8f4WQa/wvyl/SR5agtDVnkJqMHMgOuykytiF8NFbDSkJv+b
1VfyrWcfK/PVsSGBI67LCMDoP+PZBVOjDQJBAIInoCjH4aEZnYNPb5duojFpjmiw
helpZQ7yZTgxeRssSUR8IITGPuq4sSPckHyPjg/OfFuWhYXigTjU/Q7EyoECQERT
Dykna9wWLVZ/+jgLHOq3Y+L6FSRxBc/QO0LRvgblVlygAPVXmLQaqBtGVuoF4WLS
DANqSR/LH12Nn2NyPa0CQBbzoHgx2i3RncWoq1EeIg2mSMevEcjA6sxgYmsyyzlv
AnqxHi90n/p912ynLg2SjBq+03GaECeGzC/QqKK2gtA=
-----END RSA PRIVATE KEY-----
EOF

has accepts   => 10;
has iowatcher => sub {
  require Mojo::IOLoop;
  Mojo::IOLoop->singleton->iowatcher;
};

# "Your guilty consciences may make you vote Democratic, but secretly you all
#  yearn for a Republican president to lower taxes, brutalize criminals, and
#  rule you like a king!"
sub DESTROY {
  my $self = shift;
  if (my $port = $self->{port}) { $ENV{MOJO_REUSE} =~ s/(?:^|\,)$port\:\d+// }
  if (my $cert = $self->{cert}) { unlink $cert if -w $cert }
  if (my $key  = $self->{key})  { unlink $key  if -w $key }
  return unless my $watcher = $self->{iowatcher};
  $self->pause if $self->{handle};
  $watcher->drop_handle($_) for values %{$self->{handles}};
}

# "And I gave that man directions, even though I didn't know the way,
#  because that's the kind of guy I am this week."
sub listen {
  my $self = shift;
  my $args = ref $_[0] ? $_[0] : {@_};

  # Look for reusable file descriptor
  my $reuse = my $port = $self->{port} = $args->{port} || 3000;
  $ENV{MOJO_REUSE} ||= '';
  my $fd;
  if ($ENV{MOJO_REUSE} =~ /(?:^|\,)$reuse\:(\d+)/) { $fd = $1 }

  # Allow file descriptor inheritance
  local $^F = 1000;

  # Reuse file descriptor
  my $handle;
  my $class = IPV6 ? 'IO::Socket::IP' : 'IO::Socket::INET';
  if (defined $fd) {
    $handle = $class->new;
    $handle->fdopen($fd, 'r')
      or croak "Can't open file descriptor $fd: $!";
  }

  # New socket
  else {
    my %options = (
      Listen => $args->{backlog} // SOMAXCONN,
      LocalAddr => $args->{address} || '0.0.0.0',
      LocalPort => $port,
      Proto     => 'tcp',
      ReuseAddr => 1,
      Type      => SOCK_STREAM
    );
    $options{LocalAddr} =~ s/[\[\]]//g;
    $handle = $class->new(%options)
      or croak "Can't create listen socket: $!";
    $fd = fileno $handle;
    $reuse = ",$reuse" if length $ENV{MOJO_REUSE};
    $ENV{MOJO_REUSE} .= "$reuse:$fd";
  }
  $handle->blocking(0);
  $self->{handle} = $handle;

  # TLS
  return unless $args->{tls};
  croak "IO::Socket::SSL 1.37 required for TLS support" unless TLS;

  # Options
  my $options = $self->{tls} = {
    SSL_startHandshake => 0,
    SSL_cert_file      => $args->{tls_cert} || $self->_cert_file,
    SSL_key_file       => $args->{tls_key} || $self->_key_file,
  };
  %$options = (
    SSL_verify_callback => $args->{tls_verify},
    SSL_ca_file         => -T $args->{tls_ca} ? $args->{tls_ca} : undef,
    SSL_ca_path         => -d $args->{tls_ca} ? $args->{tls_ca} : undef,
    SSL_verify_mode     => $args->{tls_ca} ? 0x03 : undef,
    %$options
  ) if $args->{tls_ca};
}

sub generate_port {

  # Try random ports
  my $port = 1 . int(rand 10) . int(rand 10) . int(rand 10) . int(rand 10);
  while ($port++ < 30000) {
    return $port
      if IO::Socket::INET->new(
      Listen    => 5,
      LocalAddr => '127.0.0.1',
      LocalPort => $port,
      Proto     => 'tcp'
      );
  }

  return;
}

sub pause {
  my $self = shift;
  $self->iowatcher->drop_handle($self->{handle});
}

sub resume {
  my $self = shift;
  weaken $self;
  $self->iowatcher->watch($self->{handle},
    sub { $self->_accept for 1 .. $self->accepts });
}

sub _accept {
  my $self = shift;

  # Accept
  return unless my $handle = $self->{handle}->accept;
  $handle->blocking(0);

  # Disable Nagle's algorithm
  setsockopt $handle, IPPROTO_TCP, TCP_NODELAY, 1;

  # Start TLS handshake
  return $self->emit_safe(accept => $handle) unless my $tls = $self->{tls};
  weaken $self;
  $tls->{SSL_error_trap} = sub {
    return unless my $handle = delete $self->{handles}->{shift()};
    $self->iowatcher->drop_handle($handle);
    close $handle;
  };
  $handle = IO::Socket::SSL->start_SSL($handle, %$tls);
  $self->iowatcher->watch(
    $handle,
    sub { $self->_tls($handle) },
    sub { $self->_tls($handle) }
  );
  $self->{handles}->{$handle} = $handle;
}

sub _cert_file {
  my $self = shift;

  # Check if temporary TLS cert file already exists
  my $cert = $self->{cert};
  return $cert if $cert && -r $cert;

  # Create temporary TLS cert file
  $cert = File::Spec->catfile($ENV{MOJO_TMPDIR} || File::Spec->tmpdir,
    "mojocert-$$.pem");
  croak qq/Can't create temporary TLS cert file "$cert"/
    unless my $file = IO::File->new("> $cert");
  print $file CERT;

  $self->{cert} = $cert;
}

sub _key_file {
  my $self = shift;

  # Check if temporary TLS key file already exists
  my $key = $self->{key};
  return $key if $key && -r $key;

  # Create temporary TLS key file
  $key = File::Spec->catfile($ENV{MOJO_TMPDIR} || File::Spec->tmpdir,
    "mojokey-$$.pem");
  croak qq/Can't create temporary TLS key file "$key"/
    unless my $file = IO::File->new("> $key");
  print $file KEY;

  $self->{key} = $key;
}

# "Where on my badge does it say anything about protecting people?
#  Uh, second word, chief."
sub _tls {
  my ($self, $handle) = @_;

  # Accepted
  if ($handle->accept_SSL) {
    $self->iowatcher->drop_handle($handle);
    delete $self->{handles}->{$handle};
    return $self->emit_safe(accept => $handle);
  }

  # Switch between reading and writing
  my $err = $IO::Socket::SSL::SSL_ERROR;
  if    ($err == TLS_READ)  { $self->iowatcher->change($handle, 1, 0) }
  elsif ($err == TLS_WRITE) { $self->iowatcher->change($handle, 1, 1) }
}

1;
__END__

=head1 NAME

Mojo::IOLoop::Server - Non-blocking TCP server

=head1 SYNOPSIS

  use Mojo::IOLoop::Server;

  # Create listen socket
  my $server = Mojo::IOLoop::Server->new;
  $server->on(accept => sub {
    my ($server, $handle) = @_;
    ...
  });
  $server->listen(port => 3000);

  # Start and stop accepting connections
  $server->resume;
  $server->pause;

=head1 DESCRIPTION

L<Mojo::IOLoop::Server> accepts TCP connections for L<Mojo::IOLoop>. Note
that this module is EXPERIMENTAL and might change without warning!

=head1 EVENTS

L<Mojo::IOLoop::Server> can emit the following events.

=head2 C<accept>

  $server->on(accept => sub {
    my ($server, $handle) = @_;
    ...
  });

Emitted safely for each accepted connection.

=head1 ATTRIBUTES

L<Mojo::IOLoop::Server> implements the following attributes.

=head2 C<accepts>

  my $accepts = $server->accepts;
  $server     = $server->accepts(10);

Number of connections to accept at once, defaults to C<10>.

=head2 C<iowatcher>

  my $watcher = $server->iowatcher;
  $server     = $server->iowatcher(Mojo::IOWatcher->new);

Low level event watcher, defaults to the C<iowatcher> attribute value of the
global L<Mojo::IOLoop> singleton.

=head1 METHODS

L<Mojo::IOLoop::Server> inherits all methods from L<Mojo::EventEmitter> and
implements the following new ones.

=head2 C<listen>

  $server->listen(port => 3000);

Create a new listen socket. Note that TLS support depends on
L<IO::Socket::SSL> and IPv6 support on L<IO::Socket::IP>.

These options are currently available:

=over 2

=item C<address>

Local address to listen on, defaults to all.

=item C<backlog>

Maximum backlog size, defaults to C<SOMAXCONN>.

=item C<port>

Port to listen on.

=item C<tls>

Enable TLS.

=item C<tls_cert>

Path to the TLS cert file, defaulting to a built-in test certificate.

=item C<tls_key>

Path to the TLS key file, defaulting to a built-in test key.

=item C<tls_ca>

Path to TLS certificate authority file or directory.

=back

=head2 C<generate_port>

  my $port = $server->generate_port;

Find a free TCP port.

=head2 C<pause>

  $server->pause;

Stop accepting connections.

=head2 C<resume>

  $server->resume;

Start accepting connections.

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
