/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

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

         struct Monicelli::msg * ReadWait   ( long           from                     );  
         struct Monicelli::msg * ReadNoWait ( long           from                     );  
         bool         Write      ( long           to                      ,
                                   const char   * clsn                    ,
                                   const char   * mthn                    ,
                                   const char   * cmd                     ,
                                   const char   * args                    ,
                                   unsigned int   priority=LOWEST_PRIORITY );

         struct Monicelli::msg * Replay     ( long           to                      ,
                                   const char   * clsn                    ,
                                   const char   * mthn                    ,
                                   const char   * cmd                     ,
                                   const char   * args                    ,
                                   unsigned int   priority=LOWEST_PRIORITY );
                                   
  inline bool         Write      ( long           to                      ,
                                   std::string    clsn                    ,
                                   std::string    mthn                    ,
                                   std::string    cmd                     ,
                                   std::string    args                    ,
                                   unsigned int   priority=LOWEST_PRIORITY ) 
                                 {                                        
                                   return Write(  to                      ,
                                                  clsn.c_str()            ,
                                                  mthn.c_str()            ,
                                                  cmd.c_str()             ,
                                                  args.c_str()            ,
                                                  priority                 );
                                 }
                                                                          
   inline struct Monicelli::msg * Replay    ( long           to                      ,
                                   std::string    clsn                    ,
                                   std::string    mthn                    ,
                                   std::string    cmd                     ,
                                   std::string    args                    ,
                                   unsigned int   priority=LOWEST_PRIORITY ) 
                                 { 
                                   return Replay( to                      ,
                                                  clsn.c_str()            ,
                                                  mthn.c_str()            ,
                                                  cmd.c_str()             ,
                                                  args.c_str()            ,
                                                  priority                 );
                                 }

  inline bool         Write      ( long           to                      ,
	                           std::string    clsn                    ,
	                           std::string    mthn                    ,
	                           std::string    cmd                     ,
	                           unsigned int   priority=LOWEST_PRIORITY )
	                         {
                                  return Write(   to                      ,
                                                  clsn.c_str()            ,
                                                  mthn.c_str()            ,
                                                  cmd.c_str()             ,
                                                  ""                      ,
                                                  priority                 );
                                 }

  inline struct Monicelli::msg * Replay     ( long           to                      ,
	                           std::string    clsn                    ,
	                           std::string    mthn                    ,
	                           std::string    cmd                     ,
	                           unsigned int   priority=LOWEST_PRIORITY )
	                         {
                                  return Replay(  to                      ,
                                                  clsn.c_str()            ,
                                                  mthn.c_str()            ,
                                                  cmd.c_str()             ,
                                                  ""                      ,
                                                  priority                 );
                                 }

  inline bool         Write      ( long           to                      ,
	                           const char   * clsn                    ,
	                           const char   * mthn                    ,
	                           const char   * cmd                     ,
	                           unsigned int priority=LOWEST_PRIORITY   ) 
	                         {
                                  return Write(   to                      ,
                                                  clsn                    ,
                                                  mthn                    ,
                                                  cmd                     ,
                                                  ""                      ,
                                                  priority                 );
                                 };

  inline struct Monicelli::msg * Replay     ( long           to                      ,
	                           const char   * clsn                    ,
	                           const char   * mthn                    ,
	                           const char   * cmd                     ,
	                           unsigned int priority=LOWEST_PRIORITY   ) 
	                         {
                                  return Replay(  to                      ,
                                                  clsn                    ,
                                                  mthn                    ,
                                                  cmd                     ,
                                                  ""                      ,
                                                  priority                 );
                                 };

         bool         Check      ( long to                                 );
         void         Clear      ( long to                                 );

         void         Remove     ( void                                    );

  inline const char * GetProcId  (int id) 
                                 {
                                  return proc_id[id];
                                 }
  
private:

  int        msgqueue_id;
  struct Monicelli::msg themessage ; 
  long       source_    ;

  std::map<int, const char *> proc_id ;

};

#endif
