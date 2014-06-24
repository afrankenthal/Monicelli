#include "Data.h"
#include <TTree.h>
#include <TBranch.h>
#include <iostream>


//Index
using namespace std;

//====================================================================
Data::Data()
{ 
}

//====================================================================
Data::~Data()
{
}

//====================================================================
void Data::setBranchAddress(TTree* tree)
{
  tree->SetBranchAddress("evtId"   , &evtId_   );  //0
  tree->SetBranchAddress("ntrack"  , &ntrack_  );
  tree->SetBranchAddress("itrack"  , &itrack_  );
  tree->SetBranchAddress("nhits"   , &nhits_   );
  tree->SetBranchAddress("ngoodx"  , &ngoodx_  );  
  tree->SetBranchAddress("ngoody"  , &ngoody_  );  //5
  tree->SetBranchAddress("ndof"    , &ndof_    );
  tree->SetBranchAddress("chi2"    , &chi2_    );
  tree->SetBranchAddress("prob"    , &prob_    ); 
  tree->SetBranchAddress("xproj0"  , &xproj0_  );  
  tree->SetBranchAddress("yproj0"  , &yproj0_  ); //10
  tree->SetBranchAddress("xprojErr", &xprojErr_);
  tree->SetBranchAddress("yprojErr", &yprojErr_);
  tree->SetBranchAddress("xslope"  , &xslope_  );    	
  tree->SetBranchAddress("yslope"  , &yslope_  );
  tree->SetBranchAddress("hasHit"  , &hasHit_  ); //15
  tree->SetBranchAddress("measureX", &measureX_);
  tree->SetBranchAddress("npix"    , &npix_    );
  tree->SetBranchAddress("ncol"    , &ncol_    );
  tree->SetBranchAddress("nrow"    , &nrow_    );
  tree->SetBranchAddress("charge"  , &charge_  ); //20
  tree->SetBranchAddress("col"     , &col_     ); 		
  tree->SetBranchAddress("row"     , &row_     );
  tree->SetBranchAddress("xp"      , &xp_      );
  tree->SetBranchAddress("yp"      , &yp_      );      
  tree->SetBranchAddress("xl"      , &xl_      ); //25
  tree->SetBranchAddress("yl"      , &yl_      );     
  tree->SetBranchAddress("xpErr"   , &xpErr_   );
  tree->SetBranchAddress("ypErr"   , &ypErr_   );	  
  tree->SetBranchAddress("ndofExcl", &ndofExcl_);   
  tree->SetBranchAddress("chi2Excl", &chi2Excl_); //30
  tree->SetBranchAddress("probExcl", &probExcl_);   
  tree->SetBranchAddress("xresid"  , &xresid_  );    
  tree->SetBranchAddress("yresid"  , &yresid_  );
  tree->SetBranchAddress("xtErr"   , &xtErr_   );	  
  tree->SetBranchAddress("ytErr"   , &ytErr_   ); //35
  
  tree->SetBranchAddress("belongsToTrack", &belongsToTrack_);
};

//====================================================================
void Data::branch(TTree* tree){
  tree->Branch("evtId"   , &evtId_   , "evtId/I"   	); //0
  tree->Branch("ntrack"  , &ntrack_  , "ntrack/I"  	);
  tree->Branch("itrack"  , &itrack_  , "itrack/I"  	);
  tree->Branch("nhits"   , &nhits_   , "nhits/I"   	);
  tree->Branch("ngoodx"  , &ngoodx_  , "ngoodx/I"  	);   
  tree->Branch("ngoody"  , &ngoody_  , "ngoody/I"  	); //5
  tree->Branch("ndof"	 , &ndof_    , "ndof/I"	   	);
  tree->Branch("chi2"	 , &chi2_    , "chi2/F"	   	);
  tree->Branch("prob"	 , &prob_    , "prob/F"	   	);    
  tree->Branch("xproj0"  , &xproj0_  , "xproj0/F"  	);   
  tree->Branch("yproj0"  , &yproj0_  , "yproj0/F"  	); //10
  tree->Branch("xprojErr", &xprojErr_, "xprojErr/F"	); 
  tree->Branch("yprojErr", &yprojErr_, "yprojErr/F"	);
  tree->Branch("xslope"  , &xslope_  , "xslope/F"  	);	       
  tree->Branch("yslope"  , &yslope_  , "yslope/F"  	);	
  tree->Branch("hasHit"  , &hasHit_  , "hasHit[10]/O"   ); //15
  tree->Branch("measureX", &measureX_, "measureX[10]/O" );	  
  tree->Branch("npix"	 , &npix_    , "npix[10]/I"	);
  tree->Branch("ncol"	 , &ncol_    , "ncol[10]/I"	);	
  tree->Branch("nrow"	 , &nrow_    , "nrow[10]/I"	);     
  tree->Branch("charge"  , &charge_  , "charge[10]/F"   ); //20
  tree->Branch("col"	 , &col_     , "col[10]/F"	);	     
  tree->Branch("row"	 , &row_     , "row[10]/F"	);
  tree->Branch("xp"	 , &xp_      , "xp[10]/F"	);
  tree->Branch("yp"	 , &yp_      , "yp[10]/F"	); 
  tree->Branch("xl"	 , &xl_      , "xl[10]/F"	); //25
  tree->Branch("yl"	 , &yl_      , "yl[10]/F"	);
  tree->Branch("xpErr"   , &xpErr_   , "xpErr[10]/F"    );
  tree->Branch("ypErr"   , &ypErr_   , "ypErr[10]/F"    );	  
  tree->Branch("ndofExcl", &ndofExcl_, "ndofExcl[10]/I" );   
  tree->Branch("chi2Excl", &chi2Excl_, "chi2Excl[10]/F" ); //30
  tree->Branch("probExcl", &probExcl_, "probExcl[10]/F" );   
  tree->Branch("xresid"  , &xresid_  , "xresid[10]/F"   );	   
  tree->Branch("yresid"  , &yresid_  , "yresid[10]/F"   );
  tree->Branch("xtErr"   , &xtErr_   , "xtErr[10]/F"    );	  
  tree->Branch("ytErr"   , &ytErr_   , "ytErr[10]/F"    ); //35
  
  tree->Branch("belongsToTrack", &belongsToTrack_, "belongsToTrack[10]/O");
}
