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
 
#include "MessageTools.h"
#include "serialIdDialog.h"
#include "ui_serialIdDialog.h"

//===========================================================================
serialIdDialog::serialIdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::serialIdDialog)
{
    ui->setupUi(this);

    useToAdd_     = true  ;
    hasValidData_ = false ;

    bGroup = new QButtonGroup() ;
    bGroup->addButton(ui->IdRB);
    bGroup->addButton(ui->tabPositionRB);
    bGroup->setId(ui->IdRB,          0) ;
    bGroup->setId(ui->tabPositionRB, 1) ;

    connect(bGroup,   SIGNAL(buttonReleased (int    )),
            this,     SLOT  (changeMode     (int    ))) ;
}

//===========================================================================
serialIdDialog::~serialIdDialog()
{
    delete ui;
}

//===========================================================================
void serialIdDialog::changeMode(int buttonClicked)
{
  if( buttonClicked == 0 )
  {
    ui->IdCB         ->setEnabled(true ) ;
    ui->tabPositionCB->setEnabled(false) ;
  }
  else
  {
    ui->IdCB         ->setEnabled(false) ;
    ui->tabPositionCB->setEnabled(true ) ;
  }
}

//===========================================================================
void serialIdDialog::setTitle(QString wTitle)
{
  ui->componentLB->setText(wTitle) ;
}

//===========================================================================
bool serialIdDialog::idIsAvailable()
{
  return ui->IdCB->isEnabled() ;
}

//===========================================================================
QString serialIdDialog::getTitle(void)
{
  return ui->componentLB->text();
}

//===========================================================================
bool serialIdDialog::validate()
{
  hasValidData_ = false ;
  if( useToAdd_ )
  {
    for(idSerialListDef::iterator it =idSerialList_.begin();
                                  it!=idSerialList_.end(); ++it)
    {
      if( ui->IdSB->value() == (*it).first)
      {
        ui->IdSB->clear() ;
        ss_.str(""); ss_ << "Warning: id "
                         << (*it).first
                         << " is already used" ;
        ui->statusBarLE->setText(QString(ss_.str().c_str())) ;
        return hasValidData_;
      }
      if( ui->tabPositionSB->value() == (*it).first)
      {
        ui->tabPositionSB->clear() ;
        ss_.str(""); ss_ << "Warning: tab "
                         << (*it).first
                         << " is already used" ;
        ui->statusBarLE->setText(QString(ss_.str().c_str())) ;
        return hasValidData_;
      }
    }
  }
  else
  {
  }

  hasValidData_ = true ;
  return hasValidData_ ;
}

//===========================================================================
int serialIdDialog::getId(void)
{
  if( useToAdd_ )
    return ui->IdSB->value() ;
  else
    return ui->IdCB->itemText(ui->IdCB->currentIndex()).toInt();
}

//===========================================================================
int serialIdDialog::getSerial(void)
{
  if( useToAdd_ )
    return ui->tabPositionSB->value() ;
  else
    return ui->tabPositionCB->itemText(ui->tabPositionCB->currentIndex()).toInt();
}

//===========================================================================
void serialIdDialog::setUseToAdd(bool useToAdd)
{
  useToAdd_ = useToAdd ;

  if( useToAdd_ )
  {
    ui->IdSB         ->setEnabled(true ) ;
    ui->tabPositionSB->setEnabled(true ) ;
    ui->IdCB         ->setEnabled(false) ;
    ui->tabPositionCB->setEnabled(false) ;
    ui->IdRB         ->hide() ;
    ui->tabPositionRB->hide() ;
  }
  else
  {
    ui->IdSB         ->setEnabled(false) ;
    ui->tabPositionSB->setEnabled(false) ;
    ui->IdCB         ->setEnabled(true ) ;
    ui->tabPositionCB->setEnabled(false) ;
    ui->IdRB         ->setChecked(true ) ;
    ui->IdRB         ->show() ;
    ui->tabPositionRB->show() ;
    for(idSerialListDef::iterator it =idSerialList_.begin();
                                  it!=idSerialList_.end(); ++it)
    {
      ss_.str(""); ss_ << (*it).first ;
      ui->IdCB->addItem         (QString(ss_.str().c_str()));
      ss_.str(""); ss_ << (*it).second ;
      ui->tabPositionCB->addItem(QString(ss_.str().c_str()));
    }
  }
}

//===========================================================================
void serialIdDialog::on_OkPB_clicked()
{
  ui->statusBarLE->clear() ;
  if( this->validate() )
  {
    this->accept() ;
  }
}

//===========================================================================
void serialIdDialog::on_CancelPB_clicked()
{
  this->reject() ;
}
