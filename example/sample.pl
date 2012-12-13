use strict;
use warnings;
use Compiler::Tools::CopyPasteDetector;
my @files = ($ARGV[0]);

my $detector = CopyPasteDetector->new({job => 2});
my $data = $detector->detect(\@files);
my $score = $detector->get_score($data);
$detector->display($score);
$detector->gen_html($score);
