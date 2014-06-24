/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#include "xmlParser.h"
#include "rocwidget.h"
#include "MessageTools.h"
#include "nonstandardpitch.h"
#include "ui_nonstandardpitch.h"

//================================================================================
nonStandardPitch::nonStandardPitch(QWidget     * parent,
                                   std::string   type,
                                   int           value) :
    QWidget(parent),
    ui(new Ui::nonstandardpitch)
{
  ui->setupUi(this);
  pitchType_ = type ;
  value_     = value ;
}

//================================================================================
nonStandardPitch::~nonStandardPitch()
{
  delete ui;
}

//================================================================================
void nonStandardPitch::initialize(xmlROC * theROC)
{
  ROC_ = theROC ;

  ui->nonStandardPitchLE->setInnerGeometry(ui->nonStandardPitchLE->geometry());
  ui->nonStandardPitchLE->needsReturn() ;

  lEditsDef lEdits ;

  if( pitchType_ == "row" )
  {
    ss_.str("") ; ss_ << ROC_->nonStandardRowPitch_[value_] ;
    ui->nonStandardPitchLE->setText("nonStandardRowPitch",ss_.str());
    ss_.str("") ; ss_ << value_ ;
    ui->nonStandardPitchLE->setTabbed(ss_.str(), "rowNum") ;
  }
  else
  {
    ss_.str("") ; ss_ << ROC_->nonStandardColPitch_[value_] ;
    ui->nonStandardPitchLE->setText("nonStandardColPitch",ss_.str());
    ss_.str("") ; ss_ << value_ ;
    ui->nonStandardPitchLE->setTabbed(ss_.str(), "colNum") ;
  }

  ui->nonStandardPitchLE->setRange(0.01,1.,4) ;
  connect(ui->nonStandardPitchLE, SIGNAL(tabbedTextEdited(std::string, std::string, std::string, QString &)),
          ROC_,                   SLOT  (setTabbedText   (std::string, std::string, std::string, QString &))) ;
}
