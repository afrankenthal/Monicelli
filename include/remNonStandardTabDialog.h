/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#ifndef REMNONSTANDARDTABDIALOG_H
#define REMNONSTANDARDTABDIALOG_H

#include <sstream>
#include <vector>
#include <QDialog>

namespace Ui {
    class remNonStandardTabDialog;
}

class remNonStandardTabDialog : public QDialog
{
    Q_OBJECT

public:
    explicit remNonStandardTabDialog(QWidget             * parent = 0);
            ~remNonStandardTabDialog(void                            );

    void     setType                (QString               type      );
    void     setRange               (std::vector<QString>  tabLabels );
    int      getSelectedIndex       (void                            );
    int      getSelectedValue       (void                            );
    bool     state                  (void                            ) {return state_;}
private:
    Ui::remNonStandardTabDialog *ui;

    bool     state_ ;
    QString  type_  ;

    std::stringstream ss_ ;

private slots:
    void on_cancelPB_clicked();
    void on_okPB_clicked();
};

#endif // REMNONSTANDARDTABDIALOG_H
