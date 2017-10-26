#!/usr/bin/perl

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
     print(__LINE__ . "] [purgeConfigs.pl] removing $c2, an identical older one exists\n" ) ;
     system("rm $c2") ;
    }
   }
  }
 }
}
