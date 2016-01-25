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
 
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MessageTools.h"

//==============================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    theTimer_ = new QTimer() ;

    theTimer_->setInterval(500);

    ui->progressBar->reset();

    theThreader_ = new threader() ;

    theProcessA_ = new processA() ;

    connect(theTimer_, SIGNAL(timeout()),
            this,      SLOT  (advanceProgressBar()));
}

//==============================================================
MainWindow::~MainWindow()
{
    delete ui;
}

//==============================================================
void MainWindow::on_startPB_clicked()
{
    STDLINE("Process A started",ACWhite) ;

    maxEvents_ = ui->maxEventsSB->value() ;

    ui->progressBar->setMaximum(maxEvents_);
    theThreader_->setProcess(theProcessA_,maxEvents_);

    theTimer_->start() ;

    theThreader_->start() ;
}

//==============================================================
void MainWindow::on_abortPB_clicked()
{
    STDLINE("Aborted",ACWhite) ;

    theThreader_->terminate() ;
    theTimer_->stop();
    ui->progressBar->reset();
}

//==============================================================
void MainWindow::advanceProgressBar()
{
  int currentEvent = theThreader_->getCurrentEvent() ;

  ui->progressBar->setValue(currentEvent);

  if( currentEvent == maxEvents_ - 1)
  {
    STDLINE("All events have been processed",ACCyan) ;
    theTimer_->stop();
    ui->progressBar->setValue(maxEvents_) ;
  }

  if( theThreader_->isFinished())
  {
    STDLINE("All events have been processed",ACPurple) ;
    theTimer_->stop();
    ui->progressBar->setValue(maxEvents_) ;
  }

}
