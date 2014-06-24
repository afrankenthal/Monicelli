#include "threaderClass.h"

//=============================================================================
void threader::run(void)
{
  STDLINE("Run started",ACPurple) ;

  this->execute() ;

  exec() ;
}

//=============================================================================
void threader::setProcess(process *theProcess, int maxEvents)
{
  theProcess_ = theProcess ;
  theProcess_->setMaxEvents(maxEvents) ;
}

//=============================================================================
void threader::execute(void)
{
  theProcess_->execute() ;

  this->quit() ;
}
