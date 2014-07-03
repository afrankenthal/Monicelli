#include "Event.h"
#include <iostream>
#include <math.h>

using namespace std;


//-----------------------------------------------------------------------
Event::Event(void) : beginTimestamp_       (0)
                   , currentTimestamp_     (0)
		   , triggerNumber_        (0)
		   , hardwareTriggerNumber_(0)
		   , reset_                (false)

{
//  cout << __PRETTY_FUNCTION__ << hits_.size() << endl;
}

//-----------------------------------------------------------------------
Event::Event(const Event& event)
{
  *this=event;
//  cout << __PRETTY_FUNCTION__ << hits_.size() << endl;
}

//-----------------------------------------------------------------------
Event& Event::operator=(const Event& event)
{
  reset();
  beginTimestamp_	= event.beginTimestamp_;
  currentTimestamp_	= event.currentTimestamp_;
  triggerNumber_	= event.triggerNumber_;
  hardwareTriggerNumber_= event.hardwareTriggerNumber_;
  reset_		= event.reset_;
  timestamps_ 		= event.timestamps_;

  Hit* hitP;
  for(multimap<unsigned long long, Hit*>::const_iterator it=event.timestampHits_.begin(); it!=event.timestampHits_.end(); it++){
    hitP = new Hit(*(it->second));
    hits_.push_back(hitP);
    timestampHits_.insert(pair<unsigned long long,Hit*>(it->first, hitP));  
  }  
//  cout << __PRETTY_FUNCTION__ << hits_.size() << endl;
  return *this;
}

//-----------------------------------------------------------------------
Event::~Event(void)
{
//  cout << __PRETTY_FUNCTION__ << hits_.size() << endl;
  reset();
}

//-----------------------------------------------------------------------
void Event::setHardwareTriggerNumber(unsigned int trigger)
{
  hardwareTriggerNumber_ = trigger;
}

//-----------------------------------------------------------------------
void Event::addHit(const Hit& hit){
  Hit* hitP = new Hit(hit);
  hits_.push_back(hitP);
  timestampHits_.insert(pair<unsigned long long,Hit*>(currentTimestamp_,hitP));
}

//-----------------------------------------------------------------------
Hit& Event::getHit(unsigned int hitN){
  return *(hits_[hitN]);
}

//-----------------------------------------------------------------------
void Event::reset(void)
{
  beginTimestamp_        = 0;
  currentTimestamp_      = 0;
  triggerNumber_         = 0;
  hardwareTriggerNumber_ = 0;
  reset_                 = false;
  
  for(vector<Hit*>::iterator it = hits_.begin(); it != hits_.end(); it++)
    delete *it;
  hits_.clear();

  timestampHits_           .clear();
  timestamps_              .clear();
}

//-----------------------------------------------------------------------
Event& Event::operator+=(const Event& event)
{
  Hit* hitP = 0;
  for(multimap<unsigned long long,Hit*>::const_iterator it=event.timestampHits_.begin(); it!=event.timestampHits_.end(); it++)
  {
    hitP = new Hit(*(it->second));
    hits_.push_back(hitP);
    timestampHits_.insert(pair<unsigned long long,Hit*>(it->first, hitP));
    if(!(timestampHits_.find(it->first) != timestampHits_.end())){
      timestamps_.push_back(it->first);
    }
  }

  beginTimestamp_ = timestampHits_.begin()->first;
  if(triggerNumber_ == 0)
  {
    triggerNumber_ = event.triggerNumber_;
    hardwareTriggerNumber_ = event.hardwareTriggerNumber_;
  }
  return *this;
}
