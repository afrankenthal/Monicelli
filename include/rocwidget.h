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
