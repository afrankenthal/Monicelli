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

#include <string>
#include <sstream>

#include <QComboBox>
#include <QDomNode>

class QRect       ;
class QDomNode    ;

namespace Ui {
    class customComboBox;
}

class customComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit customComboBox  (QWidget     * parent = 0 );
            ~customComboBox  (void                     );

    void     addItem         (QString       value      );
    void     assignXmlElement(QDomNode    & element    ) {elementNode_ = element    ;}
    void     setInnerGeometry(const QRect & geometry   );
    void     setCurrentIndex (std::string   key,
                              QString       value      );
    void     textIsAttribute (bool          isAttribute) {isAttribute_ = isAttribute;}

private:
    Ui::customComboBox * ui          ;

    std::stringstream    ss_         ;
    std::string          key_        ;
    bool                 isAttribute_;
    QDomNode             elementNode_;

private slots:
    void on_comboBox_currentIndexChanged(QString       newText);
    void setNodeText                    (std::string   key,
                                         QString     & value  ) ;


signals:
    void textChanged                    (std::string   key,
                                         QString     & text   ) ;
};

#endif // CUSTOMCOMBOBOX_H
