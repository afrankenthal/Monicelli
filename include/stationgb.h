/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

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
