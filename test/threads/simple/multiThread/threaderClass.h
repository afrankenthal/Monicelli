#ifndef THREADERCLASS_H
#define THREADERCLASS_H

#include "MessageTools.h"
#include <QThread>
#include <qmutex.h>
#include <sstream>
#include <vector>

class QThread ;
class process;

class threader : public QThread
{
  public:
    threader               (void                 ) {;}
    ~threader              (void                 ) {;}
    void destroy           (void);
    void setNumberOfThreads(int nOfThreads);
    void reset             (int nOfThreads=-1);

    void run               (void                 ) ;

    void execute           (void                 ) ;
    int  getCurrentEvent   (void                 ) ;
    void setProcess        (process * theProcess,
                            int       maxEvents  ) ;
    int  getLastEvent      (void);
    void incrementLastEvent(void);

  private:

    std::stringstream ss_ ;

    std::vector<process*> theProcesses_;
    int              lastEvent_;
    QMutex           mutex_;

} ;

#endif // THREADERCLASS_H
