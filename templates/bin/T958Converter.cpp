#include "EventReader.h"
#include "T958Converter.h"

//====================================================================
int main(int argv, char **argc)
{
  
  if ( argv < 3)
    {
      std::cout << __LINE__<< " " << __FILE__ << " usage: " << argc[0] << " root-file geo-file" << std::endl;  
      return 1;
    }

  TApplication app("T958Converter",&argv, argc);
  EventReader * reader = new EventReader() ; 
  reader->setPath ("/data/TestBeam/2014_08_August/MonicelliOutput/");
  reader->openEventsFile(argc[1]);
  reader->openGeoFile   (argc[2]);
  T958Converter * analyzer = new T958Converter(reader) ;
  
  // app.Run() ;
    
  return 0 ;
}
