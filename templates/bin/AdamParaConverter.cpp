#include "EventReader.h"
#include "AdamParaConverter.h"

//====================================================================
int main(int argv, char **argc)
{
  
  if ( argv < 3)
    {
      std::cout << __LINE__<< " " << __FILE__ << " usage: " << argc[0] << " root-file geo-file" << std::endl;  
      return 1;
    }

  TApplication app("AdamParaConverter",&argv, argc);
  EventReader * reader = new EventReader() ; 
  reader->setPath ("/data/TestBeam/2014_01_January/MonicelliOutput/");
  reader->openEventsFile(argc[1]);
  reader->openGeoFile   (argc[2]);
  AdamParaConverter * analyzer = new AdamParaConverter(reader) ;
  
  // app.Run() ;
    
  return 0 ;
}
