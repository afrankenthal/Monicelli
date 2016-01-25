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

  virtual bool  	    execute	(void		   		  ){return false;}
  virtual void  	    execute	(Arg1 * par1, 
                                         Arg2 * par2       		  ) = 0;
  void                      setOperation(subMultiProcess_function function){subProcessOperation_=function;}
  subMultiProcess_function  getOperation(void                             ){return subProcessOperation_  ;}

  protected:
    subMultiProcess_function   subProcessOperation_;

 } ;


#endif // SUBMULTIPROCESS_H
