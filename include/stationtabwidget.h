/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef STATIONTABWIDGET_H
#define STATIONTABWIDGET_H

#include <sstream>
#include <map>
#include <QTabWidget>
#include <QDomNode>

namespace Ui {
    class stationTabWidget;
}

class stationTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit  stationTabWidget(QWidget  * parent = 0         ) ;
             ~stationTabWidget(void                          ) ;

    void      remTab          (int        stationSerial      ) ;
    void      setNode         (int        stationSerial,
                               QDomNode & stationNode        ) ;
    QTabBar * getTabBar       (void                          ) {return this->tabBar() ;}

private:
    Ui::stationTabWidget *ui;

    std::stringstream ss_ ;

    std::map<std::string, QDomNode> stationMap_ ;

private slots:

    void reassignPosition(int from,
                          int to   ) ;

};

#endif // STATIONTABWIDGET_H
