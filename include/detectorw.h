/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef DETECTORW_H
#define DETECTORW_H

#include "customLineEdit.h"
#include "xmlParser.h"

#include <QFrame>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>

class QString ;
class xmlDetector ;

namespace Ui {
    class detectorW;
}

class detectorW : public QFrame
{
    Q_OBJECT

public:
    explicit detectorW                 (QWidget     * parent = 0    ) ;
            ~detectorW                 (void                        ) ;

    QFrame * getROCTabWidgetPlaceHolder(void                        ) ;

    void     initialize                (xmlDetector * theDetector   ) ;
    void     setXPosition              (double        value         ) ;
    void     setYPosition              (double        value         ) ;
    void     setZPosition              (double        value         ) ;
    void     setXPositionCorrection    (double        correction    ) ;
    void     setYPositionCorrection    (double        correction    ) ;
    void     setZPositionCorrection    (double        correction    ) ;
    void     setXRotation              (double        value         ) ;
    void     setYRotation              (double        value         ) ;
    void     setZRotation              (double        value         ) ;
    void     setXRotationCorrection    (double        correction    ) ;
    void     setYRotationCorrection    (double        correction    ) ;
    void     setZRotationCorrection    (double        correction    ) ;

private:
    typedef std::map<std::string, QLineEdit*> lEditsDef ;

    Ui::detectorW     * ui        ;

    xmlDetector       * detector_ ;

    std::stringstream   ss_       ;

public slots:
    void     detectorToggled           (bool          inUse         ) ;

signals:
    void     detectorIsToggled         (int,
                                        int,
                                        bool                        ) ;
    void     placeROCTab               (xmlROC      * theXmlROC     ) ;
    void     removeROCTab              (int           stationId,
                                        int           detectorId,
                                        int           ROCSerial     ) ;

private slots:
    void on_remROCPB_clicked();
    void on_addROCPB_clicked();
};

#endif // DETECTORW_H
