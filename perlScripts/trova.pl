#!/usr/bin/perl

use Getopt::Long;

&defineColors() ;

my $cmd = &buildFindCommand() ;

&executeCommand($cmd) ;

#======================================================
sub defineColors
{
  $ACBlack	 = "\x1B[0;30m";
  $ACBlue	 = "\x1B[0;34m";
  $ACGreen	 = "\x1B[0;32m";
  $ACCyan	 = "\x1B[0;36m";
  $ACRed	 = "\x1B[0;31m";
  $ACPurple	 = "\x1B[0;35m";
  $ACBrown	 = "\x1B[0;33m";
  $ACGray	 = "\x1B[0;37m";
  $ACDarkGray	 = "\x1B[1;30m";
  $ACLightBlue   = "\x1B[1;34m";
  $ACLightGreen  = "\x1B[1;32m";
  $ACLightCyan   = "\x1B[1;36m";
  $ACLightRed	 = "\x1B[1;31m";
  $ACLightPurple = "\x1B[1;35m";
  $ACYellow	 = "\x1B[1;33m";
  $ACWhite	 = "\x1B[1;37m";

  $ACPlain	 = "\x1B[0m";
  $ACBold	 = "\x1B[1m";
  $ACUnderline   = "\x1B[4m";
  $ACBlink	 = "\x1B[5m";
  $ACReverse	 = "\x1B[7m";

  $ACClear	 = "\x1B[2J";
  $ACClearL	 = "\x1B[2K";
}
#================================================================
sub executeCommand
{
 my $cmd = shift   ;
 
 @suffixExclusions = split(/\s+/, $excludeSuffix) ;
 @pathExclusions   = split(/\s+/, $excludePath  ) ;
 
 print ("\n") ;

 print("${ACRed}${ACBold}") ;
 @unfilteredList = `$cmd` ;
 print("${ACPlain}") ;
 
 if( ${^CHILD_ERROR_NATIVE} > 0 ) 
 {
  print("\n${ACRed}${ACBold}${ACReverse}Aborting with code${ACPlain} ${^CHILD_ERROR_NATIVE}\n\n") ;
  exit(0) ;
 }
 
 foreach $f (@unfilteredList)
 {
  chomp($f) ;
  my @pathParts = split(/\//, $f) ;
  my $fileName  = $pathParts[-1]  ;
  my $dir       = $f              ;
  $dir =~ s/\/$fileName// ;

  my $excludeSuffix  = 'false' ;
  my $excludedSuffix = 0 ;
  my $excludePath    = 'false' ;
  my $excludedPath   = 0 ;
  
  foreach $e (@suffixExclusions)
  {
   if( $fileName =~ m/$e/ && $searchLogic eq "or" ) {$excludeSuffix = 'true';}
   if( $fileName =~ m/$e/ && $searchLogic eq "and") {$excludedSuffix++;}
  }
  if( $searchLogic eq "and" && scalar @suffixExclusions == $excludedSuffix) {$excludeSuffix = 'true';}

  foreach $e (@pathExclusions)
  {
   if( $dir =~ m/$e/ && $searchLogic eq "or" ) {$excludePath = 'true';}
   if( $dir =~ m/$e/ && $searchLogic eq "and") {$excludedPath++;}
  }
  if( $searchLogic eq "and" && scalar @pathExclusions == $excludedPath) {$excludePath = 'true';}

  if( $excludeSuffix eq 'true' || $excludePath eq 'true' )
  {
    print("${ACRed}${ACBold}${f}${ACPlain}\n") ;
  }
  else 
  {
    print("$f\n") ;
  }
 }
}
#================================================================
sub buildFindCommand
{
 $searchPath    = "."  ;
 $searchSuffix  = "*"  ;
 $searchLogic   = "or" ;
 $matchLogic    = "or" ;
 $excludePath   = ""   ; 
 $excludeSuffix = ""   ; 
 $verbose              ;
 $help                 ;

 GetOptions (
             "sp=s" => \$searchPath   ,  
             "s=s"  => \$searchSuffix ,  
             "sl=s" => \$searchLogic  , 
             "ep=s" => \$excludePath  ,
             "e=s"  => \$excludeSuffix,
             "ml=s" => \$matchLogic   , 
             "v"    => \$verbose      ,
             "help" => \$help         ,
             "h"    => \$help          
            )   ;    

 if( $help != 0 )
 {
  print <<EOB ;
__________________________________________________________________________________________________________  
${ACBold}Usage${ACPlain}: ${ACGreen}${ACBold}./trova.pl${ACPlain} [${ACCyan}${ACBold}qualifier${ACPlain}] [${ACCyan}${ACBold}qualifier${ACPlain}] ... [${ACCyan}${ACBold}qualifier${ACPlain}] [${ACYellow}${ACBold}searchstring${ACPlain}]

where

${ACCyan}${ACBold}qualifier${ACPlain} can be any of the following:

${ACCyan}${ACBold}--sp${ACPlain} "${ACBold}${ACRed}search-path-1 search-path-2${ACPlain} ... ${ACBold}${ACRed}search-path-n${ACPlain}" 
     where ${ACBold}${ACRed}search-path-j${ACPlain} is a relative or absolute directory path-name
     (${ACBold}${ACRed}.${ACPlain}, which is the default, stands for "any directory path below the current one, included")
${ACCyan}${ACBold}--s${ACPlain}  "${ACBold}${ACRed}filename-1 filename-2${ACPlain} ... ${ACBold}${ACRed}filename-n${ACPlain}" 
     where ${ACBold}${ACRed}filename-j${ACPlain} is a part of or a whole filename to search for
     (${ACBold}${ACRed}*${ACPlain}, which is the default, stands for "${ACBold}${ACRed}any filename${ACPlain}") 
${ACCyan}${ACBold}--sl${ACPlain}  [${ACBold}${ACRed}and${ACPlain}|${ACBold}${ACRed}or${ACPlain}]
     use any of these two booleans to specify the search logic with the above filenames
${ACCyan}${ACBold}--e${ACPlain} "match-1 match-2 ... match-n"
     ...
     ...
     ...
     
EOB
  exit(0) ;
 }

# print <<EOB;
#
# --sp searchpath    $searchPath  
# --s  searchSuffix  $searchSuffix  
# --sl searchLogic   $searchLogic  
# --e  excludeSuffix $excludeSuffix 
# --ml matchLogic    $matchLogic  
# --h  help          $help
# --v  verbose       $verbose       
#
#EOB

 my $cmd = "find " ;
 
 $cmd = &addPathList($cmd,          $searchPath                ) ;
 $cmd = &addList    ($cmd, "-name", $searchSuffix, $searchLogic) ;
 
 return $cmd ;
}
#================================================================
sub addPathList
{
 my $cmd   = shift ;
 my $path  = shift ;

 my @v = split(/\s+/, $path) ;

 foreach $sp (sort @v)
 {
  $cmd .= "${sp} " ;
 }
 
 return $cmd ;
}
#================================================================
sub addList
{
 my $cmd   = shift ;
 my $key   = shift ;
 my $list  = shift ;
 my $logic = shift ;

 my @v = split(/\s+/, $list) ;

 my $c = 0 ;
 foreach $sp (sort @v)
 {
  $cmd .= "$key \"${sp}\" " ;
  if( scalar @v > 1 && $c++ < scalar @v - 1) 
  {
    $cmd .= "-$logic ";
  }
 }
 
 
 return $cmd ;
}
