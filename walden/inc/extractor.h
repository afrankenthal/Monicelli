#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <map>
#include <string>
#include <sstream>
#include <vector>

#include <QByteArray>
#include <QString>
#include <TDirectory.h>

#include <indent.h>

using namespace std;

class Extractor
{
public:
                               Extractor   (void                      ) ;
                              ~Extractor   (void                      ){;}
    QString                    getFileName (void                      ) ;
    void                       searchFile  (TDirectory *,string,string) ;
    void                       printMap    (void                      ) ;
    QByteArray                 getByteArray(void                      ) ;


private:
    int            level_      ;
    indent         blank_      ;
    stringstream   ss_         ;
    string         tabs_       ;

};

#endif // EXTRACTOR_H
