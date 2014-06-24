#ifndef LENGTHYCOMPUTATION_H
#define LENGTHYCOMPUTATION_H

#include <QThread>
#include <QMainWindow>

class lengthyComputation : public QThread
{
public:
   lengthyComputation(QMainWindow * mW) ;
  ~lengthyComputation(void) {;}

  void run (void) ;
  void doIt(void) ;

private:
  QMainWindow * mW_ ;
};

#endif // LENGTHYCOMPUTATION_H
