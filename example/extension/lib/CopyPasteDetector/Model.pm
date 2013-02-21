package CopyPasteDetector::Model;
use strict;
use warnings;
use CopyPasteDetector::Model::Namespace;
use CopyPasteDetector::Model::Timestamp;

sub new {
    my ($class, $options) = @_;
    my $self = {
        namespace => CopyPasteDetector::Model::Namespace->new($options),
        timestamp => CopyPasteDetector::Model::Timestamp->new($options)
    };
    return bless($self, $class);
}

1;
