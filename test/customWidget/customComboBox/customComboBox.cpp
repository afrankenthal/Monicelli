/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#include <iostream>

#include "customComboBox.h"
#include "ui_customcombobox.h"

//================================================================================
customComboBox::customComboBox(QWidget *parent) : QComboBox(parent),
    ui(new Ui::customComboBox)
{
    ui->setupUi(this);

    ui->theComboBox->addItem(QString("Pippo"));
    ui->theComboBox->addItem(QString("Pluto"));
    ui->theComboBox->addItem(QString("Paperino"));

    connect(ui->theComboBox, SIGNAL(textChanged(std::string, QString &)),
            this,            SLOT  (setText    (std::string, QString &))) ;
}

//================================================================================
customComboBox::~customComboBox()
{
    delete ui;
}

//================================================================================
void customComboBox::on_theComboBox_activated(QString text)
{
}

//================================================================================
void customComboBox::setInnerGeometry(QRect & geometry)
{
  ui->theComboBox->setGeometry(0, 0, geometry.width(), geometry.height());
}
