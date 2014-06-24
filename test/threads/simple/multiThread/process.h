#ifndef PROCESS_H
#define PROCESS_H

#include <sstream>
#include "MessageTools.h"
#include <QThread>
#include "threaderClass.h"

//class threader;

class process : public QThread
{
public:
    process(int processNumber=-1) : processNumber_(processNumber),currentEvent_(0){;}
    virtual ~process(void) {;}

    void         run            (void         ){this->execute(); exec();}
    virtual void execute        (void         ){ //std::cout << __PRETTY_FUNCTION__ << maxEvents_ << std::endl;
            while( (currentEvent_=theThreader_->getLastEvent())<maxEvents_)
        //    while(++currentEvent_<maxEvents_)
            {
                theThreader_->incrementLastEvent();
                ss_.str("") ;
                ss_ << "Processing event " << currentEvent_ << " for process: " << processNumber_ ;
                STDLINE(ss_.str(),ACWhite) ;
                analyze(currentEvent_);
            }
            currentEvent_ = 0;
            ss_.str("");
            ss_ << "Process " << processNumber_ << " done!";
            STDLINE(ss_.str(),ACWhite) ;
    }


    virtual void begin          (void         )         {;}
    virtual void analyze        (int event    )         {;}
    virtual void end            (void         )         {;}

    virtual void setMaxEvents   (int maxEvents)        {maxEvents_ = maxEvents;}
    virtual void setThreader    (threader* theThreader){theThreader_ = theThreader;}
    virtual int  getCurrentEvent(void         )         = 0;

protected:
    std::stringstream ss_;
    threader* theThreader_;
    int       maxEvents_;
    int       processNumber_;
    int       currentEvent_ ;

} ;

#endif // PROCESS_H
