/*============================================================
  Purely virtual description of a process that can run
  concurrently with other processes on a multi-processor
  machine

  Author: D. Menasce
 ===========================================================*/

#ifndef CPROCESS_H
#define CPROCESS_H

#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include "util.h"


#include "ANSIColors.h"

#include <QFutureWatcher>
//#include <QMainWindow>
#include <QObject>
#include <QProgressDialog>
#include <QString>

class cProcess : public QObject
{
  Q_OBJECT

public:
               cProcess        (void                            ) { number_ = 0;                                                              ;}
              ~cProcess        (void                            ) {                                                                           ;}

//              static int  wrapperToExecute(const QString & imageFileName   ) {return derivedClassPtr_[currentClassName_]->execute(imageFileName)         ;}
               int  wrapperToExecute(const QString & imageFileName   ) {return derivedClassPtr_[currentClassName_]->execute(imageFileName)         ;}

   void        setWatcher      (QFutureWatcher<int>* watcher    ) {theWatcher_  = watcher                                                     ;}
//   void        setProcess      (std::string          processName) {processName_ = processName                                                 ;}
   void        setIterations   (int                  iterations ) {iterations_  = iterations                                                  ;}

   std::string getProcess      (void                            ) {return className_                                                          ;}

   void        setClassName    (std::string          className  ) {className_   = className                                                   ;}
   void        setCurrentClassName(std::string          className  ) {currentClassName_   = className                                         ;}

   std::string getClassName(void)                                 {return className_;}
   std::string getCurrentClassName(void)                          {return currentClassName_;}

 public slots:

   void        started         (void                            ) ;
   void        showResult      (int                             ) ;

 private:

   QFutureWatcher<int> * theWatcher_        ;
   QProgressDialog     * theProgressDialog_ ;

   int                   iterations_        ;

   static std::string    currentClassName_  ;

   std::stringstream     ss_                ;

   std::map<std::string, cProcess *> derivedClassPtr_   ;
  // static std::map<std::string, cProcess *> derivedClassPtr_   ;

protected:

   virtual int execute(const QString & imageFileName) = 0 ;

   void setDerivedPtr(cProcess * derivedPtr) {derivedClassPtr_[className_] = derivedPtr;}

   std::string    className_;
   int            number_  ;
};

#endif // CPROCESS_H
