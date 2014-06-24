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
