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
