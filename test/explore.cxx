{
 gROOT->Reset() ;
 
 TFile * f = new TFile("testFile.root") ;
 
 TIter hList(gDirectory->GetListOfKeys()) ;
 
 cout << "List of keys:" << endl ;
 while((keyH = (TKey*)hList())) 
 {
  cout << __LINE__ << "]\t" << keyH->GetName() << endl; 
  if( std::string(keyH->GetClassName()) == std::string("TFolder") ) 
  {
    cout << __LINE__ << "]\t" << keyH->GetName() << " is a folder" << endl; 
    ((TFolder*)keyH)->ls() ;
    cout << __LINE__ << "]\tNow reading " << keyH->GetName() << " in memory" << endl; 
    TFolder * mainFolder = gDirectory->GetKey(keyH->GetName())->ReadObj() ;
    cout << __LINE__ << "]\t" << mainFolder->GetName() << " content:" << endl ;
//    mainFolder->ls() ;

    TCollection *foldersColl = (TCollection *)mainFolder->GetListOfFolders()  ;
    TObject *obj = NULL;
    TIter nextobj(foldersColl);
    while ((obj = (TObject *) nextobj())) 
    {
       cout << "This is " << obj->GetName() << endl ;
//       obj->ls();
    }
  }
 }

}
