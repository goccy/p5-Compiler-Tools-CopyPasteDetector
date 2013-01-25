package Mojolicious::Types;
use Mojo::Base -base;

# "Once again, the conservative, sandwich-heavy portfolio pays off for the
#  hungry investor."
has types => sub {
  return {
    atom => 'application/atom+xml',
    bin  => 'application/octet-stream',
    css  => 'text/css',
    gif  => 'image/gif',
    gz   => 'application/gzip',
    htm  => 'text/html',
    html => 'text/html;charset=UTF-8',
    ico  => 'image/x-icon',
    jpeg => 'image/jpeg',
    jpg  => 'image/jpeg',
    js   => 'application/x-javascript',
    json => 'application/json',
    mp3  => 'audio/mpeg',
    png  => 'image/png',
    rss  => 'application/rss+xml',
    svg  => 'image/svg+xml',
    tar  => 'application/x-tar',
    txt  => 'text/plain',
    woff => 'application/x-font-woff',
    xml  => 'text/xml',
    xsl  => 'text/xml',
    zip  => 'application/zip'
  };
};

# "Magic. Got it."
sub detect {
  my ($self, $accept) = @_;

  # Detect extensions from MIME type
  return [] unless (($accept || '') =~ /^([^,]+?)(?:\;[^,]*)*$/);
  my $pattern = $1;
  my @exts;
  my $types = $self->types;
  for my $ext (sort keys %$types) {
    my $type = quotemeta $types->{$ext};
    $type =~ s/\\\;.*$//;
    push @exts, $ext if $pattern =~ /^$type$/i;
  }

  return \@exts;
}

sub type {
  my ($self, $ext, $type) = @_;
  if ($type) {
    $self->types->{$ext} = $type;
    return $self;
  }
  return $self->types->{$ext || ''};
}

1;
__END__

=head1 NAME

Mojolicious::Types - MIME types

=head1 SYNOPSIS

  use Mojolicious::Types;

  my $types = Mojolicious::Types->new;

=head1 DESCRIPTION

L<Mojolicious::Types> is a container for MIME types.

=head1 ATTRIBUTES

L<Mojolicious::Types> implements the following attributes.

=head2 C<types>

  my $map = $types->types;
  $types  = $types->types({png => 'image/png'});

List of MIME types.

=head1 METHODS

L<Mojolicious::Types> inherits all methods from L<Mojo::Base> and implements the
following ones.

=head2 C<detect>

  my $extensions = $types->detect('application/json;q=9');

Detect file extensions from C<Accept> header value. Note that this method is
EXPERIMENTAL and might change without warning!

=head2 C<type>

  my $type = $types->type('png');
  $types   = $types->type(png => 'image/png');

Get or set MIME type for file extension.

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
