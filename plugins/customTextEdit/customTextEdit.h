#ifndef CUSTOMTEXTEDIT_H
#define CUSTOMTEXTEDIT_H

#include <sstream>
#include <QTextEdit>
#include <QWidget>
#include <QDomNode>

class QTextEdit ;
class QRect     ;
class QDomNode    ;

namespace Ui {
    class customTextEdit;
}

class customTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit customTextEdit  (QWidget     * parent = 0 );
            ~customTextEdit  (void                     );

    void     assignXmlElement(QDomNode    & element    ) {elementNode_ = element    ;}
    void     setInnerGeometry(const QRect & geometry   ) ;
    void     setText         (QString       textValue  ) ;
    void     setText         (std::string   key,
                              std::string   textValue  ) ;
    void     textIsAttribute (bool          isAttribute) {isAttribute_ = isAttribute;}

private:
    Ui::customTextEdit * ui            ;

    std::stringstream    ss_           ;
    std::string          key_          ;
    bool                 isAttribute_  ;
    QDomNode             elementNode_  ;

private slots:
    void on_textEdit_textChanged();
    void setNodeText              (std::string   key,
                                   QString     & value  ) ;
};

#endif // CUSTOMTEXTEDIT_H
