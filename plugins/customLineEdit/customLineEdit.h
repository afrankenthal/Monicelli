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
 
#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H

#include <sstream>
#include <string>
#include <vector>

#include <QLineEdit>
#include <QDomNode>

class QRect       ;
class QDomNode    ;

namespace Ui {
    class customLineEdit;
}

class customLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit customLineEdit  (QWidget          * parent = 0 ) ;
            ~customLineEdit  (void                          ) ;

    QString  displayText(void){return theText_;}
    void     assignXmlElement(QDomNode         & element    ) {elementNode_ = element    ;}
    void     textIsAttribute (bool               isAttribute) {isAttribute_ = isAttribute;}
    void     setText         (std::string        key,
                              std::string        textValue  ) ;
    void     setText         (QString            textValue  ) ;

    void     setRange        (double             min,
                              double             max,
                              int                decimals   ) ;
    void     setRange        (int                min,
                              int                max        ) ;
    void     setRange        (std::string        regexp     ) ;
    void     setTabbed       (std::string        value,
                              std::string        attName    ) ;

    void     setInnerGeometry(const QRect      & geometry   ) ;
    void     needsReturn     (void                          ) ;
    void     setExclusionList(std::vector<int>   eList      ) ;
    void     backSpace       (void                          ) ;

private:
    Ui::customLineEdit * ui               ;

    QString              theText_         ;
    QString              originalText_    ;
    std::string          key_             ;
    bool                 needsReturn_     ;
    bool                 disableCheck_    ;
    bool                 isAttribute_     ;
    std::stringstream    ss_              ;
    std::vector<int>     eList_           ;

    QWidget            * theParent_       ;
    bool                 isTabbed_        ;
    std::string          tabValue_        ;
    std::string          attributeName_   ;
    QDomNode             elementNode_     ;

private slots:
    void on_lineEdit_textChanged  (QString       newText) ;
    void on_lineEdit_returnPressed(void                 ) ;
    bool checkValidity            (void                 ) ;
    void setNodeText              (std::string   key,
                                   QString     & value  ) ;

signals:
    void textEditedKeyed          (std::string   key,
                                   QString     & newText) ;
    void tabbedTextEdited         (std::string   key,
                                   std::string   tabValue,
                                   std::string   attName,
                                   QString     & newText) ;
    void valueRejected            (std::string   text   ) ;
};

#endif // CUSTOMLINEEDIT_H
