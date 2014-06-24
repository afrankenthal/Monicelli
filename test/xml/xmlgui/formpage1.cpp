#include "formpage1.h"
#include "ui_formpage1.h"

FormPage1::FormPage1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormPage1)
{
    ui->setupUi(this);
}

FormPage1::~FormPage1()
{
    delete ui;
}

void FormPage1::setLabelText(QString s)
{
  ui->label->setText(s) ;
}
