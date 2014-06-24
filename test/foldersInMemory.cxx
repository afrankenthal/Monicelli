#include <vector> 

inspectTree  (TFolder     * thisFolder,    int         & level) ;
inspectTree  (TDirectory  * thisDirectory, int         & level) ;
drawHistogram(TFolder     * thisFolder,    std::string   hName) ;
drawHistogram(TDirectory  * thisDirectory, std::string   hName) ;

void foldersInMemory()
{
 gROOT->Reset() ;
 
 std::vector<std::string> fileList ;
 
// fileList.push_back("complexFile.root") ;
 fileList.push_back("../Run23.root") ;
 TFile * fileA = new TFile(fileList.back().c_str()) ;
 
 TFolder * mainFolder  = gROOT->GetRootFolder()->AddFolder("MainFolder","Main Folder"        ) ;
 TH1D    * hMain       = new TH1D   ("hMain",     "hMain",    100,0,100) ;
 hMain->SetDirectory(0) ;
 mainFolder->Add(hMain);
 mainFolder->SetOwner(kTRUE);

 TFolder * subFolder1 = mainFolder->AddFolder             ("SubFolder1","Sub Folder 1"        ) ;
 TH1D    * hSub1      = new TH1D    ("hSub1",     "hSub1",    200,0,200) ;
 hSub1->SetDirectory(0) ;
 subFolder1->Add(hSub1) ;

 TFolder * subFolder2 = mainFolder->AddFolder             ("SubFolder2","Sub Folder 2"        ) ;
 TH1D    * hSub2      = new TH1D    ("hSub2",     "hSub2",    200,0,200) ;
 TH1D    * hSub3      = new TH1D    ("hSub3",     "hSub3",    200,0,200) ;
 hSub2->SetDirectory(0) ;
 hSub3->SetDirectory(0) ;
 subFolder2->Add(hSub2) ;
 subFolder2->Add(hSub3) ;
 
// fileList.push_back("testFile.root") ;
//  TFile * fileB = new TFile(fileList.back().c_str(),"recreate") ;
//  
//  mainFolder->Write() ;
//  ]\t
//  fileA->Close() ;
//  fileB->Close() ;
// 
//  TFolder * secondFolder = subFolder2->AddFolder("SecondFolder","Second Folder"  ) ;
//  TH1D    * hSecond      = new TH1D   ("hSecond", "hSecond", 100,0,100) ;
//  hSecond->SetDirectory(0) ;
//  secondFolder->Add(hSecond) ;
  
 TFolder * topFolder = gROOT->GetRootFolder() ;
 
 int level = 0 ;
 std::cout << __LINE__ << "]\t" << "================ First try memory: =====================\n" << std::endl ;
 inspectTree(topFolder, level) ;

 std::cout << __LINE__ << "]\t" << "================ Now files: =====================\n" << std::endl ;
 for(std::vector<std::string>::iterator fi=fileList.begin(); fi!=fileList.end(); ++fi)
 {
   level = 0 ;
   TFile * f = new TFile((*fi).c_str()) ;
   std::cout << __LINE__ << "]\t" << "\>>>>>>>>>>>>>>>  Trying with " << f->GetName() << std::endl ;
   inspectTree  ((TDirectory*)f, level) ;
   drawHistogram((TDirectory*)f,"Station: 2 - Plaq: 3") ;
   std::cout << __LINE__ << "]\t" << "Done   with " << f->GetName() << std::endl ;
 }
 
}

//===================================================================
inspectTree(TFolder * thisFolder, int & level) 
{
 std::string blanks = "" ;
 
 for(int l=0; l<level; ++l) {blanks += "  ";}

 level++ ;
 
 cout << __LINE__ << "]\t" << blanks << "=============== " << thisFolder->GetName() << " ===============" << endl ;

 TCollection * rootFoldersColl     = (TCollection *)thisFolder->GetListOfFolders() ;
 TIterator   * rootFoldersIterator = rootFoldersColl->MakeIterator() ;
 
 TObject * obj = NULL ;
 
 while(obj = rootFoldersIterator->Next())
 {
  std::string hName = " root memory " ;
  if( obj->IsA() == TH1D::Class() ) 
  {
   if( ((TH1D*)obj)->GetDirectory() ) hName = ((TH1D*)obj)->GetDirectory()->GetName() ;
   cout << __LINE__ << "]\t" << blanks << "    a histogram: " << obj->GetName() << " in " << hName<< endl ;
  }
  else
  {
    if( obj->IsA() == TFolder::Class() ) 
    { 
      cout << __LINE__ << "]\t" << blanks << "    Enter in folder   : " << obj->GetName() << endl ;
      inspectTree((TFolder*)obj, level) ;
      level-- ;
    }
  }
  if( obj->IsA() == TDirectory::Class() ) 
  {
      cout << __LINE__ << "]\t" << blanks << "    Enter in directory: " << obj->GetName() << endl ;
  }
 }

}

//===================================================================
drawHistogram(TDirectory * thisDirectory, std::string hName) 
{
 cout << __LINE__ << "]\t====== Searching for " << hName << " =============" << endl ;
 thisDirectory->ls() ; 

 TObject * obj = thisDirectory->FindObjectAny("histograms" ) ;
 cout << __LINE__ << "]\tOBJ!!!!: " << obj << endl ;
 TObject * abj = ((TFolder*)obj)->FindObjectAny("beamProfiles") ;
 cout << __LINE__ << "]\tABJ!!!!: " << abj << endl ;
 TObject * ebj = ((TFolder*)abj)->FindObjectAny("Xprofiles") ;
 cout << __LINE__ << "]\tEBJ!!!!: " << ebj << endl ;
 TObject * hbj = ((TFolder*)ebj)->FindObjectAny("Station: 0 - Plaq: 2") ;
 cout << __LINE__ << "]\tHBJ!!!!: " << hbj << endl ;
 hbj->Draw() ;
 
 TKey * keyH = NULL ;
 TIter hList(thisDirectory->GetListOfKeys());

 cout << __LINE__ << "]\t" << " Looking for " << hName << std::endl ;
 
 while((keyH = (TKey*)hList()))
 {
   cout << __LINE__ << "]\t" << "obj: " << keyH->GetName() << ", which is a " << keyH->GetClassName() << endl ;   
   if( std::string(keyH->GetClassName()) == std::string("TDirectoryFile") ) 
   { 
   }
   if( std::string(keyH->GetClassName()) == std::string("TFolder") ) 
   { 
     TFolder * aFolder = gDirectory->GetKey(keyH->GetName())->ReadObj() ;
     drawHistogram(aFolder, hName) ;
   }
 }
  
}

//===================================================================
drawHistogram(TFolder * thisFolder, std::string hName) 
{
// cout << __LINE__ << "]\t" << "folder:: " << thisFolder->GetName() << endl ;
 TCollection * rootFoldersColl     = (TCollection *)thisFolder->GetListOfFolders() ;
// cout << __LINE__ << "]\t" << "rootFoldersColl     " << rootFoldersColl     << endl ;
 TIterator   * rootFoldersIterator = rootFoldersColl->MakeIterator() ;
// cout << __LINE__ << "]\t" << "rootFoldersIterator " << rootFoldersIterator << endl ;
 
 TObject * obj = NULL ;
 
 cout << __LINE__ << "]\t" << "    Enter folder: " << thisFolder->GetName() << endl ;
 while(obj = rootFoldersIterator->Next())
 {
   cout << __LINE__ << "]\t" << "    obj: " << obj->GetName() << endl ;
   if( obj->IsA() == TFolder::Class() ) 
   { 
     drawHistogram((TFolder*)obj, hName) ;
   }
   if( obj->IsA() == TH1::Class() ) 
   { 
     cout << __LINE__ << "]\t" << "   TH1: '" << obj->GetName() << "'" << endl ;
   }
   if( obj->IsA() == TH2::Class() ) 
   { 
     cout << __LINE__ << "]\t" << "   TH2: '" << obj->GetName() << "'" << endl ;
   }
 }
}

//===================================================================
inspectTree(TDirectory * thisDirectory, int & level) 
{
 std::string blanks = "" ;
 
 for(int l=0; l<level; ++l) {blanks += "  ";}

 level++ ;
 
 cout << __LINE__ << "]\t" << blanks << "Inspecting " << thisDirectory->GetName() << endl ;
 TKey * keyH = NULL ;
 TIter hList(thisDirectory->GetListOfKeys());

 while((keyH = (TKey*)hList()))
 {
   cout << __LINE__ << "]\t" << blanks << "obj: " << keyH->GetName() << ", which is a " << keyH->GetClassName() << endl ;
   
   if( std::string(keyH->GetClassName()) == std::string("TDirectoryFile") ) 
   { 
     thisDirectory->cd(keyH->GetName());
     inspectTree(gDirectory, level) ;
     level--;
   }
   if( std::string(keyH->GetClassName()) == std::string("TFolder") ) 
   { 
     TFolder * aFolder = gDirectory->GetKey(keyH->GetName())->ReadObj() ; // Get the folder in memory (this is the trick!)
     inspectTree(aFolder, level) ;
   }
 }
}
