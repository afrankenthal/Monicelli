/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include "threader.h"

//=============================================================================
void threader::run(void)
{
  STDLINE("Thread started",ACPurple) ;

  this->execute() ;

  exec() ;
}

//=============================================================================
void threader::setProcess(process *theProcess)
{
  theProcess_ = theProcess ;
}

//=============================================================================
void threader::execute(void)
{
  bool success = theProcess_->execute();
  this->quit() ;
  emit threader::processFinished(theProcess_, success);
}
//============================================================================
process * threader::getCurrentProcess()
{
  return theProcess_;
}
//==================================================================
template<class Class> void threader::runTheMethod(Class * object, void (Class::*fn)() )
{
  future_ = QtConcurrent::run(object,fn);
}
//==================================================================
bool threader::isRunning(void )
{
  if(QThread::isRunning() || future_.isRunning())
    return true ;
  else   return false;
}
//==================================================================
void threader::terminate(void)
{
  QThread::terminate();
  future_.cancel();
}
//=================================================================
bool threader::isFinished(void)
{
  if( QThread::isFinished() || future_.isFinished() )
    return true ;
  else return false;
}
