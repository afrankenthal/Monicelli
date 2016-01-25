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
 
#include "processA.h"
#include "threaderClass.h"

//=======================================================================
/*
void processA::execute(void)
{
    //std::cout << __PRETTY_FUNCTION__ << maxEvents_ << std::endl;
    while( (currentEvent_=theThreader_->getLastEvent())<maxEvents_)
//    while(++currentEvent_<maxEvents_)
    {
        theThreader_->incrementLastEvent();
        ss_.str("") ;
        ss_ << "Taken event " << currentEvent_ << " for process: " << processNumber_;
        STDLINE(ss_.str(),ACWhite) ;
        int counter = 0;
        for(int i=0; i<1000000000;i++)
        {
            counter++;
        }
        ss_.str("") ;
        ss_ << "Processed event " << currentEvent_ << " for process: " << processNumber_ << " " << counter;
        STDLINE(ss_.str(),ACWhite) ;
    }
    currentEvent_ = 0;
    ss_.str("");
    ss_ << "Process " << processNumber_ << " done!";
    STDLINE(ss_.str(),ACWhite) ;
}
*/
//=======================================================================
void processA::analyze(int event)
{
    ss_.str("") ;
    ss_ << "Analyzing event " << event << " for process: " << processNumber_;
    STDLINE(ss_.str(),ACWhite) ;
    int counter = 0;
    for(int i=0; i<1000000000;i++)
    {
        counter++;
    }
}
