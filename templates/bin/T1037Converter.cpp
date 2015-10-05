#include "EventReader.h"
#include "T1037Converter.h"

#include <iostream>
//====================================================================
int main(int argv, char **argc)
{
  
  if ( argv < 3)
    {
      std::cout << __LINE__<< " " << __FILE__ << " usage: " << argc[0] << " root-file geo-file" << std::endl;  
      return 1;
    }

  //TApplication app("T1037Converter",&argv, argc);
  EventReader * reader = new EventReader() ; 
  reader->setPath ("/data/TestBeam/2014_02_February_T1037/MonicelliOutput/");
  reader->openEventsFile(argc[1]);
  reader->openGeoFile   (argc[2]);
  T1037Converter* analyzer = new T1037Converter(reader) ;
  
  // app.Run() ;
    
  return 0 ;
}
