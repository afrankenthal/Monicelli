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
 
#ifndef PROCESS_H
#define PROCESS_H

#include <sstream>
#include "MessageTools.h"
#include <QThread>
#include "threaderClass.h"

//class threader;

class process : public QThread
{
public:
    process(int processNumber=-1) : processNumber_(processNumber),currentEvent_(0){;}
    virtual ~process(void) {;}

    void         run            (void         ){this->execute(); exec();}
    virtual void execute        (void         ){ //std::cout << __PRETTY_FUNCTION__ << maxEvents_ << std::endl;
            while( (currentEvent_=theThreader_->getLastEvent())<maxEvents_)
        //    while(++currentEvent_<maxEvents_)
            {
                theThreader_->incrementLastEvent();
                ss_.str("") ;
                ss_ << "Processing event " << currentEvent_ << " for process: " << processNumber_ ;
                STDLINE(ss_.str(),ACWhite) ;
                analyze(currentEvent_);
            }
            currentEvent_ = 0;
            ss_.str("");
            ss_ << "Process " << processNumber_ << " done!";
            STDLINE(ss_.str(),ACWhite) ;
    }


    virtual void begin          (void         )         {;}
    virtual void analyze        (int event    )         {;}
    virtual void end            (void         )         {;}

    virtual void setMaxEvents   (int maxEvents)        {maxEvents_ = maxEvents;}
    virtual void setThreader    (threader* theThreader){theThreader_ = theThreader;}
    virtual int  getCurrentEvent(void         )         = 0;

protected:
    std::stringstream ss_;
    threader* theThreader_;
    int       maxEvents_;
    int       processNumber_;
    int       currentEvent_ ;

} ;

#endif // PROCESS_H
