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
 
#ifndef CUSTOMTABLEWIDGET_H
#define CUSTOMTABLEWIDGET_H

#include <QTableView>
#include <QDomNodeList>

class QRect;
class QTableWidgetItem;

namespace Ui {
class customTableWidget;
}

class customTableWidget : public QTableView
{
    Q_OBJECT

public:
    explicit customTableWidget(QWidget* parent = 0 );
    ~customTableWidget        (void                );

    void              init                    (void                            );
    void              assignXmlElement        (QDomNodeList&  element          );
    int 	          columnCount             (void                            ) const;
    void              setColumnCount          (int col                         );
    void              setHorizontalHeaderItem (int col, QTableWidgetItem * item);
    QTableWidgetItem* horizontalHeaderItem    (int col                         ) const;

private:
    Ui::customTableWidget* ui          ;
    QWidget*               theParent_  ;
    QDomNodeList           elementNode_;
public slots:
    void setEnabled   (int  enable      );
    void setEnabled   (bool enable      );
private slots:
    void setNodeValue (int  row, int col);

signals:
    void cellChanged  (int  row, int col);
};

#endif //CUSTOMTABLEWIDGET_H
