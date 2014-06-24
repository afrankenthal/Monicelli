/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef NONSTANDARDPITCH_H
#define NONSTANDARDPITCH_H

#include <string>
#include <sstream>
#include <map>
#include <QWidget>

class xmlROC ;
class QLineEdit ;

namespace Ui {
    class nonstandardpitch;
}

class nonStandardPitch : public QWidget
{
    Q_OBJECT

public:
  explicit nonStandardPitch(QWidget     * parent,
                            std::string   type,
                            int           value     );
            ~nonStandardPitch();

    void     initialize(xmlROC * theROC) ;

private:
    typedef std::map<std::string, QLineEdit*> lEditsDef ;

    Ui::nonstandardpitch * ui;

    xmlROC               * ROC_       ;

    std::string            pitchType_ ;
    int                    value_     ;
    std::stringstream      ss_        ;

};

#endif // NONSTANDARDPITCH_H
