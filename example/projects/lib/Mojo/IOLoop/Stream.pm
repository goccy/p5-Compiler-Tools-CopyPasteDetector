package Mojo::IOLoop::Stream;
use Mojo::Base 'Mojo::EventEmitter';

use Errno qw/EAGAIN ECONNRESET EINTR EPIPE EWOULDBLOCK/;
use Scalar::Util 'weaken';
use Time::HiRes 'time';

use constant CHUNK_SIZE => $ENV{MOJO_CHUNK_SIZE} || 131072;

has iowatcher => sub {
  require Mojo::IOLoop;
  Mojo::IOLoop->singleton->iowatcher;
};
has timeout => 15;

# "And America has so many enemies.
#  Iran, Iraq, China, Mordor, the hoochies that laid low Tiger Woods,
#  undesirable immigrants - by which I mean everyone that came after me,
#  including my children..."
sub DESTROY { shift->close }

sub new { shift->SUPER::new(handle => shift, buffer => '', active => time) }

sub close {
  my $self = shift;

  # Cleanup
  return unless my $watcher = $self->{iowatcher};
  $watcher->drop_timer(delete $self->{timer}) if $self->{timer};
  return unless my $handle = delete $self->{handle};
  $watcher->drop_handle($handle);

  # Close
  close $handle;
  $self->emit_safe('close');
}

sub handle { shift->{handle} }

sub is_readable {
  my $self = shift;
  return $self->iowatcher->is_readable($self->{handle});
}

sub is_writing {
  my $self = shift;
  return unless exists $self->{handle};
  return length($self->{buffer}) || $self->has_subscribers('drain');
}

sub pause {
  my $self = shift;
  return if $self->{paused}++;
  $self->iowatcher->change($self->{handle}, 0, $self->is_writing);
}

sub resume {
  my $self = shift;

  # Timeout
  my $watcher = $self->iowatcher;
  weaken $self;
  $self->{timer} ||= $watcher->recurring(
    '0.025' => sub {
      return unless $self && (my $t = $self->timeout);
      $self->emit_safe('timeout')->close if (time - ($self->{active})) >= $t;
    }
  );

  # Start streaming
  return $watcher->watch(
    $self->{handle},
    sub { $self->_read },
    sub { $self->_write }
  ) unless $self->{streaming}++;

  # Resume streaming
  return unless delete $self->{paused};
  $self->iowatcher->change($self->{handle}, 1, $self->is_writing);
}

# "No children have ever meddled with the Republican Party and lived to tell
#  about it."
sub steal_handle {
  my $self = shift;
  $self->iowatcher->drop_handle($self->{handle});
  return delete $self->{handle};
}

sub write {
  my ($self, $chunk, $cb) = @_;

  # Prepare chunk for writing
  $self->{buffer} .= $chunk;

  # Write with roundtrip
  if ($cb) { $self->once(drain => $cb) }
  else     { return unless length $self->{buffer} }

  # Start writing
  $self->iowatcher->change($self->{handle}, !$self->{paused}, 1)
    if $self->{handle};
}

sub _read {
  my $self = shift;

  # Read
  my $read = $self->{handle}->sysread(my $buffer, CHUNK_SIZE, 0);

  # Error
  unless (defined $read) {

    # Retry
    return if $! ~~ [EAGAIN, EINTR, EWOULDBLOCK];

    # Closed
    return $self->close if $! == ECONNRESET;

    # Read error
    return $self->emit_safe(error => $!)->close;
  }

  # EOF
  return $self->close if $read == 0;

  # Handle read
  $self->emit_safe(read => $buffer);
  $self->{active} = time;
}

# "Oh, I'm in no condition to drive. Wait a minute.
#  I don't have to listen to myself. I'm drunk."
sub _write {
  my $self = shift;

  # Write as much as possible
  my $handle = $self->{handle};
  if (length $self->{buffer}) {
    my $written = $handle->syswrite($self->{buffer});

    # Error
    unless (defined $written) {

      # Retry
      return if $! ~~ [EAGAIN, EINTR, EWOULDBLOCK];

      # Closed
      return $self->close if $! ~~ [ECONNRESET, EPIPE];

      # Write error
      return $self->emit_safe(error => $!)->close;
    }

    # Remove written chunk from buffer
    $self->emit_safe(write => substr($self->{buffer}, 0, $written, ''));
    $self->{active} = time;
  }

  # Handle drain
  $self->emit_safe('drain') if !length $self->{buffer};

  # Stop writing
  return if $self->is_writing;
  $self->iowatcher->change($handle, !$self->{paused}, 0);
}

1;
__END__

=head1 NAME

Mojo::IOLoop::Stream - Non-blocking I/O stream

=head1 SYNOPSIS

  use Mojo::IOLoop::Stream;

  # Create stream
  my $stream = Mojo::IOLoop::Stream->new($handle);
  $stream->on(read => sub {
    my ($stream, $chunk) = @_;
    ...
  });
  $stream->on(close => sub {
    my $stream = shift;
    ...
  });
  $stream->on(error => sub {
    my ($stream, $err) = @_;
    ...
  });

  # Start and stop watching for new data
  $stream->resume;
  $stream->pause;

=head1 DESCRIPTION

L<Mojo::IOLoop::Stream> is a container for I/O streams used by
L<Mojo::IOLoop>. Note that this module is EXPERIMENTAL and might change
without warning!

=head1 EVENTS

L<Mojo::IOLoop::Stream> can emit the following events.

=head2 C<close>

  $stream->on(close => sub {
    my $stream = shift;
    ...
  });

Emitted safely if the stream gets closed.

=head2 C<drain>

  $stream->on(drain => sub {
    my $stream = shift;
    ...
  });

Emitted safely once all data has been written.

=head2 C<error>

  $stream->on(error => sub {
    my ($stream, $err) = @_;
    ...
  });

Emitted safely if an error happens on the stream.

=head2 C<read>

  $stream->on(read => sub {
    my ($stream, $chunk) = @_;
    ...
  });

Emitted safely if new data arrives on the stream.

=head2 C<timeout>

  $stream->on(timeout => sub {
    my $stream = shift;
    ...
  });

Emitted safely if the stream has been inactive for too long and will get
closed automatically.

=head2 C<write>

  $stream->on(write => sub {
    my ($stream, $chunk) = @_;
    ...
  });

Emitted safely if new data has been written to the stream.

=head1 ATTRIBUTES

L<Mojo::IOLoop::Stream> implements the following attributes.

=head2 C<iowatcher>

  my $watcher = $stream->iowatcher;
  $stream     = $stream->iowatcher(Mojo::IOWatcher->new);

Low level event watcher, defaults to the C<iowatcher> attribute value of the
global L<Mojo::IOLoop> singleton.

=head2 C<timeout>

  my $timeout = $stream->timeout;
  $stream     = $stream->timeout(45);

Maximum amount of time in seconds stream can be inactive before getting
closed automatically, defaults to C<15>. Setting the value to C<0> will allow
this stream to be inactive indefinitely.

=head1 METHODS

L<Mojo::IOLoop::Stream> inherits all methods from L<Mojo::EventEmitter> and
implements the following new ones.

=head2 C<new>

  my $stream = Mojo::IOLoop::Stream->new($handle);

Construct a new L<Mojo::IOLoop::Stream> object.

=head2 C<close>

  $stream->close;

Close stream immediately.

=head2 C<handle>

  my $handle = $stream->handle;

Get handle for stream.

=head2 C<is_readable>

  my $success = $stream->is_readable;

Quick check if stream is readable, useful for identifying tainted sockets.

=head2 C<is_writing>

  my $success = $stream->is_writing;

Check if stream is writing.

=head2 C<pause>

  $stream->pause;

Stop watching for new data on the stream.

=head2 C<resume>

  $stream->resume;

Start watching for new data on the stream.

=head2 C<steal_handle>

  my $handle = $stream->steal_handle;

Steal handle from stream and prevent it from getting closed automatically.

=head2 C<write>

  $stream->write('Hello!');
  $stream->write('Hello!', sub {...});

Write data to stream, the optional drain callback will be invoked once all
data has been written.

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
