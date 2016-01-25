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
 
#include <iostream>

#include <QLabel>

#include "cProcess.h"

std::string cProcess::currentClassName_ = "cProcess" ;

std::map<std::string,cProcess *> cProcess::derivedClassPtr_ = std::map<std::string,cProcess *>() ;

//=============================================================================================
void cProcess::started(void)
{
  std::cout << ACRed << ACBold
            << __LINE__
            << "]\t["
            << __PRETTY_FUNCTION__
            << "]\tthe "
            << className_
            << " started"
            << ACPlain
            << std::endl ;

  ss_.str("") ; ss_ << "Processing " << iterations_ << " files for "  << className_;
  std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\t" << ss_.str() << std::endl ;
//  QLabel * label = new QLabel(tr(ss_.str().c_str())) ;

//  theProgressDialog_ = new QProgressDialog() ;
//  theProgressDialog_->setMaximum(iterations_);
//  theProgressDialog_->setMinimumDuration( 1 ) ;
//  theProgressDialog_->setLabel(label) ;
//  theProgressDialog_->show() ;

 // if(processName_ == "parse" ) parent_->   ui->parseRunningCB->setChecked() ;

//  connect(pDialog_[1], SIGNAL(canceled()), fWatchr_[1], SLOT(cancel()));
  ss_.str("") ; ss_ << "Started progess dialog for "  << className_;
  std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\t" << ss_.str() << std::endl ;
}

//=============================================================================================
void cProcess::showResult(int number)
{
  std::cout << __LINE__
            << "]\t["
            << __PRETTY_FUNCTION__
            << "]\tnumber "
            << number
            << " watcher ptr: "
            << theWatcher_
            << std::endl ;

  std::cout << __LINE__
            << "]\t["
            << __PRETTY_FUNCTION__
            << "]\tProcessed "
            << theWatcher_->resultAt(number)
            << " for "
            << className_
            << " at "
            << number
            << std::endl ;

//  theProgressDialog_->setValue(theWatcher_->resultAt(number)) ;

  std::cout << __LINE__
            << "]\t["
            << __PRETTY_FUNCTION__ << std::endl ;

}

//=============================================================================================

