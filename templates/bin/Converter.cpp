/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

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
 
#include "ReaderTemplate.h"
#include "EventReader.h"
#include "EventConverter.h"
#include "MessageTools.h"

//====================================================================
int main(int argv, char **args)
{
  // This programs converts a Monicelli-format file into a JC-format file.
  // The name of the input file is specified by the user in the command line:
  //
  // > ./Converter file-name
  //
  
  if( argv < 2 )
  {
     STDLINE(""                                                          , ACWhite) ;
     STDLINE("FATAL: no input file specified as argument to this command", ACRed  ) ;
     STDLINE(""                                                          , ACWhite) ;
     STDLINE("Usage:"                                                    , ACCyan ) ;
     STDLINE("       ./Converter input-file"                             , ACWhite) ;
     STDLINE(""                                                          , ACWhite) ;
     exit(0) ;
  }

  std::string fileName = args[1] ;
  std::string rootName = ""      ;
  std::string  geoName = ""      ;
  std::string path     = ""      ;
  
  int pos = fileName.find_last_of("/") ;

  path = fileName.substr(0, pos+1)  ;

  fileName.replace(0,pos+1,"") ;
  
  pos = fileName.find(".root") ;
  if( pos <=0 )
  {
     STDLINE(""                                                          , ACWhite) ;
     STDLINE("FATAL: input file specified without .root suffix"          , ACRed  ) ;
     STDLINE(""                                                          , ACWhite) ;
     exit(0) ;
  }

  fileName.replace(pos,fileName.size(),"" ) ;
  fileName.replace(pos,fileName.size(),"" ) ;
  
  rootName = fileName + ".root" ;
   geoName = fileName + ".geo"  ;
 
  // Instantiate a reader: this class is delegated to handling the root input 
  // file and provide pointers to the classes contained in the file to the
  // eventAnalyzer, wehre the actual data analysis is implemented and carried 
  // out by the user. 
  // As a user you are not supposed nor required to deal with details of this
  // class which is provided to you as a service to manage the I/O of events
  // from file.
  EventReader * reader = new EventReader() ; 

  std::stringstream ss_ ; 
  ss_ << "Converting " << path << "/" << rootName << " ";
  ss_ << "using "      << path << "/" <<  geoName;
  STDLINE(ss_.str(),ACYellow) ;
  
  reader->setPath (path    ) ;
  reader->openEventsFile(rootName   ) ;
  reader->openGeoFile   (geoName    ) ;
 
  // Finally invoke the eventAnalyzer class to process your own analysis on
  // the reconstructed track (use the predicted impact points on the DUT(s)
  // to compare with actual hits). This is the class you are supposed to 
  // modify to place your customized analysis algorithm.
  // Note that we hand out to this class the pointer to the reader in calling
  // sequence: the reader, in turn, owns the pointers to the even components 
  // stored in the file and these will be made available to the analyzer to
  // allow user manipulation of the event components (hits, clusters and 
  // reconstructed tracks)
  
  STDLINE(ss_.str(),ACYellow) ;
  EventConverter * converter = new EventConverter(reader) ;
  converter->convert();
  
  
  //CalibrationReader * calibReader = new CalibrationReader(reader) ;
  //calibReader->makeHistos();
  

  return 0 ;
}
