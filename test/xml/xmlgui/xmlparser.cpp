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
 
#include "MessageTools.h"

#include "xmlparser.h"

//================================================================================
xmlParser::xmlParser(QFile & file)
{
  document_ = new QDomDocument( "GeometryFile" );

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


  file.close();
}

//================================================================================
xmlParser::~xmlParser()
{
  delete document_ ;
}

//================================================================================
unsigned int xmlParser::getNumberOfStations(void)
{
//  return document_->elementsByTagName(QString("station")).count() ;
  return 0 ;
}

