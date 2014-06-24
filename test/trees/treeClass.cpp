#include <iostream>

#include "Event.h"

#include <TApplication.h>
#include <TFile.h>
#include <TRandom.h>
#include <TTree.h>

void doIt() ;

int main(int argc, char** argv)
{
  TApplication app("App",&argc, argv);
 
  doIt() ;
 
  app.Run() ;
}

//==================================================================
void doIt() 
{
  TFile file("treeClass.root","recreate") ;
  
  TRandom * r = new TRandom() ;

  TTree theTree("treeClass", "A tree with a class") ;

  Event * event = new Event() ;
  
  theTree.Branch("eventBranch", "Event", &event, 16000, 0) ; // No splitting

  for( int ev=0; ev<10; ++ev)
  {
   event->setEventNumber(ev) ;
   std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tEvent: " << ev         << std::endl ;
   for(int i=0; i<(int)r->Gaus(5,2); ++i)
   {
    double t = r->Gaus(50,12) ;
    std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\t   t: " << t          << std::endl ;
    event->addTrack(t) ;
   }
   double x = r->Gaus(150,12) ;
   std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\t   xw: " << x          << std::endl ;
   event->setX(x) ;
   std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\t   xr: " << event->x() << std::endl ;
   theTree.Fill() ;
   event->cleanData() ;
  }
  
//  theTree.BuildIndex("getEventNumber") ;
  
  file.Write() ;
  file.Close() ;
  
  std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tFile treeClass.root written" << std::endl ;
}
