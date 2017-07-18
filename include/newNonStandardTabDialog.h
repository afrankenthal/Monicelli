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
 
#ifndef NEWNONSTANDARDTABDIALOG_H
#define NEWNONSTANDARDTABDIALOG_H

#include <sstream>
#include <QtWidgets/QDialog>

class QString ;

namespace Ui {
    class newNonStandardTabDialog;
}

class newNonStandardTabDialog : public QDialog
{
    Q_OBJECT

public:
    explicit newNonStandardTabDialog(QWidget * parent = 0);
            ~newNonStandardTabDialog(void                );

    void     setType                (QString   type      );
    bool     state                  (void                );
    int      getNewRowCol           (void                );
    double   getNewPitch            (void                );
    void     setRange               (int       min,
                                     int       max       );

private:
    Ui::newNonStandardTabDialog *ui;

    std::stringstream ss_ ;

    bool              state_ ;
    QString           type_  ;

    int               min_   ;
    int               max_   ;

private slots:
    void on_rowColSB_valueChanged(int );
    void on_cancelPB_clicked     (void);
    void on_okPB_clicked         (void);
};

#endif // NEWNONSTANDARDTABDIALOG_H
