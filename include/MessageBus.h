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
 
/*! \file MessageBus.hh
 *  \brief Header file of the class that implements a <em>message-queue</em>
 *         to handle communication between cooperating processes.
 *
 *  (Documentation under development)
 *  
 *  \image html  separator.gif
 *  \image latex separator.pdf
 */
 
#ifndef __MESSAGEBUS
#define __MESSAGEBUS

#include <iostream>
#include <string>
#include <map>

#include "boost/filesystem/operations.hpp"   // includes all needed Boost.Filesystem declarations

#include "MessageTools.h"

#include "msg.h"

//------------------------------------------------------------------------------
/*! \brief (Documentation in progress)
 *  
 *  \image html  separator.gif
 *  \image latex separator.pdf
 */
enum
{
       MASTER_ID  = 100,
       SERVER1_ID = 200,
       SERVER2_ID = 300,
       SERVER3_ID = 400
};

//------------------------------------------------------------------------------
/*! \class MessageBus
 *  \brief (Documentation in progress)
 *  
 *  \image html separator.gif
 *  \image latex separator.pdf
 */
class MessageBus 
{
public:

  MessageBus(long source);
 ~MessageBus()             {STDLINE("Empty destructor",ACWhite);}

  struct Monicelli::msg         * ReadWait  (long           from                     );  
  struct Monicelli::msg         * ReadNoWait(long           from                     );  
  bool                            Write     (long           to                      ,
                                             const char   * clsn                    ,
                                             const char   * mthn                    ,
                                             const char   * cmd                     ,
                                             const char   * args                    ,
                                             unsigned int   priority=LOWEST_PRIORITY );

  struct Monicelli::msg         * Replay    (long           to                      ,
                                             const char   * clsn                    ,
                                             const char   * mthn                    ,
                                             const char   * cmd                     ,
                                             const char   * args                    ,
                                             unsigned int   priority=LOWEST_PRIORITY );
                                    
  inline bool                    Write      (long           to                      ,
                                             std::string    clsn                    ,
                                             std::string    mthn                    ,
                                             std::string    cmd                     ,
                                             std::string    args                    ,
                                             unsigned int   priority=LOWEST_PRIORITY ) {                                          
                                                                                        return Write(  to           ,
                                                                                                       clsn.c_str() ,
                                                                                                       mthn.c_str() ,
                                                                                                       cmd.c_str()  ,
                                                                                                       args.c_str() ,
                                                                                                       priority      );
                                                                                       }
  inline struct Monicelli::msg  * Replay    (long           to                      ,
                                             std::string    clsn                    ,
                                             std::string    mthn                    ,
                                             std::string    cmd                     ,
                                             std::string    args                    ,
                                             unsigned int   priority=LOWEST_PRIORITY ) {                                
                                                                                        return Replay( to           ,
                                                                                                       clsn.c_str() ,
                                                                                                       mthn.c_str() ,
                                                                                                       cmd.c_str()  ,
                                                                                                       args.c_str() ,
                                                                                                       priority      );
                                                                                       }
  inline bool                     Write     (long           to                      ,
	                                     std::string    clsn                    ,
	                                     std::string    mthn                    ,
	                                     std::string    cmd                     ,
	                                     unsigned int   priority=LOWEST_PRIORITY ) {
	                                                                                return Write(   to           ,
                                                                                                        clsn.c_str() ,
                                                                                                        mthn.c_str() ,
                                                                                                        cmd.c_str()  ,
                                                                                                        ""           ,
                                                                                                        priority      );
                                                                                       }
  inline struct Monicelli::msg * Replay     (long           to                      ,
	                                     std::string    clsn                    ,
	                                     std::string    mthn                    ,
	                                     std::string    cmd                     ,
	                                     unsigned int   priority=LOWEST_PRIORITY ) {
	                                                                                return Replay(  to           ,
                                                                                                        clsn.c_str() ,
                                                                                                        mthn.c_str() ,
                                                                                                        cmd.c_str()  ,
                                                                                                        ""           ,
                                                                                                        priority      );
                                                                                       }
                                            

  inline bool                    Write      (long           to                      ,
	                                     const char   * clsn                    ,
	                                     const char   * mthn                    ,
	                                     const char   * cmd                     ,
	                                     unsigned int priority=LOWEST_PRIORITY   ) {
	                                                                                return Write(   to           ,
                                                                                                        clsn         ,
                                                                                                        mthn         ,
                                                                                                        cmd          ,
                                                                                                        ""           ,
                                                                                                        priority      );
                                                                                       };
  inline struct Monicelli::msg * Replay     (long           to                      ,
	                                     const char   * clsn                    ,
	                                     const char   * mthn                    ,
	                                     const char   * cmd                     ,
	                                     unsigned int priority=LOWEST_PRIORITY   ) {
	                                                                                return Replay(  to           ,
                                                                                                        clsn         ,
                                                                                                        mthn         ,
                                                                                                        cmd          ,
                                                                                                        ""           ,
                                                                                                        priority      );
                                                                                       };
         bool                    Check      (long to                                 );
         void                    Clear      (long to                                 );

         void                    Remove     (void                                    );

  inline const char *            GetProcId  (int id                                  ) {return proc_id[id];}
  
private:

  int                         msgqueue_id;
  struct Monicelli::msg       themessage ;
  long                        source_    ;

  std::map<int, const char *> proc_id    ;

};

#endif
