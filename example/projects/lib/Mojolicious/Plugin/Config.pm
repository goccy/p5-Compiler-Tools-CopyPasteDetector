package Mojolicious::Plugin::Config;
use Mojo::Base 'Mojolicious::Plugin';

use File::Basename 'basename';
use File::Spec;
use Mojo::Util 'decamelize';

use constant DEBUG => $ENV{MOJO_CONFIG_DEBUG} || 0;

# "Who are you, my warranty?!"
sub load {
  my ($self, $file, $conf, $app) = @_;
  $app->log->debug(qq/Reading config file "$file"./);

  # Slurp UTF-8 file
  open my $handle, "<:encoding(UTF-8)", $file
    or die qq/Couldn't open config file "$file": $!/;
  my $content = do { local $/; <$handle> };

  # Process
  return $self->parse($content, $file, $conf, $app);
}

sub parse {
  my ($self, $content, $file, $conf, $app) = @_;

  # Run Perl code
  no warnings;
  die qq/Couldn't parse config file "$file": $@/
    unless my $config = eval "sub app { \$app }; $content";
  die qq/Config file "$file" did not return a hashref.\n/
    unless ref $config && ref $config eq 'HASH';

  return $config;
}

sub register {
  my ($self, $app, $conf) = @_;
  $conf ||= {};

  # Config file
  my $file = $conf->{file} || $ENV{MOJO_CONFIG};
  unless ($file) {
    $file = $ENV{MOJO_APP};

    # Class
    if ($file && !ref $file) { $file = decamelize $file }

    # File
    else { $file = basename($ENV{MOJO_EXE} || $0) }

    # Remove .pl and .t extentions
    $file =~ s/\.(?:pl|t)$//i;

    # Default extension
    $file .= '.' . ($conf->{ext} || 'conf');
  }
  warn "CONFIG FILE $file\n" if DEBUG;

  # Mode specific config file
  my $mode;
  if ($file =~ /^(.*)\.([^\.]+)$/) {
    $mode = join '.', $1, $app->mode, $2;
    warn "MODE SPECIFIC CONFIG FILE $mode\n" if DEBUG;
  }

  # Absolute path
  $file = $app->home->rel_file($file)
    unless File::Spec->file_name_is_absolute($file);
  $mode = $app->home->rel_file($mode)
    if defined $mode && !File::Spec->file_name_is_absolute($mode);

  # Read config file
  my $config = {};
  if (-e $file) { $config = $self->load($file, $conf, $app) }

  # Check for default
  else {

    # All missing
    die qq/Config file "$file" missing, maybe you need to create it?\n/
      unless $conf->{default};
    $app->log->debug(qq/Config file "$file" missing, using default config./);
  }

  # Merge everything
  $config = {%$config, %{$self->load($mode, $conf, $app)}}
    if defined $mode && -e $mode;
  $config = {%{$conf->{default}}, %$config} if $conf->{default};

  # Add "config" helper
  $app->helper(
    config => sub {
      my $self = shift;
      return $config unless @_;
      $config->{$_[0]};
    }
  );

  # Add default stash value
  $app->defaults(($conf->{stash_key} || 'config') => $config);

  return $config;
}

1;
__END__

=head1 NAME

Mojolicious::Plugin::Config - Perl-ish configuration plugin

=head1 SYNOPSIS

  # myapp.conf
  {
    foo       => "bar",
    music_dir => app->home->rel_dir('music')
  };

  # Mojolicious
  my $config = $self->plugin('Config');

  # Mojolicious::Lite
  my $config = plugin 'Config';

  # Reads myapp.conf by default and puts the parsed version into the stash
  my $config = $self->stash('config');

  # Everything can be customized with options
  my $config = plugin Config => {
    file      => '/etc/myapp.stuff',
    stash_key => 'conf'
  };

=head1 DESCRIPTION

L<Mojolicious::Plugin::Config> is a Perl-ish configuration plugin. The
application object can be accessed via the C<app> helper. You can extend the
normal config file C<myapp.conf> with C<mode> specific ones like
C<myapp.$mode.conf>.

=head1 OPTIONS

L<Mojolicious::Plugin::Config> supports the following options.

=head2 C<default>

  # Mojolicious::Lite
  plugin Config => {default => {foo => 'bar'}};

Default configuration.

=head2 C<ext>

  # Mojolicious::Lite
  plugin Config => {ext => 'stuff'};

File extension of config file, defaults to C<conf>.

=head2 C<file>

  # Mojolicious::Lite
  plugin Config => {file => 'myapp.conf'};
  plugin Config => {file => '/etc/foo.stuff'};

Configuration file, defaults to the value of the C<MOJO_CONFIG> environment
variable or C<myapp.conf> in the application home directory.

=head2 C<stash_key>

  # Mojolicious::Lite
  plugin Config => {stash_key => 'conf'};

Configuration stash key.

=head1 HELPERS

L<Mojolicious::Plugin::Config> implements the following helpers.

=head2 C<config>

  %= config 'something'
  %= config->{something}

Access config values.

=head1 METHODS

L<Mojolicious::Plugin::Config> inherits all methods from
L<Mojolicious::Plugin> and implements the following new ones.

=head2 C<load>

  $plugin->load($file, $conf, $app);

Loads config file and passes the content to C<parse>.

  sub load {
    my ($self, $file, $conf, $app) = @_;
    ...
    return $self->parse($content, $file, $conf, $app);
  }

=head2 C<parse>

  $plugin->parse($content, $file, $conf, $app);

Parse config file.

  sub parse {
    my ($self, $content, $file, $conf, $app) = @_;
    ...
    return $hash;
  }

=head2 C<register>

  $plugin->register;

Register plugin in L<Mojolicious> application.

=head1 DEBUGGING

You can set the C<MOJO_CONFIG_DEBUG> environment variable to get some
advanced diagnostics information printed to C<STDERR>.

  MOJO_CONFIG_DEBUG=1

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
