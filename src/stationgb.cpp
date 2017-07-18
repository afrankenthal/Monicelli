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

#include "stationgb.h"
#include "ui_stationgb.h"
#include "MessageTools.h"
#include "serialIdDialog.h"

#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMessageBox>

//================================================================================
stationGB::stationGB(QWidget *parent) : QWidget(parent),
ui(new Ui::stationGB)
{
  ui->setupUi(this);
}

//================================================================================
stationGB::~stationGB()
{
  delete ui;
}

//================================================================================
void stationGB::initialize(xmlStation * theStation)
{
  station_      = theStation ;
  int stationId = station_->getStationId();

  connect(ui->stationInUseCB, SIGNAL(isToggled     (bool)),
          this,               SLOT  (stationToggled(bool))) ;

  ss_.str("");  ss_ << stationId ;
  ui->stationIdLE              ->textIsAttribute  (true                                                        );
  ui->stationIdLE              ->setText          ("id",ss_.str()                                              ); // Assign value from xml file
  ui->stationIdLE              ->setRange         ("\\d+"                                                      );
  ui->stationIdLE              ->assignXmlElement (theStation->getNode()                                       );
  ui->stationIdLE              ->setInnerGeometry (ui->stationIdLE->geometry()                                 );
  ui->stationIdLE              ->setExclusionList (theStation->getAllStationsId()                              );
  ui->stationIdLE              ->needsReturn      (                                                            );

  ss_.str("");  ss_ << station_->getDescription() ;
  ui->stationDescriptionTA     ->textIsAttribute  (true                                                        );
  ui->stationDescriptionTA     ->setText          ("description",ss_.str()                                     );
  ui->stationDescriptionTA     ->assignXmlElement (theStation->getNode()                                       );
  ui->stationDescriptionTA     ->setInnerGeometry (ui->stationDescriptionTA->geometry()                        );

  ui->sCalibrationFitFunctionCB->textIsAttribute  (true                                                        );
  ui->sCalibrationFitFunctionCB->assignXmlElement (theStation->getNode()                                       );
  ui->sCalibrationFitFunctionCB->addItem          ("tanh"                                                      );
  ui->sCalibrationFitFunctionCB->addItem          ("linear"                                                    );
  ui->sCalibrationFitFunctionCB->addItem          ("parabolic"                                                 );
  ui->sCalibrationFitFunctionCB->addItem          ("none"                                                      );
  ui->sCalibrationFitFunctionCB->setCurrentIndex  ("sCalibrationFitFunction",
                                                   QString((theStation->getsCalibrationFitFunction()).c_str()) );
  ui->sCalibrationFitFunctionCB->setInnerGeometry (ui->sCalibrationFitFunctionCB->geometry()                   );

  ui->daCalibrationFitFunctionCB->textIsAttribute (true                                                        );
  ui->daCalibrationFitFunctionCB->assignXmlElement(theStation->getChildNode()                                  );
  ui->daCalibrationFitFunctionCB->addItem         ("tanh"                                                      );
  ui->daCalibrationFitFunctionCB->addItem         ("linear"                                                    );
  ui->daCalibrationFitFunctionCB->addItem         ("parabolic"                                                 );
  ui->daCalibrationFitFunctionCB->addItem         ("none"                                                      );
  ui->daCalibrationFitFunctionCB->setCurrentIndex ("daCalibrationFitFunction",
                                                   QString((theStation->getdaCalibrationFitFunction()).c_str()));
  ui->daCalibrationFitFunctionCB->setInnerGeometry(ui->daCalibrationFitFunctionCB->geometry()                  );

  ui->stationInUseCB            ->setChecked      (theStation->isEnabled()                                     );
}

//================================================================================
void stationGB::stationToggled(bool inUse)
{
  emit(stationIsToggled(station_->getStationId(), inUse)) ;
  station_->setUsed(inUse);
}

//================================================================================
void stationGB::on_addDetectorPB_clicked()
{
  serialIdDialog * dialog = new serialIdDialog() ;
  dialog->setTitle    (QString("Add new detector"));
  dialog->setTabIdList(station_->getDetectorList()) ;
  dialog->setUseToAdd (true                       ) ;
  dialog->exec        (                           ) ;
  if( !dialog->hasValidData() ) return ;

  ss_.str(""); ss_ << "Adding tab number "
      << dialog->getSerial()
      << " for detector id "
      << dialog->getId()
      << " in station "
      << station_->getStationId()
      << " at station tab "
      << station_->getStationSerial() ;
  STDLINE(ss_.str(),ACWhite);

  station_->addDetector(dialog->getId(), dialog->getSerial()); // Add the detectorID to DOM first

  emit placeDetectorTab(station_->getXmlDetectorById(dialog->getId())) ;
}

//================================================================================
void stationGB::on_remDetectorPB_clicked()
{
  serialIdDialog * dialog = new serialIdDialog(   ) ;
  dialog->setTitle    (QString("Remove detector") ) ;
  station_->dumpDetectors() ;
  dialog->setTabIdList(station_->getDetectorList()) ;
  dialog->setUseToAdd (false                      ) ;
  dialog->exec        (                           ) ;
  if( !dialog->hasValidData() ) return ;

  int detectorId     = 0 ;
  int detectorSerial = 0 ;

  if( dialog->idIsAvailable() )
  {
    detectorId     = dialog->getId() ;
    detectorSerial = station_->getXmlDetectorById(detectorId)->getDetectorSerial() ;
  }
  else
  {
    detectorSerial = dialog->getSerial() ;
    detectorId     = station_->getXmlDetectorBySerial(detectorSerial)->getDetectorId() ;
  }

  ss_.str(""); ss_ << "Removing tab " << detectorSerial << " for id " << detectorId ;
  STDLINE(ss_.str(),ACWhite) ;

  station_->getXmlDetectorBySerial(detectorSerial)->remDetector(); // Remove from DOM first

  emit removeDetectorTab(station_->getStationId(),detectorSerial);
}
//================================================================================
void stationGB::setEnabled(bool enable)
{
  ui->stationInUseCB->setChecked(enable) ;
}
