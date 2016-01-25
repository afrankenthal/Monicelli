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
