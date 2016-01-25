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

#ifndef CUSTOMCOMBOBOX_H
#define CUSTOMCOMBOBOX_H

#include <QtDesigner/QDesignerExportWidget>
#include <QComboBox>
#include <QDomNode>

class QString ;
class QDomNode    ;
class QRect       ;

namespace Ui {
    class customComboBox;
}

class QDESIGNER_WIDGET_EXPORT customComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit customComboBox  (QWidget  * parent = 0) ;
            ~customComboBox  (void                 ) ;

    void     assignXmlElement(QDomNode & element   ) {elementNode_ = element    ;}
    void     setInnerGeometry(QRect    & geometry  ) ;

private:
    Ui::customComboBox * ui               ;

    QDomNode             elementNode_     ;

private slots:
    void on_theComboBox_activated(QString );
};

#endif // CUSTOMCOMBOBOX_H
