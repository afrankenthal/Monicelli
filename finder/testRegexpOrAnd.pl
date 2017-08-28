#!/usr/bin/perl

my $string        = "C'erano pippo e minnie con pluto" ;
my $matches       = ("pippo pluto") ;
my $caseSensitive = "true" ;
my $found         = "" ;

$found = &findMatch("$string", "or" , "$matches", "true" ) ; print("Or/case sensitive   : $found\n") ;
$found = &findMatch("$string", "or" , "$matches", "false") ; print("Or/case insensitive : $found\n") ;

$found = &findMatch("$string", "and", "$matches", "true" ) ; print("And/case sensitive  : $found\n") ;
$found = &findMatch("$string", "and", "$matches", "false") ; print("And/case insensitive: $found\n") ;
#========================================================================
sub findMatch
{
 my $string    = shift   ;
 my $logic     = shift   ;
 my $matches   = shift   ;
 my $csensit   = shift   ;
 my $found     = "false" ;
 my $searchCmd = ""      ; 
 my @matches   = split(/\s+/, $matches) ;

 if( $csensit eq "true" ) {$csensit = ""  ;}
 else                     {$csensit = "i" ;}

 if(    $logic eq "or"  )
 {
   $searchCmd = "if( \$string =~ m/" ;
   foreach my $m (@matches)
   {
    $m          =~ s/"//g ;
    $searchCmd .=  "$m|" ;
   }
   $searchCmd   =~ s/\|$// ;
   $searchCmd  .= "/$csensit) {\$found = \"true\";}" ;
 }
 elsif( $logic eq "and"  )
 {
   $searchCmd = "if ("  ;

   foreach $m (@matches)
   {
    $m           =~ s/"//g ;
    $searchCmd  .= "\$string =~ m/$m/$csensit"  ;
    if( $m eq $matches[-1] )
    {
     $searchCmd .= ") {\$found = \"true\";}" ;
    }
    else
    {
     $searchCmd .= " && " ;
    } 
   }
 }
 
 eval("$searchCmd") ;
 print(__LINE__, "] search command: |$searchCmd|\n") ;
 return $found ;
}
