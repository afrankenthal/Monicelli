#include "EventReader.h"
#include "StripTestReader.h"

//====================================================================
int main(int argv, char **argc)
{
  
  if ( argv < 3)
  {
    std::cout << __LINE__<< " " << __FILE__ << " usage: " << argc[0] << " root-file geo-file" << std::endl;  
    return 1;
  }

  TApplication app("StripTestReader",&argv, argc);
  EventReader * reader = new EventReader() ; 
  //reader->setPath ("/data/TestBeam/2013_11_November/MonicelliOutput/");
  reader->setPath ("/data/TestBeam/2014_01_January/MonicelliOutput/");
  //reader->setPath ("/data/TestBeam/2013_11_November/tmp/");
  reader->openEventsFile(argc[1]);
  reader->openGeoFile   (argc[2]);
  StripTestReader * analyzer = new StripTestReader(reader) ;
  
  // app.Run() ;
    
  return 0 ;
}
