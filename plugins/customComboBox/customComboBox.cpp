/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
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
