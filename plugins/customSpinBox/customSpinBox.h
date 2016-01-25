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
 
#ifndef CUSTOMSPINBOX_H
#define CUSTOMSPINBOX_H

#include <sstream>

#include <QAbstractSpinBox>
#include <QDomNode>

class QRect       ;

namespace Ui {
    class customSpinBox;
}

class customSpinBox : public QAbstractSpinBox
{
    Q_OBJECT

public:
    explicit customSpinBox      (QWidget*     parent = 0 );
            ~customSpinBox      (void                    );

    int      value              (void                    );
    void     init               (void                    );
    void     setValue           (std::string  key,
                                 int          value      );
    void     setValue           (std::string  key,
                                 std::string  value      );
    void     setValue           (int          value      );
    void     setRange           (int          min,
                                 int          max        );
    void     setMinimum         (int          value      );
    void     setMaximum         (int          value      );
    void     assignXmlElement   (QDomNode&    element    ) {elementNode_ = element    ;}
    void     valueIsAttribute   (bool         isAttribute) {isAttribute_ = isAttribute;}
    void     setSingleStep      (int          step       );

private:
    Ui::customSpinBox * ui  ;

    QWidget*            theParent_    ;
    std::string         key_          ;
    std::stringstream   ss_           ;
    bool                isAttribute_  ;
    std::string         attributeName_;
    QDomNode            elementNode_  ;

public slots:
    void setEnabled             (int         enable        );
    void setEnabled             (bool        enable        );
private slots:
    void on_spinBox_valueChanged(int         value         );
    void setNodeText            (std::string key, int value);

signals:
    void valueChangedKeyed      (std::string key, int value);
    void valueChanged           (int value);
};

#endif // CUSTOMSPINBOX_H
