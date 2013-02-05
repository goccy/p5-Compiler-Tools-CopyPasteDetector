package CopyPasteDetector::Extension::Initializer;
use strict;
use warnings;
use Compiler::Tools::CopyPasteDetector;

sub new {
    my ($class, $options) = @_;
    my $self = {
        %$options
     };
    return bless($self, $class);
}

sub run {
    
}


1;
