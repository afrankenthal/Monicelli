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
 
#ifndef STATIONTABWIDGET_H
#define STATIONTABWIDGET_H

#include <sstream>
#include <map>
#include <QtWidgets/QTabWidget>
#include <QtXml/QDomNode>

namespace Ui {
    class stationTabWidget;
}

class stationTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit  stationTabWidget(QWidget  * parent = 0         ) ;
             ~stationTabWidget(void                          ) ;

    void      remTab          (int        stationSerial      ) ;
    void      setNode         (int        stationSerial,
                               QDomNode & stationNode        ) ;
    QTabBar * getTabBar       (void                          ) {return this->tabBar() ;}

private:
    Ui::stationTabWidget *ui;

    std::stringstream ss_ ;

    std::map<std::string, QDomNode> stationMap_ ;

private slots:

    void reassignPosition(int from,
                          int to   ) ;

};

#endif // STATIONTABWIDGET_H
