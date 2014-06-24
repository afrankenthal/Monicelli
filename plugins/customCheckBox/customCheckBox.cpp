/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#include "customCheckBox.h"
#include "ui_customCheckBox.h"

#include "MessageTools.h"

//================================================================================
customCheckBox::customCheckBox(QWidget *parent) : QCheckBox(parent),
    ui(new Ui::customCheckBox)
{
    ui->setupUi(this);

    connect(ui->checkBox, SIGNAL(toggled   (bool)),
            this,         SLOT  (nowChecked(bool))) ;
}

//================================================================================
customCheckBox::~customCheckBox()
{
    delete ui;
}

//================================================================================
void customCheckBox::setLabelText(QString labelText)
{
    ui->checkBox->setText(labelText) ;
}

//================================================================================
void customCheckBox::setChecked(bool isChecked)
{
    key_ = "isDUT";
    ui->checkBox->setChecked(isChecked) ;
}

//================================================================================
void customCheckBox::setChecked(std::string key, bool isChecked)
{
    key_ = key;
    ui->checkBox->setChecked(isChecked) ;
}

//================================================================================
void customCheckBox::nowChecked(bool isChecked)
{
    std::string value;

    if( isChecked )
        value = "yes" ;
    else
        value = "no" ;

    ui->checkBox->setChecked(isChecked) ;
    elementNode_.toElement().setAttribute(QString(key_.c_str()),QString(value.c_str())) ;

    emit(isToggled(isChecked)) ;
}
//================================================================================
bool customCheckBox::isChecked(void)
{
    return ui->checkBox->isChecked();
}
