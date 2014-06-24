/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/sax/SAXParseException.hpp>
#include "DOMTreeErrorReporter.hpp"
#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "MessageTools.h"

void DOMTreeErrorReporter::warning(const SAXParseException&)
{
    //
    // Ignore all warnings.
    //
}

void DOMTreeErrorReporter::error(const SAXParseException& toCatch)
{
    fSawErrors = true;
    STDLINE("\n--------------- F A T A L   E R R O R ---------------------",ACRed) ;
    XERCES_STD_QUALIFIER cerr << "Error at file \"" 
                              << StrX(toCatch.getSystemId())
			      << "\", line " << toCatch.getLineNumber()
			      << ", column " << toCatch.getColumnNumber()
         		      << "\n   "     << StrX(toCatch.getMessage())
			      << "\n\n"
			      << XERCES_STD_QUALIFIER endl;
    STDLINE("\n--------------- A B O R T I N G ...",ACRed) ;
    exit(-1) ;
}

void DOMTreeErrorReporter::fatalError(const SAXParseException& toCatch)
{
    fSawErrors = true;
    STDLINE("\n--------------- F A T A L   E R R O R ---------------------",ACRed) ;
    XERCES_STD_QUALIFIER cerr << "Fatal Error at file \"" 
                              << StrX(toCatch.getSystemId())
			      << "\", line " << toCatch.getLineNumber()
			      << ", column " << toCatch.getColumnNumber()
         		      << "\n   "     << StrX(toCatch.getMessage()) 
			      << "\n\n"
			      << XERCES_STD_QUALIFIER endl;
    STDLINE("\n--------------- A B O R T I N G ...",ACRed) ;
    exit(-1) ;
}

void DOMTreeErrorReporter::resetErrors()
{
    fSawErrors = false;
}


