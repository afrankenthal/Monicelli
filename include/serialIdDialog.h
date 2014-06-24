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
