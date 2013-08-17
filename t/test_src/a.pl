#!/usr/bin/env perl 
use strict;
use warnings;

sub sub1 {
    my ($str) = @_;
    my $rv = 0;
    if ($str =~ /some/) {
        $rv  = 1;
    }
    return $rv;
}

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

sub sub3 {
    my $a = 1;
    $a = 2;
    $a = 3;
    $a = 4;
    $a = 5;
    $a = 6;
    $a = 7;
    return $a;
}
