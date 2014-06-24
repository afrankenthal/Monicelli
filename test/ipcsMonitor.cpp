#include "ANSIColors.h"
#include "MessageBus.h"

#include <error.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

int main()
{
  std::stringstream sa         ;
  std::stringstream ss         ;
  int               msgqueue_id;
  
  std::string userName = getenv("USER") ;

  std::string tokName = "/tmp/" + userName ;

  if ( !boost::filesystem::exists( tokName.c_str() ) )
  {
      ss.str("") ;
      ss << "No message queue active for user " << userName ;
      STDLINE(ss.str(),ACRed) ;
      return 0 ;
  }

  key_t key;
  // Create unique key via call to ftok() 
  key = ftok(tokName.c_str(), 'M');

  // Open the queue - create if necessary 
  if((msgqueue_id = msgget(key, IPC_CREAT | 0666)) == -1) 
  {
    perror("msgget error in MessageBus ctor");
    exit(1);
  }
  
  system("clear") ;

  sa.str("") ;
  sa << "Accessing msgqueue_id " 
     << msgqueue_id
     << " with key "
     << key
     << " on "
     << tokName
     << " for "
     << userName ;
  
  struct msqid_ds msqid_ds, *buf;
  buf = & msqid_ds;

  while(true) 
  {
   system("clear") ;

   struct msqid_ds * status ;
   int retCode = msgctl(msgqueue_id, IPC_STAT, buf);

   if( retCode != 0 )
   {
     STDLINE("Invalid answer",ACRed) ;
     return 0 ;
   }
   
   ss.str("") ;
   ss << "\n"
      << ACCyan << ACBold << "Key supplied to msgget()                : " << ACYellow << buf->msg_perm.__key  << "\n"
      << ACCyan << ACBold << "Effective UID of owner                  : " << ACYellow << buf->msg_perm.uid    << "\n"
      << ACCyan << ACBold << "Effective GID of owner                  : " << ACYellow << buf->msg_perm.gid    << "\n"
      << ACCyan << ACBold << "Effective UID of creator                : " << ACYellow << buf->msg_perm.cuid   << "\n"
      << ACCyan << ACBold << "Effective GID of creator                : " << ACYellow << buf->msg_perm.cgid   << "\n"
      << ACCyan << ACBold << "Sequence number                         : " << ACYellow << buf->msg_perm.__seq  << "\n"
      << ACCyan << ACBold << "Current number of bytes in queue        : " << ACYellow << buf->__msg_cbytes    << "\n"
      << ACCyan << ACBold << "Maximum number of bytes allowed in queue: " << ACYellow << buf->msg_qbytes      << "\n"
      << ACCyan << ACBold << "Number of messages in queue             : " << ACYellow << buf->msg_qnum        << "\n"
      << ACPlain ;
   STDLINE(sa.str(),ACCyan) ;
   STDLINE(ss.str(),ACWhite) ;
   
   usleep(50000) ;
  }
}
