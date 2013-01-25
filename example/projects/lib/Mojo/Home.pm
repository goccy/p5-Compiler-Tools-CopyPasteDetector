package Mojo::Home;
use Mojo::Base -base;
use overload
  'bool'   => sub {1},
  '""'     => sub { shift->to_string },
  fallback => 1;

use Cwd 'abs_path';
use File::Basename 'dirname';
use File::Find 'find';
use File::Spec;
use FindBin;
use List::Util 'first';
use Mojo::Asset::File;
use Mojo::Command;
use Mojo::Loader;

has app_class => 'Mojo::HelloWorld';

# "I'm normally not a praying man, but if you're up there,
#  please save me Superman."
sub new { shift->SUPER::new()->parse(@_) }

sub detect {
  my ($self, $class) = @_;

  # Class
  $self->app_class($class) if $class;
  $class ||= $self->app_class;

  # Environment variable
  if ($ENV{MOJO_HOME}) {
    my @parts = File::Spec->splitdir(abs_path $ENV{MOJO_HOME});
    $self->{parts} = \@parts;
    return $self;
  }

  # Try to find home from lib directory
  if ($class) {

    # Load
    my $file = Mojo::Command->class_to_path($class);
    unless ($INC{$file}) {
      if (my $e = Mojo::Loader->load($class)) { die $e if ref $e }
    }

    # Detect
    if (my $path = $INC{$file}) {

      # Directory
      $path =~ s/$file$//;
      my @home = File::Spec->splitdir($path);

      # Remove "lib" and "blib"
      while (@home) {
        last unless $home[-1] =~ /^b?lib$/ || $home[-1] eq '';
        pop @home;
      }

      # Turn into absolute path
      $self->{parts} =
        [File::Spec->splitdir(abs_path(File::Spec->catdir(@home) || '.'))];
    }
  }

  # FindBin fallback
  $self->{parts} = [split /\//, $FindBin::Bin] unless $self->{parts};

  return $self;
}

sub lib_dir {
  my $self = shift;

  # Directory found
  my $parts = $self->{parts} || [];
  my $path = File::Spec->catdir(@$parts, 'lib');
  return $path if -d $path;

  # No lib directory
  return;
}

sub list_files {
  my ($self, $dir) = @_;

  # Files relative to directory
  my $parts = $self->{parts} || [];
  my $root = File::Spec->catdir(@$parts);
  $dir = File::Spec->catdir($root, split '/', ($dir || ''));
  return [] unless -d $dir;
  my @files;
  find {
    wanted => sub {
      my @parts =
        File::Spec->splitdir(File::Spec->abs2rel($File::Find::name, $dir));
      push @files, join '/', @parts unless first {/^\./} @parts;
    },
    no_chdir => 1
  }, $dir;

  return [sort @files];
}

sub mojo_lib_dir { File::Spec->catdir(dirname(__FILE__), '..') }

# "Don't worry, son.
#  I'm sure he's up in heaven right now laughing it up with all the other
#  celebrities: John Dilinger, Ty Cobb, Joseph Stalin."
sub parse {
  my ($self, $path) = @_;
  return $self unless defined $path;
  $self->{parts} = [File::Spec->splitdir($path)];
  return $self;
}

sub rel_dir {
  my $self = shift;
  my $parts = $self->{parts} || [];
  File::Spec->catdir(@$parts, split '/', shift);
}

sub rel_file {
  my $self = shift;
  my $parts = $self->{parts} || [];
  File::Spec->catfile(@$parts, split '/', shift);
}

sub slurp_rel_file {
  Mojo::Asset::File->new(path => shift->rel_file(@_))->slurp;
}

sub to_string {
  my $self = shift;
  my $parts = $self->{parts} || [];
  File::Spec->catdir(@$parts);
}

1;
__END__

=head1 NAME

Mojo::Home - Detect and access the project root directory in Mojo

=head1 SYNOPSIS

  use Mojo::Home;

  my $home = Mojo::Home->new;
  $home->detect;

=head1 DESCRIPTION

L<Mojo::Home> is a container for home directories.

=head1 ATTRIBUTES

L<Mojo::Home> implements the following attributes.

=head2 C<app_class>

  my $class = $home->app_class;
  $home     = $home->app_class('Foo::Bar');

Application class.

=head1 METHODS

L<Mojo::Home> inherits all methods from L<Mojo::Base> and implements the
following new ones.

=head2 C<new>

  my $home = Mojo::Home->new;
  my $home = Mojo::Home->new('/foo/bar');

Construct a new L<Mojo::Home> object.

=head2 C<detect>

  $home = $home->detect;
  $home = $home->detect('My::App');

Detect home directory from application class.

=head2 C<lib_dir>

  my $path = $home->lib_dir;

Path to C<lib> directory.

=head2 C<list_files>

  my $files = $home->list_files;
  my $files = $home->list_files('foo/bar');

List all files in directory and subdirectories recursively.

=head2 C<mojo_lib_dir>

  my $path = $home->mojo_lib_dir;

Path to C<lib> directory in which L<Mojolicious> is installed.

=head2 C<parse>

  $home = $home->parse('/foo/bar');

Parse path.

=head2 C<rel_dir>

  my $path = $home->rel_dir('foo/bar');

Generate absolute path for relative directory.

=head2 C<rel_file>

  my $path = $home->rel_file('foo/bar.html');

Generate absolute path for relative file.

=head2 C<slurp_rel_file>

  my $string = $home->slurp_rel_file('foo/bar.html');

Read all file data at once.

=head2 C<to_string>

  my $string = $home->to_string;
  my $string = "$home";

Home directory.

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
