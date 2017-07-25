#!/usr/bin/perl

use Getopt::Long;

&defineColors() ;

my $cmd = &buildFindCommand() ;


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
sub buildFindCommand
{
 my $searchPath    = "."      ;
 my $searchSuffix  = "*"      ;
 my $searchLogic   = "or"     ;
 my $excludePath   = ""       ;
 my $excludeSuffix = ".svn"   ;
 my $verbose                  ;
 my $help                     ;

 GetOptions (
             "sp=s" => \$searchPath   ,  
             "s=s"  => \$searchSuffix ,  
             "sl=s" => \$searchLogic  , 
             "ep=s" => \$excludePath  , 
             "e=s"  => \$excludeSuffix,
             "v"    => \$verbose      ,     
             "h"    => \$help          
            )   ;    

 if( $help != 0 )
 {
  print <<EOB ;
  
${ACBold}Usage${ACPlain}: ./trova.pl ....


EOB
  exit(0) ;
 }

 print <<EOB;

 searchpath    $searchPath  
 searchSuffix  $searchSuffix  
 searchLogic   $searchLogic  
 excludePath   $excludePath  
 excludeSuffix $excludeSuffix 
 help          $help
 verbose       $verbose       

EOB

 my $cmd = "find " ;
 
 $cmd = &addPathList($cmd,          $searchPath                ) ;
 $cmd = &addList    ($cmd, "-name", $searchSuffix, $searchLogic) ;
 $cmd = &addList    ($cmd, "-name", $excludePath , "exclude"   ) ;
 
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
 
 print ("\n$cmd\n\n") ;
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
  if( $logic =~ m/exclude/ )
  {
   $cmd .= "-and -not ";
  }
  $cmd .= "$key \"${sp}\" " ;
  if( scalar @v > 1 && $c++ < scalar @v - 1) 
  {
    $cmd .= "-$logic ";
  }
 }
 
 
 print ("\n$cmd\n\n") ;
 return $cmd ;
}
