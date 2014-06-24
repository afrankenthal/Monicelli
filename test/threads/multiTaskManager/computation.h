#ifndef COMPUTATION_H
#define COMPUTATION_H

#include <QWidget>
#include <QString>

class computation : public QWidget
{
public:
   computation(QWidget * parent = 0)  ;
  ~computation(void                ) {;}

   static int compute(const QString & fileName) ;

private:
};

#endif // COMPUTATION_H
