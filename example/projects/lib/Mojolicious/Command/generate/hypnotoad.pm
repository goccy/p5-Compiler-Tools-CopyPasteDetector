package Mojolicious::Command::generate::hypnotoad;
use Mojo::Base 'Mojo::Command';

has description => qq/Generate "hypnotoad.conf".\n/;
has usage       => "usage: $0 generate hypnotoad\n";

# "Oh no! Can we switch back using four or more bodies?
#  I'm not sure. I'm afraid we need to use... MATH."
sub run {
  my $self = shift;
  $self->render_to_rel_file('hypnotoad', 'hypnotoad.conf');
  $self->chmod_file('hypnotoad.conf', 0644);
}

1;
__DATA__

@@ hypnotoad
# See "perldoc Mojo::Server::Hypnotoad" for a full list of options
{
  listen  => ['http://*:8080'],
  workers => 4
};

__END__
=head1 NAME

Mojolicious::Command::generate::hypnotoad - Hypnotoad generator command

=head1 SYNOPSIS

  use Mojolicious::Command::generate::hypnotoad;

  my $hypnotoad = Mojolicious::Command::generate::hypnotoad->new;
  $hypnotoad->run(@ARGV);

=head1 DESCRIPTION

L<Mojolicious::Command::generate::hypnotoad> is a C<hypnotoad.conf>
generator.

=head1 ATTRIBUTES

L<Mojolicious::Command::generate::hypnotoad> inherits all attributes from
L<Mojo::Command> and implements the following new ones.

=head2 C<description>

  my $description = $hypnotoad->description;
  $hypnotoad      = $hypnotoad->description('Foo!');

Short description of this command, used for the command list.

=head2 C<usage>

  my $usage  = $hypnotoad->usage;
  $hypnotoad = $hypnotoad->usage('Foo!');

Usage information for this command, used for the help screen.

=head1 METHODS

L<Mojolicious::Command::generate::hypnotoad> inherits all methods from
L<Mojo::Command> and implements the following new ones.

=head2 C<run>

  $hypnotoad->run(@ARGV);

Run this command.

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
