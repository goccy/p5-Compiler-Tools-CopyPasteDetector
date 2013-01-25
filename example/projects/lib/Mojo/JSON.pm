package Mojo::JSON;
use Mojo::Base -base;

use B;
use Mojo::Util;

has 'error';

# Literal names
our $FALSE = Mojo::JSON::_Bool->new(0);
our $TRUE  = Mojo::JSON::_Bool->new(1);

my $BOM_RE = qr/
  (?:
  \357\273\277   # UTF-8
  |
  \377\376\0\0   # UTF-32LE
  |
  \0\0\376\377   # UTF-32BE
  |
  \376\377       # UTF-16BE
  |
  \377\376       # UTF-16LE
  )
/x;
my $WHITESPACE_RE = qr/[\x20\x09\x0a\x0d]*/;

# Escaped special character map (with u2028 and u2029)
my %ESCAPE = (
  '"'     => '"',
  '\\'    => '\\',
  '/'     => '/',
  'b'     => "\x07",
  'f'     => "\x0C",
  'n'     => "\x0A",
  'r'     => "\x0D",
  't'     => "\x09",
  'u2028' => "\x{2028}",
  'u2029' => "\x{2029}"
);
my %REVERSE;
for (0x00 .. 0x1F, 0x7F) { $REVERSE{pack 'C', $_} = sprintf '\u%.4X', $_ }
for my $key (keys %ESCAPE) { $REVERSE{$ESCAPE{$key}} = "\\$key" }

# Unicode encoding detection
my $UTF_PATTERNS = {
  "\0\0\0[^\0]"    => 'UTF-32BE',
  "\0[^\0]\0[^\0]" => 'UTF-16BE',
  "[^\0]\0\0\0"    => 'UTF-32LE',
  "[^\0]\0[^\0]\0" => 'UTF-16LE'
};

# "Hey...That's not the wallet inspector..."
sub decode {
  my ($self, $string) = @_;

  # Cleanup
  $self->error(undef);

  # Missing input
  $self->error('Missing or empty input.') and return unless $string;

  # Remove BOM
  $string =~ s/^$BOM_RE//g;

  # Wide characters
  $self->error('Wide character in input.') and return
    unless utf8::downgrade($string, 1);

  # Detect and decode unicode
  my $encoding = 'UTF-8';
  for my $pattern (keys %$UTF_PATTERNS) {
    if ($string =~ /^$pattern/) {
      $encoding = $UTF_PATTERNS->{$pattern};
      last;
    }
  }
  $string = Mojo::Util::decode $encoding, $string;

  # Object or array
  my $res = eval {
    local $_ = $string;

    # Leading whitespace
    m/\G$WHITESPACE_RE/xgc;

    # Array
    my $ref;
    if (m/\G\[/gc) { $ref = _decode_array() }

    # Object
    elsif (m/\G\{/gc) { $ref = _decode_object() }

    # Unexpected
    else { _exception('Expected array or object') }

    # Leftover data
    unless (m/\G$WHITESPACE_RE\z/xgc) {
      my $got = ref $ref eq 'ARRAY' ? 'array' : 'object';
      _exception("Unexpected data after $got");
    }

    $ref;
  };

  # Exception
  if (!$res && (my $e = $@)) {
    chomp $e;
    $self->error($e);
  }

  return $res;
}

sub encode {
  my ($self, $ref) = @_;
  return Mojo::Util::encode 'UTF-8', _encode_values($ref);
}

sub false {$FALSE}
sub true  {$TRUE}

sub _decode_array {
  my @array;
  until (m/\G$WHITESPACE_RE\]/xgc) {

    # Value
    push @array, _decode_value();

    # Separator
    redo if m/\G$WHITESPACE_RE,/xgc;

    # End
    last if m/\G$WHITESPACE_RE\]/xgc;

    # Invalid character
    _exception('Expected comma or right square bracket while parsing array');
  }

  return \@array;
}

sub _decode_object {
  my %hash;
  until (m/\G$WHITESPACE_RE\}/xgc) {

    # Quote
    m/\G$WHITESPACE_RE"/xgc
      or _exception('Expected string while parsing object');

    # Key
    my $key = _decode_string();

    # Colon
    m/\G$WHITESPACE_RE:/xgc
      or _exception('Expected colon while parsing object');

    # Value
    $hash{$key} = _decode_value();

    # Separator
    redo if m/\G$WHITESPACE_RE,/xgc;

    # End
    last if m/\G$WHITESPACE_RE\}/xgc;

    # Invalid character
    _exception('Expected comma or right curly bracket while parsing object');
  }

  return \%hash;
}

sub _decode_string {
  my $pos = pos;

  # Extract string with escaped characters
  m#\G(((?:[^\x00-\x1F\\"]|\\(?:["\\/bfnrt]|u[A-Fa-f0-9]{4})){0,32766})*)#gc;
  my $str = $1;

  # Missing quote
  unless (m/\G"/gc) {
    _exception('Unexpected character or invalid escape while parsing string')
      if m/\G[\x00-\x1F\\]/x;
    _exception('Unterminated string');
  }

  # Unescape popular characters
  if (index($str, '\\u') < 0) {
    $str =~ s|\\(["\\/bfnrt])|$ESCAPE{$1}|gs;
    return $str;
  }

  # Unescape everything else
  my $buffer = '';
  while ($str =~ m/\G([^\\]*)\\(?:([^u])|u(.{4}))/gc) {
    $buffer .= $1;

    # Popular character
    if ($2) { $buffer .= $ESCAPE{$2} }

    # Escaped
    else {
      my $ord = hex $3;

      # Surrogate pair
      if (($ord & 0xF800) == 0xD800) {

        # High surrogate
        ($ord & 0xFC00) == 0xD800
          or pos($_) = $pos + pos($str),
          _exception('Missing high-surrogate');

        # Low surrogate
        $str =~ m/\G\\u([Dd][C-Fc-f]..)/gc
          or pos($_) = $pos + pos($str),
          _exception('Missing low-surrogate');

        # Pair
        $ord = 0x10000 + ($ord - 0xD800) * 0x400 + (hex($1) - 0xDC00);
      }

      # Character
      $buffer .= pack 'U', $ord;
    }
  }

  # The rest
  $buffer .= substr $str, pos($str), length($str);

  return $buffer;
}

# "Eternity with nerds.
#  It's the Pasadena Star Trek convention all over again."
sub _decode_value {

  # Leading whitespace
  m/\G$WHITESPACE_RE/xgc;

  # String
  return _decode_string() if m/\G"/gc;

  # Array
  return _decode_array() if m/\G\[/gc;

  # Object
  return _decode_object() if m/\G\{/gc;

  # Number
  return 0 + $1
    if m/\G([-]?(?:0|[1-9][0-9]*)(?:\.[0-9]*)?(?:[eE][+-]?[0-9]+)?)/gc;

  # True
  return $TRUE if m/\Gtrue/gc;

  # False
  return $FALSE if m/\Gfalse/gc;

  # Null
  return undef if m/\Gnull/gc;

  # Invalid data
  _exception('Expected string, array, object, number, boolean or null');
}

sub _encode_array {
  my $array = shift;

  # Values
  my @array;
  for my $value (@$array) {
    push @array, _encode_values($value);
  }

  # Stringify
  my $string = join ',', @array;
  return "[$string]";
}

sub _encode_object {
  my $object = shift;

  # Values
  my @values;
  for my $key (keys %$object) {
    my $name  = _encode_string($key);
    my $value = _encode_values($object->{$key});
    push @values, "$name:$value";
  }

  # Stringify
  my $string = join ',', @values;
  return "{$string}";
}

sub _encode_string {
  my $string = shift;

  # Escape string
  $string
    =~ s|([\x00-\x1F\x7F\x{2028}\x{2029}\\"/\b\f\n\r\t])|$REVERSE{$1}|gs;

  # Stringify
  return "\"$string\"";
}

sub _encode_values {
  my $value = shift;

  # Reference
  if (my $ref = ref $value) {

    # Array
    return _encode_array($value) if $ref eq 'ARRAY';

    # Object
    return _encode_object($value) if $ref eq 'HASH';
  }

  # "null"
  return 'null' unless defined $value;

  # "false"
  return 'false' if ref $value eq 'Mojo::JSON::_Bool' && !$value;

  # "true"
  return 'true' if ref $value eq 'Mojo::JSON::_Bool' && $value;

  # Number
  my $flags = B::svref_2object(\$value)->FLAGS;
  return $value
    if $flags & (B::SVp_IOK | B::SVp_NOK) && !($flags & B::SVp_POK);

  # String
  _encode_string($value);
}

sub _exception {

  # Leading whitespace
  m/\G$WHITESPACE_RE/xgc;

  # Context
  my $context = 'Malformed JSON: ' . shift;
  if (m/\G\z/gc) { $context .= ' before end of data' }
  else {
    my @lines = split /\n/, substr($_, 0, pos);
    $context .= ' at line ' . @lines . ', offset ' . length(pop @lines || '');
  }

  # Throw
  die "$context.\n";
}

# Emulate boolean type
package Mojo::JSON::_Bool;
use Mojo::Base -base;
use overload
  '0+'     => sub { $_[0]->{value} },
  '""'     => sub { $_[0]->{value} },
  fallback => 1;

sub new { shift->SUPER::new(value => shift) }

1;
__END__

=head1 NAME

Mojo::JSON - Minimalistic JSON

=head1 SYNOPSIS

  use Mojo::JSON;

  my $json   = Mojo::JSON->new;
  my $string = $json->encode({foo => [1, 2], bar => 'hello!'});
  my $hash   = $json->decode('{"foo": [3, -2, 1]}');

=head1 DESCRIPTION

L<Mojo::JSON> is a minimalistic and relaxed implementation of RFC 4627. While
it is possibly the fastest pure-Perl JSON parser available, you should not
use it for validation.

It supports normal Perl data types like C<Scalar>, C<Array>, C<Hash> and will
try to stringify blessed references.

  [1, -2, 3]     -> [1, -2, 3]
  {"foo": "bar"} -> {foo => 'bar'}

Literal names will be translated to and from L<Mojo::JSON> constants or a
similar native Perl value.

  true  -> Mojo::JSON->true
  false -> Mojo::JSON->false
  null  -> undef

Decoding UTF-16 (LE/BE) and UTF-32 (LE/BE) will be handled transparently,
encoding will only generate UTF-8. The two unicode whitespace characters
C<u2028> and C<u2029> will always be escaped to make JSONP easier.

=head1 ATTRIBUTES

L<Mojo::JSON> implements the following attributes.

=head2 C<error>

  my $err = $json->error;
  $json   = $json->error('Oops!');

Parser errors.

=head1 METHODS

L<Mojo::JSON> inherits all methods from L<Mojo::Base> and implements the
following new ones.

=head2 C<decode>

  my $array = $json->decode('[1, 2, 3]');
  my $hash  = $json->decode('{"foo": "bar"}');

Decode JSON string.

=head2 C<encode>

  my $string = $json->encode({foo => 'bar'});

Encode Perl structure.

=head2 C<false>

  my $false = Mojo::JSON->false;
  my $false = $json->false;

False value, used because Perl has no native equivalent.

=head2 C<true>

  my $true = Mojo::JSON->true;
  my $true = $json->true;

True value, used because Perl has no native equivalent.

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
