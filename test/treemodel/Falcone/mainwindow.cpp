/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <iostream>

#include <QAbstractItemModel>
#include <QtXml/QDomDocument>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QTreeView>
#include <QtCore/QFile>
#include <QtGui>

#include "domitem.h"
#include "dommodel.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std ;

//===========================================================================
MainWindow::MainWindow(QWidget *parent) :
      QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model_(0)
{
    ui->setupUi(this);
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(
                        tr("&Open..."),
                        this,
                        SLOT(openFile()),
                        QKeySequence::Open
                       );
    fileMenu->addAction(
                        tr("E&xit"),
                        this,
                        SLOT(close()),
                        QKeySequence::Quit
                       );

    model_ = new DomModel (QDomDocument(), this);
    view  = ui->treeView ;
    view->setModel(model_);

    setWindowTitle(tr("XML synoptic view"));
}

//===========================================================================
MainWindow::~MainWindow()
{
    delete ui;
}

//===========================================================================
void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"),
                                                    xmlPath,
                                                    tr("XML files (*.xml)"));

    if (!filePath.isEmpty())
    {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly))
        {
            QDomDocument document;
            if (document.setContent(&file))
            {
                model_ = new DomModel(document, this);
                view->setModel(model_);
                xmlPath = filePath;
            }
            file.close();
        }
    }

    if( !model_ )
    {
     cout << "Fatal error" << endl ;
     exit(0) ;
    }

    view->setAnimated(true) ;
    view->resizeColumnToContents(0) ;
    view->resizeColumnToContents(1) ;
    view->resizeColumnToContents(2) ;
    view->setColumnWidth(0,320) ;
    view->setColumnWidth(1,100) ;
}
//===========================================================================
void MainWindow::expandChildrenBelow(const QModelIndex & index   ,
                                           QTreeView   * view    ,
                                           QStringList   elements,
                                           bool          expand  )
{
    if (!index.isValid()) return;

    int childCount = index.model()->rowCount(index);

    for (int i = 0; i < childCount; i++)
    {
        const QModelIndex &child = index.child(i, 0);

        if( model_->indexIs(child, elements) )
        {
            if( expand)
            {
                view->expand(index);
            }
            else
            {
                view->collapse(index) ;
            }
        }
        expandChildrenBelow(child, view, elements, expand );
    }

}
//===========================================================================
void MainWindow::on_expandPB_clicked()
{
    view->collapseAll() ;
    QStringList elements;
    elements << "testBeamGeometry"
             << "station"
             << "stations" ;
    this->expandChildrenBelow(model_->index(3,0),view,elements,true)  ;
    expanded_["Stations"] = true ;
}
//===========================================================================
void MainWindow::on_collapsePB_clicked()
{
    view->collapseAll() ;
    expanded_["Stations"] = false ;
}

//===========================================================================
void MainWindow::on_findUsedStationsPB_clicked()
{

}
//===========================================================================
void MainWindow::on_expandDetectorsPB_clicked()
{
    view->collapseAll() ;
    QStringList elements;
    elements << "testBeamGeometry"
             << "stations"
             << "station"
             << "detectors"
             << "detector";
    this->expandChildrenBelow(model_->index(3,0),view,elements,true)  ;
    expanded_["Detectors"] = true ;
}
//===========================================================================
void MainWindow::on_collapseDetectorsPB_clicked()
{
   on_expandPB_clicked() ;
   expanded_["Detectors"] = false ;
}
//===========================================================================
void MainWindow::on_expandROCsPb_clicked()
{
    view->collapseAll() ;
    QStringList elements;
    elements << "testBeamGeometry"
             << "stations"
             << "station"
             << "detectors"
             << "detector"
             << "ROCs"
             << "ROC";

    this->expandChildrenBelow(model_->index(3,0),view,elements,true)  ;
    expanded_["ROCs"] = true ;
}
//===========================================================================
void MainWindow::on_collapseROCsPB_clicked()
{
    on_expandDetectorsPB_clicked() ;
    expanded_["ROCs"] = false ;
}
//===========================================================================
void MainWindow::on_detailsCB_clicked(bool checked)
{
    cout << endl ;
    for(map<string, bool>::iterator it=expanded_.begin(); it!=expanded_.end(); ++it)
    {
        cout << it->first << ": " << it->second << endl ;
    }
}
//===========================================================================
void MainWindow::on_expandDetailsPB_clicked()
{
}
