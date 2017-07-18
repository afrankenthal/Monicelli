/*===============================================================================
 * Monicelli: the FERMILAB MTEST geometry builder and track reconstruction tool
 * 
 * Copyright (C) 2014 
 *
 * Authors:
 *
 * Dario Menasce      (INFN) 
 * Luigi Moroni       (INFN)
 * Jennifer Ngadiuba  (INFN)
 * Stefano Terzo      (INFN)
 * Lorenzo Uplegger   (FNAL)
 * Luigi Vigani       (INFN)
 *
 * INFN: Piazza della Scienza 3, Edificio U2, Milano, Italy 20126
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ================================================================================*/
 
#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <sstream>

#include <QResizeEvent> // ToROOT6
#include <QtWidgets/QWidget>
#include "TCanvas.h"
#include "qrootcanvas.h"

namespace Ui 
{
    class canvasWidget;
}

class canvasWidget : public QWidget
{
    Q_OBJECT

public:
    explicit canvasWidget(QWidget *parent = 0);
            ~canvasWidget();

    void     divide      (int            nx     ,
                          int            ny     ,
                          float          xmargin,
                          float          ymargin) ;
    void     cd          (int            pos    ) ;
    void     clear       (void                  ) ;
    void     flush       (void                  ) ;
    void     update      (void                  ) ;
    void     setTitle    (std::string    title  ) ;

private:
    void resizeEvent     (QResizeEvent * event  ) ;

    Ui::canvasWidget  * ui         ;
    TCanvas           * theCanvas_ ;
    QRootCanvas       * canvas_    ;
    std::stringstream   ss_        ;


private slots:
} ;

#endif // CANVASWIDGET_H
