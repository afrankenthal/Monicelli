#include "PxUtilities/PxEventBuilder/include/PxEvent.h"

#include <iostream>


using namespace std;
using namespace PxSuite;

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
int32_t PxEvent::getTriggerNumber(void)
{
    return triggerNumber_;
}

//========================================================================================================================
int64_t PxEvent::getBCONumber(void)
{
    return bcoNumber_;
}

//========================================================================================================================
uint32_t PxEvent::getRawHit(unsigned int position)
{
    if(position < rawHits_.size())
        return rawHits_[position];
    cout << __PRETTY_FUNCTION__ << "Asking for raw hit in position " << position << " but there are only " << rawHits_.size() << " hits!" << endl;
    return 0;
}

//========================================================================================================================
unsigned int PxEvent::getNumberOfRawHits(void)
{
    return rawHits_.size();
}

//========================================================================================================================
const std::vector<uint32_t>& PxEvent::getRawHits(void)
{
    return rawHits_;
}

//========================================================================================================================
void PxEvent::setTriggerNumber(int32_t triggerNumber)
{
    triggerNumber_ = triggerNumber;
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
