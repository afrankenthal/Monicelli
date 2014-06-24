/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

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


class geometryLoader
{
  public :
                   geometryLoader(void                    )  ;
                  ~geometryLoader(void                    ) {;}

    void           loadXML       (std::string configFile  )  ;
    void           fillGeometry  (std::string toRead="all")  ;
    Geometry     * getGeometry   (void                    ) {return theGeometry_;}

  private :

     bool          validContent  (std::string   tagName,
                                  std::string content     )  ;
     void          acquireInfo   (DOMElement  * element   )  ;
     std::string   stripBlanks   (std::string   theString )  ;
     std::string   toLower       (std::string   theString )  ;


     std::string    currentPlaqID_      ;
     int            currentROC_         ;
     std::string    station_            ;
     DOMElement   * testBeamGeometryXml_;
     Geometry     * theGeometry_        ;
     std::string    toRead_             ;

     std::stringstream ss_ ;
     //bool closingFlag_     ;
} ;

#endif // GEOMETRYLOADER_H
