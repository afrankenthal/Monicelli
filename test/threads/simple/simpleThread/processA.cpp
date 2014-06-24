#include "processA.h"

//=======================================================================
void processA::execute(void)
{
 currentEvent_ = 0 ;
 for(int loop=0; loop<maxEvents_; ++loop)
  {
    sleep(1) ;
    currentEvent_++ ;
    ss_.str("") ;
    ss_ << "Event " << currentEvent_ ;
    STDLINE(ss_.str(),ACWhite) ;
  }

  STDLINE("Job done!",ACGreen) ;
}
