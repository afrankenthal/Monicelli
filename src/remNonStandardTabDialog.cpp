/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#include "remNonStandardTabDialog.h"
#include "ui_remNonStandardTabDialog.h"
#include "MessageTools.h"

//================================================================================
remNonStandardTabDialog::remNonStandardTabDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::remNonStandardTabDialog)
{
    ui->setupUi(this);

    this->setModal(true) ;

    state_ = false ;

}

//================================================================================
remNonStandardTabDialog::~remNonStandardTabDialog()
{
    delete ui;
}

//================================================================================
void remNonStandardTabDialog::setType(QString type)
{
  type_  = QString("Remove ") + type + QString(":") ;
  ui->rowColLB->setText(type_);
}

//================================================================================
void remNonStandardTabDialog::setRange(std::vector<QString> tabLabels)
{
  for(int l=0; l<(int)tabLabels.size(); ++l)
  {
    ui->rowColCB->insertItem(l,tabLabels[l]);
  }
}

//================================================================================
int remNonStandardTabDialog::getSelectedValue(void)
{
  return ui->rowColCB->currentText().toInt() ;
}

//================================================================================
int remNonStandardTabDialog::getSelectedIndex(void)
{
  return ui->rowColCB->currentIndex() ;
}

//================================================================================
void remNonStandardTabDialog::on_okPB_clicked()
{
  state_ = true ;
  this->accept() ;
}

//================================================================================
void remNonStandardTabDialog::on_cancelPB_clicked()
{
  this->reject();
}
