#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "tabwidget.h"
#include "formpage1.h"

#include <QFileDialog>

//================================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    TabWidget * tw = new TabWidget(this) ;
    tw->setGeometry(0,60,tw->geometry().width(), tw->geometry().height()) ;

    FormPage1 * f1 = new FormPage1(tw) ; f1->setLabelText(QString("Pippo"));
    FormPage1 * f2 = new FormPage1(tw) ; f2->setLabelText(QString("Pluto"));

    tw->addTab((QWidget*)f1,QString("FormPage1")) ;
    tw->addTab((QWidget*)f2,QString("FormPage2")) ;

}

//================================================================
MainWindow::~MainWindow()
{
    delete ui;
}

//================================================================
void MainWindow::on_openFilePB_clicked()
{
  std::string path = std::string(getenv("MonicelliDir"))+std::string("/xml") ;
  QString path_ = QString(path.c_str()) ;
  QString fileName = QFileDialog::getOpenFileName(this,"Geometry files",path_,"XML files (*.xml)");
  if (fileName.isEmpty())  return ;

}
