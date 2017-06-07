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
 
#ifndef GEOMETRYLOADER_H
#define GEOMETRYLOADER_H

#include <errno.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <vector>

#include <xercesc/sax/SAXParseException.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMDocumentTraversal.hpp>
//#include <xercesc/dom/DOMWriter.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>

#include "DOMTreeErrorReporter.hpp"
#include "Geometry.h"
#include "MessageTools.h"

XERCES_CPP_NAMESPACE_USE

using namespace std ;

class geometryLoader
{
  public :
    typedef map< string, pair<bool, string> > sCalFitFuncDef ; // station
    typedef map< string, pair<bool, string> > dCalFitFuncDef ; // detector
    typedef map< string, pair<bool, string> > rCalFitFuncDef ; // ROC

                        geometryLoader    (void                  )  ;
                       ~geometryLoader    (void                  ) {;}

    void                loadXML           (string configFile     )  ;
    void                fillGeometry      (string toRead="all"   )  ;
    Geometry          * getGeometry       (void                  ) {return theGeometry_;}
    void                setCalibrationMaps(void                  )  ;

  private :

     bool               validContent  (string       tagName,
                                       string       content  )  ;
     void               acquireInfo   (DOMElement * element  )  ;
     string             stripBlanks   (string       theString)  ;
     string             toLower       (string       theString)  ;

     string             currentPlaqID_           ;
     int                currentROC_              ;
     string             station_                 ;
     DOMElement       * testBeamGeometryXml_     ;
     Geometry         * theGeometry_             ;
     string             toRead_                  ;
     string             gCalibrationFitFunction_ ;
     string             gDUTFitFunction_         ;
     sCalFitFuncDef     sCalibrationFitFunction_ ;
     dCalFitFuncDef     dCalibrationFitFunction_ ;
     rCalFitFuncDef     rCalibrationFitFunction_ ;

     stringstream       ss_                      ;
} ;

#endif // GEOMETRYLOADER_H
