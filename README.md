# NAME

Compiler::Tools::CopyPasteDetector

# SYNOPSIS

    use Compiler::Tools::CopyPasteDetector;

    my @files = qw(file1.pl file2.pl file3.pl);
    my $options = {
        jobs          => 1, # detect by using multi thread
        min_token_num => 30,
        min_line_num  => 4,
        encoding      => 'euc-jp',
        ignore        => 1, # ignore orthographic variation of variable name
        order_by      => 'length' # clone metrics's order name
    };

    my $detector = Compiler::Tools::CopyPasteDetector->new($options);
    my $data = $detector->detect(\@files);
    my $score = $detector->get_score($data);
    $detector->display($score);
    $detector->gen_html($score);

# DESCRIPTION

Detect Copy and Paste of Perl5 Codes

# METHODS

- my $detector = Compiler::Tools::CopyPasteDetector->new($options);

    Create new instance. You can create object from `$options` in hash reference.

- my $data = $detector->detect([$filename1, $filename2, ...]);

    Get raw detected data.
    This method requires filenames in array reference.

- my $score = $detector->get_score($data);

    Get scoring data of code clones.
    This method requires `$data` getting from $detector->detect.

- $detector->display($score);

    Output results of code clones to console.
    This method requires `$score` getting from $detector->get_score.

- $detector->gen_html($score);

    Output results of code clones to HTML.
    This method requires `$score` getting from $detector->get_score.


# LICENSE

Copyright (C) Masaaki Goshima (goccy).

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself.

# AUTHOR

Masaaki Goshima (goccy) <masaaki.goshima@mixi.co.jp>

# SEE ALSO

[Compiler::Lexer](http://search.cpan.org/perldoc?Compiler::Lexer)
