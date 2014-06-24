#ifndef PROCESS_H
#define PROCESS_H

#include <sstream>
#include "MessageTools.h"

class process
{
  public:
   process(void) {;}
   virtual ~process(void) {;}

  virtual void execute        (void         ) = 0                          ;


  virtual void setMaxEvents   (int maxEvents) = 0                          ;
  virtual int  getCurrentEvent(void         ) = 0                          ;

  private:

 } ;

#endif // PROCESS_H
