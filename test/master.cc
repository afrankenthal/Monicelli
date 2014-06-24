#include <iostream>
#include <sstream>
#include <unistd.h>

#include "MessageBus.h"

//------------------------------------------------------------------------------
int main (int argc, char**argv) 
{
  long SELF_ID = MASTER_ID ;

  MessageBus * theMessageBus = new MessageBus(SELF_ID) ;
  theMessageBus->Clear( SELF_ID );
  
  int i = 0 ;
  std::cout << "1) Start loop on server1: " << std::endl ;
  std::cout << "2) Start loop on server2: " << std::endl ;
  std::cout << "0) Quit "                   << std::endl ;
  
  std::cin >> i ;
  
  long destination = 0 ;
  if( i == 1 ) destination = SERVER1_ID ;
  if( i == 2 ) destination = SERVER2_ID ;
  
  if(!(i == 1 || i == 2 )) exit(0) ;
  
  theMessageBus->Write( destination, 
                        "Monicelli", 
                        "doSomething()", 
                        "Command to execute", 
                        "Arguments");
                        
  std::cout << "1) Abort server1"         << std::endl ;
  std::cout << "2) Abort server2"         << std::endl ;
  std::cout << "3) Get loop from server1" << std::endl ;
  std::cout << "4) Get loop from server2" << std::endl ;

  std::cout << "0) Quit "         << std::endl ;

  struct msg * themsg;

  while(i != 0 )
  {
   std::cout << "-> " ; std::cout.flush() ; std::cin >> i ;
   if( i == 1 ) destination = SERVER1_ID ;
   if( i == 2 ) destination = SERVER2_ID ;
   if( i == 0 ) exit(0) ;
   if( i == 1 || i == 2 )
     {
       theMessageBus->Write( destination, 
                             "Monicelli", 
                             "doSomething()",
                             "abort",
                             "Arguments"); 
     }
   else 
     {
       themsg = theMessageBus->Replay( destination, 
                                       "Monicelli", 
                                       "doSomething()",
                                       "getLoop",
                                       "Arguments");
       if(themsg) std::cout << themsg->command << std::endl ;
     }  
  }                      
}
