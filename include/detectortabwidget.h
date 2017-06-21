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
 
#ifndef DETECTORTABWIDGET_H
#define DETECTORTABWIDGET_H

#include <sstream>
#include <map>
#include <QtWidgets/QTabWidget>
#include <QtXml/QDomNode>

namespace Ui {
    class detectorTabWidget;
}

class detectorTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit   detectorTabWidget(QWidget  * parent = 0    );
              ~detectorTabWidget(void                     );

    void       remTab           (int        detectorSerial);
    void      setNode           (int        detectorSerial,
                                 QDomNode & detectorNode  ) ;
    QTabBar * getTabBar         (void                     ) {return this->tabBar() ;}

private:
    Ui::detectorTabWidget *ui;

    std::stringstream ss_ ;

    std::map<std::string, QDomNode> detectorMap_ ;

  private slots:

      void reassignPosition(int from,
                            int to   ) ;
};

#endif // DETECTORTABWIDGET_H
