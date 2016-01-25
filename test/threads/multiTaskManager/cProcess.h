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
