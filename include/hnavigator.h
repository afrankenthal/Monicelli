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
 
#ifndef HNAVIGATOR_H
#define HNAVIGATOR_H

#include <sstream>

#include <QList>
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
    explicit       HNavigator            (QWidget       * parent = 0) ;
                  ~HNavigator            (void                      ) ;

    void           collectExistingWidgets(QWidget       * parent = 0) ;
    void           refresh               (void                      ) ;
    std::string    twoDOption            (void                      ) ;
    bool           plotStatBox           (void                      ) ;
    bool           plotFitBox            (void                      ) ;
    void           saveAll               (void                      ) ;
    hTreeBrowser * getTheHTreeBrowser    (void                      ) {return theHTreeBrowser_;}

    public slots:
    void           addNewFile            (const QString & file      ) ;

    QMdiArea     * getMdiArea            (void                      ) ;
    MainWindow   * getMainWindow         (void                      ) ;

private:

    void           fillWidget            (void                      ) ;
    void           fillWidgetTree        (void                      ) ;
    void           fillWidgetTree        (std::string    file       ) ;
    void           resizeEvent           (QResizeEvent * event      ) ;
    void           addItem               (std::string    item       ) ;
    void           makeDir               (std::string    dirName    ) ;


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
    void on_saveComponentsPB_clicked(void               );
    void updateTree                 (QString currentFile);
    void checkNewObjectsInMemory    (void               );
};

#endif // HNAVIGATOR_H
