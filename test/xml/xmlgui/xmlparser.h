#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "sstream"

#include <QDomDocument>
#include <QFile>

class QFile ;

class xmlParser
{
public:
   xmlParser(QFile & file) ;
  ~xmlParser(void) ;

  unsigned int getNumberOfStations(void) ;

private:

  QDomDocument * document_ ;

  std::stringstream ss_ ;
};



#endif // XMLPARSER_H
