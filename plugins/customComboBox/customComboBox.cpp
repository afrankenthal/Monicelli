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
#include <QTextStream>
#include "MessageTools.h"

#include "customComboBox.h"
#include "ui_customComboBox.h"

//================================================================================
customComboBox::customComboBox(QWidget *parent) : QComboBox(parent),
ui(new Ui::customComboBox)
{
  ui->setupUi(this);

  connect(this, SIGNAL(textChanged(std::string, QString &)),
          this, SLOT  (setNodeText(std::string, QString &))) ;
}

//================================================================================
customComboBox::~customComboBox()
{
  delete ui;
}

//================================================================================
void customComboBox::setInnerGeometry(const QRect & geometry)
{
  ui->comboBox->setGeometry(0, 0, geometry.width(), geometry.height()) ;
}

//================================================================================
void customComboBox::setCurrentIndex(std::string key, QString value)
{
  key_ = key ;
  ui->comboBox->setCurrentIndex(ui->comboBox->findText(value));
}

//================================================================================
void customComboBox::addItem(QString value)
{
//  QString text1;
//  QTextStream stream1( &text1 );
//  elementNode_.save( stream1, 2 ) ;
//  int pos1 = text1.toStdString().find(">") + 1;
//  STDLINE(text1.toStdString().substr(0,pos1),ACLightGreen) ;

  ui->comboBox->addItem(value);

//  QString text2;
//  QTextStream stream2( &text2 );
//  elementNode_.save( stream2, 2 ) ;
//  int pos2 = text2.toStdString().find(">") + 1;
//  STDLINE(text2.toStdString().substr(0,pos2),ACLightBlue) ;
}

//================================================================================
void customComboBox::setNodeText(std::string   key,
                                 QString     & textValue)
{
  if(isAttribute_)
  {
//    ss_.str("") ; ss_ << key << " = " << textValue.toStdString() ; STDLINE(ss_.str(),ACGreen) ;
    if( key != "" ) elementNode_.toElement().setAttribute(QString(key.c_str()),textValue) ;
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
    QDomText text     = elementNode_.ownerDocument().createTextNode(textValue) ;

    thisNode.replaceChild(text,child) ;
  }
}

//================================================================================
void customComboBox::on_comboBox_currentIndexChanged(QString text)
{
  emit textChanged(key_, text);

}
