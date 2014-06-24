#ifndef FORMPAGE1_H
#define FORMPAGE1_H

#include <QWidget>
#include "ui_formpage1.h"

namespace Ui {
    class FormPage1;
}

class FormPage1 : public QWidget
{
    Q_OBJECT

public:
    explicit FormPage1(QWidget *parent = 0);
    ~FormPage1();

    void setLabelText(QString s) ;

private:
    Ui::FormPage1 *ui;
};

#endif // FORMPAGE1_H
