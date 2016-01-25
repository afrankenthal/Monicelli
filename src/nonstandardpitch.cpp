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
