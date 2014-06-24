/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

// g++ -o foldersInMemory -I../include foldersInMemory.cpp -I$BOOSTINC -L$BOOSTLIB -lboost_regex `root-config --cflags --libs` && ./foldersInMemory

#include <iostream> 
#include <map> 
#include <sstream> 
#include <string> 

#include <boost/regex.hpp>

#include <TApplication.h>
#include <TBrowser.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TFolder.h>
#include <TH1D.h>
#include <TObject.h>
#include <TRandom.h>
#include <TRint.h>
#include <TROOT.h>
#include <TTree.h>

typedef std::vector<std::string>         stringVDef ;
typedef std::map<std::string, TFolder *> pathMatchDef ;

void  	     doIt	     (void		      ) ;
void 	     inspectTree     (TFolder	  * thisFolder) ;
void 	     addItem	     (std::string   fullPath, 
     	     		      TObject	  * object    ) ;
stringVDef   splitPath       (std::string   s	      ) ;
bool 	     exploreHierarchy(stringVDef  & path, 
                              TFolder	  * rootFolder) ;

std::map<std::string, TObject *> map_ ;

std::stringstream ss_ ;

int  level_       ;

pathMatchDef pathMatch_ ;

//==================================================================
int main(int argc, char** argv)
{
  TApplication app("Folders",&argc, argv);
 
  level_       = 0     ;

  doIt() ;

  app.Run() ;
  
  return 0 ;
}

//==================================================================
void doIt() 
{
 
 //------------------------------------------------------------------
 // Creation of tree
 TFolder * mainFolder  = gROOT->GetRootFolder()->AddFolder("MainFolder","Main Folder" ) ;
 TH1D    * hMain       = new TH1D("hMain", "hMain", 100,0,100) ;
 hMain->SetDirectory(0) ;
 mainFolder->Add(hMain);
 mainFolder->SetOwner(kTRUE);

 TFolder * subFolder1  = mainFolder            ->AddFolder("SubFolder1","Sub Folder 1") ;
 TH1D    * hSub1       = new TH1D("hSub1", "hSub1", 100,0,100) ;
 hSub1->SetDirectory(0) ;
 subFolder1->Add(hSub1) ;

 TFolder * subFolder2  = mainFolder            ->AddFolder("SubFolder2","Sub Folder 2") ;
 TH1D    * hSub2       = new TH1D("hSub2", "hSub2", 100,0,100) ;
 TH1D    * hSub3       = new TH1D("hSub3", "hSub3", 100,0,100) ;
 hSub2->SetDirectory(0) ;
 hSub3->SetDirectory(0) ;
 subFolder2->Add(hSub2) ;
 subFolder2->Add(hSub3) ;
  
 TH1D    * hSub4       = new TH1D("hSub4", "hSub4", 100,0,100) ;
 TH1D    * hSub5       = new TH1D("hSub5", "hSub5", 100,0,100) ;
 hSub4->SetDirectory(0) ;
 hSub5->SetDirectory(0) ;

 std::string fullPath = "MainFolder/SubFolder2/EmptySubFolder/SubFolder4" ;

// addItem(fullPath, hSub4) ;

 fullPath = "MainFolder/SubFolder1" ;

// addItem(fullPath, hSub5) ;

 fullPath = "MainFolder/histograms/SubFolder6" ;

 addItem(fullPath, hSub1) ;
 
//------------------------------------------------------------------
//  Now inspection 
//  TFolder * topFolder   = gROOT->GetRootFolder() ;
//  
//  level_ = 0 ;
//  inspectTree(topFolder) ;
//  
 TFile * file = new TFile("foldersInMemory2.root","recreate" ) ;
 
 mainFolder->Write() ;
 
 file->Close() ;
// 
//  TFolder * secondFolder = subFolder2->AddFolder("SecondFolder","Second Folder") ;
//  TH1D    * hSecond      = new TH1D("hSecond", "hSecond", 100,0,100) ;
//  hSecond->SetDirectory(0) ;
//  secondFolder->Add(hSecond) ;
}

//===================================================================
void addItem(std::string fullPath , TObject * object)
{
  TFolder * target = (TFolder *)gROOT->GetRootFolder()->FindObject(fullPath.c_str()) ;
  
  if( target != NULL ) // The requested target folder already exists in memory
  {
     TObject * theObject = target->FindObjectAny(object->GetName()) ;
     if( theObject )   // The requested object already exists in the target folder, do nothing.
     {
       std::cout << __LINE__ 
     		 << "]\t "
     		 << object->GetName()
     		 << " already exists in "
     		 << target->GetName()
     		 << std::endl ;
     }
     else // The requested object does not yet exists in the target folder: add it.
     {
       std::cout << __LINE__ 
     		 << "]\t Finally adding "
     		 << object->GetName()
     		 << " to "
     		 << target->GetName()
     		 << " ptr: "
     		 << theObject
     		 << std::endl ;
       target->Add(object) ;
     }
     return ; // In both cases we are done
  }
  else // The target folder does not seem to exist in memory
  {
     std::cout << __LINE__ 
               << "]\t Path " 
	       << fullPath 
	       << " not found : explore point of broken link" 
	       << std::endl ;

     stringVDef path = splitPath(fullPath) ; // Split the full path of the target folder:
                                             
     std::string partialPath = ""      ;     // the idea is to explre the requested target folder path to establish were to
     std::string lastPath    = path[0] ;     // attach the first missing piece to the already partially existing path.
     std::string lastName    = "root"  ;

     for (stringVDef::iterator it=path.begin(); it!=path.end(); ++it)
     {
       partialPath += *it ;
       
       target = (TFolder *)gROOT->GetRootFolder()->FindObject(partialPath.c_str()) ; // Get a pointer to the i-th element of the 
                                                                                     // requested target folder path

       if( target == NULL ) // If i-th element does not exist, it is time to create it and attach it to the last valid parent
       {
         std::string thisName = *(it) ;
	 
     	 std::cout << __LINE__ 
     	 	   << "]\t Searching for "
	 	   << lastPath
	 	   << " in "
		   << gROOT->GetRootFolder()->GetName()
	 	   << std::endl ;
	 target = (TFolder *)gROOT->GetRootFolder()->FindObject(lastPath.c_str()) ;
	 if( target == NULL )
	 {
     	   std::cout << __LINE__ 
     	   	     << "]\t Could not find  "
	   	     << lastPath
		     << " in "
		     << gROOT->GetRootFolder()->GetName()
		     << " <- define this as target"
	   	     << std::endl ;
	   target = (TFolder *)gROOT->GetRootFolder() ;
	 }
     	 std::cout << __LINE__ 
     	 	   << "]\t Adding:  "
	 	   << thisName
		   << " to "
		   << target->GetName() 
	 	   << std::endl ;
	 target->AddFolder(thisName.c_str(),thisName.c_str()) ;
	 lastPath = target->GetName() ;
       }
       else
       {
     	 std::cout << __LINE__ 
     	 	   << "]\t Path "
	 	   << partialPath
		   << " already exists in memory. Ptr: "
		   << target
	 	   << std::endl ;
       }
       lastPath     = partialPath ;
       partialPath += std::string("/") ;
     }
  }
  
  target = (TFolder *)gROOT->GetRootFolder()->FindObject(fullPath.c_str()) ;

  TObject * theObject = target->FindObjectAny(object->GetName()) ;
  
  if( theObject )
  {
    std::cout << __LINE__ 
    	      << "]\t "
    	      << object->GetName()
    	      << " already exists in "
    	      << target->GetName()
    	      << std::endl ;
  }
  else
  {
    std::cout << __LINE__ 
    	      << "]\t Finally adding "
    	      << object->GetName()
    	      << " to "
    	      << target->GetName()
	      << " ptr: "
	      << target
    	      << std::endl ;
    target->Add(object) ;
  }
  
}

//===================================================================
stringVDef splitPath(std::string s) 
{
   stringVDef path ;

   boost::regex re("\\/");
   boost::sregex_token_iterator i(s.begin(), s.end(), re, -1);
   boost::sregex_token_iterator j;

   while(i != j) {path.push_back( *i++ );}
 
   return path ;
}

//===================================================================
void inspectTree(TFolder * thisFolder) 
{
 std::string blanks = "" ;
 
 level_++ ;
 for(int l=0; l<level_*6; ++l) {blanks += " ";}

 TCollection * rootFoldersColl     = (TCollection *)thisFolder->GetListOfFolders() ;
 TIterator   * rootFoldersIterator = rootFoldersColl->MakeIterator() ;
 
 TObject * obj = NULL ;
 
 std::cout << blanks << " | "                              << std::endl ;
 std::cout << blanks << " +---> " << thisFolder->GetName() << std::endl ;
 while(obj = rootFoldersIterator->Next())
 {
//  cout << "   Object: '" << obj->GetName() << "'" << endl ;

  std::string hName = " root memory " ;
  if( obj->IsA() == TH1D::Class() ) 
  {
   if( ((TH1D*)obj)->GetDirectory() ) hName = ((TH1D*)obj)->GetDirectory()->GetName() ;
   std::cout << blanks << " |     | " 								          << std::endl ;
   std::cout << blanks << " |     +---> histogram: " << obj->GetName() << " in " << thisFolder->GetName() << std::endl ;
  }
  else
  {
    if( obj->IsA() == TFolder::Class() ) 
    { 
      std::string folderName = obj->GetName() ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("Classes"))	 continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("Colors"))	 continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("MapFiles"))	 continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("Sockets"))	 continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("Canvases"))	 continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("Styles"))	 continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("Functions"))	 continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("Tasks")) 	 continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("Geometries"))	 continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("Browsers"))	 continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("Specials"))	 continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("Handlers"))	 continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("Cleanups"))	 continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("StreamerInfo"))   continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("SecContexts"))	 continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("PROOF Sessions")) continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("ROOT Memory"))	 continue ;
      if( obj->IsA() == TFolder::Class() && folderName == std::string("ROOT Files"))	 continue ;
     
      inspectTree((TFolder*)obj) ;
    }
  }
  if( obj->IsA() == TDirectory::Class() ) 
  {
      std::cout << blanks << "    Enter directory: " << obj->GetName() << std::endl ;
  }
 }

 level_-- ;
}

