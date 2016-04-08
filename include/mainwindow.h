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
 
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <sstream>

#include <QMainWindow>
#include <QPoint>
#include <QSize>
#include <QTimer> // ToROOT6

#include "canvaswidget.h"
#include "fileEater.h"
#include "HManager.h"
#include "maintabs.h"
#include "hnavigator.h"
#include "xmleditor.h"

class fileEater    ;
class editXML      ;
class MdiChild     ;
class mainTabs     ;
class HManager     ;
class HNavigator   ;
class mdiSubWindow ;

QT_BEGIN_NAMESPACE
 class QAction      ;
 class QMdiArea     ;
 class QMdiSubWindow;
 class QMenu        ;
 class QSignalMapper;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    HManager      * getHManager           (void                    ) {return theHManager_  ;}
    HNavigator    * getHNavigator         (void                    ) {return theHNavigator_;}
    mainTabs      * getTabWidget          (void                    ) {return theTabWidget_ ;}
    fileEater     * getFileEater          (void                    ) {return theFileEater_ ;}
    XMLEditor     * getXMLEditor          (void                    ) {return theXMLEditor_ ;}
    QMdiArea      * getMdiArea            (void                    ) {return mdiArea;       }
    void            editXMLPanel          (QString     fileName    ) ;

protected:
    void            closeEvent            (QCloseEvent *event      ) ;
    QTimer        * fRootTimer_;                                      // ToROOT6

public slots:
    void            handle_root_events    (void                    ) ; // ToROOT6
    void            showHNavigator        (void                    ) ;

private slots:
    void            about                 (void                    ) ;
    void            buildMainPanel        (void                    ) ;
    void            buildHNavigator       (void                    ) ;
    void            editXMLPanel          (void                    ) ;
    void            cleanClose            (void                    ) ;
    void            copy                  (void                    ) ;
    MdiChild      * createMdiChild        (void                    ) ;
    void            cut                   (void                    ) ;
    void            enableMainPanelButton (void                    ) ;
    void            enableHNavigatorButton(void                    ) ;
    void            enableEditXMLButton   (void                    ) ;
    void            newFile               (void                    ) ;
    void            open                  (void                    ) ;
    void            save                  (void                    ) ;
    void            saveAs                (void                    ) ;
    void            setActiveSubWindow    (QWidget * window        ) ;
    void            switchLayoutDirection (void                    ) ;
    void            paste                 (void                    ) ;
    void            updateMenus           (void                    ) ;
    void            updateWindowMenu      (void                    ) ;
    
private:
    MdiChild      * activeMdiChild        (void                    ) ;
    void            createActions         (void                    ) ;
    void            createMenus           (void                    ) ;
    void            createStatusBar       (void                    ) ;
    void            createToolBars        (void                    ) ;
    QMdiSubWindow * findMdiChild          (const QString & fileName) ;
    void            readSettings          (void                    ) ;
    void            writeSettings         (void                    ) ;
    void            initialize            (void                    ) ;

    std::stringstream ss_            ;

    mdiSubWindow  * cSw_             ;

    canvasWidget  * theCanvasWidget_ ;
    fileEater     * theFileEater_    ;
    HManager      * theHManager_     ;
    HNavigator    * theHNavigator_   ;
    mainTabs      * theTabWidget_    ;
    XMLEditor     * theXMLEditor_    ;

    QAction       * aboutAct         ;
    QAction       * aboutQtAct       ;
    QAction       * cascadeAct       ;
    QAction       * closeAct         ;
    QAction       * closeAllAct      ;
    QAction       * copyAct          ;
    QAction       * cutAct           ;
    QAction       * editXMLAct       ;
    QMenu         * editMenu         ;
    QToolBar      * editToolBar      ;
    QAction       * exitAct          ;
    QMenu         * fileMenu         ;
    QToolBar      * fileToolBar      ;
    QMenu         * helpMenu         ;
    QMdiArea      * mdiArea          ;
    QAction       * newAct           ;
    QAction       * nextAct          ;
    QAction       * hNavigatorAct    ;
    QAction       * newMainPanelAct  ;
    QAction       * openAct          ;
    QAction       * pasteAct         ;
    QString         path_            ;
    QAction       * previousAct      ;
    QAction       * saveAct          ;
    QAction       * saveAsAct        ;
    QAction       * separatorAct     ;
    QAction       * tileAct          ;
    QMenu         * windowMenu       ;
    QSignalMapper * windowMapper     ;

    QSize           hNavigatorSize_  ;
    QPoint          hNavigatorPos_   ;


    int mainWindowW_ 	    	     ;
    int mainWindowH_ 	    	     ;

    int mainPanelW_  	    	     ;
    int mainPanelH_  	    	     ;

    int xmlEditorW_  	    	     ;
    int xmlEditorH_  	    	     ;
    
    bool mainTabsActivated_ 	     ;
};

#endif
