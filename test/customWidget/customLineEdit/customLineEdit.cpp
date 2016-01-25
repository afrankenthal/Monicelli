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
