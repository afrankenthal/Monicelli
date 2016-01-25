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
 
#ifndef _Event_h_
#define _Event_h_

#include "Hit.h"
#include <vector>
#include <map>
#include <iostream>

class Event{
public:
  Event();
  Event(const Event& event);
  ~Event();

  //Setters
  void setTimestamp            (unsigned long long time){beginTimestamp_ = time; currentTimestamp_ = time;timestamps_.push_back(time);}
  void addTimestamp            (unsigned long long time){currentTimestamp_ = time;timestamps_.push_back(time);}
  void setTriggerNumber        (unsigned int trigger)   {triggerNumber_ = trigger;}
  void setHardwareTriggerNumber(unsigned int trigger);
  void setReset                (bool reset)             {reset_ = reset;}

  //Getters
  std::vector<unsigned long long> getTimestamps             (void)	           {return timestamps_;}
  const unsigned long long&       getTimestamp              (void)		   {return beginTimestamp_;}
  unsigned long long              getCurrentTimestamp       (void)		   {return currentTimestamp_;}
  unsigned int  		  getTriggerNumber	    (void)	           {return triggerNumber_;}
  unsigned int  		  getHardwareTriggerNumber  (void)	           {return hardwareTriggerNumber_;}
  unsigned int  		  getNumberOfHits	    (void)	           {return hits_.size();}
  Hit&          		  getHit		    (unsigned int hitN);

  //Other Methods
  bool isReset   (void)	  {return reset_;}
  void addHit	 (const Hit& hit);
  void reset	 (void);
  
  Event& operator= (const Event& event);
  Event& operator+=(const Event& event);
  
  friend std::ostream& operator<<(std::ostream& os, Event& event){
//    os << "[Event::operator<<()]\t"
    os
       << "Trigger: "   << event.triggerNumber_
       << " Time: "     << event.beginTimestamp_
       << " N times: "  << event.timestamps_.size() 
       << " N hits: "   << event.hits_.size()
       << " Reset: "    << event.reset_
       << std::endl;
    return os;
  }
  
private:
  unsigned long long	                  	     	   beginTimestamp_;
  unsigned long long                      	     	   currentTimestamp_;
  unsigned int		                  	     	   triggerNumber_;
  unsigned int		                  	     	   hardwareTriggerNumber_;
  bool        		                  	     	   reset_;
  std::vector<Hit*>                       	     	   hits_;
  std::multimap<unsigned long long,Hit*>   	     	   timestampHits_;
  std::vector<unsigned long long>         	     	   timestamps_;
};


#endif
