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
