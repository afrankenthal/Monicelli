#!/usr/bin/perl

@list = `find . ` ;

$grandTotal = 0 ;
foreach $file (@list)
{
 chomp $file ;
 if( $file =~ m/\.\/\.svn|\.svn/ ) {next;}
 if( -T "$file")
 {
   open(PIPE, "$file") ;
   $n = 0 ;
   while(<PIPE>)
   {
    $n++ ;
    $grandTotal++ ;
   }
   close(PIPE) ;
   print("${n}] $file\n") ;
 }
}

print("\n\nGrand Total: $grandTotal lines\n") ;

