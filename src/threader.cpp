/*===============================================================================
 * Monicelli: the FERMILAB MTEST geometry builder and track reconstruction tool
 * 
 * Copyright (C) 2014 
 *
 * Authors:
 *
 * Dario Menasce      (INFN) 
 * Luigi Moroni       (INFN)
 * Jennifer Ngadiuba  (INFN)
 * Stefano Terzo      (INFN)
 * Lorenzo Uplegger   (FNAL)
 * Luigi Vigani       (INFN)
 *
 * INFN: Piazza della Scienza 3, Edificio U2, Milano, Italy 20126
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ================================================================================*/
 
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
  if(QThread::isRunning() || future_.isRunning()) return true ;
  else                                            return false;
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
