/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
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
