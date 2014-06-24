/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef ROCWIDGET_H
#define ROCWIDGET_H

#include <sstream>
#include <map>
#include <QFrame>
#include <QTabWidget>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

class xmlROC ;
class QLineEdit ;

namespace Ui {
    class ROCWidget;
}

class ROCWidget : public QFrame
{
    Q_OBJECT

public:
    explicit     ROCWidget          (QWidget * parent = 0) ;
                ~ROCWidget          (void                ) ;

    QTabWidget * getNonStandardRowTW(void                ) ;
    QTabWidget * getNonStandardColTW(void                ) ;

    void         initialize         (xmlROC  * theROC    ) ;

private slots:

    void on_browsePB_clicked();
    void on_remColTabPB_clicked();
    void on_remRowTabPB_clicked();
    void on_addColTabPB_clicked();
    void on_addRowTabPB_clicked();
    void         ROCToggled         (bool      inUse     ) ;

private:

    typedef std::map<std::string, QLineEdit*> lEditsDef ;

    Ui::ROCWidget     * ui   ;

    xmlROC            * ROC_ ;

    std::stringstream   ss_  ;

signals:

    void addNewTab                (ROCWidget *,
                                   xmlROC    *,
                                   QString    ,
                                   int        ,
                                   double      ) ;
    void removeNonStandardPitchTab(ROCWidget * ) ;
};

#endif // ROCWIDGET_H
