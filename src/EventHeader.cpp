/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include "EventHeader.h"
#include "MessageTools.h"

ClassImp(EventHeader);

//================================================================================
EventHeader::EventHeader(void):
    clustersDone_    (false)
  , tracksFound_     (false)
{
}

//================================================================================
EventHeader::~EventHeader(void)
{
//    STDLINE("EventHeader::~EventHeader",ACRed);
}

//================================================================
void EventHeader::setClustersDone(bool done)
{
    clustersDone_ = done;
}

//================================================================
bool EventHeader::clustersDone(void)
{
    return clustersDone_;
}

//================================================================
void EventHeader::setTracksFound(bool found)
{
    tracksFound_ = found;
}

//================================================================
bool EventHeader::tracksFound(void)
{
    return tracksFound_;
}


