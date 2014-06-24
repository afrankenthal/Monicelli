/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef DETECTORTABWIDGET_H
#define DETECTORTABWIDGET_H

#include <sstream>
#include <map>
#include <QTabWidget>
#include <QDomNode>

namespace Ui {
    class detectorTabWidget;
}

class detectorTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit   detectorTabWidget(QWidget  * parent = 0    );
              ~detectorTabWidget(void                     );

    void       remTab           (int        detectorSerial);
    void      setNode           (int        detectorSerial,
                                 QDomNode & detectorNode  ) ;
    QTabBar * getTabBar         (void                     ) {return this->tabBar() ;}

private:
    Ui::detectorTabWidget *ui;

    std::stringstream ss_ ;

    std::map<std::string, QDomNode> detectorMap_ ;

  private slots:

      void reassignPosition(int from,
                            int to   ) ;
};

#endif // DETECTORTABWIDGET_H
