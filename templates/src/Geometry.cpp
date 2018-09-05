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

struct sort_pred
{
    bool operator()(
                    const std::pair<double,std::string> &left,
                    const std::pair<double,std::string> &right
                   )
    {
      return left.first < right.first;
    }
} geoSorter_;

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
    //STDSNAP("Warning: No detector: " + plaqID + " found",ACRed);
    return  NULL                ;
  }
  else
      return detectorsMap_[plaqID];
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
unsigned int Geometry::getDetectorsNumberByType(std::string detectorType)
{
  unsigned int numDUTs	 = 0 ;
  unsigned int numStrips = 0 ;
  unsigned int numPixels = 0 ;
  
  for (detectorsMapDef::iterator it = detectorsMap_.begin(); 
                                 it!= detectorsMap_.end(); 
			       ++it)
  {
    
    if(     (*it).second->isDUT  ()) numDUTs++   ;
    else if((*it).second->isStrip()) numStrips++ ;
    else                             numPixels++ ;
  }
  
  if( detectorType == "DUTs"    	) return numDUTs    	        ;
  if( detectorType == "Strips"  	) return numStrips  	        ;
  if( detectorType == "Pixels"  	) return numPixels  	        ;
  if( detectorType == "DUTs & Strips"   ) return numDUTs   + numStrips  ;
  if( detectorType == "DUTs & Pixels"   ) return numDUTs   + numPixels  ;
  if( detectorType == "Strips & Pixels" ) return numStrips + numPixels  ;
  return numDUTs + numStrips + numPixels ;
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
    STDLINE("----------------------------- Geometry dump ------------------",ACRed) ;
    std::vector<Detector*> dets = this->getDUTs() ;
    ss_.str("");
    ss_ << "Provenance of geometry: " << this->getGeometryFileName();
    STDLINE(ss_.str(),ACGreen) ;
    ss_.str("");
    ss_ << "Found " << this->getDetectorsNumber() << " detectors";
    STDLINE(ss_.str(),ACGreen) ;
    ss_.str("");
    ss_ << "Of these " << dets.size()  << " is/are DUTs";
    STDLINE(ss_.str(),ACGreen) ;
    STDLINE(" ",ACGreen) ;
    ss_.str("");
    if(this->calibrationDone())
    {
        STDLINE("Calibration available and used in reconstruction" ,ACGreen) ;
    }
    {
        STDLINE("Calibration NOT available or NOT used in reconstruction" ,ACRed) ;
    }

    for (detectorsMapDef::iterator it=detectorsMap_.begin(); it!=detectorsMap_.end(); ++it)
    {
        ss_.str("") ; ss_ << "========= Detector name: " << it->first << " ===========" ;
       if(it->second->isDUT()) ss_ << " " << ACRed << ACBold << ACReverse << "-----> (DUT)!!" ;
       STDLINE(ss_.str(),ACWhite);
       int nROCs    = it->second->getNumberOfROCs() ;
       int nROCRows = it->second->getNumberOfRows() ;
       int nROCCols = it->second->getNumberOfCols() ;
       ss_.str("") ; ss_ << "This detector has " << nROCs << " number of ROCs (" << nROCRows << " rows and " << nROCCols << " cols)" ;
       STDLINE(ss_.str(), ACCyan) ;
       for(int r=0; r<nROCs; ++r)
       {
           ROC * theROC = it->second->getROCByPosition(r) ;
           ss_.str("") ; ss_ << "ChipID: " << theROC->getID() << " (Cal: " << theROC->getCalibrationFilePath() << ")";
           STDLINE(ss_.str(), ACGreen) ;
       }
       it->second->dump();
       STDLINE(" ",ACGreen) ;
    }
    STDLINE("-------------------------------------------------------------",ACRed) ;
}
//=======================================================================================
void Geometry::orderPlanes(void)
{
    std::vector<std::pair<double, std::string> >  plaqByZ;

    for(std::map< std::string , Detector* >::iterator it = detectorsMap_.begin(); it!=detectorsMap_.end();++it)
    {
//        ss_.str(""); ss_ << "Detector: "
//                         << it->second->getName()
//                         << " ID(1): "
//                         << it->second->getID()
//                         << " ID(2): "
//                         << it->first
//                         << " z: "
//                         << it->second->getZPosition();
//        STDLINE(ss_.str(),ACWhite) ;
        std::string plaqID = it->first;
        double z = it->second->getZPosition();
        plaqByZ.push_back(make_pair(z,plaqID));
    }

    std::sort(plaqByZ.begin(),plaqByZ.end(),geoSorter_);

    theKalmanPlaneInfo_.setKalmanFilterOrder(plaqByZ);
}
//=======================================================================================
void Geometry::calculatePlaneMCS(void)
{
    for (unsigned int plane = 0; plane < theKalmanPlaneInfo_.getKalmanFilterOrder().size(); plane++)
    {

        std::string plaqID = theKalmanPlaneInfo_.getKalmanFilterOrder().at(plane).second;
        Detector* detector = getDetector(plaqID);

        //std::cout<<__PRETTY_FUNCTION__<<" Detector: "<<plaqID<<" zPosition: "<<theKalmanPlaneInfo_.getKalmanFilterOrder().at(plane).first<<std::endl;

        //Define variables
        double multipleScattering;
        if     (detector->isStrip()) multipleScattering =  8.26e-6;
        else if(detector->isDUT()  ) multipleScattering = 16.04e-6;
        else                         multipleScattering = 12.35e-6;

        double zValue = detector->getZPositionTotal();
        TVectorT   <double> sensorOrigin(4);
        TVectorT   <double> upVector    (4);
        TVectorT   <double> rightVector (4);
        TVectorT   <double> beamVector  (4);
        TVectorT   <double> h           (4);
        TMatrixTSym<double> trackCov    (4);
        double offset   = 0                ;

        //Define local coordinates
        sensorOrigin[0] = 0; sensorOrigin[1] = 0; sensorOrigin[2] = 0;
        upVector    [0] = 0; upVector    [1] = 1; upVector    [2] = 0;
        rightVector [0] = 1; rightVector [1] = 0; rightVector [2] = 0;
        beamVector  [0] = 0; beamVector  [1] = 0; beamVector  [2] = 1;        

        //Change to global coordinates
        detector->fromLocalToGlobal(&sensorOrigin[0],&sensorOrigin[1],&sensorOrigin[2]);
        detector->fromLocalToGlobal(&upVector    [0],&upVector    [1],&upVector    [2]);
        detector->fromLocalToGlobal(&rightVector [0],&rightVector [1],&rightVector [2]);
        detector->fromLocalToGlobal(&beamVector  [0],&beamVector  [1],&beamVector  [2]);

        //Normalize vectors
        upVector   [0] -= sensorOrigin[0]; upVector   [1] -= sensorOrigin[1]; upVector   [2] -= sensorOrigin[2];
        rightVector[0] -= sensorOrigin[0]; rightVector[1] -= sensorOrigin[1]; rightVector[2] -= sensorOrigin[2];
        beamVector [0] -= sensorOrigin[0]; beamVector [1] -= sensorOrigin[1]; beamVector [2] -= sensorOrigin[2];

        double den = upVector[1]*rightVector[0] - upVector[0]    *rightVector[1];
        offset= -sensorOrigin[0]*rightVector[0] - sensorOrigin[1]*rightVector[1]+
                rightVector[2]*(-sensorOrigin[2]+(sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
                sensorOrigin[1]*(upVector[0]*rightVector[2]-upVector[2]*rightVector[0])+
                sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))/den);
        h[1] = rightVector[0]+rightVector[2]*(upVector[1]*rightVector[2]-upVector[2]*rightVector[1])/den;
        h[3] = rightVector[1]+rightVector[2]*(upVector[2]*rightVector[0]-upVector[0]*rightVector[2])/den;
        h[0] =(sensorOrigin[0]*(upVector[2]   *rightVector[1]-upVector[1]   *rightVector[2])+
                sensorOrigin[1]*(upVector[0]   *rightVector[2]-rightVector[0]*upVector[2]   )+
                sensorOrigin[2]*(upVector[1]   *rightVector[0]-upVector[0]   *rightVector[1]))*
                (upVector[1]   *(rightVector[0]*rightVector[0]+rightVector[2]*rightVector[2])-
                rightVector[1]*(upVector[0]   *rightVector[0]+upVector[2]   *rightVector[2]))/(den*den);
        h[2] =(sensorOrigin[0]*(upVector[2]   *rightVector[1]-upVector[1]   *rightVector[2])+
                sensorOrigin[1]*(upVector[0]   *rightVector[2]-upVector[2]   *rightVector[0])+
                sensorOrigin[2]*(upVector[1]   *rightVector[0]-upVector[0]   *rightVector[1]))*
                (rightVector[0]*(upVector[1]   *rightVector[1]+upVector[2]   *rightVector[2])-
                upVector[0]   *(rightVector[1]*rightVector[1]+rightVector[2]*rightVector[2]))/(den*den);

        double mmbb = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);

        trackCov[1][1] =  zValue*zValue*mmbb;
        trackCov[1][0] = -zValue       *mmbb;
        trackCov[3][3] =  zValue*zValue*mmbb;
        trackCov[3][2] = -zValue       *mmbb;
        trackCov[0][1] = -zValue       *mmbb;
        trackCov[0][0] =                mmbb;
        trackCov[2][3] = -zValue       *mmbb;
        trackCov[2][2] =                mmbb;

        theKalmanPlaneInfo_.setH       (h,plaqID       );
        theKalmanPlaneInfo_.setTrackCov(trackCov,plaqID);
        theKalmanPlaneInfo_.setOffset  (offset,plaqID  );
    }
}

//=======================================================================================
void Geometry::calculatePlaneMCSKalman(std::string plaqID, Detector::matrix33Def fRinv, double fTz )
{
    Detector* detector = getDetector(plaqID);

    //Define variables
    TVectorT   <double> sensorOrigin(4);
    TVectorT   <double> upVector    (4);
    TVectorT   <double> rightVector (4);
    TVectorT   <double> beamVector  (4);
    TVectorT   <double> h           (4);
    TMatrixTSym<double> trackCov    (4);
    double offset   = 0                ;

    double multipleScattering;
    if     (detector->isStrip()) multipleScattering =  8.26e-6;
    else if(detector->isDUT()  ) multipleScattering = 16.04e-6;
    else                         multipleScattering = 12.35e-6;

    //Define local coordinates
    sensorOrigin[0] = 0; sensorOrigin[1] = 0; sensorOrigin[2] = 0;
    upVector    [0] = 0; upVector    [1] = 1; upVector    [2] = 0;
    rightVector [0] = 1; rightVector [1] = 0; rightVector [2] = 0;
    beamVector  [0] = 0; beamVector  [1] = 0; beamVector  [2] = 1;

    //Change to global coordinates

    detector->fromLocalToGlobalKalman(&sensorOrigin[0],&sensorOrigin[1],&sensorOrigin[2],fRinv, fTz);
    detector->fromLocalToGlobalKalman(&upVector    [0],&upVector    [1],&upVector    [2],fRinv, fTz);
    detector->fromLocalToGlobalKalman(&rightVector [0],&rightVector [1],&rightVector [2],fRinv, fTz);
    detector->fromLocalToGlobalKalman(&beamVector  [0],&beamVector  [1],&beamVector  [2],fRinv, fTz);

    //Normalize vectors
    upVector   [0] -= sensorOrigin[0]; upVector   [1] -= sensorOrigin[1]; upVector   [2] -= sensorOrigin[2];
    rightVector[0] -= sensorOrigin[0]; rightVector[1] -= sensorOrigin[1]; rightVector[2] -= sensorOrigin[2];
    beamVector [0] -= sensorOrigin[0]; beamVector [1] -= sensorOrigin[1]; beamVector [2] -= sensorOrigin[2];

    //Calculate h
    double den = upVector[1]*rightVector[0] - upVector[0]    *rightVector[1];
    offset= -sensorOrigin[0]*rightVector[0] - sensorOrigin[1]*rightVector[1]+
            rightVector[2]*(-sensorOrigin[2]+(sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
            sensorOrigin[1]*(upVector[0]*rightVector[2]-upVector[2]*rightVector[0])+
            sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))/den);
    h[1] = rightVector[0]+rightVector[2]*(upVector[1]*rightVector[2]-upVector[2]*rightVector[1])/den;
    h[3] = rightVector[1]+rightVector[2]*(upVector[2]*rightVector[0]-upVector[0]*rightVector[2])/den;
    h[0] =(sensorOrigin[0]*(upVector[2]   *rightVector[1]-upVector[1]   *rightVector[2])+
            sensorOrigin[1]*(upVector[0]   *rightVector[2]-rightVector[0]*upVector[2]   )+
            sensorOrigin[2]*(upVector[1]   *rightVector[0]-upVector[0]   *rightVector[1]))*
            (upVector[1]   *(rightVector[0]*rightVector[0]+rightVector[2]*rightVector[2])-
            rightVector[1]*(upVector[0]   *rightVector[0]+upVector[2]   *rightVector[2]))/(den*den);
    h[2] =(sensorOrigin[0]*(upVector[2]   *rightVector[1]-upVector[1]   *rightVector[2])+
            sensorOrigin[1]*(upVector[0]   *rightVector[2]-upVector[2]   *rightVector[0])+
            sensorOrigin[2]*(upVector[1]   *rightVector[0]-upVector[0]   *rightVector[1]))*
            (rightVector[0]*(upVector[1]   *rightVector[1]+upVector[2]   *rightVector[2])-
            upVector[0]   *(rightVector[1]*rightVector[1]+rightVector[2]*rightVector[2]))/(den*den);


    double mmbb = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);

    trackCov[1][1] =  fTz*fTz*mmbb;
    trackCov[1][0] = -fTz    *mmbb;
    trackCov[3][3] =  fTz*fTz*mmbb;
    trackCov[3][2] = -fTz    *mmbb;
    trackCov[0][1] = -fTz    *mmbb;
    trackCov[0][0] =          mmbb;
    trackCov[2][3] = -fTz    *mmbb;
    trackCov[2][2] =          mmbb;

    theKalmanPlaneInfo_.setH     (h,plaqID     );
    theKalmanPlaneInfo_.setOffset(offset,plaqID);
    theKalmanPlaneInfo_.setTrackCov(trackCov,plaqID);
}
