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

