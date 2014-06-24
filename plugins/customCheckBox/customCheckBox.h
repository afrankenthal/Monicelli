/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
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
