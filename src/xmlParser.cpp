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
 
#include <assert.h>

#include "geometryLoader.h"
#include <QTextStream>

#include "MessageTools.h"
#include "xmlParser.h"

//================================================================================
// Builds a DOM in memory starting from the actual content of an xml file
//
xmlParser::xmlParser(QFile & file)
{
  // Use the geometry loader to check for file syntax using xercesc components
  //STDLINE("",ACWhite) ;
  geometryLoader * theGeometryLoader_ = new geometryLoader() ;
  //STDLINE("",ACWhite) ;
  theGeometryLoader_->loadXML( file.fileName().toStdString() )  ;
  //STDLINE("",ACWhite) ;
  delete theGeometryLoader_ ;

  //STDLINE("",ACWhite) ;
  document_ = new QDomDocument( "GeometryFile" );
  //STDLINE("",ACWhite) ;

  if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
  {
    STDLINE(std::string("Could not open ")+file.fileName().toStdString(),ACRed) ;
    return;
  }

  if ( !document_->setContent( &file ) )
  {
    STDLINE(std::string("Could not access ")+file.fileName().toStdString(),ACRed) ;
    file.close();
    return;
  }

  STDLINE(std::string("Parsing ")+file.fileName().toStdString(),ACGreen) ;

  //STDLINE("",ACWhite) ;
  this->buildTelescope() ;
  //STDLINE("",ACWhite) ;

  file.close();
}

//================================================================================
// Builds a DOM in memory starting from scratch: number of inner components is
// provided through the signature of the constructor
//
xmlParser::xmlParser(int numberOfStations,
                     int numberOfDetectorsPerStation,
                     int numberOfROCsPerDetector)
{
  STDLINE("Buidling an empty DOM", ACPurple) ;
  document_ = new QDomDocument("testBeamGeometry ");

  // Create suitable header components (<?xml ...> and <!DOCTYPE ...>)
  QDomImplementation * imp = new QDomImplementation () ;
  QDomDocumentType type = imp->createDocumentType(QString("testBeamGeometry"),QString("MTEST telescope geometry definition file"),QString("../dtd/geometry.dtd")) ;
  document_ = new QDomDocument(type);
  QDomProcessingInstruction ins = document_->createProcessingInstruction("xml","version=\"1.0\" encoding=\"iso-8859-1\"") ;
  document_->appendChild(ins) ;

  document_->appendChild(document_->createComment("   Author  : D. Menasce, S. Terzo   ")) ;
  document_->appendChild(document_->createComment("   @version: 1.0                    ")) ;

  QDomElement node = document_->createElement("testBeamGeometry") ;

  node.setAttribute("run", 26);
  node.setAttribute("date",QString("12/3/1956")) ;
  document_->appendChild(node) ;

  QDomElement stations = document_->createElement("stations") ;
  stations.setAttribute("inUse",numberOfStations) ;
  node.appendChild(stations) ;

  xmlUtils::addStations(stations, numberOfStations, numberOfDetectorsPerStation, numberOfROCsPerDetector) ;

  this->buildTelescope() ;
}

//================================================================================
void xmlParser::addStation(int stationId, int stationSerial)
{
  QDomNode stations         = document_->elementsByTagName("stations"  ).at(0) ;
  QDomElement stationNode   = document_->createElement    ("station"   ) ;
  QDomElement detectorsNode = document_->createElement    ("detectors" ) ;
  stationNode  .setAttribute("id",          stationId    ) ;
  stationNode  .setAttribute("serial",      stationSerial) ;
  stationNode  .setAttribute("used",        "no"         ) ;
  stationNode  .setAttribute("description", ""           ) ;
  detectorsNode.setAttribute("inUse",       1            ) ;

  stationNode  .appendChild(detectorsNode) ;
  stations     .appendChild(stationNode  ) ;

  xmlStations_[stationId] = new xmlStation(stationNode) ;

  ss_.str(""); ss_ << "Added station serial "
                   << stationSerial
                   << " to xmlStations_ with Id "
                   << stationId ;
  STDLINE(ss_.str(),ACLightBlue) ;
}

//================================================================================
int xmlParser::getStationSerial(int stationId)
{
  return this->getXmlStationById(stationId)->getStationSerial() ;
}

//================================================================================
int xmlParser::getStationId(int stationSerial)
{
  return this->getXmlStationBySerial(stationSerial)->getStationId() ;
}

//================================================================================
int xmlParser::getDetectorId(int stationSerial, int detectorSerial)
{
  return this->getXmlDetectorBySerial(stationSerial, detectorSerial)->getDetectorId() ;
}

//================================================================================
int xmlParser::getDetectorSerial(int stationId, int detectorId)
{
  return this->getXmlStationById(stationId)->getXmlDetectorById(detectorId)->getDetectorSerial() ;
}

//================================================================================
// Reinstates a DOM in memory gathering data from file
void xmlParser::buildTelescope(void)
{
  xmlStations_.clear() ;

  rootNode_ = document_->elementsByTagName("testBeamGeometry").at(0) ;

  QDomElement rootElement = rootNode_.toElement() ;

  description_ = rootElement.attribute("description").toStdString() ;
  run_         = rootElement.attribute("run"        ).toStdString() ;
  date_        = rootElement.attribute("date"       ).toStdString() ;

  QDomNodeList stations = document_->elementsByTagName("station") ;

  for(int stationSerial=0; stationSerial<stations.size(); ++stationSerial)
  {
    QDomNode stationNode           = stations.at(stationSerial) ;
    int stationId                  = stationNode.toElement().attribute("id").toInt() ;
    xmlStations_[stationId]        = new xmlStation(stationNode) ;
    stationTabPosition_[stationId] = stationSerial ;
  }

  //this->dumpStations() ;
}

//================================================================================
void xmlUtils::addStations(QDomElement & stations, int numberOfStations,
                                                   int numberOfDetectorsPerStation,
                                                   int numberOfROCsPerDetector)
{
  for(int stationSerial=0; stationSerial<numberOfStations; ++stationSerial)
  {
    int stationId = stationSerial ;
    xmlUtils::addStation(stations,
                         stationId,
                         stationSerial,
                         numberOfDetectorsPerStation,
                         numberOfROCsPerDetector) ;
  }
}

//================================================================================
void xmlUtils::addStation(QDomElement & stations, int stationId,
                                                  int stationSerial,
                                                  int numberOfDetectorsPerStation,
                                                  int numberOfROCsPerDetector)
{
   QDomElement stationNode = stations.ownerDocument().createElement("station") ;
   stationNode.setAttribute("id",           stationId       ) ;
   stationNode.setAttribute("serial",       stationSerial   ) ;
   stationNode.setAttribute("used",        "no"             ) ;
   stationNode.setAttribute("description", ""               ) ;
   stations.appendChild(stationNode) ;

   QDomElement detectorsNode = stations.ownerDocument().createElement("detectors") ;
   detectorsNode.setAttribute("inUse",numberOfDetectorsPerStation) ;
   stationNode.appendChild(detectorsNode) ;

   xmlUtils::addDetectors(detectorsNode, numberOfDetectorsPerStation, numberOfROCsPerDetector) ;

}

//================================================================================
void xmlUtils::addDetectors(QDomElement & detectorsNode, int numberOfDetectorsPerStation,
                                                         int numberOfROCsPerDetector)
{
  for(int detectorSerial=0; detectorSerial<numberOfDetectorsPerStation; ++detectorSerial)
  {
   int detectorId = detectorSerial ;
   QDomElement detectorNode = detectorsNode.ownerDocument().createElement("detector") ;
   xmlUtils::addDetector(detectorsNode,
                         detectorNode,
                         detectorId,
                         detectorSerial,
                         numberOfROCsPerDetector) ;
  }
}

//================================================================================
void xmlUtils::addDetector(QDomElement & detectorsNode,
                           QDomElement & detectorNode,
                           int detectorId,
                           int detectorSerial,
                           int numberOfROCsPerDetector)
{
    detectorNode.setAttribute("id",         detectorId    ) ;
    detectorNode.setAttribute("serial",     detectorSerial) ;
    detectorNode.setAttribute("name",       "CMS FPIX"    ) ;
    detectorNode.setAttribute("used",       "no"          ) ;
    detectorNode.setAttribute("description", ""           ) ;
    detectorsNode.appendChild(detectorNode                ) ;

    QDomElement lg = detectorNode.ownerDocument().createElement("largeGranularity") ;
    lg.setAttribute("relativeRotations","beam") ;
    detectorNode.appendChild(lg);

    xmlUtils::appendTags(lg,"xBackFlipped",           "no" );
    xmlUtils::appendTags(lg,"yBackFlipped",           "yes");
    xmlUtils::appendTags(lg,"xPosition",                  0);
    xmlUtils::appendTags(lg,"yPosition",                  0);
    xmlUtils::appendTags(lg,"zPosition",                  0);
    xmlUtils::appendTags(lg,"xRotation",                 20);
    xmlUtils::appendTags(lg,"yRotation",                  0);
    xmlUtils::appendTags(lg,"zRotation",                  0);

    QDomElement fg = detectorNode.ownerDocument().createElement("fineGranularity") ;
    fg.setAttribute("relativeRotations","beam") ;
    detectorNode.appendChild(fg);

    xmlUtils::appendTags(fg,"xPosCorrection",             0);
    xmlUtils::appendTags(fg,"xPositionError",             0);
    xmlUtils::appendTags(fg,"yPosCorrection",             0);
    xmlUtils::appendTags(fg,"yPositionError",             0);
    xmlUtils::appendTags(fg,"zPosCorrection",             0);
    xmlUtils::appendTags(fg,"zPositionError",             0);
    xmlUtils::appendTags(fg,"xRotationCorrection",        0);
    xmlUtils::appendTags(fg,"xRotationCorrectionError",   0);
    xmlUtils::appendTags(fg,"yRotationCorrection",        0);
    xmlUtils::appendTags(fg,"yRotationCorrectionError",   0);
    xmlUtils::appendTags(fg,"zRotationCorrection",        0);
    xmlUtils::appendTags(fg,"zRotationCorrectionError",   0);

    QDomElement rocsNode = detectorNode.ownerDocument().createElement("ROCs") ;
    rocsNode.setAttribute   ("inUse",       numberOfROCsPerDetector) ;
    rocsNode.setAttribute   ("xChipsNumber",                      4) ;
    rocsNode.setAttribute   ("yChipsNumber",                      2) ;
    detectorNode.appendChild(rocsNode                              ) ;

    xmlUtils::addROCs(rocsNode,numberOfROCsPerDetector) ;

    return;
}

//================================================================================
void xmlUtils::addROCs(QDomElement & ROCsNode,
                       int numberOfROCsPerDetector)
{
  for(int ROCSerial=0; ROCSerial<numberOfROCsPerDetector; ++ROCSerial)
  {
    QDomElement ROCNode = ROCsNode.ownerDocument().createElement("ROC") ;
    int ROCId = ROCSerial ;
    xmlUtils::addROC(ROCsNode,
                     ROCNode,
                     ROCId,
                     ROCSerial) ;
  }
}

//================================================================================
void xmlUtils::addROC(QDomElement & ROCsNode,
                      QDomElement & ROCNode,
                      int ROCId,
                      int ROCSerial)
{
  ROCNode.setAttribute("id",          ROCId    ) ;
  ROCNode.setAttribute("serial",      ROCSerial) ;
  ROCNode.setAttribute("pos",         "0"      ) ;
  ROCNode.setAttribute("used",        "no"     ) ;
  ROCNode.setAttribute("description", ""       ) ;
  ROCsNode.appendChild(ROCNode                 ) ;

  xmlUtils::appendTags(ROCNode,"standardRowPitch",    0.010               ) ;
  xmlUtils::appendTags(ROCNode,"standardColPitch",    0.015               ) ;
  xmlUtils::appendTags(ROCNode,"nonStandardRowPitch", 0.02, "rowNum", "79") ;
  xmlUtils::appendTags(ROCNode,"nonStandardColPitch", 0.03, "colNum", "0" ) ;
  xmlUtils::appendTags(ROCNode,"nonStandardColPitch", 0.03, "colNum", "51") ;
  xmlUtils::appendTags(ROCNode,"MAX_ROWS",            79                  ) ;
  xmlUtils::appendTags(ROCNode,"MAX_COLS",            51                  ) ;
  xmlUtils::appendTags(ROCNode,"orientation",         0                   ) ;
}

//================================================================================
void xmlUtils::appendTags(QDomElement & father,
                           std::string   name,
                           std::string   value,
                           std::string   attName,
                           std::string   attValue)
{
  QDomElement element = father.ownerDocument().createElement (QString( name.c_str())) ;
  QDomText    text    = father.ownerDocument().createTextNode(QString(value.c_str())) ;
  if( attName != "")
  {
    element.setAttribute(QString(attName.c_str()),QString(attValue.c_str()));
  }
  element.appendChild(text) ;
  father .appendChild(element) ;
}

//================================================================================
void xmlUtils::appendTags(QDomElement & father,
                           std::string   name,
                           int           value,
                           std::string   attName,
                           std::string   attValue)
{
  std::stringstream ss_ ;
  ss_.str(""); ss_ << value ;
  xmlUtils::appendTags(father, name, ss_.str(), attName, attValue) ;
}

//================================================================================
void xmlUtils::appendTags(QDomElement & father,
                          std::string   name,
                          double        value,
                          std::string   attName,
                          std::string   attValue)
{
  std::stringstream ss_ ;
  ss_.str(""); ss_ << value ;
  xmlUtils::appendTags(father, name, ss_.str(), attName, attValue) ;
}


//================================================================================
// what: 'thisNode' dumps the content of this node
//       'parent'   dumps the content of the siblings of this node
//       'children' dumps the content of the childrens of this node
// size: 'full'     dump all details
//       'headers'  dump only headers
void xmlUtils::dumpFragment(QDomNode    & thisNode,
                            std::string   what,
                            std::string   size)
{
  STDLINE("",ACWhite) ;
  if( what == "parent" || what == "thisNode")
  {
    QDomNode theNode = thisNode ;
    if( what == "parent" ) theNode = thisNode.parentNode() ;
    QString text;
    QTextStream stream( &text );
    theNode.save( stream, 2 ) ;
    int pos = text.toStdString().find(">") + 1;
    if( size == "full" )
    {
      STDLINE(text.toStdString(),ACLightPurple) ;
    }
    else
    {
      STDLINE(text.toStdString().substr(0,pos),ACLightBlue) ;
    }
  }
  else if( what == "children")
  {
    QDomNodeList children = thisNode.childNodes() ;
    for(int child=0; child<children.size(); ++child)
    {
      QString text;
      QTextStream stream( &text );
      children.at(child).save( stream, 2 ) ;
      int pos = text.toStdString().find(">") + 1;
      if( size == "full" )
      {
        STDLINE(text.toStdString(),ACLightPurple) ;
      }
      else
      {
        STDLINE(text.toStdString().substr(0,pos),ACLightBlue) ;
      }
    }
  }
}

//================================================================================
xmlParser::~xmlParser()
{
  delete document_ ;
}

//================================================================================
unsigned int xmlParser::getNumberOfStations(void)
{
  return xmlStations_.size();
}

//================================================================================
xmlDetector * xmlParser::getXmlDetectorBySerial(int stationSerial,
                                                int detectorSerial)
{
  xmlStation::xmlDetectorsDef detectors =  this->getXmlStationBySerial(stationSerial)->getXmlDetectors() ;

  for(xmlStation::xmlDetectorsDef::iterator di=detectors.begin(); di!=detectors.end(); ++di)
  {
    if( detectorSerial == di->second->getDetectorSerial() ) return di->second;
  }
  ss_.str("") ; ss_ << "FATAL: detector serial "
      << detectorSerial
      << " is out of range ("
      << detectors.size()
      << ")" ;
  STDLINE(ss_.str(), ACRed) ;
  assert(0) ;
}

//================================================================================
xmlDetector * xmlParser::getXmlDetectorById(int stationId,
                                            int detectorId)
{
  return this->getXmlStationById(stationId)->getXmlDetectorById(detectorId) ;
}

//================================================================================
int xmlParser::getNumberOfDetectors(int stationSerial)
{
  return this->getXmlStationBySerial(stationSerial)->getXmlDetectors().size() ;
}

//================================================================================
int xmlParser::getNumberOfROCs(int stationSerial, int detectorSerial)
{
  return this->getXmlStationBySerial(stationSerial)->getXmlDetectorBySerial(detectorSerial)->getXmlROCs().size() ;
}

//================================================================================
void xmlParser::remStation(int stationSerial)
{
  xmlStation * station = this->getXmlStationBySerial(stationSerial) ;

  station->remStation() ;

  xmlStations_.erase(xmlStations_.find(station->getStationId() )) ;
}

//================================================================================
void xmlParser::dumpStations(void)
{
  STDLINE("================= Dump of xmlStations_ ================",ACYellow);

  for(xmlStationsDef::iterator it =xmlStations_.begin();
                               it!=xmlStations_.end();  ++it)
  {
    ss_.str(""); ss_ << "Station Id "
                     << it->first
                     << " is serial "
                     << it->second->getStationSerial()
                     << " and has "
                     << it->second->getXmlDetectors().size()
                     << " detectors";
    STDLINE(ss_.str(),ACWhite) ;
    it->second->dumpDetectors() ;
  }
  STDLINE("=============== End Dump of xmlStations_ ==============",ACYellow);
  STDLINE("",ACWhite) ;
}

//================================================================================
xmlStation * xmlParser::getXmlStationBySerial(int stationSerial)
{
  for(xmlStationsDef::iterator si=xmlStations_.begin(); si!=xmlStations_.end(); ++si )
  {
    if( stationSerial == si->second->getStationSerial()) return si->second ;
  }

  ss_.str("") ; ss_ << "FATAL: station serial "
      << stationSerial
      << " could not be found" ;
  STDLINE(ss_.str(), ACRed) ;
  this->dumpStations() ;
  assert(0) ;
  return NULL ;
}

//================================================================================
xmlStation * xmlParser::getXmlStationById(int stationId)
{
  if( xmlStations_.find(stationId) != xmlStations_.end() ) return xmlStations_[stationId] ;

  ss_.str("") ; ss_ << "FATAL: station Id "
      << stationId
      << " could not be found)" ;
  STDLINE(ss_.str(), ACRed) ;
  this->dumpStations();
  assert(0) ;
}

//================================================================================
xmlROC * xmlParser::getROCById(int stationId, int detectorId, int rocId)
{
  return this->getXmlStationById(stationId)->getXmlDetectorById(detectorId)->getXmlROCById(rocId) ;
}

//================================================================================
xmlROC * xmlParser::getROCBySerial(int stationSerial, int detectorSerial, int rocSerial)
{
  return this->getXmlStationBySerial(stationSerial)->getXmlDetectorBySerial(detectorSerial)->getXmlROCBySerial(rocSerial) ;
}

//================================================================================
void xmlParser::save(std::string fileName)
{
  QFile file( QString(fileName.c_str()) );
  if ( file.open( QIODevice::WriteOnly | QIODevice::Text ) )
  {
    QTextStream stream( &file );
    document_->save( stream, 2 ) ;
    file.close();
    ss_.str("") ;
    ss_  << ACCyan << ACBold
        << "File "
        << ACPlain << ACYellow << ACBold
        << fileName
        << ACPlain << ACCyan << ACBold
        << " saved"
        << ACPlain ;
    STDLINE(ss_.str(),ACWhite) ;
  }
}

//================================================================================
xmlParser::idSerialListDef xmlParser::getStationList()
{
  xmlParser::idSerialListDef list ;
  for(xmlStationsDef::iterator it =xmlStations_.begin();
                               it!=xmlStations_.end(); ++it)
  {
    xmlParser::xmlParser::pairIntDef idSerial ;
    idSerial.first  = it->second->getStationId() ;
    idSerial.second = it->second->getStationSerial() ;
    list.push_back(idSerial) ;
  }
  return list ;
}

//================================================================================
void xmlParser::dumpHeaders()
{
  QDomNodeList stations = document_->elementsByTagName("station") ;

  for(int station=0; station<stations.size(); ++station)
  {
    QDomNode stationNode = stations.at(station) ;
    QString textFrom ;
    QTextStream streamFrom( &textFrom );
    stationNode.save( streamFrom, 2 ) ;
    int pos = textFrom.toStdString().find(">") + 1;
    STDLINE(textFrom.toStdString().substr(0,pos),ACCyan) ;

    QDomNodeList detectors = stationNode.toElement().elementsByTagName("detector") ;

    for(int detector=0; detector<detectors.size(); ++detector)
    {
      QDomNode detectorNode = detectors.at(detector) ;
      QString text;
      QTextStream stream( &text );
      detectorNode.save( stream, 2 ) ;
      int pos = text.toStdString().find(">") + 1;
      STDLINE(text.toStdString().substr(0,pos),ACLightBlue) ;
    }
  }
}

//================================================================================
int xmlParser::getDetectorTabPosition(int stationId, int detectorId)
{
  return this->getXmlStationById     (stationId )
             ->getDetectorTabPosition(detectorId);
}

/*================================================================================+
|                                                                                 |
|                            x m l S t a t i o n s                                |
|                                                                                 |
+================================================================================*/

//================================================================================
xmlStation::xmlStation(QDomNode & stationNode)
{
  stationNode_ = stationNode ;
  id_          = stationNode.toElement().attribute("id"         ).toInt() ;
  serial_      = stationNode.toElement().attribute("serial"     ).toInt() ;
  description_ = stationNode.toElement().attribute("description").toStdString() ;
  if( stationNode.toElement().attribute("used").toStdString() == "yes" ) used_ = true  ;
  if( stationNode.toElement().attribute("used").toStdString() == "no"  ) used_ = false ;

  QDomNodeList detectors = stationNode.toElement().elementsByTagName("detector") ;
/*
  ss_.str("") ; ss_ << "Building station id "
      << id_
      << " (serial "
      << serial_
      << ") with "
      << detectors.size()
      << " detectors";
  STDLINE(ss_.str(),ACPurple) ;
*/
  for(int detectorSerial=0; detectorSerial<(int)detectors.size(); ++detectorSerial)
  {
    QDomNode detectorNode            = detectors.at(detectorSerial) ;
    int detectorId                   = detectorNode.toElement().attribute("id").toInt() ;
    detectors_[detectorId]           = new xmlDetector(detectorNode);
    detectorTabPosition_[detectorId] = detectorSerial ;
  }
}

//================================================================================
bool xmlStation::isEnabled(void)
{
  bool inUse = false ;
  if( stationNode_.toElement().attribute("used").toStdString() == std::string("yes"))
  {
    inUse = true ;
  }

  return inUse ;
}

//================================================================================
void xmlStation::setId(int id)
{
  stationNode_.toElement().setAttribute("id", id) ;
}

//================================================================================
void xmlStation::setUsed(bool used)
{
  if( used )
  {
    stationNode_.toElement().setAttribute("used", "yes") ;
  }
  else
  {
    stationNode_.toElement().setAttribute("used", "no" ) ;
  }

}

//================================================================================
xmlDetector * xmlStation::getXmlDetectorBySerial(int detectorSerial)
{
  for(xmlDetectorsDef::iterator di=detectors_.begin(); di!=detectors_.end(); ++di)
  {
    if( detectorSerial == di->second->getDetectorSerial() ) return di->second;
  }
  ss_.str("") ; ss_ << "FATAL: detector serial "
      << detectorSerial
      << " is out of range ("
      << detectors_.size()
      << ") for station "
      << id_ ;
  STDLINE(ss_.str(), ACRed) ;
  assert(0) ;
  return NULL ;
}

//================================================================================
xmlDetector * xmlStation::getXmlDetectorById(int detectorId)
{ 
  if( detectors_.find(detectorId) == detectors_.end() )
  {
    ss_.str(""); ss_ << "FATAL: could not locate detector Id " << detectorId << " for station Id " << id_ ;
    STDLINE(ss_.str(), ACRed ) ;
    assert(0) ;
  }
  return detectors_[detectorId] ;
}

//================================================================================
xmlROC * xmlStation::getROCById(int detector, int roc)
{
  if( detectors_.find(detector) == detectors_.end() )
  {
    ss_.str(""); ss_ << "FATAL: could not locate detector " << detector << " for station " << id_ ;
    STDLINE(ss_.str(), ACRed ) ;
    assert(0) ;
  }
  return detectors_[detector]->getXmlROCById(roc) ;
}

//================================================================================
void xmlStation::setValue(int value)
{
  stationNode_.toElement().setAttribute(QString("id"),value);
}

//================================================================================
void xmlStation::setAttributeText(std::string   key,
                                  QString     & textValue)
{
  stationNode_.toElement().setAttribute(QString(key.c_str()),textValue) ;
}

//================================================================================
std::vector<int> xmlStation::getAllStationsId(void)
{
  std::vector<int> idv ;
  QDomNodeList allStations = stationNode_.parentNode()
                             .toElement()
                             .elementsByTagName("station") ;
  for(int s=0; s<allStations.size(); ++s)
  {
    idv.push_back(allStations.at(s).toElement().attribute("id").toInt());
  }
  return idv ;
}

//================================================================================
void xmlStation::remStation(void)
{
  stationNode_.ownerDocument().elementsByTagName("stations").at(0).removeChild(stationNode_) ;
}

//================================================================================
void xmlStation::addDetector(int detectorId, int detectorSerial)
{
  QDomElement detectorsNode = stationNode_.toElement()
                                          .elementsByTagName("detectors")
                                          .at(0)
                                          .toElement() ;
  QDomElement detectorNode = detectorsNode.ownerDocument().createElement("detector") ;
  xmlUtils::addDetector(detectorsNode,
                        detectorNode,
                        detectorId,
                        detectorSerial,
                        1) ;

  detectors_[detectorId] = new xmlDetector(detectorNode) ;

  ss_.str(""); ss_ << "Now there are "
                   << detectors_.size()
                   << " detectors defined for station id "
                   << id_;
  STDLINE(ss_.str(),ACLightBlue) ;
  this->dumpDetectors();
}

//================================================================================
void xmlStation::dumpDetectors(void)
{
  STDLINE("================= Dump of detectors_ for station id " << id_ << " and serial " << serial_ << " ================",ACYellow);

  for(xmlDetectorsDef::iterator it =detectors_.begin();
                                it!=detectors_.end();  ++it)
  {
    ss_.str(""); ss_ << "Detector id "
                     << it->first
                     << " has serial "
                     << it->second->getDetectorSerial()
                     << " and "
                     << it->second->getXmlROCs().size()
                     << " ROCs";
    STDLINE(ss_.str(),ACWhite) ;
    it->second->dumpROCs() ;
  }
  STDLINE("=============== End Dump of detectors_ ==============",ACYellow);
  STDLINE("",ACWhite) ;
}

//================================================================================
xmlParser::idSerialListDef xmlStation::getDetectorList()
{
  xmlParser::idSerialListDef list ;
  xmlStation::xmlDetectorsDef detectors = this->getXmlDetectors() ;

  for(xmlStation::xmlDetectorsDef::iterator it =detectors.begin();
                                            it!=detectors.end(); ++it)
  {
    xmlParser::xmlParser::pairIntDef idSerial ;
    idSerial.first  = it->second->getDetectorId() ;
    idSerial.second = it->second->getDetectorSerial() ;
    ss_.str("") ; ss_ << "In station id "   << id_
                      << " serial "         << serial_
                      << " is detector id " << idSerial.first
                      << " serial "         << idSerial.second ; STDLINE(ss_.str(),ACCyan) ;
    list.push_back(idSerial) ;
  }
  return list ;
}

/*================================================================================

                            x m l D e t e c t o r s

 ================================================================================*/

//================================================================================
xmlDetector::xmlDetector(QDomNode & detectorNode)
{
  detectorNode_ = detectorNode ;

  stationId_ = detectorNode_.parentNode().parentNode().toElement().attribute("id").toInt() ;

  keyValue_["id"         ] = detectorNode_.toElement().attribute("id"         ) ;
  keyValue_["serial"     ] = detectorNode_.toElement().attribute("serial"     ) ;
  keyValue_["name"       ] = detectorNode_.toElement().attribute("name"       ) ;
  keyValue_["used"       ] = detectorNode_.toElement().attribute("used"       ) ;
  keyValue_["description"] = detectorNode_.toElement().attribute("description") ;
  keyValue_["isDUT"      ] = detectorNode_.toElement().attribute("isDUT"      ) ;

  id_                      = keyValue_["id"         ].toInt      () ;
  serial_                  = keyValue_["serial"     ].toInt      () ;
  description_             = keyValue_["description"].toStdString() ;

  isDUT_ = false ;
  if( detectorNode_.toElement().attribute("used" ).toStdString() == "yes" ) used_  = true  ;
  if( detectorNode_.toElement().attribute("used" ).toStdString() == "no"  ) used_  = false ;
  if( detectorNode_.toElement().attribute("isDUT").toStdString() == "yes" ) isDUT_ = true  ;
  if( detectorNode_.toElement().attribute("isDUT").toStdString() == "no"  ) isDUT_ = false ;

  /*
  ss_.str("") ; ss_ << "  Building detector serial "
      << serial_
      << " with Id "
      << keyValue_["id"].toStdString()
      << " in station id "
      << stationId_
      << " (this is a "
      << keyValue_["name"].toStdString()
      << ")";
  STDLINE(ss_.str(),ACYellow) ;
*/
  QDomNodeList lg = detectorNode_.toElement().elementsByTagName("largeGranularity") ;
  QDomNodeList fg = detectorNode_.toElement().elementsByTagName("fineGranularity" ) ;

  keyValue_["lgRelativeRotation"]       = lg.at(0).toElement().attribute("relativeRotations") ;
  keyValue_["fgRelativeRotation"]       = fg.at(0).toElement().attribute("relativeRotations") ;

  keyValue_["xBackFlipped"]             = lg.at(0).toElement().elementsByTagName("xBackFlipped"            ).at(0).toElement().text() ;
  keyValue_["yBackFlipped"]             = lg.at(0).toElement().elementsByTagName("yBackFlipped"            ).at(0).toElement().text() ;
  keyValue_["xPosition"]                = lg.at(0).toElement().elementsByTagName("xPosition"               ).at(0).toElement().text() ;
  keyValue_["yPosition"]                = lg.at(0).toElement().elementsByTagName("yPosition"               ).at(0).toElement().text() ;
  keyValue_["zPosition"]                = lg.at(0).toElement().elementsByTagName("zPosition"               ).at(0).toElement().text() ;
  keyValue_["xRotation"]                = lg.at(0).toElement().elementsByTagName("xRotation"               ).at(0).toElement().text() ;
  keyValue_["yRotation"]                = lg.at(0).toElement().elementsByTagName("yRotation"               ).at(0).toElement().text() ;
  keyValue_["zRotation"]                = lg.at(0).toElement().elementsByTagName("zRotation"               ).at(0).toElement().text() ;

  keyValue_["xPosCorrection"]           = fg.at(0).toElement().elementsByTagName("xPosCorrection"          ).at(0).toElement().text() ;
  keyValue_["yPosCorrection"]           = fg.at(0).toElement().elementsByTagName("yPosCorrection"          ).at(0).toElement().text() ;
  keyValue_["zPosCorrection"]           = fg.at(0).toElement().elementsByTagName("zPosCorrection"          ).at(0).toElement().text() ;
  keyValue_["xPositionError"]           = fg.at(0).toElement().elementsByTagName("xPositionError"          ).at(0).toElement().text() ;
  keyValue_["yPositionError"]           = fg.at(0).toElement().elementsByTagName("yPositionError"          ).at(0).toElement().text() ;
  keyValue_["zPositionError"]           = fg.at(0).toElement().elementsByTagName("zPositionError"          ).at(0).toElement().text() ;
  keyValue_["xRotationCorrection"]      = fg.at(0).toElement().elementsByTagName("xRotationCorrection"     ).at(0).toElement().text() ;
  keyValue_["yRotationCorrection"]      = fg.at(0).toElement().elementsByTagName("yRotationCorrection"     ).at(0).toElement().text() ;
  keyValue_["zRotationCorrection"]      = fg.at(0).toElement().elementsByTagName("zRotationCorrection"     ).at(0).toElement().text() ;
  keyValue_["xRotationCorrectionError"] = fg.at(0).toElement().elementsByTagName("xRotationCorrectionError").at(0).toElement().text() ;
  keyValue_["yRotationCorrectionError"] = fg.at(0).toElement().elementsByTagName("yRotationCorrectionError").at(0).toElement().text() ;
  keyValue_["zRotationCorrectionError"] = fg.at(0).toElement().elementsByTagName("zRotationCorrectionError").at(0).toElement().text() ;

  keyValue_["xChipsNumber"]             = detectorNode_.toElement().elementsByTagName("ROCs").at(0).toElement().attribute("xChipsNumber") ;
  keyValue_["yChipsNumber"]             = detectorNode_.toElement().elementsByTagName("ROCs").at(0).toElement().attribute("yChipsNumber") ;

  // Trim blanks from each field
  for(keyValueDef::iterator it=keyValue_.begin(); it!=keyValue_.end(); ++it)
  {
    it->second.replace(QString(" "), QString("")) ;
  }

  QDomNodeList rocs = detectorNode_.toElement().elementsByTagName("ROC") ;

  for(int rocSerial=0; rocSerial<(int)rocs.size(); ++rocSerial)
  {
    QDomNode rocNode = rocs.at(rocSerial) ;
    int   rocId      = rocNode.toElement().attribute("id").toInt() ;
    ROCs_[rocId]     = new xmlROC(rocNode) ;
  }
}

//================================================================================
xmlParser::idSerialListDef xmlDetector::getROCList()
{
  xmlParser::idSerialListDef list ;
  xmlDetector::xmlROCDef rocs = this->getXmlROCs() ;
  ss_.str("") ;ss_ << "Found " << rocs.size() << " rocs in detector id " << id_ ;
  STDLINE(ss_.str(),ACWhite) ;
  for(xmlDetector::xmlROCDef::iterator it =rocs.begin();
                                       it!=rocs.end(); ++it)
  {
    xmlParser::xmlParser::pairIntDef idSerial ;
    idSerial.first  = it->second->getROCId() ;
    idSerial.second = it->second->getROCSerial() ;
    list.push_back(idSerial) ;
  }
  return list ;
}

//================================================================================
xmlROC * xmlDetector::getXmlROCById(int rocId)
{
  if( ROCs_.find(rocId) == ROCs_.end() )
  {
    ss_.str(""); ss_ << "FATAL: could not locate ROC id "
                     << rocId
                     << " for detector id "
                     << id_ ;
    STDLINE(ss_.str(), ACRed ) ;
    assert(0) ;
  }
  return ROCs_[rocId];
}

//================================================================================
xmlROC * xmlDetector::getXmlROCBySerial(int rocSerial)
{
  int rocNumber = 0;

  for(xmlROCDef::iterator rocIt=ROCs_.begin(); rocIt!=  ROCs_.end(); ++rocIt)
  {
    if( rocNumber++ == rocSerial ) return rocIt->second ;
  }

  ss_.str(""); ss_ << "FATAL: could not locate ROC serial "
                   << rocSerial
                   << " for detector Id "
                   << id_ ;
  STDLINE(ss_.str(), ACRed ) ;
  assert(0) ;
}

//================================================================================
std::vector<int> xmlDetector::getAllDetectorsId(void)
{
  std::vector<int> idv ;
  QDomNodeList allDetectors = detectorNode_.parentNode()
                                           .toElement()
                                           .elementsByTagName("detector") ;
  for(int s=0; s<allDetectors.size(); ++s)
  {
    idv.push_back(allDetectors.at(s).toElement().attribute("id").toInt());
  }
  return idv ;
}

//================================================================================
bool xmlDetector::isEnabled(void)
{
  bool inUse = false ;
  if( detectorNode_.toElement().attribute("used").toStdString() == std::string("yes"))
  {
    inUse = true ;
  }

  return inUse ;
}

//================================================================================
void xmlDetector::setUsed(bool used)
{
  used_ = used ;
  if( used_ )
  {
    detectorNode_.toElement().setAttribute("used", "yes") ;
  }
  else
  {
    detectorNode_.toElement().setAttribute("used", "no" ) ;
  }

}

//================================================================================
void xmlDetector::dumpROCs(void)
{
  STDLINE("  ================= Dump of ROCs_ for station id " << stationId_ << " detector Id " << id_ << " ================",ACYellow);

  for(xmlROCDef::iterator it =ROCs_.begin();
                          it!=ROCs_.end();  ++it)
  {
    ss_.str(""); ss_ << "  ROC id "
                     << it->first
                     << " has serial "
                     << it->second->getROCSerial() ;
     STDLINE(ss_.str(),ACWhite) ;
  }
  STDLINE("  =============== End Dump of ROCs_ ==============",ACYellow);
  STDLINE("",ACWhite) ;
}

//================================================================================
void xmlDetector::remDetector(void)
{
  ss_.str("");
  ss_ << "Working on station "
      << " id "          << detectorNode_.toElement().attribute("id"         ).toStdString()
      << " serial "      << detectorNode_.toElement().attribute("serial"     ).toStdString()
      << " name "        << detectorNode_.toElement().attribute("name"       ).toStdString()
      << " used "        << detectorNode_.toElement().attribute("used"       ).toStdString()
      << " description " << detectorNode_.toElement().attribute("description").toStdString() ;
  STDLINE(ss_.str(),ACWhite) ;

  detectorNode_.parentNode().removeChild(detectorNode_) ;
}

//================================================================================
void xmlDetector::addROC(int ROCId, int ROCSerial)
{
  QDomElement ROCsNode = detectorNode_.toElement()
                                      .elementsByTagName("ROCs")
                                      .at(0)
                                      .toElement() ;
  QDomElement ROCNode = ROCsNode.ownerDocument().createElement("ROC") ;
  xmlUtils::addROC(ROCsNode,
                   ROCNode,
                   ROCId,
                   ROCSerial) ;

  ROCs_[ROCId] = new xmlROC(ROCNode) ;

  ss_.str(""); ss_ << "Now there are "
                   << ROCs_.size()
                   << " ROCs defined for detector id "
                   << id_;
  STDLINE(ss_.str(),ACLightBlue) ;
  this->dumpROCs();
}

/*================================================================================#
#                                                                                 #
#                            x m l R O C s                                        #
#                                                                                 #
# ================================================================================*/

//================================================================================
xmlROC::xmlROC(QDomNode & ROCNode)
{
  ROCNode_ = ROCNode ;

  QDomNode detectorNode = ROCNode_    .parentNode().parentNode() ;
  QDomNode stationNode  = detectorNode.parentNode().parentNode() ;

  detectorId_  = detectorNode.toElement().attribute("id"         ).toInt() ;
  stationId_   = stationNode .toElement().attribute("id"         ).toInt() ;
  id_          = ROCNode     .toElement().attribute("id"         ).toInt() ;
  serial_      = ROCNode     .toElement().attribute("serial"     ).toInt() ;
  description_ = ROCNode     .toElement().attribute("description").toStdString() ;

  ss_.str(""); ss_ << "ROC - Serial: "
                   << serial_
                   << " id: "
                   << id_
                   << " at detector id "
                   << detectorId_
                   << " in station Id "
                   << stationId_ ;
//  STDLINE(ss_.str(),ACWhite) ;
  this->update()     ;
}

//================================================================================
void xmlROC::update(void)
{
  keyValue_["id"                 ] = ROCNode_.toElement().attribute        ("id"                 ) ;
  keyValue_["pos"                ] = ROCNode_.toElement().attribute        ("pos"                ) ;
  keyValue_["MAX_ROWS"           ] = ROCNode_.toElement().elementsByTagName("MAX_ROWS"           ).at(0).toElement().text();
  keyValue_["MAX_COLS"           ] = ROCNode_.toElement().elementsByTagName("MAX_COLS"           ).at(0).toElement().text();
  keyValue_["standardRowPitch"   ] = ROCNode_.toElement().elementsByTagName("standardRowPitch"   ).at(0).toElement().text();
  keyValue_["standardColPitch"   ] = ROCNode_.toElement().elementsByTagName("standardColPitch"   ).at(0).toElement().text();
  keyValue_["orientation"        ] = ROCNode_.toElement().elementsByTagName("orientation"        ).at(0).toElement().text();
  keyValue_["calibrationFilePath"] = ROCNode_.toElement().elementsByTagName("calibrationFilePath").at(0).toElement().text();

  if( ROCNode_.toElement().attribute("used"  ).toStdString() == "yes") used_ = true  ;
  if( ROCNode_.toElement().attribute("used"  ).toStdString() == "no" ) used_ = false ;

  QDomNodeList nonStandardRowPitchNodes = ROCNode_.toElement().elementsByTagName("nonStandardRowPitch") ;
  QDomNodeList nonStandardColPitchNodes = ROCNode_.toElement().elementsByTagName("nonStandardColPitch") ;

  for(int ns=0; ns<(int)nonStandardRowPitchNodes.size(); ++ns)
  {
    nonStandardRowPitch_[nonStandardRowPitchNodes.at(ns).toElement().attribute("rowNum").toInt()] =
                         nonStandardRowPitchNodes.at(ns).toElement().text().toDouble() ;
  }

  for(int ns=0; ns<(int)nonStandardColPitchNodes.size(); ++ns)
  {
    nonStandardColPitch_[nonStandardColPitchNodes.at(ns).toElement().attribute("colNum").toInt()] =
                         nonStandardColPitchNodes.at(ns).toElement().text().toDouble() ;
  }
}

//================================================================================
void xmlROC::setTabbedText(std::string   key,
                           std::string   tabValue,
                           std::string   attName,
                           QString     & textValue)
{
  QDomNodeList nodes = ROCNode_.toElement()
                               .elementsByTagName(QString(key.c_str())) ;

  for(int e=0; e<(int)nodes.size(); ++e)
  {
    QDomNamedNodeMap atts = nodes.at(e).attributes() ;
    for(int a=0; a<(int)atts.size(); ++a)
    {   
      if( atts.item(a).nodeName() .toStdString() == attName &&
          atts.item(a).nodeValue().toStdString() == tabValue )
      {
        QDomNode child = nodes.at(e).firstChild() ;

        QDomText text = ROCNode_.ownerDocument().createTextNode(textValue) ;

        nodes.at(e).replaceChild(text,child) ;
      }
    }
  }
  keyValue_[key] = textValue ;
}

//================================================================================
void xmlROC::setAttributeText(std::string   key,
                              QString     & textValue)
{
  ROCNode_.toElement()
          .setAttribute(QString(key.c_str()),textValue) ;

  keyValue_[key] = textValue ;
}

//================================================================================
void xmlROC::setUsed(bool used)
{
  if( used )
  {
    ROCNode_.toElement().setAttribute("used", "yes") ;
  }
  else
  {
    ROCNode_.toElement().setAttribute("used", "no" ) ;
  }

}

//================================================================================
void xmlROC::addNonStandardPitch(xmlROC    * ROC,
                                 QString     type   ,
                                 int         rowCol ,
                                 double      pitch   )
{
  QDomElement nsr ;
  if( type.toStdString() == "row")
  {
    nsr = ROC->getNode().ownerDocument()
                        .createElement("nonStandardRowPitch") ;
    nsr.setAttribute("rowNum",   rowCol  ) ;
  }
  if( type.toStdString() == "col")
  {
    nsr = ROC->getNode().ownerDocument()
                        .createElement("nonStandardColPitch") ;
    nsr.setAttribute("colNum",   rowCol  ) ;
  }
  ss_.str(""); ss_ << pitch ;
  QDomText text = ROC->getNode().ownerDocument().createTextNode(QString(ss_.str().c_str())) ;
  nsr.appendChild(text);
  ROC->getNode().appendChild(nsr) ;

  this->update() ;
}

//================================================================================
void xmlROC::removeNonStandardPitch(QString     type   ,
                                    int         rowCol )
{
  QDomElement nsr ;
  if( type.toStdString() == "row")
  {
     QDomNodeList children = ROCNode_.toElement().elementsByTagName("nonStandardRowPitch") ;
     for(int n=0; n<children.count(); ++n)
     {
       if( children.at(n).attributes().namedItem(QString("rowNum")).nodeValue().toInt() == rowCol)
       {
         ROCNode_.removeChild(children.at(n)) ;
       }
     }
  }

  if( type.toStdString() == "col")
  {
    QDomNodeList children = ROCNode_.toElement().elementsByTagName("nonStandardColPitch") ;
    for(int n=0; n<children.count(); ++n)
    {
      if( children.at(n).attributes().namedItem(QString("colNum")).nodeValue().toInt() == rowCol)
      {
        ROCNode_.removeChild(children.at(n)) ;
      }
    }
  }

  this->update() ;
}

//================================================================================
void xmlROC::remROC(void)
{
  ss_.str("");
  ss_ << "Working on ROC "
      << " id "          << ROCNode_.toElement().attribute("id"         ).toStdString()
      << " serial "      << ROCNode_.toElement().attribute("serial"     ).toStdString()
      << " used "        << ROCNode_.toElement().attribute("used"       ).toStdString()
      << " description " << ROCNode_.toElement().attribute("description").toStdString() ;
  STDLINE(ss_.str(),ACWhite) ;

  ROCNode_.parentNode().removeChild(ROCNode_) ;
}
