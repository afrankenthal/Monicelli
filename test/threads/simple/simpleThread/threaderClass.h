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
 
#ifndef THREADERCLASS_H
#define THREADERCLASS_H

#include "MessageTools.h"
#include "process.h"
#include <QThread>
#include <sstream>

class QThread ;

class threader : public QThread
{
  public:
     threader           (void                 ) {;}
    ~threader           (void                 ) {;}

    void run            (void                 ) ;

    void execute        (void                 ) ;
    int  getCurrentEvent(void                 ) {return theProcess_->getCurrentEvent();}
    void setProcess     (process * theProcess,
                         int       maxEvents  ) ;

  private:

    std::stringstream ss_ ;

    process * theProcess_ ;

} ;

#endif // THREADERCLASS_H
