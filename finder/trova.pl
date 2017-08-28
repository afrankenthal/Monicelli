#!/usr/bin/perl
#================================================================
#
# Author: D. Menasce
#

use Getopt::Long;

             &defineColors    (          ) ;
             &collectArguments(          ) ;
$cmd       = &buildFindCommand(          ) ;
@finalList = &findFiles       ($cmd      ) ;
             &performSearch   (@finalList) ;

#================================================================
sub addPathList
{
 $cmd  = shift ;
 $path = shift ;

 foreach $sp (sort split(/\s+/, $path)) {$cmd .= "${sp} " ;}
 
 return $cmd .= "-name \"*\" " ;
}
#================================================================
sub buildFindCommand
{
 return &addPathList("find ", $pathSelection)  ;
}
#================================================================
sub collectArguments
{
 $pathSelection              = "."    ; # List of directories to look into (first argument to find command)
 $matchPath                  = "*"    ; # List of strings that candidate files must match to be considered
 $matchPathLogic             = "or"   ; # Boolean to discriminate between input paths
 $excludePaths               = ""     ; 
 $excludePLogic              = "or"   ; 
 $caseSensitiveMatch         = "false";
 $caseSensitiveExclusion     = "false";
 $caseSensitiveString        = "false";
 $caseSensitiveEString       = "false";
 $searchStrings              = ""     ;
 $excludeStrings             = ""     ;
 $searchStringsLogic         = ""     ;
 $excludeStringsLogic        = ""     ;
 $suffixes                   = ""     ;
 $maxTotalFilesFound         = "200"  ;
 $maxTotalFilesConsidered    = "150"  ;
 $maxTotalFileMatchesFound   = "30"   ;
 $maxTotalStringMatchesFound = "300"  ;
 $verbose                    = 0      ;
 $help                       = 0      ;
 $isHTML                     = 0      ;
 $isGUI                      = 0      ;

 GetOptions (
             "ps=s" => \$pathSelection             ,
             "mp=s" => \$matchPath                 ,
             "ml=s" => \$matchPathLogic            ,
             "ep=s" => \$excludePaths              ,
             "pl=s" => \$excludePLogic             ,
             "cm=s" => \$caseSensitiveMatch        ,
             "em=s" => \$caseSensitiveExclusion    ,
             "cs=s" => \$caseSensitiveString       ,
             "es=s" => \$caseSensitiveEString      ,
             "ss=s" => \$searchStrings             ,
             "xs=s" => \$excludeStrings            ,
             "sl=s" => \$searchStringsLogic        ,
             "el=s" => \$excludeStringsLogic       ,
             "su=s" => \$suffixes                  ,
             "mh=s" => \$maxTotalFilesFound        ,
             "mc=s" => \$maxTotalFilesConsidered   ,
             "mt=s" => \$maxTotalFileMatchesFound  ,
             "mz=s" => \$maxTotalStringMatchesFound,
             "v"    => \$verbose                   ,
             "help" => \$help                      ,
             "h"    => \$help                      ,
             "html" => \$isHTML                    ,
             "gui"  => \$isGUI
            )   ;    

 $pathSelection =~ s/"//g ;
 unless( $searchStrings =~ /.+/ ) {$searchStrings = "\\w+" ;}
 
 
 print <<EOB ;
 
 ------------- Selected arguments --------------------
 pathSelection              $pathSelection   
 matchPath                  $matchPath    
 matchPathLogic             $matchPathLogic  
 excludePaths               $excludePaths    
 excludePLogic              $excludePLogic   
 caseSensitiveMatch         $caseSensitiveMatch 
 caseSensitiveExclusion     $caseSensitiveExclusion 
 caseSensitiveString        $caseSensitiveString 
 caseSensitiveEString       $caseSensitiveEString 
 searchStrings              $searchStrings 
 excludeStrings             $excludeStrings
 searchStringsLogic         $searchStringsLogic
 excludeStringsLogic        $excludeStringsLogic
 suffixes                   $suffixes
 maxTotalFilesFound         $maxTotalFilesFound
 maxTotalFilesConsidered    $maxTotalFilesConsidered
 maxTotalFileMatchesFound   $maxTotalFileMatchesFound
 maxTotalStringMatchesFound $maxTotalStringMatchesFound
 verbose                    $verbose
 help                       $help  
 isHTML                     $isHTML
 isGUI                      $isGUI
______________________________________________________           
EOB

 if($caseSensitiveMatch     eq "true") {$caseSensitiveMatch     = "" ;}
 else                                  {$caseSensitiveMatch     = "i";}
 if($caseSensitiveExclusion eq "true") {$caseSensitiveExclusion = "" ;}
 else                                  {$caseSensitiveExclusion = "i";}
 if($caseSensitiveString    eq "true") {$caseSensitiveString    = "" ;}
 else                                  {$caseSensitiveString    = "i";}
 if($caseSensitiveEString   eq "true") {$caseSensitiveEString   = "" ;}
 else                                  {$caseSensitiveEString   = "i";}

 if( $help != 0 )
 {
  print <<EOB ;
__________________________________________________________________________________________________________  
${ACBold}Usage${ACPlain}: ${ACGreen}${ACBold}./trova.pl${ACPlain} [${ACLightBlue}${ACBold}path${ACPlain}] [${ACLightBlue}${ACBold}path${ACPlain}] ... [${ACLightBlue}${ACBold}path${ACPlain}] [${ACCyan}${ACBold}qualifier${ACPlain}] [${ACCyan}${ACBold}qualifier${ACPlain}] ... [${ACCyan}${ACBold}qualifier${ACPlain}] [${ACYellow}${ACBold}searchstring${ACPlain}]

where

${ACCyan}${ACBold}qualifier${ACPlain} can be any of the following:

${ACCyan}${ACBold}-ps${ACPlain} "${ACBold}${ACRed}search-path-1 search-path-2${ACPlain} ... ${ACBold}${ACRed}search-path-n${ACPlain}" 
     where ${ACBold}${ACRed}search-path-j${ACPlain} is a relative or absolute directory path-name
     (${ACBold}${ACRed}.${ACPlain}, which is the default, stands for "any directory path below the current one, included")
${ACCyan}${ACBold}-mp${ACPlain}  "${ACBold}${ACRed}filename-1 filename-2${ACPlain} ... ${ACBold}${ACRed}filename-n${ACPlain}" 
     where ${ACBold}${ACRed}filename-j${ACPlain} is a part of or a whole filename to search for
     (${ACBold}${ACRed}*${ACPlain}, which is the default, stands for "${ACBold}${ACRed}any filename${ACPlain}") 
${ACCyan}${ACBold}-ml${ACPlain}  [${ACBold}${ACRed}and${ACPlain}|${ACBold}${ACRed}or${ACPlain}]
     use any of these two booleans to specify the search logic with the above filenames
${ACCyan}${ACBold}-ep${ACPlain} "match-1 match-2 ... match-n"
${ACCyan}${ACBold}-pl${ACPlain} "match-1 match-2 ... match-n"
${ACCyan}${ACBold}-cm${ACPlain} "match-1 match-2 ... match-n"
${ACCyan}${ACBold}-em${ACPlain} "match-1 match-2 ... match-n"
${ACCyan}${ACBold}-ss${ACPlain} "match-1 match-2 ... match-n"
${ACCyan}${ACBold}-es${ACPlain} "match-1 match-2 ... match-n"
${ACCyan}${ACBold}-sl${ACPlain} "match-1 match-2 ... match-n"
${ACCyan}${ACBold}-mh${ACPlain} "match-1 match-2 ... match-n"
${ACCyan}${ACBold}-mc${ACPlain} "match-1 match-2 ... match-n"
${ACCyan}${ACBold}-mt${ACPlain} "match-1 match-2 ... match-n"
${ACCyan}${ACBold}-mT${ACPlain} "match-1 match-2 ... match-n"
${ACCyan}${ACBold}-v${ACPlain} 
${ACCyan}${ACBold}-h -help${ACPlain} 
${ACCyan}${ACBold}-html${ACPlain} 
${ACCyan}${ACBold}-gui${ACPlain} 
     ...
     ...
     ...
     
EOB
  exit(0) ;
 }
 
 if( $isGUI == 1 )
 {
  $FINDERHOME = $ENV{"FINDERHOME"} ;
  unless( $FINDERHOME =~ m/\w+/ )
  {
   print("\n${ACRed}${ACBold}FATAL${ACPlain}: no FINDERHOME environment defined\n\n" ) ;
   exit(0) ;
  }
  $cmd = "${FINDERHOME}/finder" ;
  exec($cmd) ;
 }
}
#================================================================
sub computeLine
{
 return sprintf("%05d", shift);
}
#================================================================
sub dbg
{
 $line    = shift ;
 $theName = ""    ;

 print("${line}] ") ;
 foreach $e (@_)
 {
  local *theVar = $e     ;
        $name   = $theVar;
  for (keys %::)
  {
   next if /^theVar$/;
   $theName  = "" ;
   if( $name =~ m/\w+/ ) {$theName = "= $name";}
   print "$_ $theName " 
       if *{$::{$_}}{SCALAR} eq *{$::{theVar}}{SCALAR};
  }
 }
 print("\n") ;
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
#================================================================
sub findFiles
{
 $cmd       = shift   ;
 @finalList = ()      ;
 
 $matchPath       =~ s/"//g ;
 $excludePaths    =~ s/"//g ;
 @matchPaths   = split(/\s+/, $matchPath    ) ;
 @excludePList = split(/\s+/, $excludePaths ) ;
 @searchSList  = split(/\s+/, $searchStrings) ;
 @suffixList   = split(/\s+/, $suffixes     ) ;

 print(__LINE__, "] $cmd\n\n" ) ;
 
 print("${ACRed}${ACBold}") ;
 @unfilteredList = `$cmd` ;
 print("${ACPlain}") ;
 
 if( ${^CHILD_ERROR_NATIVE} > 0 ) 
 {
  print("\n${ACRed}${ACBold}${ACReverse}Aborting with code${ACPlain} ${^CHILD_ERROR_NATIVE}\n\n") ;
  exit(0) ;
 }
 
 $counts = scalar @unfilteredList ;
 if( $counts >= $maxTotalFilesFound )
 {
  toClient((*{"[TotalFilesFound]"}, *counts       )) ;# Total number of files found with file selection criteria only
  print STDERR ("[Warning] too many files found: try again with a tighter files-selection criteria or increase the \"Total files found\" threshold value\n") ;
  exit(0) ;
 }

 foreach $f (@unfilteredList)
 {
  chomp($f) ;

  @pathParts = split(/\//, $f) ;
  $fileName  = $pathParts[-1]  ;
  $dir       = $f              ;
  unless( -d "$dir" ) {$dir =~ s/\/$fileName// ;}

  $includeFile = "false" ;
  foreach $suff (@suffixList)
  {
   $suff =~ s/"//g ;
   if( $fileName =~ m/\.${suff}$/ ) {$includeFile = "true" ;}
  }

  if( $includeFile eq "false" ) {next ;}

  $includeFile  = 'false' ;
  $includedFile = 0      ;
  $nMatches     = scalar @matchPaths ;
  
  # Include files with match in directory path name only
  foreach $m (@matchPaths)
  {
   $match =  $m ;
   $match =~ s/\*/.*?/g ;
   $match =~ s/"//g     ;
   if( $caseSensitiveM eq "false" )
   {
    if   ( $dir =~ m/$match/i && $matchPathLogic eq "and" && $nMatches > 1) {$includedFile++      ;}
    elsif( $dir =~ m/$match/i && $matchPathLogic eq "or"                  ) {$includeFile = 'true';}    
    $match      =~ s/\.\*\?//g ;
   }
   else
   {
    if   ( $dir =~ m/$match/  && $matchPathLogic eq "and" && $nMatches > 1) {$includedFile++      ;}
    elsif( $dir =~ m/$match/  && $matchPathLogic eq "or"                  ) {$includeFile = 'true';}  
    $match      =~ s/\.\*\?//g ;
   }
  }
  if( $nMatches > 0 && $matchPathLogic =~ m/and/ && 
      $nMatches == $includedFile) {$includeFile = 'true';}

  $keepFile    = $includeFile ;
  $keepedFile  = 0      ;
  $nExclusions = scalar @excludePList ;

  # Exclude files with match in directory path name only
  foreach $e (@excludePList)
  {
   $match = $e ;
   if( $caseSensitiveE eq "false" )
   {
    if( $dir =~ m/${e}/i && $excludePLogic eq "or"                     ) {$keepFile = "false";}
    if( $dir =~ m/${e}/i && $excludePLogic eq "and" && $nExclusions > 0) {$keepedFile++      ;}
   }
   else
   {
    if( $dir =~ m/${e}/  && $excludePLogic eq "or"                     ) {$keepFile = "false";}
    if( $dir =~ m/${e}/  && $excludePLogic eq "and" && $nExclusions > 0) {$keepedFile++      ;}
   }
  }
  if( $nMatches > 0 && $matchPathLogic =~ m/and/ && 
      $nMatches == $includedFile) {$includeFile = 'true';}

  if( $nExclusions > 0              && 
     ($nExclusions == $keepedFile)  && 
      $excludePLogic eq "and"     ) {$keepFile = 'false';}

  $file = "${dir}/${fileName}" ;
  
  if( $keepFile    eq 'true')
  {
    push @finalList, $file ;
  }
  else 
  {
    $rejected = "${ACBold}${ACRed}${file}${ACPlain}\n" ;
  }
 }
 
 $counts = scalar @finalList ;
 if( $counts >= $maxTotalFilesFound )
 {
  toClient((*{"[TotalFilesFound]"}, *counts       )) ;# Total number of files found with file selection criteria only
  print STDERR ("[Warning] too many files found: try again with a tighter files-selection criteria or increase the \"Total files found\" threshold value\n") ;
  exit(0) ;
 }

 return @finalList ;
}
#================================================================
sub findMatch
{
 my $string    = shift ;
 my $logic     = shift ;
 my $matches   = shift ;
 my $found     = shift ;
 my $csensit   = shift ;
 my $statemt   = shift ;
 my $searchCmd = ""    ;
 my $state     = ""    ;
 
 unless( $matches =~ m/.+/ ) {return $found;}
 
 if( $statemt eq "affirm" ) {$state = "true"  ;}
 else                       {$state = "false" ;}
 
 my @matches   = split(/\s+/, $matches) ;
 
 if(    $logic eq "or"  )
 {
   $searchCmd = "if( \$string =~ m/" ;
   foreach my $m (@matches)
   {
    $m          =~ s/"//g ;
    $searchCmd .=  "$m|" ;
   }
   $searchCmd   =~ s/\|$// ;
   $searchCmd  .= "/$csensit) {\$found = \"$state\";}" ;
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
     $searchCmd .= ") {\$found = \"$state\";}" ;
    }
    else
    {
     $searchCmd .= " && " ;
    } 
   }
 }
 
 eval("$searchCmd") ;

 return $found ;
}
#================================================================
sub performSearch
{

 $separator  = "\n        ${ACYellow}${ACBold}${ACReverse}" ;
 foreach (0 .. 132) {$separator .= "=";}
 $separator .= "${ACPlain}" ;

 @finalList               = @_ ;
 $totalFilesFound         = 0  ;
 $totalFileMatchesFound   = 0  ;
 $totalStringMatchesFound = 0  ;
 $totalFilesConsidered    = 0  ;

 $lineOnScreen = 0 ;

 foreach $f (@finalList)
 {
  chomp($f) ;
  $totalFilesFound++;
  unless(-T "$f") {next;}
  $totalFilesConsidered++ ;
  if( $totalFilesConsidered >= $maxTotalFilesConsidered )
  {
   toClient((*{"[TotalFilesFound]"}     , *totalFilesFound     )) ;
   toClient((*{"[TotalFilesConsidered]"}, *totalFilesConsidered)) ;
   print STDERR<<EOB ;
[Warning] list interrupted. Too many files have been considered: try again with a tighter files-selection criteria 
or increase the "Total files considered" threshold
EOB
   exit(0) ;
  }
  open(IN,"$f") ;
  $line       = 0       ;
  @matches    = ()      ;
  $considered = "false" ;
  while($code = <IN>)
  {
   $line++ ;
   chomp($code) ;
   
   $code =~ s/</&lt;/g ;
   
   my $searchFound = &findMatch("${code}", "$searchStringsLogic" , "$searchStrings" , "false", "$caseSensitiveString" , "affirm") ;
   my $excludFound = &findMatch("${code}", "$excludeStringsLogic", "$excludeStrings", "true" , "$caseSensitiveEString", "negate") ;

   if($searchFound eq "true" && $excludFound eq "true" ) 
   {
    $code  = &substituteWithTags("$code", "$searchStrings", "red"   ) ;
    $lineN = computeLine($line)   ;
    if( $isHTML == 1 ) { $lineN =~ s/(\d+)/<font color=\"#00ff00\" wheight=\"bold\">${1}<\/font>/g ;}
    else               { $lineN =~ s/($s)/${ACGreen}${ACBold}${1}${ACPlain}/g                      ;}

    push @matches, "${lineN}] ${code}" ; # <------- set strings aside

    $considered = "true"          ;
    $totalStringMatchesFound++    ;
    if( $totalStringMatchesFound >= $maxTotalStringMatchesFound )
    {
     toClient((*{"[TotalFilesFound]"}        , *totalFilesFound        )) ;
     toClient((*{"[TotalFilesConsidered]"}   , *totalFilesConsidered   )) ;
     toClient((*{"[TotalFileMatchesFound]"}  , *totalFileMatchesFound  )) ;
     toClient((*{"[TotalStringMatchesFound]"}, *totalStringMatchesFound)) ;
     print STDERR<<EOB ;
[Warning] list interrupted. Too many string matches have been found: try again with a tighter selection criteria 
or increase the "Total search string matches found" threshold. 
EOB
     exit(0) ;
    }
   }
  }
  close(IN);
  
  if( $considered eq "true" ) {$totalFileMatchesFound++ ;}
  if( $totalFileMatchesFound >= $maxTotalFileMatchesFound )
  {
   toClient((*{"[TotalFilesFound]"}      , *totalFilesFound       )) ;
   toClient((*{"[TotalFilesConsidered]"} , *totalFilesConsidered  )) ;
   toClient((*{"[TotalFileMatchesFound]"}, *totalFileMatchesFound )) ;
   print STDERR<<EOB ;
[Warning] list interrupted. Too many files have been found with a match: try again with a tighter files-selection criteria 
or increase the "Total files with at least a match" threshold. 
EOB
   exit(0) ;
  }
  
  $matchesFound = scalar @matches ;

  if( $matchesFound  > 0 ) 
  {
   $matchesFound = computeLine($matchesFound) ;
   if( $isHTML == 1 ) { 
    $matchesFound  =~ s/(\d+)/<font color=\"#ffaa00\" wheight=\"bold\">${1}<\/font>/g ;
    $f             = "<font color=\"#b0e4ff\" wheight=\"bold\">${f}</font>"           ;
    $matchesFound .= "]"                                                              ;
    toClient((*{"[FileMatch]"}, *matchesFound, *f))                                   ;
   }
   else               
   { 
    $matchesFound  =~ s/($s)/${ACYellow}${ACBold}${1}${ACPlain}/g                     ;
    $f             = "${ACCyan}${ACBold}${f}${ACPlain}"                               ;
    $matchesFound .= "]"                                                              ;
    toClient((*{""}, *separator  ))                                                   ;
    toClient((*{""}, *matchesFound, *f))                                              ;
   }
  }
  
  foreach $match (@matches)
  {
   if( $isHTML == 1 ) {toClient((*{"[stringMatch]"}, *match)) ;}
   else               {toClient((*{""}             , *match)) ;}
  }
 }
 toClient((*{"[TotalFilesFound]"        }, *totalFilesFound        )) ;# Total number of files found with file selection criteria only
 toClient((*{"[TotalFilesConsidered]"   }, *totalFilesConsidered   )) ;# Total number of files inspected (of ASCII type)
 toClient((*{"[TotalFileMatchesFound]"  }, *totalFileMatchesFound  )) ;# Total number of files with a string match inside
 toClient((*{"[TotalStringMatchesFound]"}, *totalStringMatchesFound)) ;# Total number of matches 
}
#========================================================================
sub substituteWithTags
{
 my $string  = shift ;
 my $matches = shift ;
 my $color   = shift ;
 my @matches = split(/\s+/, $matches) ;
 
 foreach my $m (@matches)
 {
  $m      =~ s/"//g                              ;
  $string =~ s/($m)/|++|${1}|--|/g               ;
 }

 if( $isHTML == 1 ) 
 { 
  $string =~ s/\|\+\+\|/<font color="${color}">/g;
  $string =~ s/\|\-\-\|/<\/font>/g               ;
 }
 else               
 { 
  $color  =  ucfirst($color)                     ;
  $color  =  ${'AC' . "${color}"}                ;
  $string =~ s/\|\+\+\|/${color}${ACBold}/g      ;
  $string =~ s/\|\-\-\|/${ACPlain}/g             ;
 }
 
 return $string ;
}

#================================================================
sub toClient
{
 $theName = ""    ;

 foreach $e (@_)
 {
  local *theVar = $e     ;
        $name   = $theVar;
  for (keys %::)
  {
   next if /^theVar$/;
   if( $name =~ m/\w+/ ) 
   {
    print "${name} " 
        if *{$::{$_}}{SCALAR} eq *{$::{theVar}}{SCALAR};
   }
   else
   {
    print "$_ " 
        if *{$::{$_}}{SCALAR} eq *{$::{theVar}}{SCALAR};
   }
  }
 }
 print("\n") ;
}
