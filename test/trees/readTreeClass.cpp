#include <iostream>

#include "Event.h"

#include <TApplication.h>
#include <TBranch.h>
#include <TFile.h>
#include <TRandom.h>
#include <TTree.h>
#include <cstdlib>

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
  TFile file("treeClass.root") ;
  
  TTree * theTree = (TTree*)file.Get("treeClass") ;

  if( !theTree )
    {
      std::cout << "No 'treeClass' tree found in file 'treeClass.root'" << std::endl ;
      exit(0) ;
    }

  Event * event = new Event() ;
  
  TBranch * branch = theTree->GetBranch("eventBranch") ;
  std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tBefore setAddress" << std::endl ;
  branch->SetAddress(&event) ;
  std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tAfter  setAddress" << std::endl ;
  
  int events = theTree->GetEntries() ;
  std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tFound " << events << " events in tree" << std::endl ;
  
  for( int ev=0; ev<events; ++ev)
  {
   theTree->GetEntry(ev) ;
   int evNum   = event->getEventNumber() ;
   int nTracks = event->getNTracks() ;
   event->dumpTracks() ;
  }
  
  file.Close() ;
}
