#ifndef MANIPULATEPATH_H
#define MANIPULATEPATH_H

#include <QString>
#include <QStringList>

class manipulatePath
{
  public:
    explicit manipulatePath(QString path) {path_ = path;}
    QString  getPath       (void        ) ;
    QString  getFileName   (void        ) ;

  private:
    QString path_ ;
} ;

#endif // MANIPULATEPATH_H
