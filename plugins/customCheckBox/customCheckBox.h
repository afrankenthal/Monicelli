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
 
#ifndef CUSTOMCHECKBOX_H
#define CUSTOMCHECKBOX_H

#include <iostream>
#include <string>
#include <sstream>

#include <QCheckBox>
#include <QDomNode>

class QDomNode    ;

namespace Ui {
    class customCheckBox;
}

class customCheckBox : public QCheckBox
{
    Q_OBJECT

public:
    explicit customCheckBox  (QWidget * parent = 0);
            ~customCheckBox  (void                );

    void     assignXmlElement(QDomNode & element   ) {elementNode_ = element;}
    void     setLabelText    (QString    labelText ) ;
    void     setChecked      (bool       isChecked ) ;
    void     setChecked      (std::string key, 
                              bool isChecked       ) ;
    bool     isChecked       (void                 ) ;

signals:
    void     isToggled       (bool       isChecked ) ;

private:
    Ui::customCheckBox *ui;

    std::stringstream ss_ ;
    QDomNode          elementNode_     ;
    std::string       key_;

private slots:
    void     nowChecked      (bool       isChecked ) ;
};

#endif // CUSTOMCHECKBOX_H
