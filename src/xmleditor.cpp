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
#include <string>
#include <cstdlib>
#include <stdio.h>
#include <time.h>

#include "xmleditor.h"
#include "ui_xmleditor.h"
#include "MessageTools.h"
#include "xmlcreationdialog.h"
#include "serialIdDialog.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

using namespace std ;

//===========================================================================
XMLEditor::XMLEditor(QWidget *parent) : QWidget(parent), ui(new Ui::XMLEditor)
{
    isActive_       = false ;
    alreadyLaidOut_ = false ;
    theXMLParser_   = NULL ;

    ui->setupUi(this);

    this->setGeometry(
                      this                    ->geometry().x()           ,
                      this                    ->geometry().y()           ,
                      ui  ->stationsGB        ->geometry().width () + 10 ,
                      ui  ->geometryControlsGB->geometry().height()      +
                      ui  ->stationsGB        ->geometry().height() + 75
                     ) ;

    QMessageBox msgBox(
                       QMessageBox::Question, tr("Warning"),
                       tr("Choose an already existing geometry file or create a new one?"), 0, this
                      );
    QPushButton *useExisting = msgBox.addButton(tr("Select an already existing one"), QMessageBox::AcceptRole);
    QPushButton *createOne   = msgBox.addButton(tr("Create a new one"              ), QMessageBox::AcceptRole);
    QPushButton *abort       = msgBox.addButton(tr("Abort"                         ), QMessageBox::RejectRole);

    msgBox.exec();

    QApplication::processEvents(QEventLoop::AllEvents);

    if (     msgBox.clickedButton() == abort      )
    {
        STDLINE("Editing of an XML document has been aborted",ACPurple) ;
        return ;
    }
    else if (msgBox.clickedButton() == useExisting)
    {
        STDLINE("Editing an already existing XML document",ACPurple) ;

        if(!this->loadGeometryFile())
            return;
    }
    else if (msgBox.clickedButton() == createOne  )
    {
        STDLINE("Editing a new XML document",ACPurple) ;
        xmlCreationDialog * cd = new xmlCreationDialog(this) ;
        cd->exec() ;
        if( !cd->state() ) return ;
        theXMLParser_ = new xmlParser(
                                      cd->getStations(),
                                      cd->getDetectors(),
                                      cd->getROCs()
                                     ) ;
    }
    else
    {
        STDLINE("Unknown answer",ACPurple) ; return ;
    }

    this->layout() ;

    connect(ui->gCalibrationFitFunctionCB, SIGNAL(textChanged(std::string, QString&)),
            this,                          SLOT  (textChanged(std::string, QString&))) ;

    isActive_ = true ;
}

//===========================================================================
void XMLEditor::textChanged(std::string from, QString & to)
{
    STDLINE(from            ,ACWhite) ;
    STDLINE(to.toStdString(),ACWhite) ;
}

//===========================================================================
XMLEditor::XMLEditor(QString fileName) : ui(new Ui::XMLEditor)
{
    isActive_       = false ;
    alreadyLaidOut_ = false ;
    theXMLParser_   = NULL ;

    ui->setupUi(this);

    this->setGeometry(this->geometry().x(),
                      this->geometry().y(),
                      ui->stationsGB->geometry().width ()+ 10,
                      ui->stationsGB->geometry().height()+ 20) ;

    STDLINE(fileName.toStdString(),ACRed);
    QFile file(fileName) ;

    theXMLParser_ = new xmlParser(file) ;

    this->layout() ;

    this->reorderStationTabs() ;

    this->setGeometryFileName(fileName) ;

    isActive_ = true ;
}

//===========================================================================
XMLEditor::~XMLEditor()
{
    STDLINE("Dtor...",ACRed) ;
    delete ui;
}

//===========================================================================
void XMLEditor::setGeometryFileName(QString fileName)
{
    ui->geometryFileLE->setText   (fileName);
    ui->geometryFileLE->setToolTip(fileName);
}

//===========================================================================
bool XMLEditor::loadGeometryFile(void)
{
    std::string path = std::string(getenv("Monicelli_XML_Dir")) ;
    QString path_    = QString(path.c_str()) ;
    QString fileName = QFileDialog::getOpenFileName(this,"Geometry files",path_,"XML files (*.xml)");
    if (fileName.isEmpty())  return false;

    if( theXMLParser_ ) delete theXMLParser_ ;

    QFile file(fileName) ;
    theXMLParser_ = new xmlParser(file) ;

    ui->geometryFileLE->setText   (fileName);
    ui->geometryFileLE->setToolTip(fileName);

    this->layout() ;

    this->reorderStationTabs() ;
    return true;
}

//===========================================================================
void XMLEditor::layout()
{ 
    if( alreadyLaidOut_ ) return ;

    alreadyLaidOut_     = true                       ;
    theStationTW_       = new stationTabWidget(this) ;
    QRect theStationsGB = ui->stationsGB->geometry() ;


    ui->fileDescriptionTA        ->setInnerGeometry(ui->fileDescriptionTA->geometry()                              );
    ui->fileDescriptionTA        ->textIsAttribute (true                                                           );
    ui->fileDescriptionTA        ->setText         ("description",
                                                    theXMLParser_->getDesctiption()                                );
    ui->fileDescriptionTA        ->assignXmlElement(theXMLParser_->getNode()                                       );

    ui->runLE                    ->setInnerGeometry(ui->runLE->geometry()                                          );
    ui->runLE                    ->textIsAttribute (true                                                           );
    ui->runLE                    ->setText         ("run",
                                                    theXMLParser_->getRun ()                                       );
    ui->runLE                    ->assignXmlElement(theXMLParser_->getNode()                                       );

    ui->dateLE                   ->setInnerGeometry(ui->dateLE->geometry()                                         );
    ui->dateLE                   ->textIsAttribute (true                                                           );
    ui->dateLE                   ->setText         ("date",
                                                    theXMLParser_->getDate()                                       );
    ui->dateLE                   ->assignXmlElement(theXMLParser_->getNode()                                       );

    ui->gCalibrationFitFunctionCB->textIsAttribute (true                                                           );
    ui->gCalibrationFitFunctionCB->assignXmlElement(theXMLParser_->getChildNode()                                  );
    ui->gCalibrationFitFunctionCB->addItem         ("tanh"                                                         );
    ui->gCalibrationFitFunctionCB->addItem         ("linear"                                                       );
    ui->gCalibrationFitFunctionCB->addItem         ("parabolic"                                                    );
    ui->gCalibrationFitFunctionCB->setCurrentIndex ("gCalibrationFitFunction",
                                                    QString((theXMLParser_->getgCalibrationFitFunction()).c_str()) );
    ui->gCalibrationFitFunctionCB->setInnerGeometry(ui->gCalibrationFitFunctionCB->geometry()                      );
    
    ui->gDUTFitFunctionCB        ->textIsAttribute (true                                                           );
    ui->gDUTFitFunctionCB        ->assignXmlElement(theXMLParser_->getChildNode()                                  );
    ui->gDUTFitFunctionCB        ->addItem         ("linear"                                                       );
    ui->gDUTFitFunctionCB        ->addItem         ("tanh"                                                         );
    ui->gDUTFitFunctionCB        ->addItem         ("parabolic"                                                    );
    ui->gDUTFitFunctionCB        ->setCurrentIndex ("gDUTFitFunction",
                                                    QString((theXMLParser_->getgDUTFitFunction()).c_str())         );
    ui->gDUTFitFunctionCB        ->setInnerGeometry(ui->gDUTFitFunctionCB->geometry()                              );

    theStationTW_                ->setGeometry     (ui->stationsGB->geometry()                                     );
    theStationTW_                ->setGeometry     (theStationsGB.x()     ,
                                                    theStationsGB.y()     ,
                                                    theStationsGB.width() ,
                                                    theStationsGB.height()                                         );


    xmlParser::xmlStationsDef stations = theXMLParser_->getXmlStations() ;
    for(xmlParser::xmlStationsDef::iterator it =stations.begin(); it!=stations.end(); it++)
        this->placeStation(it->second) ;
    theStationTW_->setCurrentIndex(0);
}

//===========================================================================
void XMLEditor::reorderStationTabs()
{
    for(int tab=0; tab<theStationTW_->count(); ++tab)
    {
        std::string tabLabel = theStationTW_->tabText(tab).toStdString() ;
        int stationSerial    = atoi(tabLabel.substr(8).c_str()) ;
        int tabPosition      = theXMLParser_->getStationTabPosition(theXMLParser_->getStationId(stationSerial)) ;

        theStationTW_->getTabBar()->moveTab(tab,tabPosition) ;
        //    ss_.str(""); ss_<<"Moved tab from " << tab << " to "<<tabPosition; STDLINE(ss_.str(),ACWhite) ;
//        theStationTW_->setCurrentIndex(tabPosition) ;
    }
    theStationTW_->setCurrentIndex(0) ;
}

//===========================================================================
void XMLEditor::reorderDetectorTabs(int stationId)
{
    stationWMapDef::iterator tabIt = stationWMap_.find(stationId) ;
    if( tabIt == stationWMap_.end() ) return ;

    for(int tab=0; tab<tabIt->second->count(); ++tab)
    {

        std::string tabLabel = tabIt->second->tabText(tab).toStdString() ;
        int stationSerial    = theXMLParser_->getStationSerial(stationId) ;
        int detectorSerial   = atoi(tabLabel.substr(8).c_str()) ;
        int detectorId       = theXMLParser_->getDetectorId(stationSerial, detectorSerial) ;
        int tabPosition      = theXMLParser_->getDetectorTabPosition(stationId,detectorId) ;

        tabIt->second->getTabBar()->moveTab(tab,tabPosition) ;
        tabIt->second->setCurrentIndex(tabPosition) ;
    }
}

//===========================================================================
void XMLEditor::placeStation(xmlStation * theXmlStation)
{
    QRect xmlEditorGBGeo = ui->xmlEditorGB->geometry() ;

    // Add station-specific components to the current station-tab widget
    QFrame * detTabFrame = new QFrame(this) ;
    theStationGB_        = new stationGB(detTabFrame) ;
    theDetectorTW_       = new detectorTabWidget(detTabFrame) ;
    stationWMap_[theXmlStation->getStationId()] = theDetectorTW_ ;
    theStationGB_ ->initialize(theXmlStation) ;
    theDetectorTW_->setEnabled(theXmlStation->isEnabled());

    connect(theStationGB_, SIGNAL(stationIsToggled   (int, bool)),
            this,          SLOT  (toggleStationWidget(int, bool))) ;

    connect(theStationGB_, SIGNAL(placeDetectorTab   (xmlDetector *)),
            this,          SLOT  (placeDetectorTab   (xmlDetector *))) ;

    connect(theStationGB_, SIGNAL(removeDetectorTab  (int, int)),
            this,          SLOT  (removeDetectorTab  (int, int))) ;

    // Adjust geometries
    QRect theDetectorTWGeo  = theDetectorTW_->geometry() ;
    int theDetectorTWTop    = theDetectorTWGeo.y() + theStationGB_->geometry().height() ;
    theStationGB_ ->setGeometry(theStationGB_->geometry().x(),
                                theStationGB_->geometry().y(),
                                theStationGB_->geometry().width(),
                                theStationGB_->geometry().height()) ;
    theDetectorTW_->setGeometry(xmlEditorGBGeo.x(),
                                theDetectorTWTop,
                                theDetectorTWGeo.width(),
                                theDetectorTWGeo.height()) ;

    // Add a tab for this station
    ss_.str(""); ss_ << "Station " << theXmlStation->getStationSerial() ;
    theStationTW_->addTab((QWidget*)detTabFrame,QString(ss_.str().c_str())) ;
//    theStationTW_->setCurrentIndex(theStationTW_->count()-1);
    theStationTW_->setNode(theXmlStation->getStationSerial(), theXmlStation->getNode());
//    theStationTW_->setCurrentIndex(1);

    xmlStation::xmlDetectorsDef detectors = theXmlStation->getXmlDetectors() ;
    for(xmlStation::xmlDetectorsDef::iterator it =detectors.begin(); it!=detectors.end(); it++)
    {
        this->placeDetector(it->second) ;
    }

    this->reorderDetectorTabs(theXmlStation->getStationId()) ;
    theStationTW_ ->setCurrentIndex(0);
    theDetectorTW_->setCurrentIndex(0);
}

//===========================================================================
void XMLEditor::toggleStationWidget(int id, bool inUse)
{
    stationWMap_[id]->setEnabled(inUse);
}

//===========================================================================
void XMLEditor::placeDetector(xmlDetector * theXmlDetector)
{
    QFrame * detectorFrame = new QFrame(this) ;
    theDetectorW_          = new detectorW(detectorFrame) ;
//    QRect geo              = stationWMap_[theXmlDetector->getStationId()]->geometry() ;

    ss_.str(""); ss_ << "Detector "
                     <<  theXmlDetector->getDetectorSerial() ;
    stationWMap_[theXmlDetector->getStationId()]->addTab         (detectorFrame,
                                                                  QString(ss_.str().c_str())) ;
    stationWMap_[theXmlDetector->getStationId()]->setCurrentIndex(stationWMap_.size()-1     ) ;
    stationWMap_[theXmlDetector->getStationId()]->setCurrentIndex(1                         ) ;
    ss_.str("") ;
    ss_ << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
           "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
           "p, li { white-space: pre-wrap; }\n"
           "</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
           "<table border=\"0\" style=\"-qt-table-type: root; margin-top:4px; margin-bottom:4px; margin-left:4px; margin-right:4px;\">\n"
           "<tr>\n"
           "<td style=\"border: none;\">\n"
           "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
           "<span style=\" font-size:18pt; font-weight:600; text-decoration: underline; color:#0000ff;\">"
           "Detector serial number ("                                      //
        << theXmlDetector->getDetectorSerial()                             // text
        << ")"                                                             //
           "</span></p>\n"
           "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p>\n"
           "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
           "The detector order in the xml file can be changed by dragging" // text
           "</p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
           "and dropping this tab to another location."                    // text
           "</p></td></tr></table></body></html>" ;
    stationWMap_[theXmlDetector->getStationId()]->setToolTip(QApplication::translate("detectorW",ss_.str().c_str(), 0, QApplication::UnicodeUTF8));

    theROCTW_ = new ROCTabWidget(theDetectorW_->getROCTabWidgetPlaceHolder()) ;

    theROCTW_->setGeometry(0,
                           0,
                           theDetectorW_->getROCTabWidgetPlaceHolder()->geometry().width(),
                           theDetectorW_->getROCTabWidgetPlaceHolder()->geometry().height()) ;

    detectorWMap_[theXmlDetector->getStationId()][theXmlDetector->getDetectorId()] = theDetectorW_;
    detROCsWMap_ [theXmlDetector->getStationId()][theXmlDetector->getDetectorId()] = theROCTW_    ;

    theDetectorW_->initialize(theXmlDetector) ;

    connect(theDetectorW_, SIGNAL(placeROCTab (xmlROC *)),
            this,          SLOT  (placeROCTab (xmlROC *))) ;

    connect(theDetectorW_, SIGNAL(removeROCTab(int, int, int)),
            this,          SLOT  (removeROCTab(int, int, int))) ;

    ss_.str(""); ss_ << "Added tab "
                     << theXmlDetector->getDetectorSerial()
                     << " with id "
                     << theXmlDetector->getDetectorId()
                     << " at station Id "
                     << theXmlDetector->getStationId() ;
    //  STDLINE(ss_.str(),ACLightGreen) ;

    theDetectorTW_->setNode(theXmlDetector->getDetectorSerial(), theXmlDetector->getNode());

    xmlDetector::xmlROCDef rocs = theXmlDetector->getXmlROCs() ;

    for(xmlDetector::xmlROCDef::iterator it =rocs.begin();
        it!=rocs.end(); ++it)
    {
        this->placeROC(it->second) ;
    }

}

//===========================================================================
void XMLEditor::setXPosition(int    stationId,
                             int    detectorId,
                             double value)
{
    detectorWMap_[stationId][detectorId]->setXPosition(value);
}

//===========================================================================
void XMLEditor::setYPosition(int    stationId,
                             int    detectorId,
                             double value)
{
    detectorWMap_[stationId][detectorId]->setYPosition(value);
}

//===========================================================================
void XMLEditor::setZPosition(int    stationId,
                             int    detectorId,
                             double value)
{
    detectorWMap_[stationId][detectorId]->setZPosition(value);
}

//===========================================================================
void XMLEditor::setXPositionCorrection(int    stationId,
                                       int    detectorId,
                                       double correction)
{
    detectorWMap_[stationId][detectorId]->setXPositionCorrection(correction);
}

//===========================================================================
void XMLEditor::setYPositionCorrection(int    stationId,
                                       int    detectorId,
                                       double correction)
{
    detectorWMap_[stationId][detectorId]->setYPositionCorrection(correction);
}

//===========================================================================
void XMLEditor::setZPositionCorrection(int    stationId,
                                       int    detectorId,
                                       double correction)
{
    detectorWMap_[stationId][detectorId]->setZPositionCorrection(correction);
}

//===========================================================================
void XMLEditor::setXRotation(int    stationId,
                             int    detectorId,
                             double value)
{
    detectorWMap_[stationId][detectorId]->setXRotation(value);
}

//===========================================================================
void XMLEditor::setYRotation(int    stationId,
                             int    detectorId,
                             double value)
{
    detectorWMap_[stationId][detectorId]->setYRotation(value);
}

//===========================================================================
void XMLEditor::setZRotation(int    stationId,
                             int    detectorId,
                             double value)
{
    detectorWMap_[stationId][detectorId]->setZRotation(value);
}

//===========================================================================
void XMLEditor::setXRotationCorrection(int    stationId,
                                       int    detectorId,
                                       double correction)
{
    detectorWMap_[stationId][detectorId]->setXRotationCorrection(correction);
}

//===========================================================================
void XMLEditor::setYRotationCorrection(int    stationId,
                                       int    detectorId,
                                       double correction)
{
    detectorWMap_[stationId][detectorId]->setYRotationCorrection(correction);
}

//===========================================================================
void XMLEditor::setZRotationCorrection(int    stationId,
                                       int    detectorId,
                                       double correction)
{
    detectorWMap_[stationId][detectorId]->setZRotationCorrection(correction);
}

//===========================================================================
void XMLEditor::placeROCTab(xmlROC * theXmlROC)
{
    this->placeROC(theXmlROC);
}

//===========================================================================
void XMLEditor::placeROC(xmlROC * theXmlROC)
{
    QFrame * ROCFrame = new QFrame(theDetectorW_) ;
    theROCW_          = new ROCWidget(ROCFrame) ;

    connect(theROCW_, SIGNAL(addNewTab(ROCWidget *,xmlROC*,QString,int,double)),
            this,     SLOT  (addNewTab(ROCWidget *,xmlROC*,QString,int,double))) ;

    theROCW_->initialize(theXmlROC) ;

    ss_.str(""); ss_ << "ROC " << theXmlROC->getROCSerial() ;
    detROCsWMap_[theXmlROC->getStationId()][theXmlROC->getDetectorId()]->addTab(ROCFrame,QString(ss_.str().c_str())) ;
    ss_.str("") ;
    ss_ << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        << "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        << "p, li { white-space: pre-wrap; }\n"
        << "</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
        << "<table border=\"0\" style=\"-qt-table-type: root; margin-top:4px; margin-bottom:4px; margin-left:4px; margin-right:4px;\">\n"
        << "<tr>\n"
        << "<td style=\"border: none;\">\n"
        << "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:18pt; font-weight:600; text-decoration: underline; color:#0000ff;\">ROC serial number</span></p>\n"
        << "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p>\n"
        << "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; te"
        <<                   "xt-indent:0px;\">Unlike stations and detectors, ROC positions in the xml files CANNOT be changed </p></td></tr></table></body></html>" ;
    detROCsWMap_[theXmlROC->getStationId()][theXmlROC->getDetectorId()]->setToolTip(QApplication::translate("detectorW",ss_.str().c_str(), 0, QApplication::UnicodeUTF8));

    for(xmlROC::nonStandardPitchDef::iterator it =theXmlROC->nonStandardRowPitch_.begin();
        it!=theXmlROC->nonStandardRowPitch_.end();
        ++it)
    {
        ss_.str(""); ss_ << it->first;
        nonStandardPitch * nsrp = new nonStandardPitch(this,"row",it->first) ;
        nsrp->initialize(theXmlROC);
        theROCW_->getNonStandardRowTW()->addTab((QWidget*)nsrp,QString(ss_.str().c_str())) ;
    }

    for(xmlROC::nonStandardPitchDef::iterator it =theXmlROC->nonStandardColPitch_.begin();
        it!=theXmlROC->nonStandardColPitch_.end();
        ++it)
    {
        ss_.str(""); ss_ << it->first;
        nonStandardPitch * nscp = new nonStandardPitch(this,"col",it->first) ;
        nscp->initialize(theXmlROC);
        theROCW_->getNonStandardColTW()->addTab((QWidget*)nscp,QString(ss_.str().c_str())) ;
    }
}

//===========================================================================
bool XMLEditor::stationInUse(int stationId)
{
    for(int stationSerial=0; stationSerial<(int)theXMLParser_->getNumberOfStations(); ++stationSerial)
    {
        if( stationId == theXMLParser_->getXmlStationBySerial(stationSerial)->getStationId() ) return true ;
    }

    return false ;
}

//===========================================================================
void XMLEditor::on_savePB_clicked()
{
    QString path;
    if(ui->geometryFileLE->text().contains(".xml"))
        path = ui->geometryFileLE->text();
    else
        path = QString(getenv("MonicelliDir")) + "/xml/geometryTest.xml";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Geometry XML File"), path,tr("XML files (*.xml)"));
    if (fileName.isEmpty())  return ;

    //  theXMLParser_->dumpHeaders() ;

    theXMLParser_->save(fileName.toStdString()) ;
    ui->geometryFileLE->setText(fileName);
}

//===========================================================================
void XMLEditor::addNewTab(ROCWidget * theROCW,
                          xmlROC    * ROC    ,
                          QString     type   ,
                          int         rowCol ,
                          double      pitch   )
{
    ROC->addNonStandardPitch(ROC,type,rowCol,pitch) ; // Update DOM first

    ss_.str(""); ss_ << rowCol;
    nonStandardPitch * nsrp = new nonStandardPitch(this,type.toStdString(),rowCol) ;
    nsrp->initialize(ROC);

    if( type.toStdString() == "row")
    {
        theROCW->getNonStandardRowTW()->addTab((QWidget*)nsrp,QString(ss_.str().c_str())) ;
    }
    if( type.toStdString() == "col")
    {
        theROCW->getNonStandardColTW()->addTab((QWidget*)nsrp,QString(ss_.str().c_str())) ;
    }

}

//===========================================================================
void XMLEditor::on_addNewStationPB_clicked()
{
    serialIdDialog * dialog = new serialIdDialog() ;
    dialog->setTitle    (QString("Add new station")     );
    dialog->setTabIdList(theXMLParser_->getStationList()) ;
    dialog->setUseToAdd (true                           ) ;
    dialog->exec        (                               ) ;
    if( !dialog->hasValidData() ) return ;

    ss_.str(""); ss_ << "Adding tab number "
                     << dialog->getSerial()
                     << " for station id "
                     << dialog->getId() ;
    STDLINE(ss_.str(),ACWhite);

    theXMLParser_->addStation(dialog->getId(), dialog->getSerial() ); // Add to DOM first

    this->placeStation(theXMLParser_->getXmlStationById(dialog->getId())) ;

    delete dialog ;
}

//===========================================================================
void XMLEditor::on_removeStationPB_clicked()
{
    serialIdDialog * dialog = new serialIdDialog() ;
    dialog->setTitle    (QString("Remove station")     );
    dialog->setTabIdList(theXMLParser_->getStationList()) ;
    dialog->setUseToAdd (false                          ) ;
    dialog->exec        (                               ) ;
    if( !dialog->hasValidData() ) return ;

    int stationId     = 0 ;
    int stationSerial = 0 ;

    if( dialog->idIsAvailable() )
    {
        stationId     = dialog->getId() ;
        stationSerial = theXMLParser_->getXmlStationById(stationId)->getStationSerial() ;
    }
    else
    {
        stationSerial = dialog->getSerial() ;
        stationId     = theXMLParser_->getXmlStationBySerial(stationSerial)->getStationId() ;
    }

    ss_.str(""); ss_ << "Removing tab " << stationSerial << " for id " << stationId ;
    STDLINE(ss_.str(),ACWhite) ;

    theXMLParser_->remStation(stationSerial); // Remove from DOM first

    theStationTW_->removeTab (stationSerial) ;
}

//===========================================================================
void XMLEditor::placeDetectorTab(xmlDetector * theXmlDetector)
{
    this->placeDetector(theXmlDetector);
}

//===========================================================================
void XMLEditor::removeDetectorTab(int stationId, int detectorSerial)
{
    stationWMap_[stationId]->remTab(detectorSerial) ;
}

//===========================================================================
void XMLEditor::removeROCTab(int stationId,
                             int detectorId,
                             int ROCSerial)
{
    detROCsWMap_[stationId][detectorId]->remTab(ROCSerial) ;
}
//===========================================================================
void XMLEditor::on_showHierachyPB_clicked()
{
    STDLINE("Sorry, not implemented yet",ACWhite) ;
}

//===========================================================================
void XMLEditor::on_chooseDatePB_clicked()
{
    time_t rawtime ;
    struct tm * timeinfo ;
    time( &rawtime ) ;
    timeinfo = localtime( &rawtime ) ;
    std::string today = asctime(timeinfo) ;
    ui->dateLE->setText(QString(today.c_str())) ;
}

//===========================================================================
void XMLEditor::on_resetCalibFitFuncPB_clicked()
{
    STDLINE("Not yet implemented...",ACRed) ;
}

//===========================================================================
void XMLEditor::on_enableALLPB_clicked()
{
    bool enable = true ;
    if( ui->enableALLPB->text() == QString("Enable ALL") )
    {
        ui->enableALLPB->setText(QString("Disable ALL")) ;
        ui->enableALLPB->setPalette(QColor(255,0,0)) ;        
        STDLINE("Enable ALL",ACGreen) ;
        enable = true ;
    }
    else
    {
        ui->enableALLPB->setText(QString("Enable ALL")) ;
        ui->enableALLPB->setPalette(QColor(117,201,122)) ;
        STDLINE("Disable ALL",ACRed) ;
        enable = false ;
    }

//    for(stationGBDef::iterator itSt =stationUi_.begin(); itSt!=stationUi_.end(); itSt++)
//    {
//        itSt->setEnabled(enable) ;
//        ss_.str(""); ss_ << "Enabling station " << itSt->first ;
//        STDLINE(ss_.str(),ACGreen) ;
//    }
    
}

//===========================================================================
void XMLEditor::on_enableAllStationsPB_clicked()
{
    bool enable = true ;
    if( ui->enableALLPB->text() == QString("Enable ALL stations") )
    {
        ui->enableALLPB->setText(QString("Disable ALL stations")) ;
        ui->enableALLPB->setPalette(QColor(255,0,0)) ;        
        STDLINE("Enabled ALL stations",ACGreen) ;
        enable = true ;
    }
    else
    {
        ui->enableALLPB->setText(QString("Enable ALL station")) ;
        ui->enableALLPB->setPalette(QColor(117,201,122)) ;
        STDLINE("Disabled ALL stations",ACRed) ;
        enable = false ;
    }
//    for(xmlParser::xmlStationsDef::iterator it =stations.begin(); it!=stations.end(); it++)
//        this->placeStation(it->second) ;
}
