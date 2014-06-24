#ifndef THREADERCLASS_H
#define THREADERCLASS_H

#include <QThread>
#include <sstream>
#include <QtConcurrentRun>
#include "MessageTools.h"
#include "HManager.h"
#include "process.h"

class QThread ;
class QObject ;

class threader : public QThread
{
  Q_OBJECT

  public:
     threader               (void                    ) {;}
    ~threader               (void                    ) {;}

    void         run                   (void                    ) ;

    template<class Class> void runTheMethod(Class * object, void (Class::*fn)()    ) ;

    void         execute               (void                                       ) ;
    int          getCurrentIteration   (void                                       ) {return theProcess_->getCurrentIteration();}
    process *    getCurrentProcess     (void                                       ) ;
    void         setProcess            (process * theProcess                       ) ;
    bool         isRunning             (void                                       ) ;
    bool         isFinished            (void                                       ) ;
    void         terminate             (void                                       ) ;
    void         setFuture             (QFuture<void>  future                      ) {future_ = future;}

  signals:
    void processFinished(process*, bool success);

  private:

    QFuture<void> future_;

    std::stringstream ss_ ;

    process * theProcess_ ;

} ;

#endif // THREADERCLASS_H
