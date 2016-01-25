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
 
#include "Geometry.h"
#include "MessageTools.h"

ClassImp(Geometry);

//===============================================================================
Geometry::Geometry (void) :
    calibrationDone_(false)
{
  //STDLINE("Empty constructor",ACWhite) ;
}

//===============================================================================
Geometry::~Geometry(void)
{
//    STDLINE("Geometry::~Geometry",ACRed);
    this->clear();
//    STDLINE("Geometry::~Geometry",ACGreen);
}

    //=============================================================================
void Geometry::setGeometryFileName(std::string fileName)
{
  geometryFileName_ = fileName;
}

//=============================================================================
bool Geometry::compare_zPosition(std::string first, std::string second)
{
    if ( this->getDetector(first)->getZPositionTotal() < this->getDetector(second)->getZPositionTotal() )
        return true;
    else
        return false;
}

//===============================================================================
Detector * Geometry::addDetector(std::string plaqID, bool isDUT, bool isStrip)
{
  if( detectorsMap_.find( plaqID ) == detectorsMap_.end() )
  {
    detectorsMap_[ plaqID ] = new Detector(plaqID, isDUT, isStrip);
    if( isDUT ) dutNumbers_++                ;
  }
  else     STDLINE("Warning: Detector: " + plaqID + " was already added!!",ACRed);

  return detectorsMap_[ plaqID ];
}

//===============================================================================
Detector * Geometry::getDetector (std::string plaqID)
{
  if( detectorsMap_.find( plaqID ) == detectorsMap_.end() )
  {
    //STDLINE("Warning: No detector: " + plaqID + " found",ACRed);
    return  NULL                ;
  }
  else return detectorsMap_[plaqID];
}

//===============================================================================
Detector * Geometry::getDetector(int station , int plaq)
{
  std::stringstream ss_;
  ss_.str("");
  ss_ << "Station: " << station << " - " << "Plaq: " << plaq;
  return this->getDetector(ss_.str());
}

//===============================================================================
int Geometry::getDetectorStation(std::string  plaqID)
{
    return atoi(plaqID.substr(9,plaqID.find('-')-10).c_str());
}

//===============================================================================
int Geometry::getDetectorModule(std::string  plaqID)
{
    return atoi(plaqID.substr(plaqID.find("Plaq:")+6,plaqID.size()-plaqID.find("Plaq:")-6).c_str());
}

//===============================================================================
std::string Geometry::getDetectorID(int station , int plaq)
{
  std::stringstream ss_;
  ss_.str("");
  ss_ << "Station: " << station << " - " << "Plaq: " << plaq;
  return ss_.str();
}

//===============================================================================
unsigned int  Geometry::getMaxRowsNum(void)
{
  unsigned int maxRowsNum=0;
  for (detectorsMapDef::iterator it=detectorsMap_.begin(); it!=detectorsMap_.end(); ++it)
  {
    if ( (*it).second->getNumberOfRows() > maxRowsNum ) maxRowsNum = (*it).second->getNumberOfRows();
  }
  return maxRowsNum;
}

//===============================================================================
unsigned int  Geometry::getMaxColsNum(void)
{
  unsigned int maxColsNum=0;
  for (detectorsMapDef::iterator it=detectorsMap_.begin(); it!=detectorsMap_.end(); ++it)
  {
    if ( (*it).second->getNumberOfCols() > maxColsNum ) maxColsNum = (*it).second->getNumberOfCols();
  }
  return maxColsNum;
}

//===============================================================================
double   Geometry::getMaxDetectorsLength(void)
{
  double maxDetectorsLength=0;
  for (detectorsMapDef::iterator it=detectorsMap_.begin(); it!=detectorsMap_.end(); ++it)
  {
    if ( (*it).second->getDetectorLengthX() > maxDetectorsLength ) maxDetectorsLength = (*it).second->getDetectorLengthX();
    if ( (*it).second->getDetectorLengthY() > maxDetectorsLength ) maxDetectorsLength = (*it).second->getDetectorLengthY();
  }
  return maxDetectorsLength;
}

//================================================================================
unsigned int Geometry::getDetectorsNumber(bool excludeDUT)
{
    if (!excludeDUT) return detectorsMap_.size()               ;
    else             return (detectorsMap_.size()-dutNumbers_ );
}

//================================================================================
std::vector<Detector*>   Geometry::getDUTs(void )
{
    std::vector<Detector*> DUTs;
    for (detectorsMapDef::iterator it=detectorsMap_.begin(); it!=detectorsMap_.end(); ++it)
    {
       if( (*it).second->isDUT() ) DUTs.push_back( (*it).second );
    }
    return DUTs;
}

//================================================================================
void Geometry::dump(void)
{
    for (detectorsMapDef::iterator it=detectorsMap_.begin(); it!=detectorsMap_.end(); ++it)
    {
       STDLINE(it->first,ACRed);
       it->second->dump();
    }
}
