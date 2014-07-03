#include "PxEvent.h"

#include <iostream>


using namespace std;
//using namespace PxSuite;

//ClassImp(PxEvent)

//========================================================================================================================
PxEvent::PxEvent() :
        triggerNumber_(-1),
        bcoNumber_    (-1)
{}

//========================================================================================================================
PxEvent::PxEvent(int64_t bcoNumber) :
        triggerNumber_(-1),
        bcoNumber_    (bcoNumber)
{}

//========================================================================================================================
PxEvent::~PxEvent(void)
{}

//========================================================================================================================
int32_t PxEvent::getTriggerNumber(void) const
{
    return triggerNumber_;
}

//========================================================================================================================
int32_t PxEvent::getTriggerBcoNumber(void) const
{
    return triggerBcoNumber_;
}

//========================================================================================================================
int64_t PxEvent::getBCONumber(void) const
{
    return bcoNumber_;
}

//========================================================================================================================
unsigned int PxEvent::getNumberOfHits(void) const
{
    return rawHits_.size();
}

//========================================================================================================================
uint32_t PxEvent::getRawHit(unsigned int position) const
{
    if(position < rawHits_.size())
        return rawHits_[position];
    cout << __PRETTY_FUNCTION__ << "Asking for raw hit in position " << position << " but there are only " << rawHits_.size() << " hits!" << endl;
    return 0;
}

//========================================================================================================================
const std::vector<uint32_t>& PxEvent::getRawHits(void) const
{
    return rawHits_;
}

//========================================================================================================================
Hit PxEvent::getDecodedHit(unsigned int position) const
{
    if(position >= decodedHits_.size())
      cout << __PRETTY_FUNCTION__ << "Asking for decoded hit in position " << position << " but there are only " << decodedHits_.size() << " hits!" << endl;
    return decodedHits_[position];
}

//========================================================================================================================
const std::vector<Hit>& PxEvent::getDecodedHits(void) const
{
    return decodedHits_;
}

//========================================================================================================================
void PxEvent::setTriggerNumber(int32_t triggerNumber)
{
    triggerNumber_ = triggerNumber;
}

//========================================================================================================================
void PxEvent::setTriggerBcoNumber(int32_t triggerBcoNumber)
{
    triggerBcoNumber_ = triggerBcoNumber;
}

//========================================================================================================================
void PxEvent::setBCONumber(int64_t bcoNumber)
{
    bcoNumber_ = bcoNumber;
}

//========================================================================================================================
void PxEvent::setRawHit(uint32_t rawHit)
{
    rawHits_.push_back(rawHit);
}

//========================================================================================================================
void PxEvent::setRawHits(const std::vector<uint32_t>& rawHits)
{
    rawHits_ = rawHits;
}

//========================================================================================================================
void PxEvent::setDecodedHit(const Hit& hit)
{
    decodedHits_.push_back(hit);
}

//========================================================================================================================
void PxEvent::setDecodedHits(const std::vector<Hit>& hits)
{
    decodedHits_ = hits;
}

//========================================================================================================================
PxEvent PxEvent::operator+=(const PxEvent& event)
{
  //cout << event.getTriggerNumber() << ":" << this->getTriggerNumber() << endl;
  if(this->getTriggerNumber() == -1)
    this->setTriggerNumber(event.getTriggerNumber());
  for(unsigned int i=0; i<event.getNumberOfHits(); i++)
  {
    this->setRawHit(event.getRawHit(i));
    this->setDecodedHit(event.getDecodedHit(i));    
  }  
  return *this;
}

//========================================================================================================================
void PxEvent::addTriggerNumberToHits(void)
{
  int64_t triggerNumberOrdered = ((triggerNumber_ & 0xff) << 16) 
                               + (((triggerNumber_>>8) & 0xff) << 8) 
                               + (((triggerNumber_>>16) & 0xf));
  
  for(std::vector<Hit>::iterator it=decodedHits_.begin(); it!=decodedHits_.end(); it++)
  {
    triggerNumberOrdered += (it->data & 0xf000);
    it->data = (it->data & 0xffffffff000000ff) + ((triggerNumberOrdered & 0xffffff) << 8);
  }
  /*
  */
}
