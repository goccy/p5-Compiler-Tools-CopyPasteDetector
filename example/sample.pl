use strict;
use warnings;
use Compiler::Tools::CopyPasteDetector;
sub get_target_files_list_by_namespace {
    my $ns = shift;
    my @cur_files = glob("$ns/*");
    my @names = ();
    foreach (@cur_files) {
        if (-d $_) {
            push(@names, get_target_files_list_by_namespace($_));
        } else {
            push(@names, $_);
        }
    }
    return map { $_ =~ s!//!/!; $_ } @names;
}
my $namespace = $ARGV[0];
my @files = get_target_files_list_by_namespace($namespace);
#my @files = (@ARGV);

my $detector = Compiler::Tools::CopyPasteDetector->new({job => 8});
my $data = $detector->detect(\@files);
my $score = $detector->get_score($data);
$detector->display($score);
$detector->gen_html($score);
