/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <sstream>

#include <QMainWindow>

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

    HManager   * getHManager        (void                ) {return theHManager_  ;}
    HNavigator * getHNavigator      (void                ) {return theHNavigator_;}
    mainTabs   * getTabWidget       (void                ) {return theTabWidget_ ;}
    fileEater  * getFileEater       (void                ) {return theFileEater_ ;}
    XMLEditor  * getXMLEditor       (void                ) {return theXMLEditor_ ;}
    QMdiArea   * getMdiArea         (void                ) {return mdiArea;       }
    void         editXMLPanel       (QString     fileName) ;

protected:
    void closeEvent(QCloseEvent *event);

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

    mdiSubWindow  * cSw              ;

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

    int mainWindowW_ ;
    int mainWindowH_ ;

    int mainPanelW_  ;
    int mainPanelH_  ;

    int xmlEditorW_  ;
    int xmlEditorH_  ;
};

#endif
