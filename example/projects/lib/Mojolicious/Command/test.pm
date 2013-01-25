package Mojolicious::Command::test;
use Mojo::Base 'Mojo::Command';

use Cwd;
use FindBin;
use File::Spec;
use Getopt::Long qw/GetOptions :config no_auto_abbrev no_ignore_case/;

has description => "Run unit tests.\n";
has usage       => <<"EOF";
usage: $0 test [OPTIONS] [TESTS]

These options are available:
  -v, --verbose   Print verbose debug information to STDERR.
EOF

# "Why, the secret ingredient was...water!
#  Yes, ordinary water, laced with nothing more than a few spoonfuls of LSD."
sub run {
  my $self = shift;

  # Options
  local @ARGV = @_;
  GetOptions('v|verbose' => sub { $ENV{HARNESS_VERBOSE} = 1 });
  my @tests = @ARGV;

  # Search tests
  unless (@tests) {
    my @base = File::Spec->splitdir(File::Spec->abs2rel($FindBin::Bin));

    # Test directory in the same directory as "mojo" (t)
    my $path = File::Spec->catdir(@base, 't');

    # Test dirctory in the directory above "mojo" (../t)
    $path = File::Spec->catdir(@base, '..', 't') unless -d $path;
    unless (-d $path) {
      say "Can't find test directory.";
      return;
    }

    # List test files
    my @dirs = ($path);
    while (my $dir = shift @dirs) {
      opendir(my $fh, $dir);
      for my $file (readdir($fh)) {
        next if $file eq '.';
        next if $file eq '..';
        my $fpath = File::Spec->catfile($dir, $file);
        push @dirs, File::Spec->catdir($dir, $file) if -d $fpath;
        push @tests,
          File::Spec->abs2rel(
          Cwd::realpath(File::Spec->catfile(File::Spec->splitdir($fpath))))
          if (-f $fpath) && ($fpath =~ /\.t$/);
      }
      closedir $fh;
    }

    $path = Cwd::realpath($path);
    say "Running tests from '$path'.";
  }

  # Run tests
  require Test::Harness;
  Test::Harness::runtests(sort @tests);
}

1;
__END__

=head1 NAME

Mojolicious::Command::test - Test command

=head1 SYNOPSIS

  use Mojolicious::Command::test;

  my $test = Mojolicious::Command::test->new;
  $test->run(@ARGV);

=head1 DESCRIPTION

L<Mojolicious::Command::test> is a test script.

=head1 ATTRIBUTES

L<Mojolicious::Command::test> inherits all attributes from L<Mojo::Command>
and implements the following new ones.

=head2 C<description>

  my $description = $test->description;
  $test           = $test->description('Foo!');

Short description of this command, used for the command list.

=head2 C<usage>

  my $usage = $test->usage;
  $test     = $test->usage('Foo!');

Usage information for this command, used for the help screen.

=head1 METHODS

L<Mojolicious::Command::test> inherits all methods from L<Mojo::Command> and
implements the following new ones.

=head2 C<run>

  $test->run(@ARGV);

Run this command.

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
