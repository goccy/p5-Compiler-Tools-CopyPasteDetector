package CopyPasteDetector::DB::Schema;
use strict;
use warnings;
use DBIx::Skinny::Schema;

install_table copy_and_paste_record => schema {
    pk qw/file start_line end_line hash/;
    columns qw/file
               lines
               start_line
               end_line
               src
               hash
               parents
               revision
               token_num/;
};

1;
