use strict;
use warnings;
use Compiler::Tools::CopyPasteDetector;

my @files = qw();

my $detector = CopyPasteDetector->new();
my $data = $detector->detect(\@files);
my $score = $detector->get_score($data);
$detector->display($score);
