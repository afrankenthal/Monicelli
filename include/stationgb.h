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
 
#ifndef STATIONGB_H
#define STATIONGB_H

#include "xmlParser.h"
#include "customSpinBox.h"

#include <QWidget>

class xmlStation ;

namespace Ui {
    class stationGB;
}

class stationGB : public QWidget
{
    Q_OBJECT

public:
    explicit stationGB (QWidget    * parent = 0);
            ~stationGB (void                   );

    void     initialize(xmlStation * theStation) ;

private:
    Ui::stationGB     * ui               ;

    xmlStation        * station_         ;

    std::stringstream   ss_              ;

private slots:
    void on_remDetectorPB_clicked(void                        ) ;
    void on_addDetectorPB_clicked(void                        ) ;
    void stationToggled          (bool          inUse         ) ;

signals:
    void stationIsToggled        (int           id,
                                  bool          inUse         ) ;
    void placeDetectorTab        (xmlDetector * theXmlDetector) ;
    void removeDetectorTab       (int           stationId,
                                  int           detectorSerial) ;

};

#endif // STATIONGB_H
