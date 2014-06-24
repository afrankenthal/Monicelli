#ifndef PROCESSA_H
#define PROCESSA_H

#include "process.h"
#include <sstream>


class process ;

class processA : public process
{
  public:
      processA           (void         ) {;}
     ~processA           (void         ) {;}

     void execute        (void         ) ;
     int  getCurrentEvent(void         ) {return currentEvent_ ;}
     void setMaxEvents   (int maxEvents) {maxEvents_ = maxEvents;}

  private:

     std::stringstream ss_ ;

     int currentEvent_ ;
     int maxEvents_    ;
} ;


#endif // PROCESSA_H
