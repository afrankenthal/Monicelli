#include <iostream>

#include "customLineEdit.h"
#include "ui_customLineEdit.h"

customLineEdit::customLineEdit(QWidget *parent) : QLineEdit(parent),
    ui(new Ui::customLineEdit)
{
  std::cout << __LINE__
            << "]\t["
            << __PRETTY_FUNCTION__
            << "]\t" 
            << std::endl ;
  ui->setupUi(this);
  std::cout << __LINE__
            << "]\t["
            << __PRETTY_FUNCTION__
            << "]\t"
            << std::endl ;
}

customLineEdit::~customLineEdit()
{
  delete ui;
}

void customLineEdit::on_lineEdit_textChanged(QString newText)
{
  theText_ = newText ;
  std::cout << __LINE__
            << "]\t["
            << __PRETTY_FUNCTION__
            << "]\t"
            << newText.toStdString()
            << std::endl ;
}

void customLineEdit::on_lineEdit_returnPressed()
{
  std::cout << __LINE__
            << "]\t["
            << __PRETTY_FUNCTION__
            << "]\t"
            << theText_.toStdString()
            << " <-- final word"
            << std::endl ;

}

void customLineEdit::setInnerGeometry(QRect & geometry)
{
  ui->lineEdit->setGeometry(0, 0, geometry.width(), geometry.height());
}
