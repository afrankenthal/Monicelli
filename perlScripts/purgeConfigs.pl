#!/usr/bin/perl

&defineColors() ;

my $conf = $ENV{HOME} . "/.config/CMS";
my $cmd  = "ls ${conf}/MonicelliDefaults*.conf" ;
my @list = `$cmd` ;

foreach my $n1 (0 .. scalar @list - 1)
{
 my $c1 = $list[$n1] ;
 chomp($c1) ;
 foreach my $n2 ($n1 + 1 .. scalar @list - 1)
 {
  my $c2 = $list[$n2] ;
  chomp($c2) ;
  if( $c1 eq $c2 ) {next;}
  unless( -e $c1 ) {next;} # Might have already been deleted
  unless( -e $c2 ) {next;} # Might have already been deleted
  
  open(PIPE,"diff -q $c1 $c2 | ") ;
  my $differ = "false" ;
  while(<PIPE>)
  {
   $differ = "true" ;
  }
  close(PIPE) ;
  if( $differ eq "false" )
  {
   my ($dev1,$ino1,$mode1,$nlink1,$uid1,$gid1,$rdev1,$size1,
       $atime1,$mtime1,$ctime1,$blksize1,$blocks1)
   	  = stat($c1);   
   my ($dev2,$ino2,$mode2,$nlink2,$uid2,$gid2,$rdev2,$size2,
       $atime2,$mtime2,$ctime2,$blksize2,$blocks2)
   	  = stat($c2);
   if( $mtime2 > $mtime1 )
   {   
    unless( $c2 =~ m/MonicelliDefaults.conf/) 
    {
     print("${ACCyan}${ACBold}" . __LINE__ . "      ${ACYellow}] [${ACWhite}purgeConfigs.pl                  ${ACYellow}]${ACPlain}   \n") ;
     print("${ACCyan}${ACBold}" . __LINE__ . "      ${ACYellow}] [${ACWhite}purgeConfigs.pl                  ${ACYellow}]   ${ACRed}${ACBold}Removing ${ACCyan}$c2${ACRed}: an identical older one exists${ACPlain}\n") ;
     print("${ACCyan}${ACBold}" . __LINE__ . "      ${ACYellow}] [${ACWhite}purgeConfigs.pl                  ${ACYellow}]${ACPlain}   \n") ;
     system("rm $c2") ;
    }
   }
  }
 }
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
