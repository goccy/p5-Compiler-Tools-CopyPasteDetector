use strict;
use warnings;

use Test::More 0.96;
use File::Temp qw(tempdir);
use File::Spec;
eval {
    require YAML;
    require XML::Simple;
};

if ($@) {
    plan skip_all => 'No modules for test';
}

use Compiler::Tools::CopyPasteDetector;

my $dir = tempdir(CLENAUP => 1);
my $ditector = Compiler::Tools::CopyPasteDetector->new({output_dirname => $dir});
my $yaml = join q{}, <DATA>;
my $score = YAML::Load($yaml);
$ditector->gen_checkstyle_report($score);
my $data = XML::Simple::XMLin(File::Spec->catfile($dir,'checkstyle-result.xml'));

my $errors = $data->{file}->{'a.pl'}->{error};
is scalar @$errors, 2, 'error num';

{
    my $err= shift @$errors;
    is $err->{line}, 15, 'line number';
    is $err->{severity}, 'warning', 'severity';
    is $err->{source}, 'com.puppycrawl.tools.checkstyle.checks.duplicates.StrictDuplicateCodeCheck', 'source';
    like $err->{message}, qr/\ba\.pl\b/, 'source' or diag $err->{message};
    like $err->{message}, qr/\bline\s29\b/, 'source' or diag $err->{message};
}

{
    my $err= shift @$errors;
    is $err->{line}, 15, 'line number';
    is $err->{severity}, 'warning', 'severity';
    is $err->{source}, 'com.puppycrawl.tools.checkstyle.checks.duplicates.StrictDuplicateCodeCheck', 'source';
    like $err->{message}, qr/\bb\.pl\b/, 'source' or diag $err->{message};
    like $err->{message}, qr/\bline\s6\b/, 'source' or diag $err->{message};
}

done_testing;

__DATA__
---
clone_set_score:
  - metrics:
      kind_of_token: ~
      length: 48
      nif: 2
      population: 2
      radius: 1
    score: 48
    set:
      - block_id: 0
        end_line: 26
        file: a.pl
        from_names: &1
          a.pl: 1
          b.pl: 1
        hash: z256IDniGyYLe04SW0bYog
        indent: 0
        lines: 12
        orig: |-
          sub sub2 {
              my $a = 1;
              $a = 2;
              $a = 3;
              $a = 4;
              $a = 5;
              $a = 6;
              $a = 7;
              $a = 8;
              $a = 9;
              $a = 10;
              return $a;
          }
        parents: &2
          - eAZf6pUipLRcr8B7isGllg
          - oKmLs4mvjh50wZ+SntxIbg
        src: c3ViIHN1YjIgewogICAgbXkgJGEgPSAxOwogICAgJGEgPSAyOwogICAgJGEgPSAzOwogICAgJGEgPSA0OwogICAgJGEgPSA1OwogICAgJGEgPSA2OwogICAgJGEgPSA3OwogICAgJGEgPSA4OwogICAgJGEgPSA5OwogICAgJGEgPSAxMDsKICAgIHJldHVybiAkYTsKfQ==
        start_line: 14
        stmt_num: 1
        token_num: 48
        uniq_name: 0_z256IDniGyYLe04SW0bYog
      - block_id: 0
        end_line: 17
        file: b.pl
        from_names: &3
          a.pl: 1
          b.pl: 1
        hash: z256IDniGyYLe04SW0bYog
        indent: 0
        lines: 12
        orig: |-
          sub sub2 {
              my $a = 1;
              $a = 2;
              $a = 3;
              $a = 4;
              $a = 5;
              $a = 6;
              $a = 7;
              $a = 8;
              $a = 9;
              $a = 10;
              return $a;
          }
        parents: &4 []
        src: c3ViIHN1YjIgewogICAgbXkgJGEgPSAxOwogICAgJGEgPSAyOwogICAgJGEgPSAzOwogICAgJGEgPSA0OwogICAgJGEgPSA1OwogICAgJGEgPSA2OwogICAgJGEgPSA3OwogICAgJGEgPSA4OwogICAgJGEgPSA5OwogICAgJGEgPSAxMDsKICAgIHJldHVybiAkYTsKfQ==
        start_line: 5
        stmt_num: 0
        token_num: 48
        uniq_name: 1_z256IDniGyYLe04SW0bYog
  - metrics:
      kind_of_token: ~
      length: 33
      nif: 2
      population: 3
      radius: 1
    score: 33
    set:
      - block_id: 3
        end_line: 22
        file: a.pl
        from_names:
          a.pl: 2
          b.pl: 1
        hash: wXPHPuh3vHLJS/upXiA/NQ
        indent: 1
        lines: 7
        orig: |-
          my $a = 1;
          $a = 2;
          $a = 3;
          $a = 4;
          $a = 5;
          $a = 6;
          $a = 7;
          $a = 8;
        parents:
          - z256IDniGyYLe04SW0bYog
          - HGh5hmxAbo3pw0RsECon8A
        src: bXkgJGEgPSAxOwokYSA9IDI7CiRhID0gMzsKJGEgPSA0OwokYSA9IDU7CiRhID0gNjsKJGEgPSA3OwokYSA9IDg7
        start_line: 15
        stmt_num: 7
        token_num: 33
        uniq_name: 0_wXPHPuh3vHLJS/upXiA/NQ
      - block_id: 4
        end_line: 36
        file: a.pl
        from_names: &5
          a.pl: 2
          b.pl: 1
        hash: wXPHPuh3vHLJS/upXiA/NQ
        indent: 1
        lines: 7
        orig: |-
          my $a = 1;
          $a = 2;
          $a = 3;
          $a = 4;
          $a = 5;
          $a = 6;
          $a = 7;
          $a = 8;
        parents: &6
          - m243mfFu4y6fWRLg2n4Rxg
          - NU1L5T/qmwHmsrARyHEgig
        src: bXkgJGEgPSAxOwokYSA9IDI7CiRhID0gMzsKJGEgPSA0OwokYSA9IDU7CiRhID0gNjsKJGEgPSA3OwokYSA9IDg7
        start_line: 29
        stmt_num: 7
        token_num: 33
        uniq_name: 1_wXPHPuh3vHLJS/upXiA/NQ
      - block_id: 1
        end_line: 13
        file: b.pl
        from_names: &7
          a.pl: 2
          b.pl: 1
        hash: wXPHPuh3vHLJS/upXiA/NQ
        indent: 1
        lines: 7
        orig: |-
          my $a = 1;
          $a = 2;
          $a = 3;
          $a = 4;
          $a = 5;
          $a = 6;
          $a = 7;
          $a = 8;
        parents: &8
          - z256IDniGyYLe04SW0bYog
          - HGh5hmxAbo3pw0RsECon8A
        src: bXkgJGEgPSAxOwokYSA9IDI7CiRhID0gMzsKJGEgPSA0OwokYSA9IDU7CiRhID0gNjsKJGEgPSA3OwokYSA9IDg7
        start_line: 6
        stmt_num: 7
        token_num: 33
        uniq_name: 2_wXPHPuh3vHLJS/upXiA/NQ
directory_score:
  data:
    metrics:
      another_directories_similarity: 0
      coverage: 183.333333333333
      neighbor: 0
      self_similarity: 183.333333333333
file_score:
  a.pl:
    clone:
      wXPHPuh3vHLJS/upXiA/NQ:
        count: 2
        end_line:
          - 22
          - 36
        from_names: *5
        parents: *6
        start_line:
          - 15
          - 29
        token_num: 33
      z256IDniGyYLe04SW0bYog:
        count: 1
        end_line:
          - 26
        from_names: *1
        parents: *2
        start_line:
          - 14
        token_num: 48
    metrics:
      another_files_similarity: 89.7637795275591
      coverage: 89.7637795275591
      neighbor: 1
      self_similarity: 51.9685039370079
    token_num: 127
  b.pl:
    clone:
      wXPHPuh3vHLJS/upXiA/NQ:
        count: 1
        end_line:
          - 13
        from_names: *7
        parents: *8
        start_line:
          - 6
        token_num: 33
      z256IDniGyYLe04SW0bYog:
        count: 1
        end_line:
          - 17
        from_names: *3
        parents: *4
        start_line:
          - 5
        token_num: 48
    metrics:
      another_files_similarity: 61.1111111111111
      coverage: 61.1111111111111
      neighbor: 1
      self_similarity: 0
    token_num: 54

