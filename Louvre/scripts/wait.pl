#!/usr/bin/perl

$loops = 20 ;
$| = 1;

foreach $l (0 ... $loops)
{
 print("Loop $l\n") ;
 sleep(1) ;
}

print("Done\n") ;


