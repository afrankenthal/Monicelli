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
                         multiProcess(void                          ) {;}
   virtual              ~multiProcess(void                          ) {;}

  typedef                Output      (Class::*multiProcess_function )();

  virtual bool           execute     (void                          ) = 0;
  virtual void           setOperation(multiProcess_function function) {processOperation_=function;}
  multiProcess_function  getOperation(void                          ) {return processOperation_  ;}

 protected:
  multiProcess_function  processOperation_;
 } ;

#endif // MULTIPROCESS_H
