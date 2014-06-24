/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef ROCTABWIDGET_H
#define ROCTABWIDGET_H

#include <sstream>
#include <QTabWidget>

namespace Ui {
    class ROCTabWidget;
}

class ROCTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit ROCTabWidget(QWidget * parent = 0);
            ~ROCTabWidget(                    );

    void     remTab      (int       ROCSerial );

    Ui::ROCTabWidget *ui;

private:
    std::stringstream ss_ ;
};

#endif // ROCTABWIDGET_H
