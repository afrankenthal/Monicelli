#include "threaderClass.h"
#include "process.h"
#include <iostream>

//=============================================================================
void threader::run(void)
{
  STDLINE("Run started",ACPurple) ;
  lastEvent_ = 1;
  std::cout << __PRETTY_FUNCTION__ << lastEvent_ << std::endl;
  //this->execute() ;
  for(int p=0; p<theProcesses_.size(); p++)
  {
    if(theProcesses_[p] != 0)
    {
        ss_.str("") ;
        ss_ << "Executing process: " << p ;
        STDLINE(ss_.str(),ACBlack) ;

        theProcesses_[p]->start() ;

        /*
        theProcesses_[p]->quit() ;
        ss_.str("") ;
        ss_ << "Process: " << p << " is finished? " << theProcesses_[p]->isFinished() ;
        STDLINE(ss_.str(),ACWhite) ;
        */
        STDLINE("Processes started",ACBlack) ;
    }
  }
  exec();
  for(int p=0; p<theProcesses_.size(); p++)
  {
    if(theProcesses_[p] != 0)
    {
        ss_.str("") ;
        ss_ << "Process: " << p << " is finished? " << theProcesses_[p]->isFinished() ;
        STDLINE(ss_.str(),ACBlack) ;

    }
  }
}

//=============================================================================
void threader::destroy(void)
{
    for(int p=0; p<theProcesses_.size(); p++)
    {
        if(theProcesses_[p] != 0)
        {
            delete theProcesses_[p];
            theProcesses_[p] = 0;
        }
    }

}
//=============================================================================
void threader::setNumberOfThreads(int nOfThreads)
{
  theProcesses_.resize(nOfThreads,0);
  destroy();
}

//=============================================================================
void threader::reset(int nOfThreads)
{
    if(nOfThreads!=-1)
    {
        setNumberOfThreads(nOfThreads);
    }
    destroy();

}

//=============================================================================
int  threader::getCurrentEvent(void)
{
    return lastEvent_;
}

//=============================================================================
void threader::setProcess(process *theProcess, int maxEvents)
{

  for(int p=0; p<theProcesses_.size(); p++)
  {
    if(theProcesses_[p] == 0)
    {
        ss_.str("") ;
        ss_ << "Setting process: " << p ;
        STDLINE(ss_.str(),ACBlack) ;

        theProcesses_[p] = theProcess ;
        theProcesses_[p]->setMaxEvents(maxEvents) ;
        theProcesses_[p]->setThreader(this) ;
        break;
    }
  }
}

//=============================================================================
void threader::execute(void)
{
    for(int p=0; p<theProcesses_.size(); p++)
    {
      if(theProcesses_[p] != 0)
      {
          ss_.str("") ;
          ss_ << "Executing process: " << p ;
          STDLINE(ss_.str(),ACBlack) ;

          theProcesses_[p]->execute() ;
      }
    }

  this->quit() ;
}

//=============================================================================
void threader::incrementLastEvent(void)
{
    mutex_.tryLock();
    ++lastEvent_;
    mutex_.unlock();
}

//=============================================================================
int  threader::getLastEvent      (void)
{
    //ss_.str("") ;
    //ss_ << "lastEvent " << lastEvent_ ;
    ///STDLINE(ss_.str(),ACWhite) ;
    //std::cout << __PRETTY_FUNCTION__ << lastEvent_ << std::endl;
    return lastEvent_;
}
