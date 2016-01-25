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
#include <math.h>
#include <sstream>
#include <unistd.h>

#include "multiTask.h"

int multiTask::compute(const QString &imageFileName)
{
  const  int imageSize = 100;
  static int counter   = 0 ;
  QImage image(imageFileName);
  QImage scaledImage = image.scaled(QSize(imageSize, imageSize), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  return ++counter ;
}

multiTask::multiTask(QWidget *parent) : QWidget(parent)
{
  setWindowTitle(tr("Image loading and scaling example"));
  resize(800, 600);

  counter_ = 0 ;

  theComputation_ = new computation() ;

  buttonGroup_    = new QButtonGroup(parent) ;

  fWatchr_[0] = new QFutureWatcher<int>   (this);
  fWatchr_[1] = new QFutureWatcher<int>   (theComputation_);
  connect(fWatchr_[0], SIGNAL(started()),          this, SLOT(started0()      ));
  connect(fWatchr_[1], SIGNAL(started()),          this, SLOT(started1()      ));
  connect(fWatchr_[0], SIGNAL(resultReadyAt(int)), this, SLOT(showResult0(int)));
  connect(fWatchr_[1], SIGNAL(resultReadyAt(int)), this, SLOT(showResult1(int)));

  for( int i=0; i<2; ++i)
  {
    std::stringstream ss ; ss << "Computation " << i ;
    actions_[i] = new QPushButton(tr(ss.str().c_str()));
    buttonGroup_->addButton(actions_[i],i);
  }

  connect(buttonGroup_, SIGNAL(buttonClicked(int)), SLOT(open(int))) ;

  cancelButton = new QPushButton(tr("Open multiTaskA"));
  cancelButton->setEnabled(false);

  pauseButton = new QPushButton(tr("Idem"));
  pauseButton->setEnabled(false);

  QHBoxLayout *buttonLayout = new QHBoxLayout();

  for(actionsDef::iterator it =actions_.begin(); it!=actions_.end(); ++it)
  {
    buttonLayout->addWidget(it->second);
  }

  buttonLayout->addWidget(cancelButton);
  buttonLayout->addWidget(pauseButton);
  buttonLayout->addStretch();

  imagesLayout = new QGridLayout();

  mainLayout   = new QVBoxLayout();
  mainLayout->addLayout(buttonLayout);
  mainLayout->addStretch();
  setLayout(mainLayout);

  timer_ = new QTimer() ;
  connect(timer_,
          SIGNAL(timeout     () ),
          this,
          SLOT  (checkWatcher()));

  timer_->start(1000) ;
}

multiTask::~multiTask()
{
  for(fWatchrDef::iterator it=fWatchr_.begin(); it!=fWatchr_.end(); ++it)
  {
    it->second->cancel();
    it->second->waitForFinished();
  }
}

void multiTask::checkWatcher()
{
 for(fWatchrDef::iterator it= fWatchr_.begin(); it!=fWatchr_.end(); ++it)
 {
//  if( it->second->isStarted() ) std::cout << "Process " << it->first << " has started" << std::endl ;
  if( it->second->isRunning() )
  std::cout << __LINE__
            << "] Process "
            << it->first
            << " is running "
            << it->second->progressValue()
            << std::endl ;
 }
}

void multiTask::open(int theButton)
{
  if (fWatchr_[theButton]->isRunning()) {
      fWatchr_[theButton]->cancel();
      fWatchr_[theButton]->waitForFinished();
  }

  QStringList files = QFileDialog::getOpenFileNames(this,
                                                    tr("Select multiTask"),
                                                    tr("../../imagescaling/images"),
                                                    "*.jpg *.png");

  numFiles_[theButton] = files.count() ;

  if(numFiles_[theButton] == 0)
      return;

  if( theButton == 0 ) fWatchr_[theButton]->setFuture(QtConcurrent::mapped(files, &multiTask::compute  ));
  if( theButton == 1 ) fWatchr_[theButton]->setFuture(QtConcurrent::mapped(files, &computation::compute));

  cancelButton->setEnabled(true );
  pauseButton ->setEnabled(true );
}

void multiTask::showResult0(int num)
{
  if( fWatchr_[0]->resultAt(num) % 10 == 0 )
  std::cout << __LINE__ << " [0] num: " << num << " " << fWatchr_[0]->resultAt(num) << std::endl ;
  pDialog_[0]->setValue(fWatchr_[0]->resultAt(num)) ;
}

void multiTask::showResult1(int num)
{
  if( fWatchr_[1]->resultAt(num) % 10 == 0 )
  std::cout << __LINE__ << " [1] num: " << num << " " << fWatchr_[1]->resultAt(num) << std::endl ;
  pDialog_[1]->setValue(fWatchr_[1]->resultAt(num)) ;
}

void multiTask::started0()
{
  ss_.str("") ; ss_ << " Processing " << numFiles_[0] << " files for computation 0"  ;
  std::cout << __LINE__ << "] " << ss_.str() << std::endl ;
  QLabel * label = new QLabel(tr(ss_.str().c_str())) ;

  pDialog_[0] = new QProgressDialog() ;
  pDialog_[0]->setMaximum(numFiles_[0]);
  pDialog_[0]->setMinimumDuration( 1 ) ;
  pDialog_[0]->setLabel(label) ;

  connect(pDialog_[0], SIGNAL(canceled()), fWatchr_[0], SLOT(cancel()));
}

void multiTask::started1()
{
  ss_.str("") ; ss_ << " Processing " << numFiles_[1] << " files for computation 1"  ;
  std::cout << __LINE__ << "] " << ss_.str() << std::endl ;
  QLabel * label = new QLabel(tr(ss_.str().c_str())) ;

  pDialog_[1] = new QProgressDialog() ;
  pDialog_[1]->setMaximum(numFiles_[1]);
  pDialog_[1]->setMinimumDuration( 1 ) ;
  pDialog_[1]->setLabel(label) ;

  connect(pDialog_[1], SIGNAL(canceled()), fWatchr_[1], SLOT(cancel()));
}

void multiTask::finished()
{
  cancelButton->setEnabled(false);
  pauseButton ->setEnabled(false);
}

