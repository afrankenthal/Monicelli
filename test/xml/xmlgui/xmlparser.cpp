/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

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

