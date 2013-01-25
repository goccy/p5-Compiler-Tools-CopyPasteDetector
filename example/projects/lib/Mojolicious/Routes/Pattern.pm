package Mojolicious::Routes::Pattern;
use Mojo::Base -base;

has defaults => sub { {} };
has format   => sub {qr#\.([^/]+)$#};
has [qw/pattern regex/];
has quote_end      => ')';
has quote_start    => '(';
has relaxed_start  => '.';
has reqs           => sub { {} };
has symbol_start   => ':';
has symbols        => sub { [] };
has tree           => sub { [] };
has wildcard_start => '*';

# "This is the worst kind of discrimination. The kind against me!"
sub new { shift->SUPER::new()->parse(@_) }

sub match {
  my ($self, $path) = @_;
  my $result = $self->shape_match(\$path);
  return $result if !$path || $path eq '/';
  return;
}

sub parse {
  my ($self, $pattern) = (shift, shift);

  # Make sure we have a viable pattern
  return $self if !defined $pattern || $pattern eq '/';
  $pattern = "/$pattern" unless $pattern =~ m#^/#;

  # Requirements
  my $reqs = ref $_[0] eq 'HASH' ? $_[0] : {@_};
  $self->reqs($reqs);

  # Format in pattern
  $reqs->{format} = quotemeta($self->{strict} = $1)
    if $pattern =~ m#\.([^/\)]+)$#;

  # Tokenize
  $self->pattern($pattern);
  $self->_tokenize;

  return $self;
}

sub render {
  my ($self, $values) = @_;
  $values ||= {};

  # Merge values with defaults
  $values = {%{$self->defaults}, %$values};

  # Turn pattern into path
  my $string   = '';
  my $optional = 1;
  for my $token (reverse @{$self->tree}) {
    my $op       = $token->[0];
    my $rendered = '';

    # Slash
    if ($op eq 'slash') { $rendered = '/' unless $optional }

    # Text
    elsif ($op eq 'text') {
      $rendered = $token->[1];
      $optional = 0;
    }

    # Relaxed, symbol or wildcard
    elsif ($op ~~ [qw/relaxed symbol wildcard/]) {
      my $name = $token->[1];
      $rendered = $values->{$name} // '';
      my $default = $self->defaults->{$name};
      if (!defined $default || ($default ne $rendered)) { $optional = 0 }
      elsif ($optional) { $rendered = '' }
    }

    $string = "$rendered$string";
  }

  return $string || '/';
}

sub shape_match {
  my ($self, $pathref, $detect) = @_;

  # Compile on demand
  my $regex;
  $regex = $self->_compile unless $regex = $self->regex;

  # Match
  return unless my @captures = $$pathref =~ $regex;
  $$pathref =~ s/($regex)//;

  # Merge captures
  my $result = {%{$self->defaults}};
  for my $symbol (@{$self->symbols}) {
    last unless @captures;
    my $capture = shift @captures;
    $result->{$symbol} = $capture if defined $capture;
  }

  # Format
  $result->{format} ||= $self->{strict} if $detect && exists $self->{strict};
  my $req = $self->reqs->{format};
  return $result if defined $req && !$req;
  my $format = $self->format;
  if ($detect && $$pathref =~ s|^/?$format||) { $result->{format} ||= $1 }
  elsif ($req) { return if !$result->{format} }

  return $result;
}

sub _compile {
  my $self = shift;

  # Compile format regular expression
  my $reqs = $self->reqs;
  if (!exists $reqs->{format} || $reqs->{format}) {
    my $format =
      defined $reqs->{format} ? _compile_req($reqs->{format}) : '([^/]+)';
    $self->format(qr#\.$format$#);
  }

  # Compile tree to regular expression
  my $block    = '';
  my $regex    = '';
  my $optional = 1;
  my $defaults = $self->defaults;
  for my $token (reverse @{$self->tree}) {
    my $op       = $token->[0];
    my $compiled = '';

    # Slash
    if ($op eq 'slash') {

      # Full block
      $block = $optional ? "(?:/$block)?" : "/$block";
      $regex = "$block$regex";
      $block = '';
      next;
    }

    # Text
    elsif ($op eq 'text') {
      $compiled = quotemeta $token->[1];
      $optional = 0;
    }

    # Symbol
    elsif ($op ~~ [qw/relaxed symbol wildcard/]) {
      my $name = $token->[1];
      unshift @{$self->symbols}, $name;

      # Relaxed
      if ($op eq 'relaxed') { $compiled = '([^\/]+)' }

      # Symbol
      elsif ($op eq 'symbol') { $compiled = '([^\/\.]+)' }

      # Wildcard
      elsif ($op eq 'wildcard') { $compiled = '(.+)' }

      # Custom regex
      my $req = $reqs->{$name};
      $compiled = _compile_req($req) if $req;

      # Optional placeholder
      $optional = 0 unless exists $defaults->{$name};
      $compiled .= '?' if $optional;
    }

    # Add to block
    $block = "$compiled$block";
  }

  # Not rooted with a slash
  $regex = "$block$regex" if $block;

  # Compile
  $regex = qr/^$regex/;
  $self->regex($regex);

  return $regex;
}

# "Interesting... Oh no wait, the other thing, tedious."
sub _compile_req {
  my $req = shift;
  return "($req)" if !ref $req || ref $req ne 'ARRAY';
  return '(' . join('|', map {quotemeta} reverse sort @$req) . ')';
}

sub _tokenize {
  my $self = shift;

  # Token
  my $quote_end      = $self->quote_end;
  my $quote_start    = $self->quote_start;
  my $relaxed_start  = $self->relaxed_start;
  my $symbol_start   = $self->symbol_start;
  my $wildcard_start = $self->wildcard_start;

  # Parse the pattern character wise
  my $pattern = $self->pattern;
  my $tree    = [];
  my $state   = 'text';
  my $quoted  = 0;
  while (length(my $char = substr $pattern, 0, 1, '')) {

    # Inside a symbol
    my $symbol = $state ~~ [qw/relaxed symbol wildcard/] ? 1 : 0;

    # Quote start
    if ($char eq $quote_start) {
      $quoted = 1;
      $state  = 'symbol';
      push @$tree, ['symbol', ''];
      next;
    }

    # Symbol start
    if ($char eq $symbol_start) {
      push @$tree, ['symbol', ''] if $state ne 'symbol';
      $state = 'symbol';
      next;
    }

    # Relaxed start (needs to be quoted)
    if ($quoted && $char eq $relaxed_start && $state eq 'symbol') {
      $state = 'relaxed';
      $tree->[-1]->[0] = 'relaxed';
      next;
    }

    # Wildcard start (upgrade when quoted)
    if ($char eq $wildcard_start) {
      push @$tree, ['symbol', ''] unless $quoted;
      $state = 'wildcard';
      $tree->[-1]->[0] = 'wildcard';
      next;
    }

    # Quote end
    if ($char eq $quote_end) {
      $quoted = 0;
      $state  = 'text';
      next;
    }

    # Slash
    if ($char eq '/') {
      push @$tree, ['slash'];
      $state = 'text';
      next;
    }

    # Relaxed, symbol or wildcard
    elsif ($symbol && $char =~ /\w/) {
      $tree->[-1]->[-1] .= $char;
      next;
    }

    # Text
    else {
      $state = 'text';

      # New text element
      unless ($tree->[-1]->[0] eq 'text') {
        push @$tree, ['text', $char];
        next;
      }

      # More text
      $tree->[-1]->[-1] .= $char;
    }
  }
  $self->tree($tree);

  return $self;
}

1;
__END__

=head1 NAME

Mojolicious::Routes::Pattern - Routes pattern engine

=head1 SYNOPSIS

  use Mojolicious::Routes::Pattern;

  # Create pattern
  my $pattern = Mojolicious::Routes::Pattern->new('/test/:name');

  # Match routes
  my $result  = $pattern->match('/test/sebastian');
  say $result->{name};

=head1 DESCRIPTION

L<Mojolicious::Routes::Pattern> is the core of L<Mojolicious::Routes>.

=head1 ATTRIBUTES

L<Mojolicious::Routes::Pattern> implements the following attributes.

=head2 C<defaults>

  my $defaults = $pattern->defaults;
  $pattern     = $pattern->defaults({foo => 'bar'});

Default parameters.

=head2 C<format>

  my $regex = $pattern->format;
  $pattern  = $pattern->format($regex);

Compiled regex for format matching, defaults to C<\.([^/]+)$>. Note that this
attribute is EXPERIMENTAL and might change without warning!

=head2 C<pattern>

  my $pattern = $pattern->pattern;
  $pattern    = $pattern->pattern('/(foo)/(bar)');

Raw unparsed pattern.

=head2 C<quote_end>

  my $quote = $pattern->quote_end;
  $pattern  = $pattern->quote_end(']');

Character indicating the end of a quoted placeholder, defaults to C<)>.

=head2 C<quote_start>

  my $quote = $pattern->quote_start;
  $pattern  = $pattern->quote_start('[');

Character indicating the start of a quoted placeholder, defaults to C<(>.

=head2 C<regex>

  my $regex = $pattern->regex;
  $pattern  = $pattern->regex($regex);

Pattern in compiled regex form.

=head2 C<relaxed_start>

  my $relaxed = $pattern->relaxed_start;
  $pattern    = $pattern->relaxed_start('*');

Character indicating a relaxed placeholder, defaults to C<.>.

=head2 C<reqs>

  my $reqs = $pattern->reqs;
  $pattern = $pattern->reqs({foo => qr/\w+/});

Regex constraints.

=head2 C<symbol_start>

  my $symbol = $pattern->symbol_start;
  $pattern   = $pattern->symbol_start(':');

Character indicating a placeholder, defaults to C<:>.

=head2 C<symbols>

  my $symbols = $pattern->symbols;
  $pattern    = $pattern->symbols(['foo', 'bar']);

Placeholder names.

=head2 C<tree>

  my $tree = $pattern->tree;
  $pattern = $pattern->tree([ ... ]);

Pattern in parsed form.

=head2 C<wildcard_start>

  my $wildcard = $pattern->wildcard_start;
  $pattern     = $pattern->wildcard_start('*');

Character indicating the start of a wildcard placeholder, defaults to C<*>.

=head1 METHODS

L<Mojolicious::Routes::Pattern> inherits all methods from L<Mojo::Base> and
implements the following ones.

=head2 C<new>

  my $pattern = Mojolicious::Routes::Pattern->new('/:controller/:action',
    action => qr/\w+/
  );

Construct a new pattern object.

=head2 C<match>

  my $result = $pattern->match('/foo/bar');

Match pattern against a path.

=head2 C<parse>

  $pattern = $pattern->parse('/:controller/:action', action => qr/\w+/);

Parse a raw pattern.

=head2 C<render>

  my $path = $pattern->render({action => 'foo'});

Render pattern into a path with parameters.

=head2 C<shape_match>

  my $result = $pattern->shape_match(\$path);
  my $result = $pattern->shape_match(\$path, $detect);

Match pattern against a path and remove matching parts.

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
