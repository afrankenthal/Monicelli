/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#include <iostream>

#include "stationgb.h"
#include "ui_stationgb.h"
#include "MessageTools.h"
#include "serialIdDialog.h"

#include <QInputDialog>
#include <QMessageBox>

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
  station_ = theStation ;

  ss_.str(""); ss_ << station_->getStationId() ;
  ui->stationIdLE->textIsAttribute (true                          ) ;
  ui->stationIdLE->setText         ("id",ss_.str()                ) ; // Assign value from xml file
  ui->stationIdLE->setRange        ("\\d+"                        ) ;
  ui->stationIdLE->assignXmlElement(theStation->getNode()         ) ;
  ui->stationIdLE->setInnerGeometry(ui->stationIdLE->geometry()   ) ;
  ui->stationIdLE->setExclusionList(theStation->getAllStationsId()) ;
  ui->stationIdLE->needsReturn     (                              ) ;

  ss_.str(""); ss_ << station_->getDescription() ;
  ui->stationDescriptionTA->textIsAttribute (true                                 ) ;
  ui->stationDescriptionTA->setText("description",ss_.str()                       ) ;
  ui->stationDescriptionTA->assignXmlElement(theStation->getNode()                ) ;
  ui->stationDescriptionTA->setInnerGeometry(ui->stationDescriptionTA->geometry() ) ;

  connect(ui->stationInUseCB,       SIGNAL(isToggled     (bool)),
          this,                     SLOT  (stationToggled(bool))) ;

  ui->stationInUseCB->setChecked(theStation->isEnabled()) ;

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
