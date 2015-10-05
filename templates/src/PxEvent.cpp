#include "PxEvent.h"

#include <iostream>


using namespace std;
//using namespace PxSuite;

ClassImp(PxEvent)

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
PxDecodedHit PxEvent::getDecodedHit(unsigned int position) const
{
    if(position >= PxDecodedHits_.size())
      cout << __PRETTY_FUNCTION__ << "Asking for decoded hit in position " << position << " but there are only " << PxDecodedHits_.size() << " hits!" << endl;
    return PxDecodedHits_[position];
}

//========================================================================================================================
const std::vector<PxDecodedHit>& PxEvent::getDecodedHits(void) const
{
    return PxDecodedHits_;
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
void PxEvent::setDecodedHit(const PxDecodedHit& PxDecodedHit)
{
    PxDecodedHits_.push_back(PxDecodedHit);
}

//========================================================================================================================
void PxEvent::setDecodedHits(const std::vector<PxDecodedHit>& PxDecodedHits)
{
    PxDecodedHits_ = PxDecodedHits;
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
