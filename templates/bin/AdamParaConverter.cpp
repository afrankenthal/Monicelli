#include "EventReader.h"
#include "AdamParaConverter.h"
#include <iostream>

//====================================================================
int main(int argv, char **argc)
{
  
  if ( argv < 3)
    {
      std::cout << __LINE__<< " " << __FILE__ << " usage: " << argc[0] << " root-file geo-file" << std::endl;  
      return 1;
    }

  std::cout << "Argument 0: " << argc[0] << "Argument 1: " << argc[1] << "Argument 2: " << argc[2] << std::endl;
  EventReader * reader = new EventReader() ; 
  //reader->setPath ("/data/TestBeam/2014_01_January/MonicelliOutput/");
  reader->setPath ("/home/uplegger/Programming/MonicelliOutput/");
  reader->openEventsFile(argc[1]);
  reader->openGeoFile   (argc[2]);
  AdamParaConverter * analyzer = new AdamParaConverter(reader) ;
  
  //TApplication app("AdamParaConverter",&argv, argc);
  // app.Run() ;
    
  return 0 ;
}
