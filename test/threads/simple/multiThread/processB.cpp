#include "processB.h"

//=======================================================================
void processB::execute(void)
{
 for(currentEvent_=0; currentEvent_<maxEvents_; ++currentEvent_)
  {
    sleep(1) ;
    ss_.str("") ;
    ss_ << "Event " << currentEvent_ ;
    STDLINE(ss_.str(),ACBlack) ;
  }

  STDLINE("Job done!",ACBlack) ;
}
