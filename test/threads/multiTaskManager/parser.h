#ifndef PARSER_H
#define PARSER_H

#include <QImage>
#include <QString>
#include <QStringList>
#include <QWidget>

#include "cProcess.h"

class parser : public cProcess
{
  Q_OBJECT

public:

    parser(std::string name                         ) {cProcess::setClassName(name);  setDerivedPtr(this);}

  //std::string getClassName(void                         ) {return std::string("parser");}

  int         execute(     const QString & imageFileName) ;

private:
};

#endif // PARSER_H
