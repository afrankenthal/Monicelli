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
 
#ifndef SERIALIDDIALOG_H
#define SERIALIDDIALOG_H

#include "QButtonGroup"
#include <QDialog>
#include <vector>
#include <sstream>

namespace Ui {
    class serialIdDialog;
}

class serialIdDialog : public QDialog
{
    Q_OBJECT

public:

    explicit serialIdDialog(QWidget          * parent = 0) ;
            ~serialIdDialog(void                         ) ;

    typedef std::pair<int, int    > pairIntDef      ;
    typedef std::vector<pairIntDef> idSerialListDef ;

    void     setTitle      (QString            wTitle    ) ;
    QString  getTitle      (void                         ) ;
    void     setTabIdList  (idSerialListDef    list      ) {idSerialList_ = list             ;}
    void     setUseToAdd   (bool               useToAdd  ) ;
    int      getId         (void                         ) ;
    int      getSerial     (void                         ) ;
    bool     hasValidData  (void                         ) {return hasValidData_             ;}
    bool     idIsAvailable (void                         ) ;
private:
    Ui::serialIdDialog *ui;

    std::stringstream ss_ ;

    bool     validate      (void                         ) ;

    QButtonGroup    * bGroup        ;
    bool              hasValidData_ ;
    bool              useToAdd_     ;
    idSerialListDef   idSerialList_ ;


private slots:
    void     on_CancelPB_clicked(void                 );
    void     on_OkPB_clicked    (void                 );
    void     changeMode         (int     buttonClicked) ;
};

#endif // SERIALIDDIALOG_H
