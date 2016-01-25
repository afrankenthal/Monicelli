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


