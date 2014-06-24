#include <iostream>
#include <sstream>
#include <unistd.h>

#include "MessageBus.h"

//------------------------------------------------------------------------------
int main (int argc, char**argv) 
{
  long SELF_ID = SERVER1_ID ;

  MessageBus * theMessageBus = new MessageBus(SELF_ID) ;
  theMessageBus->Clear( SELF_ID );
  
  std::cout << "SELF: Waiting a confirmation from master... " << std::endl ;
  
  struct msg * themsg;
  themsg = theMessageBus->ReadWait( SELF_ID ); 

  std::cout << "Got it!"     << std::endl ;
  std::cout << "\nTo     : " << theMessageBus->GetProcId(themsg->to  )
            << "\nFrom   : " << theMessageBus->GetProcId(themsg->from)
            << "\nCommand: " << themsg->command
            << std::endl ;
  
  bool noAbort = true ;
  
  int c = 0 ;
  while(noAbort)
  {
    c++ ;
    std::cout << "Going to read... " << c << std::endl ;
    themsg = theMessageBus->ReadNoWait( SELF_ID );
    if( themsg )
      {
        if( themsg->from != SELF_ID ) // Do not consider messages issued by itself
          {
            if( std::string(themsg->command) == "abort" ) 
              {
               noAbort = false ;
              }
            if( std::string(themsg->command) == "getLoop" ) 
              {
                std::stringstream ss ; ss << "Loops so far: " << c ;
                theMessageBus->Write( MASTER_ID, 
//                                      SELF_ID, 
                                      "Monicelli", 
                                      "doSomething()",
                                      ss.str().c_str(),
                                      "Arguments"); 
              }
            std::cout << "Inquiry: " 
                      << c 
                      << std::endl ;
          }
      }
      usleep(250000) ;
//    sleep(1) ;
  }

}
