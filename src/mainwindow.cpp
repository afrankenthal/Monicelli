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

#include <QtWidgets/QApplication>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtGui/QBrush>
#include <QtGui/QtGui>

#include <TBrowser.h>
#include <TSystem.h>

#include "mainwindow.h"
#include "mdichild.h"
#include "mdisubwindow.h"

//===========================================================================
MainWindow::MainWindow()
{
    path_ = QString(getenv("MonicelliDir")) ;
    this->initialize() ;

    fRootTimer_ = new QTimer( this );                   // ToROOT6
    QObject::connect(                                   // ToROOT6
                     fRootTimer_,                       // ToROOT6
                     SIGNAL(timeout()),                 // ToROOT6
                     this,                              // ToROOT6
                     SLOT(handle_root_events())         // ToROOT6
                    );                                  // ToROOT6
    fRootTimer_->start( 20 );                           // ToROOT6

    mdiArea      = new QMdiArea;
    windowMapper = new QSignalMapper(this);

    mdiArea->setGeometry(10,10,mainWindowW_,mainWindowH_);
    this   ->setGeometry(10,10,mainWindowW_,mainWindowH_);

    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(  Qt::ScrollBarAsNeeded);

    QLinearGradient gradient(0,mainWindowW_,0,mainWindowH_/4);
    gradient.setColorAt(0.5, QColor::fromRgb( 0,  0,  0, 255));
    gradient.setColorAt(1.0, QColor::fromRgb(60, 60, 60, 255));

    QBrush mdiAreaBrush(gradient);
    mdiArea->setBackground(mdiAreaBrush);

    setCentralWidget(mdiArea);

    connect(mdiArea,
            SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this,
            SLOT  (updateMenus())                    );
    connect(windowMapper,
            SIGNAL(mapped(QWidget*)),
            this,
            SLOT  (setActiveSubWindow(QWidget*))     );
    connect(this,
            SIGNAL(destroyed()),
            this,
            SLOT  (cleanClose())                     ) ;

    createActions  ();
    createMenus    ();
    createToolBars ();
    createStatusBar();
    updateMenus    ();

    readSettings   ();

    setWindowTitle(tr("Monicelli: the track reconstruction program of the Fermilab test-beam"));

    setUnifiedTitleAndToolBarOnMac(true);

    theTabWidget_      = NULL ;
    theHNavigator_     = NULL ;
    theXMLEditor_      = NULL ;
    
    mainTabsActivated_ = false ;
    theFileEater_      = new fileEater();
    theHManager_       = new HManager(theFileEater_);
    theFileEater_->setHManger(theHManager_);

    usePartitions_ = false;

    this->buildHNavigator() ;
}

//===========================================================================
void MainWindow::initialize()
{
    mainWindowW_ = 1200 ;
    mainWindowH_ =  800 ;
    mainPanelW_  = 1010 ;
    mainPanelH_  =  780 ;
    xmlEditorW_  =  970 ;
    xmlEditorH_  =  735 ;
}

//===========================================================================
void MainWindow::cleanClose()
{
    STDLINE("Closing everything",ACRed) ;
    QApplication::processEvents(QEventLoop::AllEvents);
    qApp->exit() ;
    exit(0) ;
}


//===========================================================================
void MainWindow::closeEvent(QCloseEvent *event)
{
    STDLINE("Closing everything",ACRed) ;

    theHNavigator_->getTheHTreeBrowser()->expandAll() ;
    theHNavigator_->saveAll() ;
    mdiArea->closeAllSubWindows();

    if (mdiArea->currentSubWindow())
    {
        event->ignore();
    }
    else
    {
        writeSettings();
        event->accept();
    }

//    qApp->exit() ;    
}

//===========================================================================
void MainWindow::newFile()
{
    MdiChild *child = createMdiChild();
    child->newFile();
    child->show();
}

//===========================================================================
void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        QMdiSubWindow *existing = findMdiChild(fileName);
        if (existing)
        {
            mdiArea->setActiveSubWindow(existing);
            return;
        }

        MdiChild *child = createMdiChild();
        if (child->loadFile(fileName))
        {
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();
        }
        else
        {
            child->close();
        }
    }
}

//===========================================================================
void MainWindow::save()
{
    if (activeMdiChild() && activeMdiChild()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

//===========================================================================
void MainWindow::saveAs()
{
    if (activeMdiChild() && activeMdiChild()->saveAs())
        statusBar()->showMessage(tr("File saved"), 2000);
}

//===========================================================================
void MainWindow::cut()
{
    if (activeMdiChild())
        activeMdiChild()->cut();
}

//===========================================================================
void MainWindow::copy()
{
    if (activeMdiChild())
        activeMdiChild()->copy();
}

//===========================================================================
void MainWindow::paste()
{
    if (activeMdiChild())
        activeMdiChild()->paste();
}

//===========================================================================
void MainWindow::about()
{
    QMessageBox msgBox;

    msgBox.setText("To get online help for <b>Monicelli</b> click Open");
    msgBox.setInformativeText("This requires Firefox to be installed on the local machine");
    msgBox.setStandardButtons(QMessageBox::Open | QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    if( msgBox.exec() == QMessageBox::Open )
    {
        system("firefox http://hal9000.mib.infn.it/~menasce/Monicelli &") ;
    }
}

//===========================================================================
void MainWindow::updateMenus()
{
}

//===========================================================================
void MainWindow::updateWindowMenu()
{
    windowMenu->clear       ();

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i)
    {
        MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());

        if( child )
        {
            QString text;
            if (i < 9)
            {
                text = tr("&%1 %2").arg(i + 1)
                       .arg(child->userFriendlyCurrentFile());
            }
            else
            {
                text = tr("%1 %2").arg(i + 1)
                       .arg(child->userFriendlyCurrentFile());
            }

            QAction *action  = windowMenu->addAction(text);
            action->setCheckable(true);
            connect(action,
                    SIGNAL(triggered()),
                    windowMapper,
                    SLOT  (map()));
            windowMapper->setMapping(action, windows.at(i));
            connect(action,
                    SIGNAL(triggered()),
                    windowMapper,
                    SLOT  (map()));
            action ->setChecked(child == activeMdiChild());
        }
    }
}

//===========================================================================
MdiChild *MainWindow::createMdiChild()
{
    MdiChild *child = new MdiChild;
    mdiArea->addSubWindow(child);

    connect(child,
            SIGNAL(copyAvailable(bool)),
            cutAct,
            SLOT  (setEnabled(bool))  );
    connect(child,
            SIGNAL(copyAvailable(bool)),
            copyAct,
            SLOT  (setEnabled(bool))  );

    return child;
}

//===========================================================================
void MainWindow::createActions()
{
    QString iconPath ;

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction(tr("&About Monicelli"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    iconPath  = path_ ;
    iconPath += "/images/newCanvas.png" ;
    hNavigatorAct = new QAction(QIcon(iconPath), tr("Histogram Navigator"), this);
    hNavigatorAct->setStatusTip(tr("Create a Histogram Navigator")) ;
    connect(hNavigatorAct,   SIGNAL(triggered()), this, SLOT(showHNavigator()));

    iconPath  = path_ ;
    iconPath += "/images/editXML.png" ;
    editXMLAct = new QAction(QIcon(iconPath), tr("XML Geometry-Configuration file editor"), this);
    editXMLAct->setStatusTip(tr("Edit an xml geometry-configuration file")) ;
    connect(editXMLAct,      SIGNAL(triggered()), this, SLOT(editXMLPanel()));

    iconPath  = path_ ;
    iconPath += "/images/mainTabs.png" ;
    newMainPanelAct = new QAction(QIcon(iconPath), tr("Main Panel"), this);
    newMainPanelAct->setStatusTip(tr("Create a Main Panel")) ;
    connect(newMainPanelAct, SIGNAL(triggered()), this, SLOT(buildMainPanel()));

    iconPath  = path_ ;
    iconPath += "/images/treeIcon.png" ;
    synopticViewerAct = new QAction(QIcon(iconPath), tr("Synoptic Viewer"), this);
    synopticViewerAct->setStatusTip(tr("Create a Synoptic Viewer")) ;
    connect(synopticViewerAct, SIGNAL(triggered()), this, SLOT(buildSynopticViewer()));
}

//===========================================================================
void MainWindow::enableMainPanelButton()
{
    newMainPanelAct->setEnabled(true) ;
    theTabWidget_ = NULL ;
}


//===========================================================================
void MainWindow::enableEditXMLButton()
{
    editXMLAct->setEnabled(true) ;
    theXMLEditor_ = NULL ;
}

//===========================================================================
void MainWindow::enableHNavigatorButton()
{
    STDLINE("HNavigator destroyed and rebuilt",ACGreen) ;
    hNavigatorAct->setEnabled(true) ;
    theHNavigator_ = NULL ;
    this->buildHNavigator() ;
}

//===========================================================================
void MainWindow::buildSynopticViewer()
{
    STDLINE("Synoptic Viewer",ACWhite) ;
    
}
//===========================================================================
void MainWindow::editXMLPanel()
{
    if( !theXMLEditor_ )
    {
        theXMLEditor_ = new XMLEditor(this) ;

        if( !theXMLEditor_->isActive() )
        {
            theXMLEditor_ = NULL ;
            return ;
        }

        mdiSubWindow * cSw = (mdiSubWindow*)mdiArea->addSubWindow(theXMLEditor_) ;

        editXMLAct->setEnabled(false) ;

        connect(cSw,
                SIGNAL(destroyed()           ),
                this,
                SLOT  (enableEditXMLButton()));
        cSw->setGeometry(5,
                         5,
                         theXMLEditor_->geometry().width(),
                         theXMLEditor_->geometry().height()) ;

        cSw->show() ;
    }

}

//===========================================================================
void MainWindow::editXMLPanel(QString fileName)
{
    if( !theXMLEditor_ )
    {
        theXMLEditor_ = new XMLEditor(fileName) ;

        if( !theXMLEditor_->isActive() )
        {
            theXMLEditor_ = NULL ;
            return ;
        }

        mdiSubWindow * cSw = (mdiSubWindow*)mdiArea->addSubWindow(theXMLEditor_) ;

        editXMLAct->setEnabled(false) ;

        connect(cSw,
                SIGNAL(destroyed()           ),
                this,
                SLOT  (enableEditXMLButton()));
        cSw->setGeometry(5,
                         5,
                         theXMLEditor_->geometry().width(),
                         theXMLEditor_->geometry().height()) ;

        cSw->show() ;
    }

}

//===========================================================================
void MainWindow::buildMainPanel()
{
    if( !theTabWidget_ )
    {
        theTabWidget_ = new mainTabs(this) ;

        mdiSubWindow * cSw = (mdiSubWindow*)mdiArea->addSubWindow(theTabWidget_) ;

        connect(cSw,
                SIGNAL(destroyed()             ),
                this,
                SLOT  (enableMainPanelButton()));
        cSw->setGeometry(5,5,mainPanelW_,mainPanelH_) ;

        cSw->show() ;
    }

    theTabWidget_->collectExistingWidgets(this);

    mainTabsActivated_ = true ;
}

//===========================================================================
void MainWindow::buildHNavigator()
{
    if( !theHNavigator_)
    {
        STDLINE("Build the navigator",ACCyan) ;
        theHNavigator_ = new HNavigator(this) ;

        theHNavigator_->getPartitionsInfos(usePartitions_  ,
                                           theCurrentDUT_  ,
                                           theCurrentSector_);

        theHNavigator_->getGeometryFileName(geometryFileName_);
        theHNavigator_->getInputFileName   (inputFileName_);

        cSw_ = (mdiSubWindow*)mdiArea->addSubWindow(theHNavigator_) ;

        connect(cSw_, SIGNAL(destroyed()), this, SLOT(enableHNavigatorButton()));

        cSw_->setGeometry(1015,5,theHNavigator_->width()+8,theHNavigator_->height()+40) ;

        cSw_->hide() ;
        return ;
    }

    theHNavigator_->collectExistingWidgets(this);
}
//===========================================================================
void MainWindow::showHNavigator()
{
    cSw_->show() ;
}
//===========================================================================
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *action = fileMenu->addAction(tr("Switch layout direction"));
    connect(action, SIGNAL(triggered()), this, SLOT(switchLayoutDirection()));
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));

    windowMenu = menuBar()->addMenu(tr("&Window"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

//===========================================================================
void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));

    fileToolBar->addAction(newMainPanelAct) ;
    fileToolBar->addAction(hNavigatorAct) ;
    fileToolBar->addAction(editXMLAct) ;
    fileToolBar->addAction(synopticViewerAct) ;

    editToolBar = addToolBar(tr("Edit"));
}

//===========================================================================
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

//===========================================================================
void MainWindow::readSettings()
{
    QSettings settings("CMS", "Monicelli");
    QPoint pos  = settings.value("pos" , QPoint(200, 200)).toPoint();
    QSize  size = settings.value("size", QSize (400, 400)).toSize();
    this->move(pos);
    this->resize(size);
}

//===========================================================================
void MainWindow::writeSettings()
{
    QSettings settings("CMS", "Monicelli");
    settings.setValue("pos" , pos ());
    settings.setValue("size", size());
    if( theHNavigator_ )
    {
        hNavigatorSize_ = QSize (
                                 theHNavigator_->geometry().width(),
                                 theHNavigator_->geometry().height()
                                ) ;
        hNavigatorPos_  = QPoint(
                                 theHNavigator_->pos().x(),
                                 theHNavigator_->pos().y()
                                ) ;
        settings.setValue("posHnavigator" ,  hNavigatorSize_) ;
        settings.setValue("sizeHnavigator" , hNavigatorPos_ ) ;
    }
    else
    {
        settings.setValue("posHnavigator" ,  hNavigatorSize_) ;
        settings.setValue("sizeHnavigator" , hNavigatorPos_ ) ;
    }
    STDLINE("Updating GUI geometry",ACCyan) ;
}

//===========================================================================
MdiChild *MainWindow::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MdiChild *>(activeSubWindow->widget());
    return 0;
}

//===========================================================================
QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

//===========================================================================
void MainWindow::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        qApp->setLayoutDirection(Qt::RightToLeft);
    else
        qApp->setLayoutDirection(Qt::LeftToRight);
}

//===========================================================================
void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

//===========================================================================
void MainWindow::handle_root_events()         // ToROOT6
{                                             // ToROOT6
   //call the inner loop of ROOT              // ToROOT6
   gSystem->ProcessEvents();                  // ToROOT6
}
//===========================================================================
void MainWindow::getPartitionsInfos(bool usePartitions, string theCurrentDUT, string theCurrentSector)
{
    usePartitions_    = usePartitions;
    theCurrentDUT_    = theCurrentDUT;
    theCurrentSector_ = theCurrentSector;
}
