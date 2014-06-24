#ifndef PROCESSA_H
#define PROCESSA_H

#include "process.h"
#include <sstream>

class process ;

class processA : public process
{
  public:
    processA             (int processNumber=-1) : process(processNumber){;}
     ~processA           (void         ) {;}

//     void execute        (void         ) ;
     void analyze        (int event    ) ;
     int  getCurrentEvent(void         ) {return currentEvent_ ;}

  private:

     std::stringstream ss_ ;

     //int currentEvent_ ;
} ;


#endif // PROCESSA_H
