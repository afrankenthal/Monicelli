#!/usr/bin/perl

#  ./convert ~ngadiuba/Programming/MonicelliOutput ~ngadiuba/Programming/AnalOutput

&checkEnvironment() ;

&defineColors() ;

&aquireInput() ;

&checkInput() ;

&doJob() ;

#===============================================================
sub doJob
{
  foreach $file (@inputFiles)
  {
    &exec("${MonicelliDir}/templates/Converter ${inputDir}/${file}") ;
  }
  
  &execJob("${MonicelliDir}/TestBeamAnalysis/bin/TBAnalysis ${inputDir} ${outputDir} @inputConve" ) ;
}

#===============================================================
sub aquireInput
{
  $args = $#ARGV + 1 ;

  if( $args < 3 )
  {
    print <<EOB ;

  ${ACRed}${ACBold}
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!    
  FATAL: insufficient number of arguments in input.
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!${ACPlain}    


  ${ACLightBlue}Usage:

     ./convert input-dir output-dir input-file1 [input-file2] [input-file3] ...${ACPlain}
     
EOB
    exit(0) ;
  }

  $inputDir  = $ARGV[0] ;
  $outputDir = $ARGV[1] ;

  print <<EOB ;

${ACLightCyan}
Aquiring data from    : $inputDir
Writing conversion to : $outputDir
${ACPlain}

${ACLightPurple}
List of runs to convert:

EOB

  for($i=2; $i<$args; $i++)
  {
    $c = $i - 2 ;
    push @inputFiles,  "$ARGV[$i].root" ;
    push @inputConve,  "$ARGV[$i]_Converted.root" ;
    push @outputFiles, "Histos_$inputConve[$c]" ;
    print(" $c] $inputFiles[$c]\n" ) ;
  }
  ${ACPlain}
}

#===============================================================
sub checkEnvironment
{
  $MonicelliDir = $ENV{"MonicelliDir"} ;
  if( $MonicelliDir eq "" ) 
  {
    print("${ACRed}${ACBold}FATAL:${ACPlain}${ACBold} MonicelliDir is not defined\n") ;
    exit(0) ;
  } 
}

#===============================================================
sub checkInput
{
  &checkDir("$inputDir" ) ;
  &checkDir("$outputDir") ;
  
  foreach $file (@inputFiles) 
  {
    &checkFile("$file") ;
  }
}

#===============================================================
sub checkFile
{
  my $file = "${inputDir}/@_[0]" ;
  if( !(-e "$file") ) 
  {
    print <<EOB;

${ACRed}${ACBold}
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!    
FATAL: input file $file does not exist
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!${ACPlain}    

EOB
   exit(0) ;
  }
}  

#===============================================================
sub checkDir
{
  my $dir = @_[0] ;
  if( !(-e "$dir") ) 
  {
    print <<EOB;

${ACRed}${ACBold}
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!    
FATAL: input directory $dir does not exist
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!${ACPlain}    

EOB
   exit(0) ;
  }
  
  if( !(-w "$dir") ) 
  {
    print <<EOB;

${ACRed}${ACBold}
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!    
FATAL: input directory $dir does not have write permissions
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!${ACPlain}    

EOB
   exit(0) ;
  }
}

#===============================================================
sub exec
{
  $cmd = @_[0] ;

  print <<EOB ;
  
${ACYellow}+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
exec command: $cmd
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++${ACPlain}  


EOB

  open(PIPE,"$cmd |") ;

  while(<PIPE>)
  {
    print;
  }

  close(PIPE) ;
}

#===============================================================
sub execJob
{
  $cmd = @_[0] ;

  print <<EOB ;
  
${ACYellow}+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
exec command: $cmd
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++${ACPlain}


EOB

  open(OUT,">log.log") ;

  open(PIPE,"$cmd |") ;

  print OUT ("${ACLightGreen}Efficiency summary for files:${ACPlain} \n") ;
  
  foreach $file (@inputConve) 
  {
    if($file =~ m/(\d+)/) {print OUT ("${ACLightGreen}Run: $1\n${ACPlain}") ;}
  }
  
  print OUT ("\n") ;

  while(<PIPE>)
  {
    if( s/!-!// ) {print OUT ("$_");}
  }

  close(PIPE) ;
  close(OUT ) ;

  print <<EOB ;

${ACLightBlue}++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
2D efficiency histograms can be found in directory $outputDir:
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
EOB

  for($i=2; $i<$args; $i++)
  {
    $c = $i - 2 ;
    print(" $c] $outputFiles[$c]\n${ACPlain}" ) ;
  }
    
  print("\n\n\n\nReport written into log.log\n\n\n" ) ;
  system("cat log.log") ;
}
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
