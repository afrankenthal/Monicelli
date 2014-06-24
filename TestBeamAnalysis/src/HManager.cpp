#include "HManager.h"
#include "Utils.h"

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraphAsymmErrors.h>
#include <TROOT.h>
#include <TGraph.h>
#include <cmath>
#include <string> 
#include <iostream>
#include "../include/MessageTools.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////////
HManager::HManager(TTree* tree) : outFile_(0), theTree_(tree), theCuts_(theData_), nEvents_(0)
{
 
  stationNames.push_back("Downstream Station");
  stationNames.push_back("Upstream Station");

  planeNames.push_back(  "Station_downstream_Plaq0"  );
  planeNames.push_back(  "Station_downstream_Plaq1"  );
  planeNames.push_back(  "Station_downstream Plaq2"  );
  planeNames.push_back(  "Station_downstream_Plaq3"  );
  planeNames.push_back(  "Station_upstream_Plaq0"  );
  planeNames.push_back(  "Station_upstream_Plaq1"  );
  planeNames.push_back(  "Station_upstream_Plaq2"  );
  planeNames.push_back(  "Station_upstream_Plaq3"  );
  planeNames.push_back(  "Dut1"    );
  planeNames.push_back(  "Dut0"    );
 
  planeIDs.push_back(    "0" 	   );
  planeIDs.push_back(	 "1" 	   );
  planeIDs.push_back(	 "2" 	   );
  planeIDs.push_back(	 "3" 	   );			
  planeIDs.push_back(	 "4" 	   );
  planeIDs.push_back(	 "5" 	   );
  planeIDs.push_back(	 "6" 	   );
  planeIDs.push_back(	 "7" 	   );
  planeIDs.push_back(	 "Dut1"    );
  planeIDs.push_back(	 "Dut0"    );
 
  // Book planes
  planes_[PLANE0] = Plane(PLANE0, -383, YPLANE,  -0.446940,  0.000000);//2-3
  planes_[PLANE1] = Plane(PLANE1, -344, YPLANE,  -0.446620, -0.004720);//2-2
  planes_[PLANE2] = Plane(PLANE2, -243, XPLANE,   0.000000,  0.452320);//2-1
  planes_[PLANE3] = Plane(PLANE3, -204, XPLANE,   0.000260, -0.452590);//2-0
  planes_[PLANE4] = Plane(PLANE4,  204, XPLANE,  -0.002390,  0.478950);//0-3
  planes_[PLANE5] = Plane(PLANE5,  243, XPLANE,  -0.005734,  0.483750);//0-2
  planes_[PLANE6] = Plane(PLANE6,  344, YPLANE,  -0.471070,  0.055940);//0-1
  planes_[PLANE7] = Plane(PLANE7,  383, YPLANE,  -0.473730,  0.051110);//0-0
  planes_[DUT1  ] = Plane(DUT1,    -50, DUT   ,   0.000000, -0.000000);//4-1
  planes_[DUT0  ] = Plane(DUT0,     70, DUT   ,   0.000000, -0.000000);//4-0

  if(tree)
  {     
     for(int i = 0 ; i < NPLANES; i++)
     {
         cout << __PRETTY_FUNCTION__ << "Setting window for plane " << i << endl;
         planes_[i].setupWindow(theTree_);
     }
     theData_.setBranchAddress(theTree_);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
HManager::~HManager()
{
  closeFile();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void HManager::openFile(TString fileName)
{
  closeFile();
  outFile_ = new TFile(fileName, "RECREATE");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void HManager::closeFile(void)
{
  if(outFile_)
  {
    outFile_->Close();
    delete outFile_;
    outFile_ = 0;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void HManager::writeAndCloseFile(void)
{
  if(outFile_)
  {
    outFile_->Write();
    closeFile();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void HManager::setWindow(TTree* tree)
{
   theTree_ = tree;
   
   for(int p=0 ; p<NPLANES; p++)
   {
       cout << __PRETTY_FUNCTION__ << "Setting window for plane " << p << endl;
       planes_[p].setupWindow(theTree_);
   } 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void HManager::setupTree(TTree* tree)
{
   STDLINE("",ACWhite);
   theTree_ = tree;
   theData_.setBranchAddress(theTree_);
   STDLINE("",ACWhite);     
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void HManager::setHistos(TH2F* histos,int i)
{
    calibHistos2D_[i] = histos;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
int HManager::allTrackProj(Plane& planeN)
{
  unsigned int planeID = planeN.getPlaneID()     ;
  float        xp      = theData_.getXp(planeID) ;
  float        yp      = theData_.getYp(planeID) ;
  float        col     = theData_.getCol(planeID);
  float        row     = theData_.getRow(planeID);
  
  hAllTrackProjXY_[planeID]->Fill(xp,yp);
  
  if(col != -1 && row != -1)
      hAllTrackProjPix_[planeID]->Fill(col,row);
    
  return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
int HManager::planeHits(Plane& planeN)
{
  unsigned int planeID = planeN.getPlaneID()     ;
  float        xp      = theData_.getXp(planeID) ;
  float        yp      = theData_.getYp(planeID) ;
  float        col     = theData_.getCol(planeID);
  float        row     = theData_.getRow(planeID);
  
  if(theData_.getHasHit(planeID))
  {
    hPlaneHitsXY_ [planeID]->Fill(xp ,yp );
    hPlaneHitsPix_[planeID]->Fill(col,row);
  }  

  return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
int HManager::planeWindowHits(Plane& planeN)
{
  unsigned int planeID = planeN.getPlaneID();
  float        col     = theData_.getCol(planeID);
  float        row     = theData_.getRow(planeID); 
   
  if( planeN.getWindow().checkWindow(col,row) )
      hPlaneWindowHits_[planeID]->Fill(col,row) ;
      
  return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
int HManager::clusterSizeDistribution(Plane& planeN)
{
  unsigned int planeID = planeN.getPlaneID();

  if( theData_.getHasHit(planeID) )
      hClusterSize_[planeID]->Fill(theData_.getNpix(planeID)) ;
      
  return 1;      
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
int HManager::charge(Plane& planeN)
{
  unsigned int planeID = planeN.getPlaneID()        ; 
  float        col     = theData_.getCol(planeID)   ;
  float        row     = theData_.getRow(planeID)   ;
  float        charge  = theData_.getCharge(planeID);
  //float        xRes    = theData_.getXresid(planeID);
  //float        yRes    = theData_.getYresid(planeID);
  
  if( theData_.getHasHit(planeID) && theData_.getNtrack()==1 && (theData_.getChi2()/theData_.getNdof())<=2 && col!=-1 && row!=-1)
      //calibHistos2D_[3]->GetBinContent(theData_.getCol(planeID),theData_.getRow(planeID)) >= -1.6 &&
      //calibHistos2D_[2]->GetBinContent(theData_.getCol(planeID),theData_.getRow(planeID)) >= 0.00009 &&
    //theData_.getXresid(planeID)            <= 50 &&
    //theData_.getYresid(planeID)            <= 50 && 

  {     
      if( theData_.getNpix(planeID)   == 1 ) h1PxlCharge_ [planeID]->Fill(charge/2);
      if( theData_.getNpix(planeID)   == 2 ) h2PxlCharge_ [planeID]->Fill(charge  );   
      //if( theData_.getCharge(planeID) <= 0 ) h2DNegCharge_[planeID]->Fill(col,row );      
  }
  
  return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
int HManager::predictedErrs(Plane& planeN)
{
    unsigned int planeID = planeN.getPlaneID()       ;
    float        xpErr   = theData_.getXpErr(planeID);
    float        ypErr   = theData_.getYpErr(planeID);
    
    if(theData_.getHasHit(planeID) && theData_.getBelongsToTrack(planeID))
    {
       hPredictedXpError_[planeID]->Fill(xpErr*10);
       hPredictedYpError_[planeID]->Fill(ypErr*10);
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
int HManager::eventResiduals(Plane& planeN)
{
    unsigned int planeID = planeN.getPlaneID()        ;
    float        col     = theData_.getCol   (planeID);
    float        row     = theData_.getRow   (planeID);
    float        xRes    = theData_.getXresid(planeID);
    float        yRes    = theData_.getYresid(planeID);
    //float        xpErr   = theData_.getXpErr (planeID);
    //float        ypErr   = theData_.getYpErr (planeID);
    
    //if(planeID==8)
      // std::cout << xpErr << "\t" << ypErr << std::endl;
   
    if( theData_.getHasHit(planeID) && theData_.getNtrack()==1 && theData_.getBelongsToTrack(planeID) && col!=-1 && row!=-1)// && xpErr<=6 && ypErr<=6 && (theData_.getChi2()/theData_.getNdof())<=2)
    {
        if( theData_.getNpix(planeID) == 1 ) 
        {
            hXresidOnePxl_[planeID]->Fill(xRes);
	    hYresidOnePxl_[planeID]->Fill(yRes);
        }
       
        if( theData_.getNpix(planeID) == 2 && (row-floor(row)) == 0.5) hXresidTwoPxl_[planeID]->Fill(xRes);  
        if( theData_.getNpix(planeID) == 2 && (col-floor(col)) == 0.5) hYresidTwoPxl_[planeID]->Fill(yRes);
           	                         
    }
    return 0;
 }

///////////////////////////////////////////////////////////////////////////////////////////////////////
 int HManager::chargeResidualPlot(Plane& planeN)
 {
     unsigned int planeID = planeN.getPlaneID()        ;
     float        col     = theData_.getCol(planeID)   ;
     float        row     = theData_.getRow(planeID)   ;
     float        xRes    = theData_.getXresid(planeID);
     float        yRes    = theData_.getYresid(planeID);
     //float        xRes    = theData_.getXresidCalc(planeID);
     //float        yRes    = theData_.getYresidCalc(planeID);
     float        charge  = theData_.getCharge(planeID);
     
     if( theData_.getHasHit(planeID) && theData_.getNtrack()==1 && (theData_.getChi2()/theData_.getNdof())<=2 && col!=-1 && row!=-1)
     { 
         if( theData_.getNpix(planeID) == 1 ) 
         {
	     h2DChargeXResidual1Pxl_[planeID]->Fill(xRes,charge/2);
	     h2DChargeYResidual1Pxl_[planeID]->Fill(yRes,charge/2);
	 }
	 
	 if( theData_.getNpix(planeID) == 2 && (row-floor(row)) == 0.5 ) h2DChargeXResidual2Pxl_[planeID]->Fill(xRes,charge);
         if( theData_.getNpix(planeID) == 2 && (col-floor(col)) == 0.5 ) h2DChargeYResidual2Pxl_[planeID]->Fill(yRes,charge);	     	     	           
     }
 
     if( theData_.getHasHit(planeID) && theData_.getNtrack()==1 && charge>=0 && charge<=80E+03 && theData_.getNpix(planeID)<=2 && (theData_.getChi2()/theData_.getNdof())<=2 && col!=-1 && row!=-1)		 
     { 
         h2DCellChargeDenom_[planeID]->Fill(xRes,yRes);
	 if( theData_.getNpix(planeID) == 1 ) h2DCellCharge_[planeID]->Fill(xRes,yRes,charge/2);
	 if( theData_.getNpix(planeID) == 2 ) h2DCellCharge_[planeID]->Fill(xRes,yRes,charge  );
         
     }
      
     return 0;
 }

///////////////////////////////////////////////////////////////////////////////////////////////////////
int HManager::planeEfficiency(Plane& planeN)
{
  unsigned int   planeID = planeN.getPlaneID()         ;
           int   hits    = 0                           ;
           float row     = theData_.getRow(planeID)    ;
           float col     = theData_.getCol(planeID)    ;
           //float xRes    = theData_.getXresid(planeID) ;
           //float yRes    = theData_.getYresid(planeID) ; 
  
  for (int p=0; p<8; p++)
  {
    if (theData_.getHasHit(p)) hits++;
  }  
  
  if( planeN.getWindow().checkWindow(col,row) && (theData_.getChi2()/theData_.getNdof())<=2 && theData_.getNtrack()<=1 && hits>=8)
  {
    //std::cout << "\t" << col << "\t" << row << std::endl;
    hEfficiencyDenom_  [planeID]->Fill(1        );
    h2DEfficiencyDenom_[planeID]->Fill(col ,row );
    //h2DResEffDenom_    [planeID]->Fill(xRes,yRes);

    if( theData_.getHasHit(planeID) )
    {
      hEfficiency_  [planeID]->Fill(1        );
      h2DEfficiency_[planeID]->Fill(col ,row );
      //h2DResEff_    [planeID]->Fill(xRes,yRes);
    }
    else  
    {

      h2DInefficiency_    [planeID]->Fill(col ,row );
      //h2DResInefficiency_ [planeID]->Fill(xRes,yRes);
    }
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
int HManager::cellEfficiency(Plane& planeN)
{
    unsigned int planeID = planeN.getPlaneID()            ;
    float        xRes    = theData_.getXresid(planeID);
    float        yRes    = theData_.getYresid(planeID);
    //float        xRes    = theData_.getXresidCalc(planeID);
    //float        yRes    = theData_.getYresidCalc(planeID);
    float        row     = theData_.getRow(planeID)       ;
    float        col     = theData_.getCol(planeID)       ;

    int hits = 0;
    
    for(int p = 0;p < 8; p++)
    {
      if(theData_.getHasHit(p)) 
         hits++;
    } 
      
    if(planeN.getWindow().checkWindow(col,row) && (theData_.getChi2()/theData_.getNdof())<=2 && theData_.getNtrack()<=1 && hits>=8)
    {
       h2DCellEfficiencyDenom_[planeID]->Fill(xRes,yRes);
       if(theData_.getHasHit(planeID))
          h2DCellEfficiency_[planeID]->Fill(xRes,yRes);
       else
          h2DCellInefficiency_ [planeID]->Fill(xRes,yRes);  
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void HManager::book(void)
{

  string sName;
  string sTitle;

  outFile_->cd();
  outFile_->mkdir("Misc");
  outFile_->cd("Misc");
  
  hNTrack   	      = new TH1F("hNtracks"	     , "Number of Tracks"	 ,      20,   0,      20);
  hChi2_    	      = new TH1F("hChi2"	     , "Chisquare"		 ,     100,   0,      20);
  hChi2Excl_ 	      = new TH1F("hChi2Excl"	     , "Chisquare without hit"   ,     100,   0,      20);

  for( int i = 0; i < NPLANES; ++i )
  {
    outFile_->cd();
    outFile_->mkdir(planeNames[i].c_str());
    outFile_->cd(planeNames[i].c_str());
    cout << "Filling Directory: " << planeNames[i] << endl;

//----------------------------------2DHitTrackProjections-------------------------------------------------------------------------

    sName  = "hAllTrackProjXY"                  + planeIDs[i] ;
    sTitle =  "XY projection of all tracks "    + planeNames[i] ;   
    hAllTrackProjXY_[i]	                        = new TH2F( sName.c_str(), sTitle.c_str(), 100, -10000,  10000, 100, -10000, 10000  ) ;
    
    sName  = "hAllTrackProjPixels"              + planeIDs[i];
    sTitle =  "Pixel projection of all tracks " + planeNames[i];   
    hAllTrackProjPix_[i]	                = new TH2F( sName.c_str(), sTitle.c_str(), 70,  -10,  60, 100, -10,  90 ) ;

    sName  = "hPlaneHitsXY"                     + planeIDs[i];
    sTitle =  "XY view of hits recorded on "    + planeNames[i];   
    hPlaneHitsXY_[i]	                        = new TH2F( sName.c_str(), sTitle.c_str(), 100, -10000,  10000, 100, -10000, 10000  ) ;
    
    sName  = "hPlaneHitsPixels"                 + planeIDs[i];
    sTitle =  "Pixel view of hits recorded on " + planeNames[i];   
    hPlaneHitsPix_[i]	                        = new TH2F( sName.c_str(), sTitle.c_str(), 70,  -10,  60, 100, -10, 90  ) ;

    sName  = "hPlaneWindowHits"                 + planeIDs[i];
    sTitle = "Hits in window "                  + planeNames[i];
    //hPlaneWindowHits_[i]                        = new TH2F( sName.c_str(), sTitle.c_str(), 70,  -10,  60, 100, -10,  90 ) ;
    hPlaneWindowHits_[i]                        = new TH2F( sName.c_str(), sTitle.c_str(),300,-10,290,200,-10,190);

//------------------------------------Residuals-----------------------------------------------------------------------------------

    sName  = "XresidOnePxl"                + planeIDs[i];
    sTitle = "X residuals cluster size 1 " + planeNames[i];
    hXresidOnePxl_[i]                      = new TH1F( sName.c_str(), sTitle.c_str(), 125, -500, 500 ) ;  
        
    sName  = "YresidOnePxl"                + planeIDs[i];
    sTitle = "Y residuals cluster size 1 " + planeNames[i];
    hYresidOnePxl_[i]                      = new TH1F( sName.c_str(), sTitle.c_str(), 125, -500, 500 ) ;  
    
    sName  = "XresidTwoPxl"                + planeIDs[i];
    sTitle = "X residuals cluster size 2 " + planeNames[i];
    hXresidTwoPxl_[i]	                   = new TH1F( sName.c_str(), sTitle.c_str(), 125, -500, 500 ) ; 

    sName  = "YresidTwoPxl"                + planeIDs[i];
    sTitle = "Y residuals cluster size 2 " + planeNames[i];
    hYresidTwoPxl_[i]                      = new TH1F( sName.c_str(), sTitle.c_str(), 125, -500, 500 ) ;  
       
    /*sName  = "hXresidCalculatedOnePxl" + planeIDs[i];
    sTitle = "XresidCalculatedOnePxl"  + planeNames[i];
    hXresidCalculatedOnePxl_[i]        = new TH1F( sName.c_str(), sTitle.c_str(), 100, -1, 1 ) ;  
   
    sName  = "hYresidCalculatedOnePxl" + planeIDs[i];
    sTitle = "YresidCalculatedOnePxl"  + planeNames[i];
    hYresidCalculatedOnePxl_[i]        = new TH1F( sName.c_str(), sTitle.c_str(), 100, -1, 1 ) ; 
           
    sName  = "hXresidCalculatedTwoPxl" + planeIDs[i];
    sTitle = "XresidCalculatedTwoPxl"  + planeNames[i];
    hXresidCalculatedTwoPxl_[i]        = new TH1F( sName.c_str(), sTitle.c_str(), 100, -1, 1 ) ;  
    
    sName  = "hYresidCalculatedTwoPxl" + planeIDs[i];
    sTitle = "YresidCalculatedTwoPxl"  + planeNames[i];
    hYresidCalculatedTwoPxl_[i]        = new TH1F( sName.c_str(), sTitle.c_str(), 100, -1, 1 ) ;*/

//-----------------------------------Efficiency------------------------------------------------------------------------------------------

    sName  = "hEfficiency"                     + planeIDs[i];
    sTitle = "Overall efficiency "             + planeNames[i];
    hEfficiency_[i]                            = new TH1F( sName.c_str(), sTitle.c_str(), 1, .5, 1.5 ) ;
									   
    sName  = "hEfficiencyDenom"                + planeIDs[i];
    sTitle = "Overall efficiency denominator " + planeNames[i];
    hEfficiencyDenom_[i]                       = new TH1F( sName.c_str(), sTitle.c_str(), 1, .5, 1.5 ) ;
        
    sName  = "2DEfficiency"                    + planeIDs[i];
    sTitle = "2D efficiency distribution "     + planeNames[i];	
    //h2DEfficiency_[i]	                       = new TH2F( sName.c_str(), sTitle.c_str(), 70,  -10,  60, 100,  -10,  90 ) ;
    h2DEfficiency_[i]	                       = new TH2F( sName.c_str(), sTitle.c_str(), 300,-10,290,200,-10,190) ;

    sName  = "h2DInefficiency"                 + planeIDs[i];
    sTitle = "2D inefficiency "                + planeNames[i];	
    //h2DInefficiency_[i]	               = new TH2F( sName.c_str(), sTitle.c_str(), 70,  -10,  60, 100,  -10,  90 ) ;
    h2DInefficiency_[i]	                       = new TH2F( sName.c_str(), sTitle.c_str(), 300,-10,290,200,-10,190) ;
									   
    sName  = "h2DEfficiencyDenom"              + planeIDs[i];
    sTitle =  "2D efficiency denominator "     + planeNames[i];   
    //h2DEfficiencyDenom_[i]                   = new TH2F( sName.c_str(), sTitle.c_str(), 70,  -10,  60, 100,  -10,  90 ) ;
    h2DEfficiencyDenom_[i]                     = new TH2F( sName.c_str(), sTitle.c_str(), 300,-10,290,200,-10,190) ;
    
    sName  = "h2DCellEfficiencyDenom"          + planeIDs[i];
    sTitle = "2D Cell efficiency Denominator " + planeNames[i];   
    h2DCellEfficiencyDenom_[i]                 = new TH2F( sName.c_str(), sTitle.c_str(), 40, -100, 100,  40, -100, 100 ) ;
    
    sName  = "2DCellEfficiency"                + planeIDs[i];
    sTitle = "Cell efficiency "                + planeNames[i];   
    h2DCellEfficiency_[i]                      = new TH2F( sName.c_str(), sTitle.c_str(), 40, -100, 100,  40, -100, 100 ) ;
    
    sName  = "h2DCellInefficiency"             + planeIDs[i];
    sTitle = "2D Cell inefficiency "           + planeNames[i];   
    h2DCellInefficiency_[i]                    = new TH2F( sName.c_str(), sTitle.c_str(), 40, -100, 100,  40, -100, 100 ) ;
    
//---------------------------------Clusters----------------------------------------------------------------------------------------------
    
    sName  = "hClusterSizeDistribution"   + planeIDs[i];
    sTitle = "Cluster size distribution " + planeNames[i];
    hClusterSize_[i]	                  = new TH1F( sName.c_str(), sTitle.c_str(), 10, 0, 10) ;

//---------------------------------Charge-------------------------------------------------------------------------------------------------

    sName  = "h1pxlCharge"                         + planeIDs[i];
    sTitle = "Charge distribution cluster size 1 " + planeNames[i];
    h1PxlCharge_[i] 	                           = new TH1F( sName.c_str(), sTitle.c_str(), 200, -10000, 100000 ) ; 
    
    sName  = "h2pxlCharge"                         + planeIDs[i];
    sTitle = "Charge distribution cluster size 2 " + planeNames[i];
    h2PxlCharge_[i] 	                           = new TH1F( sName.c_str(), sTitle.c_str(), 200, -10000, 100000 ) ; 

    //sName  = "h2DNegCharge"               + planeIDs[i];
    //sTitle = "h2DNegCharge "              + planeNames[i];
    //h2DNegCharge_[i] 	                  = new TH2F( sName.c_str(), sTitle.c_str(), 70, -10 ,  60, 100, -10   , 90     ) ;

    sName  = "h2DChargeXResidual1Pxl"             + planeIDs[i];								    
    sTitle = "Charge-X residuals cluster size 1 " + planeNames[i];								    
    h2DChargeXResidual1Pxl_[i]                    = new TH2F( sName.c_str(), sTitle.c_str(), 40, -100, 100, 100, -10000, 100000 ) ; 

    sName  = "h2DChargeXResidual2Pxl"             + planeIDs[i];								    
    sTitle = "Charge-X residuals cluster size 2 " + planeNames[i];								    
    h2DChargeXResidual2Pxl_[i]                    = new TH2F( sName.c_str(), sTitle.c_str(), 40, -100, 100, 100, -10000, 100000 ) ;
 
    sName  = "2DhChargeYResidual1PXl"             + planeIDs[i];								    
    sTitle = "Charge-Y residuals cluster size 1 " + planeNames[i];								    
    h2DChargeYResidual1Pxl_[i]                    = new TH2F( sName.c_str(), sTitle.c_str(), 40, -100, 100, 100, -10000, 100000 ) ;

    sName  = "h2DChargeYResidual2PXl"             + planeIDs[i];								    
    sTitle = "Charge-Y residuals cluster size 2 " + planeNames[i];								    
    h2DChargeYResidual2Pxl_[i]                     = new TH2F( sName.c_str(), sTitle.c_str(), 40, -100, 100, 100, -10000, 100000 ) ;
    
    sName  = "h2DCellCharge"                      + planeIDs[i];								    
    sTitle = "Cell charge "                       + planeNames[i];								    
    h2DCellCharge_[i]                             = new TH2F( sName.c_str(), sTitle.c_str(), 40, -100, 100,  40, -100  , 100    ) ; 
    
    sName  = "h2DCellChargeDenom"                 + planeIDs[i];								    
    sTitle = "Cell charge denominator "           + planeNames[i];								    
    h2DCellChargeDenom_[i]                        = new TH2F( sName.c_str(), sTitle.c_str(), 40, -100, 100,  40, -100  , 100    ) ;  
    
//---------------------------------predicted errors-------------------------------------------------------------------------------------------------     	 								     

    sName  = "hPredictedXpError"  + planeIDs[i];								    
    sTitle = "predicted X error " + planeNames[i];								    
    hPredictedXpError_[i]         = new TH1F( sName.c_str(), sTitle.c_str(), 40, 0, 100 );
    
    sName  = "hPredictedYpError"  + planeIDs[i];								    
    sTitle = "predicted Y error " + planeNames[i];								    
    hPredictedYpError_[i]         = new TH1F( sName.c_str(), sTitle.c_str(), 40, 0, 100 );

  } 

  outFile_->cd();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void HManager::fill(void){

  if( theTree_ == 0 )
      return;

  int nEvents = theTree_->GetEntries();
  std::cout << __PRETTY_FUNCTION__ << nEvents << std::endl;
  nEvents_ = nEvents_ + nEvents;

  for( int i=0; i<nEvents; i++ )   
  {
    theTree_->GetEntry(i);
    for( int p = 0 ; p < NPLANES; p++ )
         planes_[p].calcProjections(theData_);  

    if( theCuts_.newEvent() )		    
        hNTrack->Fill(theData_.getNtrack());

    hChi2_->Fill(theData_.getChi2()/theData_.getNdof());
    
    if( theCuts_.chi2Excl(DUT0) )
        hChi2Excl_->Fill(theData_.getChi2Excl(DUT0)/theData_.getNdofExcl(DUT0));
 
    for( int p = 0; p < NPLANES; p++)   
    {
         allTrackProj            ( planes_[p] ) ;
         planeHits	         ( planes_[p] ) ;
         planeWindowHits         ( planes_[p] ) ;
         clusterSizeDistribution ( planes_[p] ) ;
         charge	                 ( planes_[p] ) ;
         planeEfficiency         ( planes_[p] ) ;
	 cellEfficiency          ( planes_[p] ) ;
         eventResiduals          ( planes_[p] ) ;
         chargeResidualPlot      ( planes_[p] ) ;
	 predictedErrs           ( planes_[p] ) ;  
    } 
  }
      		     
  /*for( int p = 0; p < NPLANES; p++)   
  {
       
       hEfficiency_       [p]->Divide(hEfficiencyDenom_       [p]);
       h2DEfficiency_     [p]->Divide(h2DEfficiencyDenom_     [p]);
       h2DResEff_         [p]->Divide(h2DResEffDenom_         [p]);
       h2DChargeResiduals_[p]->Divide(h2DChargeResidualsDenom_[p]);
       h2DResInefficiency_[p]->Divide(h2DResEffDenom_         [p]);
  }

  std::cout << "DUTevents = " << DUTevents << std::endl;  
  stringstream ss_ ;
  
  for( int p = 0; p < NPLANES; p++)
  {
    ss_.str("") ;
    ss_ << "!-!" << planeNames[p] << "	" << hEfficiency_[p]->GetBinContent(1) ;
    STDLINE(ss_.str(),ACGreen) ;
    //!!! Do not remove "!-!" cout statement (needed by a perl script parsing this output)
  
  }

  ss_.str("") ;
  ss_ << "!-!" << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" ;
  STDLINE(ss_.str(),ACWhite) ;*/
  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void HManager::endJob(void)
{
  std::cout << __PRETTY_FUNCTION__ << nEvents_ << std::endl;
  
  for( int p = 0; p < NPLANES; p++)   
  {       
       hEfficiency_        [p]->Divide(hEfficiencyDenom_      [p]);
       h2DEfficiency_      [p]->Divide(h2DEfficiencyDenom_    [p]);
       h2DCellEfficiency_  [p]->Divide(h2DCellEfficiencyDenom_[p]);
       h2DCellCharge_      [p]->Divide(h2DCellChargeDenom_    [p]);
       h2DCellInefficiency_[p]->Divide(h2DCellEfficiencyDenom_[p]);
  }

  stringstream ss_ ;
  
  for( int p = 0; p < NPLANES; p++)
  {
    ss_.str("") ;
    ss_ << "!-!" << planeNames[p] << "	" << hEfficiency_[p]->GetBinContent(1) ;
    STDLINE(ss_.str(),ACGreen) ;
    //!!! Do not remove "!-!" cout statement (needed by a perl script parsing this output)
  
  }

  ss_.str("") ;
  ss_ << "!-!" << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" ;
  STDLINE(ss_.str(),ACWhite) ;
}
