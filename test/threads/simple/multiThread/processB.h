#ifndef PROCESSB_H
#define PROCESSB_H

#include "process.h"
#include <sstream>

class process ;

class processB : public process
{
public:
   processB            (void         ) {;}
  ~processB            (void         ) {;}

   void execute        (void         ) ;
   int  getCurrentEvent(void         ) {return currentEvent_ ;}

private:

   std::stringstream ss_ ;

   //int currentEvent_ ;
} ;

#endif // PROCESSB_H
