package Mojo::Exception;
use Mojo::Base -base;
use overload
  'bool'   => sub {1},
  '""'     => sub { shift->to_string },
  fallback => 1;

use IO::File;
use Scalar::Util 'blessed';

has [qw/frames line lines_before lines_after/] => sub { [] };
has [qw/message raw_message/] => 'Exception!';
has verbose => sub { $ENV{MOJO_EXCEPTION_VERBOSE} || 0 };

# "Attempted murder? Now honestly, what is that?
#  Do they give a Nobel Prize for attempted chemistry?"
sub new {
  my $self = shift->SUPER::new();
  return @_ ? $self->_detect(@_) : $self;
}

sub throw { die shift->new->trace(2)->_detect(@_) }

sub to_string {
  my $self = shift;

  # Message
  return $self->message unless $self->verbose;
  my $string = '';
  $string .= $self->message if $self->message;

  # Before
  $string .= $_->[0] . ': ' . $_->[1] . "\n" for @{$self->lines_before};

  # Line
  $string .= ($self->line->[0] . ': ' . $self->line->[1] . "\n")
    if $self->line->[0];

  # After
  $string .= $_->[0] . ': ' . $_->[1] . "\n" for @{$self->lines_after};

  return $string;
}

sub trace {
  my ($self, $start) = @_;
  $start //= 1;
  my @frames;
  while (my @trace = caller($start++)) { push @frames, \@trace }
  $self->frames(\@frames);
  return $self;
}

sub _detect {
  my $self = shift;

  # Message
  my $message = shift;
  return $message if blessed $message && $message->isa('Mojo::Exception');
  $self->message($message)->raw_message($message);

  # Extract file and line from message
  my @trace;
  while ($message =~ /at\s+(.+?)\s+line\s+(\d+)/g) { push @trace, [$1, $2] }

  # Extract file and line from stacktrace
  if (my $first = $self->frames->[0]) {
    unshift @trace, [$first->[1], $first->[2]] if $first->[1];
  }

  # Search for context
  foreach my $frame (reverse @trace) {
    next unless -r $frame->[0];
    my $handle = IO::File->new($frame->[0], '<:utf8');
    $self->_parse_context($frame->[1], [[<$handle>]]);
    return $self;
  }

  # More context
  return $self unless my $files = shift;
  my @lines = map { [split /\n/] } @$files;

  # Fix file in message
  return $self unless my $name = shift;
  unless (ref $message) {
    my $filter = sub {
      my $num  = shift;
      my $new  = "$name line $num";
      my $line = $lines[0]->[$num];
      $new .= qq/, near "$line"/ if defined $line;
      return $new .= '.';
    };
    $message =~ s/\(eval\s+\d+\) line (\d+).*/$filter->($1)/ge;
    $self->message($message);
  }

  # Search for better context
  $name = quotemeta $name;
  my $line;
  if ($self->message =~ /at\s+$name\s+line\s+(\d+)/) { $line = $1 }
  else {
    for my $frame (@{$self->frames}) {
      next unless $frame->[1] =~ /^\(eval\ \d+\)$/;
      $line = $frame->[2];
      last;
    }
  }
  $self->_parse_context($line, \@lines) if $line;

  return $self;
}

# "You killed zombie Flanders!
#  He was a zombie?"
sub _parse_context {
  my ($self, $line, $lines) = @_;

  # Wrong file
  return unless defined $lines->[0]->[$line - 1];

  # Line
  $self->line([$line]);
  for my $l (@$lines) {
    chomp(my $code = $l->[$line - 1]);
    push @{$self->line}, $code;
  }

  # Before
  for my $i (2 .. 6) {
    last if ((my $previous = $line - $i) < 0);
    if (defined($lines->[0]->[$previous])) {
      unshift @{$self->lines_before}, [$previous + 1];
      for my $l (@$lines) {
        chomp(my $code = $l->[$previous]);
        push @{$self->lines_before->[0]}, $code;
      }
    }
  }

  # After
  for my $i (0 .. 4) {
    next if ((my $next = $line + $i) < 0);
    if (defined($lines->[0]->[$next])) {
      push @{$self->lines_after}, [$next + 1];
      for my $l (@$lines) {
        next unless defined(my $code = $l->[$next]);
        chomp $code;
        push @{$self->lines_after->[-1]}, $code;
      }
    }
  }

  return $self;
}

1;
__END__

=head1 NAME

Mojo::Exception - Exceptions with context

=head1 SYNOPSIS

  use Mojo::Exception;

  my $e = Mojo::Exception->new;

=head1 DESCRIPTION

L<Mojo::Exception> is a container for exceptions with context information.

=head1 ATTRIBUTES

L<Mojo::Exception> implements the following attributes.

=head2 C<frames>

  my $frames = $e->frames;
  $e         = $e->frames($frames);

Stacktrace.

=head2 C<line>

  my $line = $e->line;
  $e       = $e->line([3, 'foo']);

The line where the exception occured.

=head2 C<lines_after>

  my $lines = $e->lines_after;
  $e        = $e->lines_after([[1, 'bar'], [2, 'baz']]);

Lines after the line where the exception occured.

=head2 C<lines_before>

  my $lines = $e->lines_before;
  $e        = $e->lines_before([[4, 'bar'], [5, 'baz']]);

Lines before the line where the exception occured.

=head2 C<message>

  my $message = $e->message;
  $e          = $e->message('Oops!');

Exception message.

=head2 C<raw_message>

  my $message = $e->raw_message;
  $e          = $e->raw_message('Oops!');

Raw unprocessed exception message.

=head2 C<verbose>

  my $verbose = $e->verbose;
  $e          = $e->verbose(1);

Activate verbose rendering, defaults to the value of
C<MOJO_EXCEPTION_VERBOSE> or C<0>.

=head1 METHODS

L<Mojo::Exception> inherits all methods from L<Mojo::Base> and implements the
following new ones.

=head2 C<new>

  my $e = Mojo::Exception->new('Oops!');
  my $e = Mojo::Exception->new('Oops!', $files, $name);

Construct a new L<Mojo::Exception> object.

=head2 C<throw>

  Mojo::Exception->throw('Oops!');
  Mojo::Exception->throw('Oops!', $files, $name);

Throw exception with stacktrace.

=head2 C<to_string>

  my $string = $e->to_string;
  my $string = "$e";

Render exception with context.

=head2 C<trace>

  $e = $e->trace;
  $e = $e->trace(2);

Store stacktrace.

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
