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

#include "customSpinBox.h"
#include "ui_customSpinBox.h"
#include "MessageTools.h"

//================================================================================
customSpinBox::customSpinBox(QWidget *parent) :
    QAbstractSpinBox(parent)
  , ui(new Ui::customSpinBox)
{
    theParent_    = parent ;
    ui->setupUi(this);
    connect(this,         SIGNAL(valueChangedKeyed(std::string,int)),
            this,         SLOT  (setNodeText      (std::string,int))) ;
}

//================================================================================
customSpinBox::~customSpinBox()
{
    delete ui;
}

//================================================================================
void customSpinBox::init(void)
{
    ui->spinBox->setGeometry     (0, 0, geometry().width(), geometry().height()) ;
    ui->spinBox->setAlignment    (alignment());
    ui->spinBox->setReadOnly     (isReadOnly());
    ui->spinBox->setButtonSymbols(buttonSymbols());
}

//================================================================================
int customSpinBox::value(void)
{
    return ui->spinBox->value();
}

//================================================================================
void customSpinBox::setValue(int value)
{
//  ss_.str("");ss_ << "Spinbox "
//                  << ui->spinBox->objectName().toStdString()
//                  << " Setting value "
//                  << value;
//  STDLINE(ss_.str(),ACPurple) ;
  ui->spinBox->setValue(value) ;
}

//================================================================================
void customSpinBox::setValue(std::string key, std::string value)
{
  key_          = key ;
  //ss_.str("");ss_ << "Setting value key" << value; STDLINE(ss_.str(),ACPurple) ;
  ui->spinBox->setValue(QString(value.c_str()).toInt());
}

//================================================================================
void customSpinBox::setValue(std::string key, int value)
{
  key_          = key ;
  //ss_.str("");ss_ << "Setting value key" << value; STDLINE(ss_.str(),ACPurple) ;
  ui->spinBox->setValue(value);
}

//================================================================================
void customSpinBox::setEnabled(int enable)
{
    ui->spinBox->setEnabled((bool)enable);
}

//================================================================================
void customSpinBox::setEnabled(bool enable)
{
    ui->spinBox->setEnabled(enable);
}

//================================================================================
void customSpinBox::on_spinBox_valueChanged(int value)
{
  //ss_.str("");ss_<< "Changed value to " << value << " emitting signal!"; STDLINE(ss_.str(),ACYellow) ;
  emit valueChangedKeyed(key_,value) ;
  emit valueChanged(value);
}
//================================================================================
void customSpinBox::setNodeText(std::string key,int value)
{
  if(isAttribute_)
  {
    ss_.str(""); ss_<< key
                    << " was |"
                    << elementNode_.toElement().attribute(QString(key.c_str())).toStdString()
                    << "| now is |"
                    << value
                    << "|";
    STDLINE(ss_.str(),ACWhite) ;
    elementNode_.toElement().setAttribute(QString(key.c_str()),QString("%1").arg(value));
  }
  else
  {
    QDomNode thisNode = elementNode_.toElement()
                                    .elementsByTagName(QString(key.c_str()))
                                    .at(0)
                                    .parentNode()
                                    .toElement()
                                    .elementsByTagName(QString(key.c_str()))
                                    .at(0);
    QDomNode child    = thisNode.firstChild() ;
    QDomText text     = elementNode_.ownerDocument().createTextNode(QString("%1").arg(value)) ;

    thisNode.replaceChild(text,child) ;
  }
  //ui->spinBox->setToolTip(QString("%1").arg(value));
}

//================================================================================
void customSpinBox::setRange(int min, int max)
{
    ui->spinBox->setMinimum(min);
    ui->spinBox->setMaximum(max);
}

//================================================================================
void customSpinBox::setMinimum(int value)
{
    ui->spinBox->setMinimum(value);
}

//================================================================================
void customSpinBox::setMaximum(int value)
{
    ui->spinBox->setMaximum(value);
}

//================================================================================
void customSpinBox::setSingleStep(int value)
{
    ui->spinBox->setSingleStep(value);
}
