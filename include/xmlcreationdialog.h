/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef XMLCREATIONDIALOG_H
#define XMLCREATIONDIALOG_H

#include <QDialog>

namespace Ui {
    class xmlCreationDialog;
}

class xmlCreationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit xmlCreationDialog(QWidget *parent = 0) ;
            ~xmlCreationDialog(void               ) ;

    int      getDetectors     (void               ) ;
    int      getStations      (void               ) ;
    int      getROCs          (void               ) ;
    bool     state            (void               ) ;

private:
    Ui::xmlCreationDialog *ui;

    bool state_ ;

private slots:
    void on_cancelPB_clicked();
    void on_okPB_clicked();
};

#endif // XMLCREATIONDIALOG_H
