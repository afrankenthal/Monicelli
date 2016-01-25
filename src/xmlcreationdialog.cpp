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
 
#include "xmlcreationdialog.h"
#include "ui_xmlcreationdialog.h"
#include "MessageTools.h"

//===========================================================================
xmlCreationDialog::xmlCreationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xmlCreationDialog)
{
  ui->setupUi(this);

  this->setModal(true) ;

  state_ = false ;
}

//===========================================================================
xmlCreationDialog::~xmlCreationDialog()
{
  delete ui;
}

//===========================================================================
int xmlCreationDialog::getStations()
{
  return ui->stationsSB->value() ;
}

//===========================================================================
int xmlCreationDialog::getDetectors()
{
  return ui->detectorsSB->value() ;
}

//===========================================================================
int xmlCreationDialog::getROCs()
{
  return ui->ROCsSB->value() ;
}

//===========================================================================
void xmlCreationDialog::on_okPB_clicked()
{
 state_ = true ;
 this->accept() ;
}

//===========================================================================
bool xmlCreationDialog::state(void)
{
  return state_ ;
}

//===========================================================================
void xmlCreationDialog::on_cancelPB_clicked()
{
    this->reject();
}
