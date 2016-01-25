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
                                                            ss_ << "No header settings implemented(" 
							        << theHeader 
								<< ")";
                                                            STDLINE(ss_.str(),ACRed);
                                                           }

  protected:

 } ;

#endif // SUBPROCESS_H
