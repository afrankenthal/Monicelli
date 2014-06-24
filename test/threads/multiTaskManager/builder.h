#ifndef BUILDER_H
#define BUILDER_H

#include <QImage>
#include <QString>
#include <QStringList>
#include <QWidget>

#include "cProcess.h"

class builder : public cProcess
{
  Q_OBJECT

public:

    builder(std::string name) {cProcess::setClassName(name); setDerivedPtr(this);}

//std::string getClassName(void                         ) {return std::string("builder");}

int         execute(     const QString & imageFileName) ;

private:

};

#endif // BUILDER_H
