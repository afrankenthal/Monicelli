/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#ifndef NEWNONSTANDARDTABDIALOG_H
#define NEWNONSTANDARDTABDIALOG_H

#include <sstream>
#include <QDialog>

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

    bool     state_ ;
    QString  type_  ;

    int      min_   ;
    int      max_   ;

private slots:
    void on_rowColSB_valueChanged(int );
    void on_cancelPB_clicked();
    void on_okPB_clicked();
};

#endif // NEWNONSTANDARDTABDIALOG_H
