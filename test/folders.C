{
 gROOT->Reset() ;
 
 
 TFolder * mainFolder = new TFolder("MainFolder","Main Folder") ;
 TH1D    * hMain      = new TH1D(   "hMain","hMain",100,0,100) ;
 mainFolder->Add(hMain);
 
 TFolder * subFolder = new TFolder("SubFolder","Sub Folder") ;
 TH1D    * hSub      = new TH1D(   "hSub","hSub",200,0,200) ;
 subFolder->Add(hSub) ;
 
 mainFolder->Add(subFolder);
 
 TFile * file = new TFile("testFile.root","recreate" ) ;
 
 mainFolder->Write() ;
 
 file->Close() ;
 
}
