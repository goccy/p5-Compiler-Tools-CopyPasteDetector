package Mojolicious::Command::generate;
use Mojo::Base 'Mojolicious::Commands';

# "Ah, nothing like a warm fire and a SuperSoaker of fine cognac."
has description => "Generate files and directories from templates.\n";
has hint        => <<"EOF";

See '$0 generate help GENERATOR' for more information on a specific generator.
EOF
has message => <<"EOF";
usage: $0 generate GENERATOR [OPTIONS]

These generators are currently available:
EOF
has namespaces =>
  sub { [qw/Mojolicious::Command::generate Mojo::Command::generate/] };
has usage => "usage: $0 generate GENERATOR [OPTIONS]\n";

1;
__END__

=head1 NAME

Mojolicious::Command::generate - Generator command

=head1 SYNOPSIS

  use Mojolicious::Command::generate;

  my $generator = Mojolicious::Command::generate->new;
  $generator->run(@ARGV);

=head1 DESCRIPTION

L<Mojolicious::Command::generate> lists available generators.

=head1 ATTRIBUTES

L<Mojolicious::Command::generate> inherits all attributes from
L<Mojolicious::Commands> and implements the following new ones.

=head2 C<description>

  my $description = $generator->description;
  $generator      = $generator->description('Foo!');

Short description of this command, used for the command list.

=head2 C<hint>

  my $hint   = $generator->hint;
  $generator = $generator->hint('Foo!');

Short hint shown after listing available generator commands.

=head2 C<message>

  my $message = $generator->message;
  $generator  = $generator->message('Bar!');

Short usage message shown before listing available generator commands.

=head2 C<namespaces>

  my $namespaces = $generator->namespaces;
  $generator     = $generator->namespaces(['Mojo::Command::generate']);

Namespaces to search for available generator commands, defaults to
L<Mojolicious::Command::generate> and L<Mojo::Command::generate>.

=head1 METHODS

L<Mojolicious::Command::generate> inherits all methods from
L<Mojolicious::Commands>.

=head1 SEE ALSO

L<Mojolicious>, L<Mojolicious::Guides>, L<http://mojolicio.us>.

=cut
