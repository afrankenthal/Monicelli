/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include "ReaderTemplate.h"
#include "MessageTools.h"
#include "EventReader.h"
#include "Geometry.h"
#include <fstream>
#include <iostream>

#define PI 3.1415

//====================================================================
int main(int argv, char **args)
{
  // This programs converts a Monicelli-format file into a JC-format file.
  // The name of the input file is specified by the user in the command line:
  //
  // > ./GeometryConverter file-name
  //
  
  if( argv < 2 )
  {
     STDLINE(""                                                          , ACWhite) ;
     STDLINE("FATAL: no input file specified as argument to this command", ACRed  ) ;
     STDLINE(""                                                          , ACWhite) ;
     STDLINE("Usage:"                                                    , ACCyan ) ;
     STDLINE("       ./GeometryConverter geometry-file"                  , ACWhite) ;
     STDLINE(""                                                          , ACWhite) ;
     exit(0) ;
  }

  std::string fileName = args[1] ;
  std::string path     = ""      ;
  
  unsigned int pos = fileName.find_last_of("/") ;

  path = fileName.substr(0, pos+1);

  fileName.replace(0,pos+1,"") ;
  
  
  pos = fileName.find(".geo") ;
  if( pos == std::string::npos )
  {
     STDLINE(""                                                          , ACWhite) ;
     STDLINE("FATAL: input file specified without .geo suffix"           , ACRed  ) ;
     STDLINE(""                                                          , ACWhite) ;
     exit(EXIT_FAILURE) ;
  }

  std::string geoFileName = fileName;

  fileName.replace(pos,fileName.size(),"" ) ;
  std::string outFileName = fileName + ".pcf";

  // Instantiate a reader: this class is delegated to handling the root input 
  // file and provide pointers to the classes contained in the file to the
  // eventAnalyzer, wehre the actual data analysis is implemented and carried 
  // out by the user. 
  // As a user you are not supposed nor required to deal with details of this
  // class which is provided to you as a service to manage the I/O of events
  // from file.
  EventReader reader; 

  std::stringstream ss_ ; 
  ss_ << "Converting " << path << geoFileName << " ";
  ss_ << "to "         << "./" << outFileName;
  STDLINE(ss_.str(),ACYellow) ;
  
  reader.setPath    (path);
  reader.openGeoFile(geoFileName) ;
 
  Geometry* theGeometry = reader.getGeometryPointer();
  
  std::ofstream outFile(outFileName.c_str());
  outFile << "PLANES " << theGeometry->getDetectorsNumber() << "\r\n";
  for(Geometry::iterator it=theGeometry->begin(); it!=theGeometry->end(); it++)
  {
    double x0 = 0;
    double y0 = 0;
    double z0 = 0;
    double xUp = 0;
    double yUp = 1;
    double zUp = 0;
    double xRight = 1;
    double yRight = 0;
    double zRight = 0;
    //double alpha,beta,gamma;
    unsigned int numberOfCols, numberOfRows;
    
    //alpha = (theGeometry->getXRotation() + theGeometry->getXRotationCorrection())*PI/180.;
    //beta  = (theGeometry->getYRotation() + theGeometry->getYRotationCorrection())*PI/180.;
    //gamma = (theGeometry->getZRotation() + theGeometry->getZRotationCorrection())*PI/180.;
    //it->second->setXRotation(0);
    //it->second->setXRotationCorrection(0);
    //it->second->setYRotation(0);
    //it->second->setYRotationCorrection(0);
    //it->second->setZRotation(0);
    //it->second->setZRotationCorrection(0);
    it->second->fromLocalToGlobal(&x0,&y0,&z0);
//    std::cout << it->first
//    << " x0: " << x0
//    << " y0: " << y0
//    << " z0: " << z0
//    << std::endl;
    it->second->fromLocalToGlobal(&xUp,&yUp,&zUp);
    it->second->fromLocalToGlobal(&xRight,&yRight,&zRight);
    numberOfCols = it->second->getNumberOfROCsLocalX();
    numberOfRows = it->second->getNumberOfROCsLocalY();
    outFile << "BOTLEFT " << x0*10  	  << "\r\n"
     	    << "BOTLEFT " << y0*10  	  << "\r\n"
     	    << "BOTLEFT " << z0*10  	  << "\r\n"
     	    << "RIGHT "   << xRight-x0    << "\r\n"
     	    << "RIGHT "   << yRight-y0    << "\r\n"
     	    << "RIGHT "   << zRight-z0    << "\r\n"
     	    << "UP "	  << xUp-x0 	  << "\r\n"
     	    << "UP "	  << yUp-y0 	  << "\r\n"
     	    << "UP "	  << zUp-z0 	  << "\r\n"
     	    << "ROWS "    << numberOfRows << "\r\n"
     	    << "COLS "    << numberOfCols << "\r\n";
  }

  
  return 0 ;
}
