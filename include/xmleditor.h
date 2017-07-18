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
 
#ifndef XMLEDITOR_H
#define XMLEDITOR_H

#include "detectortabwidget.h"
#include "detectorw.h"
#include "nonstandardpitch.h"
#include "stationgb.h"
#include "stationtabwidget.h"
#include "roctabwidget.h"
#include "rocwidget.h"
#include "xmlParser.h"

#include <map>
#include <string>

#include <QtWidgets/QWidget>
#include <QtCore/QString>

namespace Ui
{
    class XMLEditor;
}

class XMLEditor : public QWidget
{
    Q_OBJECT

public:
    explicit XMLEditor             (QWidget * parent = 0 );
             XMLEditor             (QString   fileName   );
            ~XMLEditor             (void                 );
    bool     isActive              (void                 ){return isActive_ ;}
    void     setGeometryFileName   (QString   fileName   );
    void     setXPosition          (int       stationId ,
                                    int       detectorId,
                                    double    value      );
    void     setYPosition          (int       stationId ,
                                    int       detectorId,
                                    double    value      );
    void     setZPosition          (int       stationId ,
                                    int       detectorId,
                                    double    value      );
    void     setXPositionCorrection(int       stationId ,
                                    int       detectorId,
                                    double    correction );
    void     setYPositionCorrection(int       stationId ,
                                    int       detectorId,
                                    double    correction );
    void     setZPositionCorrection(int       stationId ,
                                    int       detectorId,
                                    double    correction );
    void     setXRotation          (int       stationId ,
                                    int       detectorId,
                                    double    correction );
    void     setYRotation          (int       stationId ,
                                    int       detectorId,
                                    double    correction );
    void     setZRotation          (int       stationId ,
                                    int       detectorId,
                                    double    correction );
    void     setXRotationCorrection(int       stationId ,
                                    int       detectorId,
                                    double    correction );
    void     setYRotationCorrection(int       stationId ,
                                    int       detectorId,
                                    double    correction );
    void     setZRotationCorrection(int       stationId ,
                                    int       detectorId,
                                    double    correction );

private:

    typedef std::map<int, ROCTabWidget*>              detWMapDef      ; // Associates a detectorId with its ROCTabWidget
    typedef std::map<int, detWMapDef>                 detROCWMapDef   ; // Associates a stationId AND a detectorId with its ROCTabWidget
    typedef std::map<int, detectorTabWidget*>         stationWMapDef  ; // Associates a stationId  with its detectorTabWidget
    typedef std::map<int, std::map<int, detectorW*> > detectorWMapDef ;

    typedef std::map<int, stationGB*>                 stationGBDef    ;

    Ui::XMLEditor                 * ui             ;

    stationGB                     * theStationGB_  ;
    detectorTabWidget             * theDetectorTW_ ;
    detectorW                     * theDetectorW_  ;
    stationTabWidget              * theStationTW_  ;
    ROCTabWidget                  * theROCTW_      ;
    ROCWidget                     * theROCW_       ;
    xmlParser                     * theXMLParser_  ;

    detROCWMapDef                   detROCsWMap_   ;
    detectorWMapDef                 detectorWMap_  ;
    stationWMapDef                  stationWMap_   ;

    stationGBDef                    stationGB_     ;

    bool                            isActive_      ;
    bool                            alreadyLaidOut_;

    std::stringstream               ss_            ;

private:
    void     placeDetector                 (xmlDetector   * theXmlDetector) ;
    void     placeStation                  (xmlStation    * theXmlStation ) ;
    void     placeROC                      (xmlROC        * theXmlROC     ) ;
    void     layout                        (void                          ) ;
    void     reorderStationTabs            (void                          ) ;
    void     reorderDetectorTabs           (int             stationId     ) ;
    bool     stationInUse                  (int             station       ) ;
    bool     loadGeometryFile              (void                          ) ;


private slots:
    void     on_chooseDatePB_clicked       (void                          );
    void     on_showHierachyPB_clicked     (void                          );
    void     on_removeStationPB_clicked    (void                          );
    void     on_addNewStationPB_clicked    (void                          );
    void     on_savePB_clicked             (void                          );
    void     toggleStationWidget           (int             id,
                                            bool            inUse         );
    void     addNewTab                     (ROCWidget     *              ,
                                            xmlROC        *              ,
                                            QString                      ,
                                            int,
                                            double                        );
    void     placeDetectorTab              (xmlDetector   * theXmlDetector);
    void     placeROCTab                   (xmlROC        * theXmlROC     );
    void     removeDetectorTab             (int             stationId    ,
                                            int             detectorSerial);
    void     removeROCTab                  (int             stationId,
                                            int             detectorId   ,
                                            int             ROCSerial     );
    void     textChanged                   (std::string                  ,
                                            QString       &               );
    void     on_resetCalibFitFuncPB_clicked(void                          );
    void     on_enableAllStationsPB_clicked(void                          );
};

#endif // XMLEDITOR_H
