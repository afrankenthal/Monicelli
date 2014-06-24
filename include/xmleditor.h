/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

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

#include <QWidget>
#include <QString>

namespace Ui {
    class XMLEditor;
}

class XMLEditor : public QWidget
{
    Q_OBJECT

public:
    explicit XMLEditor             (QWidget * parent = 0);
             XMLEditor             (QString   fileName  );
            ~XMLEditor             (void                );
    bool     isActive              (void                ) {return isActive_ ;}
    void     setGeometryFileName   (QString   fileName  ) ;
    void     setXPosition          (int    stationId,
                                    int    detectorId,
                                    double value        ) ;
    void     setYPosition          (int    stationId,
                                    int    detectorId,
                                    double value        ) ;
    void     setZPosition          (int    stationId,
                                    int    detectorId,
                                    double value        ) ;
    void     setXPositionCorrection(int    stationId,
                                    int    detectorId,
                                    double correction   ) ;
    void     setYPositionCorrection(int    stationId,
                                    int    detectorId,
                                    double correction   ) ;
    void     setZPositionCorrection(int    stationId,
                                    int    detectorId,
                                    double correction   ) ;
    void     setXRotation          (int    stationId,
                                    int    detectorId,
                                    double correction   ) ;
    void     setYRotation          (int    stationId,
                                    int    detectorId,
                                    double correction   ) ;
    void     setZRotation          (int    stationId,
                                    int    detectorId,
                                    double correction   ) ;
    void     setXRotationCorrection(int    stationId,
                                    int    detectorId,
                                    double correction   ) ;
    void     setYRotationCorrection(int    stationId,
                                    int    detectorId,
                                    double correction   ) ;
    void     setZRotationCorrection(int    stationId,
                                    int    detectorId,
                                    double correction   ) ;

private:

    typedef std::map<int, ROCTabWidget*>              detWMapDef      ; // Associates a detectorId with its ROCTabWidget
    typedef std::map<int, detWMapDef>                 detROCWMapDef   ; // Associates a stationId AND a detectorId with its ROCTabWidget
    typedef std::map<int, detectorTabWidget*>         stationWMapDef  ; // Associates a stationId  with its detectorTabWidget
    typedef std::map<int, std::map<int, detectorW*> > detectorWMapDef ;

    Ui::XMLEditor *ui;

    void placeDetector      (xmlDetector * theXmlDetector) ;
    void placeStation       (xmlStation  * theXmlStation ) ;
    void placeROC           (xmlROC      * theXmlROC     ) ;
    void layout             (void                        ) ;
    void reorderStationTabs (void                        ) ;
    void reorderDetectorTabs(int           stationId     ) ;
    bool stationInUse       (int           station       ) ;
    bool loadGeometryFile   (void                        ) ;

    stationGB         * theStationGB_  ;
    detectorTabWidget * theDetectorTW_ ;
    detectorW         * theDetectorW_  ;
    stationTabWidget  * theStationTW_  ;
    ROCTabWidget      * theROCTW_      ;
    ROCWidget         * theROCW_       ;
    xmlParser         * theXMLParser_  ;

    detROCWMapDef       detROCsWMap_   ;
    detectorWMapDef     detectorWMap_  ;
    stationWMapDef      stationWMap_   ;

    bool isActive_        ;
    bool alreadyLaidOut_  ;

    std::stringstream ss_ ;

private slots:
    void on_chooseDatePB_clicked();
    void on_showHierachyPB_clicked();
    void on_removeStationPB_clicked();
    void on_addNewStationPB_clicked();
    void on_savePB_clicked            (void                        );
    void toggleStationWidget          (int           id,
                                       bool          inUse         );
    void addNewTab                    (ROCWidget   *,
                                       xmlROC      *,
                                       QString,
                                       int,
                                       double                      );
    void placeDetectorTab             (xmlDetector * theXmlDetector);
    void placeROCTab                  (xmlROC      * theXmlROC     );
    void removeDetectorTab            (int           stationId,
                                       int           detectorSerial);
    void removeROCTab                 (int           stationId,
                                       int           detectorId,
                                       int           ROCSerial     );
};

#endif // XMLEDITOR_H
