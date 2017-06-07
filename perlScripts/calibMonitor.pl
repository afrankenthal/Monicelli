#!/usr/bin/perl

my $path = "/tmp" ;

my @list = `ls ${path}/CalGain*.root` ;

open(OUT,">/tmp/calibMonitor.cxx") ;

print OUT<<EOB;
{
 gROOT->Reset() ;
 TCanvas uDStream("cDStream","cDStream",1200,1000) ;
 TCanvas cDStream("cDStream","cDStream",1200,1000) ;
EOB

my $fileN = 0 ;
foreach my $file (@list) 
{
 chomp $file ;
 my @p = split(/_/, $file) ;
 my $ROC = $p[7] ; 
 print OUT (" TFile file$fileN(\"$file\"); \n") ;
 print OUT (" file${fileN}.ls() ;          \n") ;
 $fileN++ ;
}

print OUT ("}\n") ;
close(OUT) ;

#print("Running root...\n") ;
#open(PIPE,"root -l /tmp/calibMonitor.cxx |" ) ;
#while(<PIPE>) 
#{
# if( m/Chi2/ )
# {
#   print ;
# }
#}
#close(PIPE) ;
