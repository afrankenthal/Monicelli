/*===============================================================================
 * Monicelli: the FERMILAB MTEST geometry builder and track reconstruction tool
 * 
 * Copyright (C) 2014 
 *
 * Authors:
 *
 * Dario Menasce      (INFN) 
 * Luigi Moroni       (INFN)
 * Jennifer Ngadiuba  (INFN)
 * Stefano Terzo      (INFN)
 * Lorenzo Uplegger   (FNAL)
 * Luigi Vigani       (INFN)
 *
 * INFN: Piazza della Scienza 3, Edificio U2, Milano, Italy 20126
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ================================================================================*/
 
/*! \file MessageBus.cc 
 *  \brief Source code of the class that implements a <em>message-queue</em>
 *         to handle communication between cooperating processes.
 *
 *  (Documentation under development)
 *  
 *  \image html separator.gif
 *  \image latex separator.pdf
 */
 
#include "ANSIColors.h"
#include "MessageBus.h"

#include <errno.h>
//#include <error.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

using namespace std;

//------------------------------------------------------------------------------
/*! \brief (Documentation in progress)
 *  
 *  \image html  separator.gif
 *  \image latex separator.pdf
 */
MessageBus::MessageBus(long source) 
{
  proc_id[MASTER_ID]  = "master"   ;
  proc_id[SERVER1_ID] = "server 1" ;
  proc_id[SERVER2_ID] = "server 2" ; 
  proc_id[SERVER3_ID] = "server 3" ;

  std::string userName = getenv("USER") ;

  userName = "/tmp/" + userName ;

  if ( !boost::filesystem::exists( userName.c_str() ) )
  {
      boost::filesystem::create_directory( userName.c_str());
  }

  key_t key;
  // Create unique key via call to ftok() 
  key = ftok(userName.c_str(), 'M');

  // Open the queue - create if necessary 
  if((msgqueue_id = msgget(key, IPC_CREAT | 0666)) == -1) 
  {
    perror("msgget error in MessageBus ctor");
    exit(1);
  }

  std::stringstream ss ;
  ss << "msgqueue_id: " << msgqueue_id << " key: " << key << " for token " << userName;
//  STDLINE(ss.str(),ACCyan) ;

  source_ = source ;
 }

//------------------------------------------------------------------------------
/*! \brief (Documentation in progress)
 *  
 *  \image html  separator.gif
 *  \image latex separator.pdf
 */
bool MessageBus::Write( long           to     ,
                        const char   * clsn   ,
                        const char   * mthn   ,
                        const char   * cmd    ,
                        const char   * args   ,
                        unsigned int   priority) 
{
  if( priority > HIGHEST_PRIORITY )
    {
      std::stringstream ss ;
      ss << "[MessageBus::Write()]\t\t\t"
         << ACRed << ACBold
         << "Warning: undefined priority ("
         << priority
         << ") switched to "
         << LOWEST_PRIORITY
         << ACPlain ;
     STDLINE(ss.str(),ACWhite) ;
     priority = LOWEST_PRIORITY ;
    }

  themessage.to         = to      ;
  themessage.from       = source_ ;
  themessage.priority   = priority;
  
  themessage.command[0] = '\0';
  themessage.args[0]    = '\0';

  strcpy( themessage.classname,  clsn );
  strcpy( themessage.methodname, mthn );
  strcpy( themessage.command,     cmd );
  strcpy( themessage.args,       args );

  int len = sizeof(struct msg) - sizeof(long); // (one long is already allocated, see 'man msgsnd')

  if( (msgsnd( msgqueue_id, (struct msg *)(&themessage), len, 0)) ==-1)
    {
      perror( "[MessageBus::Write()]\t\t\tmsgsnd");
      cout << "[MessageBus::Write()]\t\t\tto       = " << to       << endl ;
      cout << "[MessageBus::Write()]\t\t\tfrom     = " << source_  << endl ;
      cout << "[MessageBus::Write()]\t\t\tclsn     = " << clsn     << endl ;
      cout << "[MessageBus::Write()]\t\t\tmthn     = " << mthn     << endl ;
      cout << "[MessageBus::Write()]\t\t\tcmd      = " << cmd      << endl ;
      cout << "[MessageBus::Write()]\t\t\targs     = " << args     << endl ;
      cout << "[MessageBus::Write()]\t\t\tpriority = " << priority << endl ;
      exit(1);
    }

  return true;
}

//------------------------------------------------------------------------------
/*! \brief (Documentation in progress)
 *  
 *  \image html  separator.gif
 *  \image latex separator.pdf
 */
struct Monicelli::msg * MessageBus::Replay(long           to, 
                                const char   * clsn,
                                const char   * mthn,
                                const char   * cmd, 
                                const char   * args,
                                unsigned int   priority)
{
  if( priority > HIGHEST_PRIORITY )
    {
      std::stringstream ss ;
      ss << "[MessageBus::Write()]\t\t\t"
         << ACRed << ACBold
         << "Warning: undefined priority ("
         << priority
         << ") switched to "
         << LOWEST_PRIORITY
         << ACPlain ;
     STDLINE(ss.str(),ACWhite) ;
     priority = LOWEST_PRIORITY ;
    }

  themessage.to         = to      ;
  themessage.from       = source_ ;
  themessage.priority   = priority;
  
  themessage.command[0] = '\0';
  themessage.args[0]    = '\0';

  strcpy( themessage.classname,  clsn );
  strcpy( themessage.methodname, mthn );
  strcpy( themessage.command,     cmd );
  strcpy( themessage.args,       args );

  int len = sizeof(struct msg) - sizeof(long); // (one long is already allocated, see 'man msgsnd')

  if( (msgsnd( msgqueue_id, (struct msg *)(&themessage), len, 0)) ==-1)
    {
      perror( "[MessageBus::Write()]\t\t\tmsgsnd");
      cout << "[MessageBus::Write()]\t\t\tto       = " << to       << endl ;
      cout << "[MessageBus::Write()]\t\t\tfrom     = " << source_  << endl ;
      cout << "[MessageBus::Write()]\t\t\tclsn     = " << clsn     << endl ;
      cout << "[MessageBus::Write()]\t\t\tmthn     = " << mthn     << endl ;
      cout << "[MessageBus::Write()]\t\t\tcmd      = " << cmd      << endl ;
      cout << "[MessageBus::Write()]\t\t\targs     = " << args     << endl ;
      cout << "[MessageBus::Write()]\t\t\tpriority = " << priority << endl ;
      exit(1);
    }

// STDLINE("Waiting for a replay",ACRed) ;
  struct Monicelli::msg * retMsg = this->ReadWait( source_ );
  std::stringstream ss ;
  ss << "Got: " << retMsg->command ;
//  STDLINE(ss.str(),ACGreen) ;
  return retMsg ;
}

//------------------------------------------------------------------------------
/*! \brief (Documentation in progress)
 *  
 *  \image html  separator.gif
 *  \image latex separator.pdf
 */ 
struct Monicelli::msg * MessageBus::ReadWait( long from )
{
  int result;
  int len = sizeof(struct msg) - sizeof(long); // (one long is already allocated, see 'man msgsnd')
  result = msgrcv(msgqueue_id, (struct msgbuf *)(&themessage), len , from, 0);
  return &themessage;

}

//------------------------------------------------------------------------------
/*! \brief (Documentation in progress)
 *  
 *  \image html  separator.gif
 *  \image latex separator.pdf
 */ 
struct Monicelli::msg * MessageBus::ReadNoWait( long from )
{
  int result;
  int len = sizeof(struct msg) - sizeof(long); // (one long is already allocated, see 'man msgsnd')
  result = msgrcv(msgqueue_id, (struct msgbuf *)(&themessage), len , from, IPC_NOWAIT);
  if ( errno == ENOMSG && result < 0 ) return NULL;
  return &themessage;

}

//------------------------------------------------------------------------------
/*! \brief Remove from the message queue all pending messages referred to 'to'
 *  
 *  \image html  separator.gif
 *  \image latex separator.pdf
 */
void MessageBus::Clear( long from )
{
  while ( Check ( from ) ) ReadWait ( from );
}

//------------------------------------------------------------------------------
/*! \brief (Documentation in progress)
 *  
 *  \image html  separator.gif
 *  \image latex separator.pdf
 */
bool MessageBus::Check( long from )
{  
  int result;
  result = msgrcv(msgqueue_id, NULL, 0 , from, IPC_NOWAIT );
  if ( errno == E2BIG ) return true;
  return false;

}

//------------------------------------------------------------------------------
/*! \brief (Documentation in progress)
 *  
 *  \image html separator.gif
 *  \image latex separator.pdf
 */
void MessageBus::Remove( )
{
  msgctl(msgqueue_id, IPC_RMID, 0);
}
