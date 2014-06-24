#include <iostream>

#include "lengthyComputation.h"

lengthyComputation::lengthyComputation(QMainWindow * mW)
{
  mW_ = mW ;
}

void lengthyComputation::run()
{
  std::cout << "Beginning og lengthy computation: " << std::endl ;

  this->doIt() ;
}


void lengthyComputation::doIt(void)
{
   for(int i=0; i<100000; ++i)
   {
     std::cout << "Computing " << i << std::endl ;
   }
}
