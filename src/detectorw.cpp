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

#include <QDoubleValidator>
#include <QDomNode>
#include <QTextStream>

#include "MessageTools.h"
#include "detectorw.h"
#include "ui_detectorw.h"
#include "serialIdDialog.h"

//================================================================================
detectorW::detectorW(QWidget *parent) : QFrame(parent),
    ui(new Ui::detectorW)
{
    ui->setupUi(this);    
}

//================================================================================
detectorW::~detectorW()
{
    delete ui;
}

//================================================================================
QFrame * detectorW::getROCTabWidgetPlaceHolder(void)
{
  return ui->ROCTabWidgetPlaceHolder ;
}

//================================================================================
void detectorW::initialize(xmlDetector * theDetector)
{
  detector_ = theDetector ;

  connect(ui->detectorInUseCB, SIGNAL(isToggled      (bool)),
          this,                SLOT  (detectorToggled(bool))) ;


  ss_.str(""); ss_ << detector_ ->getDetectorId() ;
  ui->detectorIdLE              ->textIsAttribute (true                                                         ) ;
  ui->detectorIdLE              ->setText         ("id",ss_.str()                                               ) ; // Assign value from xml file
  ui->detectorIdLE              ->setRange        ("\\d+"                                                       ) ;
  ui->detectorIdLE              ->assignXmlElement(theDetector->getNode()                                       ) ;
  ui->detectorIdLE              ->setInnerGeometry(ui->detectorIdLE->geometry()                                 ) ;
  ui->detectorIdLE              ->setExclusionList(theDetector->getAllDetectorsId()                             ) ;
  ui->detectorIdLE              ->needsReturn     (                                                             ) ;

  ss_.str(""); ss_ << detector_ ->getDescription() ;
  ui->detectorDescriptionTA     ->textIsAttribute (true                                                         ) ;
  ui->detectorDescriptionTA     ->setText         ("description",ss_.str()                                      ) ; // Assign value from xml file
  ui->detectorDescriptionTA     ->assignXmlElement(detector_->getNode()                                         ) ;
  ui->detectorDescriptionTA     ->setInnerGeometry(ui->detectorDescriptionTA->geometry()                        ) ;

  ui->isDUTCB                   ->assignXmlElement(theDetector->getNode()                                       ) ;
  ui->isDUTCB                   ->setChecked      (theDetector->isDUT()                                         ) ;

  ui->detectorInUseCB           ->setChecked      (theDetector->isEnabled()                                     ) ;
  ui->detectorGeoROCGB          ->setEnabled      (theDetector->isEnabled()                                     ) ;

  ui->backFlipXCB               ->textIsAttribute (false                                                        ) ;
  ui->backFlipXCB               ->assignXmlElement(theDetector->getNode()                                       ) ;
  ui->backFlipXCB               ->addItem(QString ("yes")                                                       ) ;
  ui->backFlipXCB               ->addItem(QString ("no" )                                                       ) ;
  ui->backFlipXCB               ->setCurrentIndex ("xBackFlipped",
                                                   detector_->keyValue_["xBackFlipped"]                         ) ;
  ui->backFlipXCB               ->setInnerGeometry(ui->backFlipXCB->geometry()                                  ) ;

  ui->backFlipYCB               ->textIsAttribute (false                                                        ) ;
  ui->backFlipYCB               ->assignXmlElement(theDetector->getNode()                                       ) ;
  ui->backFlipYCB               ->addItem(QString ("yes")                                                       ) ;
  ui->backFlipYCB               ->addItem(QString ("no" )                                                       ) ;
  ui->backFlipYCB               ->setCurrentIndex ("yBackFlipped",
                                                   detector_->keyValue_["yBackFlipped"]                         ) ;
  ui->backFlipYCB               ->setInnerGeometry(ui->backFlipYCB->geometry()                                  ) ;

  ss_.str(""); ss_ << detector_ ->keyValue_["xChipsNumber"].toInt() ;
  ui->xChipsNumberLE            ->textIsAttribute (true                                                         ) ;
  ui->xChipsNumberLE            ->setText         ("xChipsNumber",ss_.str()                                     ) ; // Assign value from xml fil
  ui->xChipsNumberLE            ->assignXmlElement(theDetector->getROCNode()                                    ) ;
  ui->xChipsNumberLE            ->setInnerGeometry(ui->xChipsNumberLE->geometry()                               ) ;

  ss_.str(""); ss_ << detector_ ->keyValue_["yChipsNumber"].toInt() ;
  ui->yChipsNumberLE            ->textIsAttribute (true                                                         ) ;
  ui->yChipsNumberLE            ->setText         ("yChipsNumber",ss_.str()                                     ) ; // Assign value from xml file
  ui->yChipsNumberLE            ->assignXmlElement(theDetector->getROCNode()                                    ) ;
  ui->yChipsNumberLE            ->setInnerGeometry(ui->yChipsNumberLE->geometry()                               ) ;

  ui->detectorTypeLE            ->textIsAttribute (true                                                         ) ;
  ui->detectorTypeLE            ->setText         ("name",
                                                   detector_->keyValue_["name"].toStdString()                   ) ;
  ui->detectorTypeLE            ->assignXmlElement(theDetector->getNode()                                       ) ;
  ui->detectorTypeLE            ->setInnerGeometry(ui->detectorTypeLE->geometry()                               ) ;

  ui->dCalibrationFitFunctionCB ->textIsAttribute (true                                                         ) ;
  ui->dCalibrationFitFunctionCB ->assignXmlElement(theDetector->getNode()                                       ) ;
  ui->dCalibrationFitFunctionCB ->addItem         ("tanh"                                                       ) ;
  ui->dCalibrationFitFunctionCB ->addItem         ("linear"                                                     ) ;
  ui->dCalibrationFitFunctionCB ->addItem         ("parabolic"                                                  ) ;
  ui->dCalibrationFitFunctionCB ->setCurrentIndex ("dCalibrationFitFunction",
                                                   QString((theDetector->getdCalibrationFitFunction()).c_str()) ) ;
  ui->dCalibrationFitFunctionCB ->setInnerGeometry(ui->dCalibrationFitFunctionCB->geometry()                    ) ;

  ui->raCalibrationFitFunctionCB->textIsAttribute (true                                                         ) ;
  ui->raCalibrationFitFunctionCB->assignXmlElement(theDetector->getROCNode()                                    ) ;
  ui->raCalibrationFitFunctionCB->addItem         ("tanh"                                                       ) ;
  ui->raCalibrationFitFunctionCB->addItem         ("linear"                                                     ) ;
  ui->raCalibrationFitFunctionCB->addItem         ("parabolic"                                                  ) ;
  ui->raCalibrationFitFunctionCB->setCurrentIndex ("raCalibrationFitFunction",
                                                   QString((theDetector->getraCalibrationFitFunction()).c_str())) ;
  ui->raCalibrationFitFunctionCB->setInnerGeometry(ui->raCalibrationFitFunctionCB->geometry()                   ) ;

  lEditsDef lEdits ;

  // WARNING: the order below is important, it reflects the order assumed by the dtd!!
  lEdits["xPosition"               ] = ui->xPosition_cg_LE   ;
  lEdits["yPosition"               ] = ui->yPosition_cg_LE   ;
  lEdits["zPosition"               ] = ui->zPosition_cg_LE   ;
  lEdits["xRotation"               ] = ui->xRotation_cg_LE   ;
  lEdits["yRotation"               ] = ui->yRotation_cg_LE   ;
  lEdits["zRotation"               ] = ui->zRotation_cg_LE   ;
  lEdits["xPosCorrection"          ] = ui->xPosition_fg_LE   ;
  lEdits["xPositionError"          ] = ui->xPosition_e_fg_LE ;
  lEdits["yPosCorrection"          ] = ui->yPosition_fg_LE   ;
  lEdits["yPositionError"          ] = ui->yPosition_e_fg_LE ;
  lEdits["zPosCorrection"          ] = ui->zPosition_fg_LE   ;
  lEdits["zPositionError"          ] = ui->zPosition_e_fg_LE ;
  lEdits["xRotationCorrection"     ] = ui->xRotation_fg_LE   ;
  lEdits["xRotationCorrectionError"] = ui->xRotation_e_fg_LE ;
  lEdits["yRotationCorrection"     ] = ui->yRotation_fg_LE   ;
  lEdits["yRotationCorrectionError"] = ui->yRotation_e_fg_LE ;
  lEdits["zRotationCorrection"     ] = ui->zRotation_fg_LE   ;
  lEdits["zRotationCorrectionError"] = ui->zRotation_e_fg_LE ;

  for(lEditsDef::iterator lEi=lEdits.begin(); lEi!=lEdits.end(); ++lEi)
  {
    ((customLineEdit*)lEi->second)->setRange        (-1000.,1000.,4                                  ) ;
    ((customLineEdit*)lEi->second)->setInnerGeometry(((customLineEdit*)lEi->second)->geometry()      ) ;
    ((customLineEdit*)lEi->second)->textIsAttribute (false                                           ) ;
    ((customLineEdit*)lEi->second)->setText         (lEi->first,
                                                     boost::algorithm::trim_right_copy(
                                                     boost::algorithm::trim_left_copy (
                                                     detector_->keyValue_[lEi->first].toStdString()))) ;
    ((customLineEdit*)lEi->second)->assignXmlElement(theDetector->getNode()                          ) ;
  }


}

//================================================================================
void detectorW::detectorToggled(bool inUse)
{
   emit(detectorIsToggled(detector_->getStationId(), detector_->getDetectorId(), inUse)) ;

   ui->detectorGeoROCGB->setEnabled(inUse) ;

   detector_->setUsed(inUse);
}

//================================================================================
void detectorW::setXPosition(double value)
{
  ss_.str(""); ss_ << value;
  ui->xPosition_cg_LE->setText(QString(ss_.str().c_str()));
}

//================================================================================
void detectorW::setYPosition(double value)
{
  ss_.str(""); ss_ << value;
  ui->yPosition_cg_LE->setText(QString(ss_.str().c_str()));
}

//================================================================================
void detectorW::setZPosition(double value)
{
  ss_.str(""); ss_ << value;
  ui->zPosition_cg_LE->setText(QString(ss_.str().c_str()));
}

//================================================================================
void detectorW::setXPositionCorrection(double correction)
{
  ss_.str(""); ss_ << correction;
  ui->xPosition_fg_LE->setText(QString(ss_.str().c_str()));
}
//================================================================================
void detectorW::setYPositionCorrection(double correction)
{
  ss_.str(""); ss_ << correction;
  ui->yPosition_fg_LE->setText(QString(ss_.str().c_str()));
}

//================================================================================
void detectorW::setZPositionCorrection(double correction)
{
  ss_.str(""); ss_ << correction;
  ui->zPosition_fg_LE->setText(QString(ss_.str().c_str()));
}

//================================================================================
void detectorW::setXRotation(double value)
{
  ss_.str(""); ss_ << value;
  ui->xRotation_cg_LE->setText(QString(ss_.str().c_str()));
}

//================================================================================
void detectorW::setYRotation(double value)
{
  ss_.str(""); ss_ << value;
  ui->yRotation_cg_LE->setText(QString(ss_.str().c_str()));
}

//================================================================================
void detectorW::setZRotation(double value)
{
  ss_.str(""); ss_ << value;
  ui->zRotation_cg_LE->setText(QString(ss_.str().c_str()));
}

//================================================================================
void detectorW::setXRotationCorrection(double correction)
{
  ss_.str(""); ss_ << correction;
  ui->xRotation_fg_LE->setText(QString(ss_.str().c_str()));
}

//================================================================================
void detectorW::setYRotationCorrection(double correction)
{
  ss_.str(""); ss_ << correction;
  ui->yRotation_fg_LE->setText(QString(ss_.str().c_str()));
}

//================================================================================
void detectorW::setZRotationCorrection(double correction)
{
  ss_.str(""); ss_ << correction;
  ui->zRotation_fg_LE->setText(QString(ss_.str().c_str()));
}

//================================================================================
void detectorW::on_addROCPB_clicked()
{
  STDLINE("Adding new ROC",ACGreen) ;
  serialIdDialog * dialog = new serialIdDialog() ;
  dialog->setTitle    (QString("Add new ROC") );
  dialog->setTabIdList(detector_->getROCList()) ;
  dialog->setUseToAdd (true                   ) ;
  dialog->exec        (                       ) ;
  if( !dialog->hasValidData() ) return ;

  ss_.str(""); ss_ << "Adding tab number "
      << dialog->getSerial()
      << " for ROC id "
      << dialog->getId()
      << " in detector "
      << detector_->getDetectorId()
      << " at detector tab "
      << detector_->getDetectorSerial() ;
  STDLINE(ss_.str(),ACWhite);

  detector_->addROC(dialog->getId(), dialog->getSerial()); // Add the ROCID to DOM first

  emit placeROCTab(detector_->getXmlROCById(dialog->getId())) ;
}

//================================================================================
void detectorW::on_remROCPB_clicked()
{
  serialIdDialog * dialog = new serialIdDialog() ;
  dialog->setTitle    (QString("Remove ROC")  ) ;
  detector_->dumpROCs() ;
  dialog->setTabIdList(detector_->getROCList()) ;
  dialog->setUseToAdd (false                  ) ;
  dialog->exec        (                       ) ;
  if( !dialog->hasValidData() ) return ;

  int ROCId     = 0 ;
  int ROCSerial = 0 ;

  if( dialog->idIsAvailable() )
  {
    ROCId     = dialog->getId() ;
    ROCSerial = detector_->getXmlROCById(ROCId)->getROCSerial() ;
  }
  else
  {
    ROCSerial = dialog->getSerial() ;
    ROCId     = detector_->getXmlROCBySerial(ROCSerial)->getROCId() ;
  }

  ss_.str(""); ss_ << "Removing tab " << ROCSerial << " for id " << ROCId ;
  STDLINE(ss_.str(),ACWhite) ;

  detector_->getXmlROCBySerial(ROCSerial)->remROC(); // Remove from DOM first

  emit removeROCTab(detector_->getStationId(),
                    detector_->getDetectorId(),
                    ROCSerial);
}
