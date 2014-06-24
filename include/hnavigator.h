/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef HNAVIGATOR_H
#define HNAVIGATOR_H

#include <sstream>

#include <QTabWidget>
#include <TBrowser.h>
#include <TFolder.h>

#include "fileEater.h"
#include "HManager.h"
#include "hTreeBrowser.h"
#include "maintabs.h"
#include "mainwindow.h"

class MainWindow ;
class mainTabs ;
class hTreeBrowser ;

QT_BEGIN_NAMESPACE
class QMdiArea ;
QT_END_NAMESPACE

namespace Ui {
    class HNavigator;
}

class HNavigator : public QTabWidget
{
    Q_OBJECT

public:
    explicit     HNavigator            (QWidget       * parent = 0) ;
                ~HNavigator            (void                      ) ;

    void         collectExistingWidgets(QWidget       * parent = 0) ;
    void         refresh               (void                      ) ;
    std::string  twoDOption            (void                      ) ;
    bool         plotStatBox           (void                      ) ;
    bool         plotFitBox            (void                      ) ;

public slots:
    void         addNewFile            (const QString & file      ) ;

    QMdiArea   * getMdiArea            (void                      ) ;
    MainWindow * getMainWindow         (void                      ) ;

private:

    void         fillWidget            (void                      ) ;
    void         fillWidgetTree        (void                      ) ;
    void         fillWidgetTree        (std::string    file       ) ;
    void         resizeEvent           (QResizeEvent * event      ) ;
    void         addItem               (std::string    item       ) ;
    void         makeDir               (std::string    dirName    ) ;

    QString                 emptyFileLabel_     ;
    QString                 displayAllLabel_    ;
    QString                 displayMemoryLabel_ ;

    QTimer                * timer_              ;

    Ui::HNavigator        * ui                  ;

    HManager              * theHManager_        ;
    fileEater             * theFileEater_       ;
    hTreeBrowser          * theHTreeBrowser_    ;
    mainTabs              * theTabWidget_       ;
    MainWindow            * parent_             ;

    fileEater::fileMapDef   openFiles_          ;

    std::stringstream       ss_                 ;

    int                     counter_            ;

private slots:
    void on_unZoomPB_clicked        (void               );
    void on_canvasSizeCB_activated  (QString            );
    void on_hCanvasCB_activated     (int     index      );
    void on_saveComponentsPB_clicked(                   );
    void updateTree                 (QString currentFile);
    void checkNewObjectsInMemory    (void               );
};

#endif // HNAVIGATOR_H
