/*===============================================================================
 * Monicelli: the FERMILAB MTEST geometry builder and track reconstruction tool
 * 
 * Copyright (C) 2014 
 *
 * Authors:
 *
 * Dario Menasce      (INFN) 
 * Luigi Moroni       (INFN)
 * Jennifer Ngadiuba  (INFN)
 * Stefano Terzo      (INFN)
 * Lorenzo Uplegger   (FNAL)
 * Luigi Vigani       (INFN)
 *
 * INFN: Piazza della Scienza 3, Edificio U2, Milano, Italy 20126
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ================================================================================*/
 
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
