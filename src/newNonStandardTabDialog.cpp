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
 
#include "newNonStandardTabDialog.h"
#include "ui_newNonStandardTabDialog.h"
#include "MessageTools.h"
#include <QtWidgets/QMessageBox>

//================================================================================
newNonStandardTabDialog::newNonStandardTabDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newNonStandardTabDialog)
{
    ui->setupUi(this);

    this->setModal(true) ;

    state_ = false ;
}

//================================================================================
newNonStandardTabDialog::~newNonStandardTabDialog()
{
    delete ui;
}

//================================================================================
void newNonStandardTabDialog::setType(QString type)
{
  type_  = QString("New ") + type + QString(":") ;
  ui->rowColLB->setText(type_);
}

//================================================================================
void newNonStandardTabDialog::on_okPB_clicked()
{
  state_ = true ;
  this->accept() ;
}

//================================================================================
void newNonStandardTabDialog::on_cancelPB_clicked()
{
  this->reject();
}

//================================================================================
bool newNonStandardTabDialog::state(void)
{
  return state_ ;
}

//================================================================================
int newNonStandardTabDialog::getNewRowCol(void)
{
  return ui->rowColSB->value();
}

//================================================================================
double newNonStandardTabDialog::getNewPitch(void)
{
  return ui->pitchSB->value();
}

//================================================================================
void newNonStandardTabDialog::setRange(int min, int max)
{
  min_ = min; max_ = max ;
}

//================================================================================
void newNonStandardTabDialog::on_rowColSB_valueChanged(int value)
{
  if( ui->rowColSB->value() < min_ || ui->rowColSB->value() > max_ )
  {
    ui->rowColSB->setValue(0) ;
    ss_.str("") ; ss_ << "Value "
                      << value
                      << " is out of allowed range ("
                      << min_
                      << ", "
                      << max_
                      << ")" ;
    QMessageBox msgBox(QMessageBox::Warning, tr("Warning"),
                       tr(ss_.str().c_str()), 0, this);
    msgBox.addButton(tr("Ok"    ), QMessageBox::AcceptRole);
  }
}
