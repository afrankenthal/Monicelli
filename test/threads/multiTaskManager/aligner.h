#ifndef ALIGNER_H
#define ALIGNER_H

#include <QImage>
#include <QString>
#include <QStringList>
#include <QWidget>

#include "cProcess.h"

class aligner : public cProcess
{
  Q_OBJECT

public:

                aligner         (std::string name                ) {cProcess::setClassName(name);  setDerivedPtr(this);}

     int  wrapperToExecute(const QString & imageFileName   ) {return derivedClassPtr_[currentClassName_]->execute(imageFileName)         ;}

    //std::string getClassName(void                         ) {return std::string("aligner");}

int         execute(     const QString & imageFileName) ;

private:
};

#endif // ALIGNER_H
