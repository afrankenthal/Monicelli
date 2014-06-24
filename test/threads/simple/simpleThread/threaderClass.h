#ifndef THREADERCLASS_H
#define THREADERCLASS_H

#include "MessageTools.h"
#include "process.h"
#include <QThread>
#include <sstream>

class QThread ;

class threader : public QThread
{
  public:
     threader           (void                 ) {;}
    ~threader           (void                 ) {;}

    void run            (void                 ) ;

    void execute        (void                 ) ;
    int  getCurrentEvent(void                 ) {return theProcess_->getCurrentEvent();}
    void setProcess     (process * theProcess,
                         int       maxEvents  ) ;

  private:

    std::stringstream ss_ ;

    process * theProcess_ ;

} ;

#endif // THREADERCLASS_H
