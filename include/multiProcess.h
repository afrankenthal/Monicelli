/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef MULTIPROCESS_H
#define MULTIPROCESS_H

#include <sstream>
#include "MessageTools.h"

#include "Event.h"
#include "Geometry.h"
#include "process.h"
#include "subProcess.h"

template<class Class,class Output>
class multiProcess : public process
{
 public:
            multiProcess(void) {;}
   virtual ~multiProcess(void) {;}

  typedef Output (Class::*multiProcess_function)();

  virtual bool           execute     (void                          ) = 0;
  virtual void           setOperation(multiProcess_function function){processOperation_=function;}
  multiProcess_function  getOperation(void                          ){return processOperation_  ;}

 protected:
  multiProcess_function  processOperation_;
 } ;

#endif // MULTIPROCESS_H
