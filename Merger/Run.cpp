#include "Run.h"
#include <iostream>

using namespace std;

//--------------------------------------------------------------------
Run::Run()
{
}

//--------------------------------------------------------------------
Run::~Run()
{
}

//--------------------------------------------------------------------
void Run::reset(void)
{
  eventsMap_.clear();
}

//--------------------------------------------------------------------
void Run::addEvent(Event& event)
{
//  cout << "[Run::addEvent()]\tN of Events: " << events_.size() << endl;
  const unsigned int& triggerNumber = event.getHardwareTriggerNumber();
//  cout << __PRETTY_FUNCTION__ << "Trig: " << triggerNumber << endl;
  if(eventsMap_.find(triggerNumber) != eventsMap_.end())
    eventsMap_[triggerNumber] += event;
  else
    eventsMap_[triggerNumber] = event;
}

//--------------------------------------------------------------------
Event& Run::getEvent(unsigned int triggerNumber)
{
  if( !(eventsMap_.find(triggerNumber) != eventsMap_.end()) )
    eventsMap_[triggerNumber] = Event();
  return eventsMap_[triggerNumber];
}

//--------------------------------------------------------------------
unsigned int Run::getNumberOfEvents(void)
{
  return eventsMap_.size();
}
