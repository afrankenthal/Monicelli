/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
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
  //ss_.str("");ss_ << "Setting value " << value; STDLINE(ss_.str(),ACPurple) ;
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
//    ss_.str(""); ss_<< key
//                    << " was |"
//                    << elementNode_.toElement().attribute(QString(key.c_str())).toStdString()
//                    << "| now is |"
//                    << textValue.toStdString()
//                    << "|";
//    STDLINE(ss_.str(),ACWhite) ;
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
