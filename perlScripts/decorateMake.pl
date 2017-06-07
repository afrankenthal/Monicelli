#!/usr/bin/perl

open(IN,"Makefile") ;
open(OU,">/tmp/Makefile") ;
while(<IN>)
{
 s/^CXX(\s+|\t+)=\s+g\+\+/CXX           = echo ' ' && g++/ ;
 print OU ;
}
close(IN) ;
close(OU) ;

open(PIPE,"mv /tmp/Makefile . |" ) ;
while(<PIPE>) {print;} close(PIPE) ;
