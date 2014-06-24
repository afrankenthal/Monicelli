#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H

#include <QLineEdit>
#include <QRect>

class QRect ;

namespace Ui {
    class customLineEdit;
}

class customLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit customLineEdit  (QWidget * parent = 0);
            ~customLineEdit  (void                );

    void     setInnerGeometry(QRect   & geometry  );

private:
    Ui::customLineEdit *ui;

    QString theText_ ;

private slots:
    void on_lineEdit_returnPressed();
    void on_lineEdit_textChanged(QString );
};

#endif // CUSTOMLINEEDIT_H
