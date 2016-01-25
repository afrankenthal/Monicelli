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
 
#include "customTextEdit.h"
#include "ui_customTextEdit.h"
#include "MessageTools.h"
#include <QTextStream>

#include <unistd.h>

//================================================================================
customTextEdit::customTextEdit(QWidget *parent) :
    QTextEdit(parent),
    ui(new Ui::customTextEdit)
{
    ui->setupUi(this);

//    connect(this, SIGNAL(textEdited     (std::string, QString &)),
//            this, SLOT  (setNodeText    (std::string, QString &))) ;
}

//================================================================================
customTextEdit::~customTextEdit()
{
    delete ui;
}

//================================================================================
void customTextEdit::setText(std::string key, std::string textValue)
{
  key_ = key ;
//  ss_.str(""); ss_<<key_<<" = "<<textValue; STDLINE(ss_.str(),ACWhite);
  ui->textEdit->setText(textValue.c_str());
}

//================================================================================
void customTextEdit::setNodeText(std::string   key,
                                 QString     & textValue)
{
  if(isAttribute_)
  {
//    ss_.str(""); ss_<< key
//                    << " was |"
//                    << elementNode_.toElement().attribute(QString(key.c_str())).toStdString()
//                    << "| now is |"
//                    << textValue.toStdString()
//                    << "|";
//    STDLINE(ss_.str(),ACRed) ;
    elementNode_.toElement().setAttribute(QString(key.c_str()),textValue) ;

//    QString text ;
//    QTextStream stream( &text );
//    elementNode_.save( stream, 2 ) ;

//    STDLINE(std::string("Node content sofar: ")+text.toStdString(),ACYellow) ;

    usleep(100000) ; // This is important: if users type charecters in the TestEdit box too
                    // fast, for some reason the elementNode_ is scrambled up. Slow down.
  }
  else
  {
//    ss_.str(""); ss_<< key
//                    << " = "
//                    << elementNode_.toElement().attribute(QString(key.c_str())).toStdString();
//    STDLINE(ss_.str(),ACWhite) ;
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
void customTextEdit::setInnerGeometry(const QRect & geometry)
{
  ui->textEdit->setGeometry(0, 0, geometry.width(), geometry.height()) ;
}

//================================================================================
void customTextEdit::setText(QString textValue)
{
  ui->textEdit->setText(textValue.toStdString().c_str());
}

//================================================================================
void customTextEdit::on_textEdit_textChanged()
{
  QString currentText = ui->textEdit->toPlainText() ;
//  ss_.str(""); ss_<<key_<<" = "<<currentText.toStdString(); STDLINE(ss_.str(),ACWhite);
  this->setNodeText(key_,currentText) ;
}
