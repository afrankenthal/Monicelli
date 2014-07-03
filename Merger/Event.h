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
