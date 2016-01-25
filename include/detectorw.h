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
