/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef PROCESS_H
#define PROCESS_H

#include <sstream>
#include "MessageTools.h"

#include "Event.h"
#include "Geometry.h"
//#include "subProcess.h"

#define NO_PROCESS_LABEL_ "No process"

class process
{
 public:
   process(void) {;}
   virtual ~process(void) {;}

  virtual bool         execute            (void) = 0 ;

  virtual std::string  getLabel           (void) = 0;
  virtual int          getMaxIterations   (void) = 0;
  int                  getCurrentIteration(void) {return currentIteration_;}
  virtual std::string  getName            (void) = 0;

 protected:
  int                  currentIteration_ ;
  std::stringstream    ss_               ;
 } ;

#endif // PROCESS_H
