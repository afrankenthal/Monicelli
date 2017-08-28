#!/usr/bin/perl
#
# ARGV arguments:
#
# 0: [true|false]
# 1: [fileName]
# 2: [lineNumber]

use Cwd qw(cwd getcwd);

my $HOST = $ENV{"HOSTNAME"} ;

if( $HOST eq "hal9000.mib.infn.it") 
{
 print(__LINE__,"] [perl] here we are\n" ) ;
 my $cmd = "/usr/bin/nedit-client -line $ARGV[2] $ARGV[1]" ;
 print(__LINE__,"] [perl] \$cmd = $cmd\n" ) ;
 exec("$cmd") or print(__LINE__," ] [perl] Could not start server: $!\n"); exit(0) ;
}
elsif( $HOST eq "MAC" )
{
 my $pid = fork();
 my $cmd = "" ;
 #print(__LINE__,"] [perl] ARGV[0]: $ARGV[0]\n" ) ;
 if( $ARGV[0] eq "first" )
 {
 # print(__LINE__,"] [perl] \$pid: $pid\n" ) ;
  if ($pid == 0)
  {
    $cmd = "export DYLD_LIBRARY_PATH=/usr/lib/:/opt/X11/lib/flat_namespace  &&  ".
           "/Applications/nedit-5.5-Darwin-i386/nedit -server               &   " ;
 #   print(__LINE__," ] [perl] cmd      : $cmd\n" ) ;
    exec("$cmd") or print(__LINE__," ] [perl] Could not start server: $!\n"); exit(0) ;
  }
  elsif (!defined($pid))
  {
    print(__LINE__," ] [perl] could not fork: $cmd\n" ) ;
    exit(0);
  }
 }

 #print(__LINE__," ] [perl] going to sleep\n" ) ;
 sleep(1) ;
 $cmd = "export DYLD_LIBRARY_PATH=/usr/lib/:/opt/X11/lib/flat_namespace  && " . 
        "cd /Applications/nedit-5.5-Darwin-i386/                         && " .
        "./nc -line $ARGV[2] $ARGV[1]" ;
 #print(__LINE__," ] [perl] cmd: $cmd\n" ) ;
 exec("$cmd") or print(__LINE__," ] [perl] Could not start editor: $!\n"); exit(0) ;
}             
