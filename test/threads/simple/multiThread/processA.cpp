#include "processA.h"
#include "threaderClass.h"

//=======================================================================
/*
void processA::execute(void)
{
    //std::cout << __PRETTY_FUNCTION__ << maxEvents_ << std::endl;
    while( (currentEvent_=theThreader_->getLastEvent())<maxEvents_)
//    while(++currentEvent_<maxEvents_)
    {
        theThreader_->incrementLastEvent();
        ss_.str("") ;
        ss_ << "Taken event " << currentEvent_ << " for process: " << processNumber_;
        STDLINE(ss_.str(),ACWhite) ;
        int counter = 0;
        for(int i=0; i<1000000000;i++)
        {
            counter++;
        }
        ss_.str("") ;
        ss_ << "Processed event " << currentEvent_ << " for process: " << processNumber_ << " " << counter;
        STDLINE(ss_.str(),ACWhite) ;
    }
    currentEvent_ = 0;
    ss_.str("");
    ss_ << "Process " << processNumber_ << " done!";
    STDLINE(ss_.str(),ACWhite) ;
}
*/
//=======================================================================
void processA::analyze(int event)
{
    ss_.str("") ;
    ss_ << "Analyzing event " << event << " for process: " << processNumber_;
    STDLINE(ss_.str(),ACWhite) ;
    int counter = 0;
    for(int i=0; i<1000000000;i++)
    {
        counter++;
    }
}
