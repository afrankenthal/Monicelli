/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#include <iostream>

#include "customLineEdit.h"
#include "ui_customLineEdit.h"
#include "MessageTools.h"

#include <QEvent>
#include <QMessageBox>
#include <QRect>
#include <QRegExpValidator>
#include <QTextStream>
#include <QValidator>

//================================================================================
customLineEdit::customLineEdit(QWidget *parent) : QLineEdit(parent),
ui(new Ui::customLineEdit)
{
  theParent_    = parent ;

  needsReturn_  = false ;
  disableCheck_ = false ;
  isTabbed_     = false ;

  ui->setupUi(this);
  ui->lineEdit->setGeometry(this->geometry().x(),
                            this->geometry().y(),
                            this->geometry().width(),
                            this->geometry().height()) ;

  connect(ui->lineEdit, SIGNAL(editingFinished(                      )),
          this,         SLOT  (checkValidity  (                      ))) ;
  connect(this,         SIGNAL(textEditedKeyed(std::string, QString &)),
          this,         SLOT  (setNodeText    (std::string, QString &))) ;
}

//================================================================================
customLineEdit::~customLineEdit()
{
  delete ui;
}

//================================================================================
void customLineEdit::setNodeText(std::string   key,
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
//    STDLINE(ss_.str(),ACWhite) ;
    elementNode_.toElement().setAttribute(QString(key.c_str()),textValue) ;
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
  ui->lineEdit->setToolTip(textValue);
}

//================================================================================
void customLineEdit::setRange(double min,
                              double max,
                              int decimals)
{
  QDoubleValidator * v = new QDoubleValidator(min,max,decimals,this) ;
  v->setNotation(QDoubleValidator::ScientificNotation);
//  this->setValidator(v) ;
}

//================================================================================
void customLineEdit::setRange(int min,
                              int max)
{
  this->setValidator(new QIntValidator(min,max,this)) ;
}

//================================================================================
void customLineEdit::setRange(std::string regexp)
{
  QRegExp theRegexp(QString(regexp.c_str()));
  QRegExpValidator * validator = new QRegExpValidator(this) ;
  validator->setRegExp(theRegexp);
  this->setValidator(validator) ;
}

//================================================================================
void customLineEdit::setText(QString textValue)
{
  //theText_ = textValue;
  ui->lineEdit->setText(textValue.toStdString().c_str());
}

//================================================================================
void customLineEdit::setText(std::string key, std::string textValue)
{
  key_          = key ;
  originalText_ = QString(textValue.c_str()) ;
  ui->lineEdit->setText(textValue.c_str());
}

//================================================================================
void customLineEdit::backSpace()
{
  disableCheck_ = true ; // Repeat this statement before any change to the lineEdit!!!!!
  ui->lineEdit->backspace();
  if( ui->lineEdit->text().size() == 0 )
  {
    disableCheck_ = true ; // Repeat this statement before any change to the lineEdit!!!!!
    ui->lineEdit->setText(originalText_) ;
  }

}

//================================================================================
void customLineEdit::on_lineEdit_textChanged(QString newText)
{
  theText_ = newText ;
  int pos  = newText.size() ;

  if( this->validator() && pos > 0 &&
    ((this->validator()->validate(newText,pos) == QValidator::Intermediate) ||
     (this->validator()->validate(newText,pos) == QValidator::Invalid     ) ))
  {
    this->backSpace();
  }

  if( !needsReturn_ )
  {
    emit textEditedKeyed(key_, theText_);
  }

  if( isTabbed_ )
  {
    emit tabbedTextEdited(key_, tabValue_, attributeName_, theText_) ;
  }

}

//================================================================================
void customLineEdit::on_lineEdit_returnPressed()
{
}

//================================================================================
void customLineEdit::setTabbed(std::string value, std::string attName)
{
  isTabbed_      = true ;
  tabValue_      = value ;
  attributeName_ = attName ;
}

//================================================================================
bool customLineEdit::checkValidity()
{
  if( disableCheck_ )
  {
    disableCheck_ = false ;
    return true;
  }

  for(std::vector<int>::iterator it=eList_.begin(); it!=eList_.end(); ++it)
  {
    if(theText_.toInt() == *it)
    {
      this->backSpace() ;
      ss_.str(""); ss_ << "The value " << *it << " is invalid (already used or out of allowed range!";

      QMessageBox msgBox;
      msgBox.setText(QString(ss_.str().c_str()));
      msgBox.exec();

      return false;
    }
  }

  if( theText_.size() == 0 ) ui->lineEdit->setText(originalText_) ;

  emit textEditedKeyed(key_, theText_);

  return true ;
}

//================================================================================
void customLineEdit::setInnerGeometry(const QRect & geometry)
{
  ui->lineEdit->setGeometry(0, 0, geometry.width(), geometry.height()) ;
}

//================================================================================
void customLineEdit::needsReturn(void)
{
  needsReturn_ = true ;
}

////================================================================================
void customLineEdit::setExclusionList(std::vector<int> eList)
{
  eList_ = eList ;
}
