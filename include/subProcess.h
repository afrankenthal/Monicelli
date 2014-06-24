/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef SUBPROCESS_H
#define SUBPROCESS_H

#include <sstream>
#include "MessageTools.h"

#include "Event.h"
#include "Geometry.h"
#include "EventHeader.h"
#include "process.h"

class Event;

template<class Arg1, class Arg2>
class subProcess : public process
{
  public:
   subProcess(void) {;}
   virtual ~subProcess(void) {;}

  virtual bool execute            (void       ) {return false;}
  virtual void execute            (Arg1 * par1,
                                   Arg2 * par2) = 0;

  virtual int  getMaxIterations   (void       ) {return -1;   }
  virtual int  getCurrentIteration(void       ) {return -1;   }

  virtual void setHeader          (EventHeader *theHeader) {
                                                             ss_.str("");
                                                             ss_ << "No header settings implemented(" << theHeader << ")";
                                                             STDLINE(ss_.str(),ACRed);
                                                           }

  protected:


 } ;

#endif // SUBPROCESS_H
