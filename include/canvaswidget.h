/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <sstream>

#include <QWidget>
#include "TCanvas.h"

namespace Ui {
    class canvasWidget;
}

class canvasWidget : public QWidget
{
    Q_OBJECT

public:
    explicit canvasWidget(QWidget *parent = 0);
            ~canvasWidget();

    void     divide      (int nx,
                          int ny            ) ;
    void     cd          (int pos           ) ;
    void     clear       (void              ) ;
    void     flush       (void              ) ;
    void     update      (void              ) ;
    void     setTitle    (std::string title ) ;

private:
    Ui::canvasWidget  * ui;

    TCanvas           * theCanvas_ ;

    std::stringstream   ss_ ;

    void resizeEvent ( QResizeEvent * event ) ;

private slots:
} ;

#endif // CANVASWIDGET_H
