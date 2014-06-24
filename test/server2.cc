#include <iostream>

#include "MessageBus.h"

//------------------------------------------------------------------------------
int main (int argc, char**argv) 
{
  long SELF_ID = SERVER2_ID ;

  MessageBus * theMessageBus = new MessageBus(SELF_ID) ;
  theMessageBus->Clear( SERVER2_ID );
  
  std::cout << "Server2: Waiting a confirmation from master... " << std::endl ;
  
  struct msg * themsg;
  themsg = theMessageBus->ReadWait( SELF_ID ); 

  std::cout << "Got it!"     << std::endl ;
  std::cout << "\nTo     : " << theMessageBus->GetProcId(themsg->to  )
            << "\nFrom   : " << theMessageBus->GetProcId(themsg->from)
            << "\nCommand: " << themsg->command
            << std::endl ;
}
