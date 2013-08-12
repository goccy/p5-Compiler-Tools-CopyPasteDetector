requires 'Compiler::Lexer' => 0.13;
requires 'HTML::Template';
requires 'File::Copy::Recursive';
requires 'JSON::XS';
requires 'List::MoreUtils';

on 'test' => sub {
    requires 'Test::More', 0.96;
    recommends 'YAML';
    recommends 'XML::Simple';
};
