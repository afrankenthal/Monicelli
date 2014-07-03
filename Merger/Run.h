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
