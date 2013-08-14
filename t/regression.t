use strict;
use warnings;
use File::Basename;
use File::Spec;
use File::Temp;
use Test::More 0.96;
use Compiler::Lexer;
use Compiler::Tools::CopyPasteDetector;

BEGIN {
    eval {
        require Test::Exception;
        Test::Exception->import;
    };
    if ($@) {
        plan skip_all => 'No modules for test';
    }
}

my $test_src_dir = File::Spec->catfile(dirname(__FILE__), 'test_src');
my $temp_dir     = File::Temp::tempdir( CLEANUP => 1);

my $opt = {
    output_dirname => $temp_dir,
};
{
    my $detector = Compiler::Tools::CopyPasteDetector->new($opt);
    my $files    = $detector->get_target_files_by_project_root($test_src_dir);
    my $file   = $files->[0];
    my $script = $detector->__get_script($file);
    lives_ok { $detector->__get_stmt_data($file, $script)} '__get_stmt_data: $stmts is not scalar ref';
    my $lexer = Compiler::Lexer->new($file);
    my $tokens = $lexer->tokenize($script);
    lives_ok { $detector->__ignore_orthographic_variation_of_variable_name($tokens)} '__ignore_orthographic_variation_of_variable_name: $tokens is not scalar ref';
}

done_testing;
