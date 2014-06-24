#include <iostream>

#include "Event.h"

ClassImp(Event) 

//===================================================
Event::Event()
{
  myStruct_.x_ = -1 ;
  myStruct_.y_ = -1 ;
}

//===================================================
void Event::setEventNumber(int event)
{
 eventNumber_ = event ;
}

//===================================================
int Event::getEventNumber()
{
 return eventNumber_ ;
}

//===================================================
void Event::addTrack(double p)
{
 p_.push_back(p) ;
}

//===================================================
void Event::addVertex(int v)
{
 vertices_.push_back(v) ;
}

//===================================================
int Event::getNTracks(void)
{
 return (int)p_.size() ;
}

//===================================================
std::vector<int> Event::getVertices()
{
 return vertices_ ;
}

//===================================================
void Event::cleanData(void)
{
 p_.clear() ;
}

//===================================================
void Event::dumpTracks()
{
 std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tFound " << p_.size() << " tracks for event " << eventNumber_ << std::endl ;

 std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\t  x: " << myStruct_.x_ << std::endl ;
 
 for(std::vector<double>::iterator it=p_.begin(); it!=p_.end(); ++it)
 {
   std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\t  " << *it << std::endl ;
 }

 std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tFound " << vertices_.size() << " vertices for event " << eventNumber_ << std::endl ;
 for(std::vector<int>::iterator it=vertices_.begin(); it!=vertices_.end(); ++it)
 {
   std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\t  " << *it << std::endl ;
 }
}
