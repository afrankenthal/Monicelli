#!/usr/bin/perl

my $a = "Io mi chiamo pippo e sono amico di paperino";
my $b = "Io mi chiamo pippa e non sono amico di paperina";
$_ = $a ^ $b;
s/./ord $& ? "^" : " "/ge;
print "$_\n" for $a, $b, $_;

