#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "util.h"

#include <QtCore>
#include <QButtonGroup>
#include <QFileDialog>

#include "boost/bind.hpp"

//=============================================================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  theProcesses_[0] = new parser ("parser" ) ;
  theProcesses_[1] = new builder("builder" ) ;
  theProcesses_[2] = new aligner("aligner" ) ;

  connect(ui->fileLoaderPB, SIGNAL(clicked()    ),
          this,             SLOT  (loadFiles()) ) ;

  buttons_.push_back(ui->parsePB) ;
  buttons_.push_back(ui->buildPB) ;
  buttons_.push_back(ui->alignPB) ;

  QButtonGroup * buttonGroup = new QButtonGroup(parent) ;

  int c = 0 ;
  for(buttonsDef::iterator it=buttons_.begin(); it!=buttons_.end(); ++it)
  {
    buttonGroup->addButton(*it, c++) ;
  }

  connect(buttonGroup, SIGNAL(buttonClicked(int) ),
          this,        SLOT  (start        (int))) ;

  this->enableButtons(false) ;


}

//=============================================================================================
MainWindow::~MainWindow()
{
  delete ui;
}

//=============================================================================================
void MainWindow::start(int theButton)
{
  std::string className = Util::getClassName(std::string(__PRETTY_FUNCTION__)) ;

  watcher_[theButton] = new QFutureWatcher<int>(theProcesses_[theButton]);

  std::cout << __LINE__
            << "]\t["
            << __PRETTY_FUNCTION__
            << "]\tstarting watcher for "
            << theProcesses_[theButton]->getProcess()
            << " button: "
            << theButton
            << std::endl ;

  theProcesses_[theButton]->setCurrentClassName(theProcesses_[theButton]->getClassName());
  if(theButton == 0) watcher_[theButton]->setFuture(QtConcurrent::mapped(files_, &parser::wrapperToExecute));
  if(theButton == 1) watcher_[theButton]->setFuture(QtConcurrent::mapped(files_, &builder::wrapperToExecute));
  if(theButton == 2) watcher_[theButton]->setFuture(QtConcurrent::mapped(files_, &aligner::wrapperToExecute));

  theProcesses_[theButton]->setWatcher(watcher_[theButton]) ;

  connect(watcher_     [theButton], SIGNAL(started()         ),
          theProcesses_[theButton], SLOT  (started()         ));
  connect(watcher_     [theButton], SIGNAL(resultReadyAt(int)),
          theProcesses_[theButton], SLOT  (showResult   (int)));

  std::cout << __LINE__ << "]\t["<< __PRETTY_FUNCTION__
            << ACPurple << ACBold
            << "]\tProcess " << theProcesses_[theButton]->getProcess() << " has been submitted"
            << ACPlain
            << std::endl ;
}

//=============================================================================================
void MainWindow::loadFiles(void)
{
  files_ = QFileDialog::getOpenFileNames(this,
                                         tr("Select multiTask input files"),
                                         tr("../multiTaskManager/images"),
                                         "*.jpg *.png");

  if( files_.count() == 0 ) return ;


  this->enableButtons(true) ;
}

//=============================================================================================
void MainWindow::enableButtons(bool enable)
{
  for(buttonsDef::iterator it=buttons_.begin(); it!=buttons_.end(); ++it)
  {
    (*it)->setEnabled(enable) ;
  }
}
