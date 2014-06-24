/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef SUBMULTIPROCESS_H
#define SUBMULTIPROCESS_H

#include <sstream>
#include "MessageTools.h"

#include "Event.h"
#include "Geometry.h"
#include "subProcess.h"

class Event;

template<class Class, class Arg1, class Arg2>
class subMultiProcess : public subProcess<Arg1,Arg2>
{
  public:
   subMultiProcess(void) {;}
   virtual ~subMultiProcess(void) {;}

  typedef void (Class::*subMultiProcess_function)(Arg1*,Arg2*);

  virtual bool execute            (void              ){return false;}
  virtual void execute            (Arg1 * par1, Arg2 * par2) = 0;
  void                      setOperation(subMultiProcess_function function){subProcessOperation_=function;}
  subMultiProcess_function  getOperation(void                             ){return subProcessOperation_  ;}

  protected:
    subMultiProcess_function   subProcessOperation_;

 } ;


#endif // SUBMULTIPROCESS_H
