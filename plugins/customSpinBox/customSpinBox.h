/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
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
