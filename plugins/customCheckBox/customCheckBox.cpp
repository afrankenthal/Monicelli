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
