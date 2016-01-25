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
 
#ifndef _Run_h_
#define _Run_h_

#include "Event.h"
#include <vector>
#include <map>

class Run{
 public:
  Run();
  ~Run();
//  Run& operator=(const Run& a){
//    this->events_.clear();
//    this->events_ = a.events_;
//    return *this;
//  };
  
  void addEvent(Event& event);
  Event& getEvent(unsigned int trigger);
  void reset   (void);
//  std::map<unsigned int, Event>::iterator& begin(void){return eventsMap_.begin();}
//  std::map<unsigned int, Event>::iterator& end  (void){return eventsMap_.end();}

  //Getters
  std::map<unsigned int, Event>& getEvents        (void) {return eventsMap_;}
  unsigned int                   getNumberOfEvents(void);
  
 private:
  //std::vector<Event> events_;
  //      <Trigger number, event>
  std::map<unsigned int, Event> eventsMap_;
};




#endif 
