#include "HistogramWindow.h"
#include "Plane.h"
#include "Cuts.h"
#include "DataClass.h"
#include <iostream>
#include <sstream>
#include <TH2F.h>
#include <TAxis.h>
#include <TTree.h>
#include <TROOT.h>
#include <math.h>
#include "../include/MessageTools.h"

using namespace std;

/*-----------------------------------------------------------------------------------*/
HistogramWindow::HistogramWindow(TTree* tree) : Window(tree), theHWindow_(0)
{
}

/*-----------------------------------------------------------------------------------*/
HistogramWindow::~HistogramWindow(void)
{
  if(theHWindow_ != 0)
    delete theHWindow_;
}

/*-----------------------------------------------------------------------------------*/
// bool HistogramWindow::checkWindow(double x, double y) const
// {
//   TAxis* xAxis = theHWindow_->GetXaxis();
//   TAxis* yAxis = theHWindow_->GetYaxis();
//   
//   if( (x >= xAxis->GetXmin()) && (x <= xAxis->GetXmax()) && (y >= yAxis->GetXmin()) && (y <= yAxis->GetXmax()) )
//     if(theHWindow_->GetCellContent(xAxis->FindBin(x),yAxis->FindBin(y)) != 0)
//       return true;
//   return false;
// }

/*-----------------------------------------------------------------------------------*/
// void HistogramWindow::calculateWindow(Plane& plane, TTree* tree)
// {
//   if(tree != 0)
//     theTree_ = tree;
//   else if(theTree_ == 0)
//   {
//     cout << __PRETTY_FUNCTION__ << "ERROR: Don't have any tree set up!" << endl;
//     return;
//   }
//   if(theHWindow_ != 0)
//     delete theHWindow_;
// 
//   int planeID = plane.getPlaneID();
//   
//   stringstream name; 
//   name.str("");
//   name << "hWindow" << planeID; 
//   gROOT->cd();
//   theHWindow_ = new TH2F(name.str().c_str(),name.str().c_str(), 100, -10, 10, 100, -10, 10);
// 
//   DataClass data;
//   data.setBranchAddress(theTree_);
//   Cuts cuts(data);
//   
//   for(int i = 0; i < theTree_->GetEntries(); i++)
//   {
//     theTree_->GetEntry(i);
//     if(cuts.hitTrackProj(planeID))
//     { 		      
//        theHWindow_->Fill(data.getXl(planeID),data.getYl(planeID));
//     }
//   }
// }

/*-----------------------------------------my functions---------------------------------------*/

bool HistogramWindow::checkWindow(float col, float row) const
{
  TAxis* xAxis = theHWindow_->GetXaxis() ;
  TAxis* yAxis = theHWindow_->GetYaxis() ; 
      
  if( theHWindow_->GetCellContent(xAxis->FindBin(col),yAxis->FindBin(row)) != 0 ) 
      return true;
  else 
      return false;
}

////////////////////////////////////////////////////////////////////////////////////
void HistogramWindow::calculateWindow(Plane& plane, TTree* tree)
{
  
  stringstream ss_;
  
  if( tree != 0 )
    theTree_ = tree ;
  else if( theTree_ == 0 )
  {
    ss_.str("") ;
    ss_ << "ERROR: Don't have any tree set up!" ;
    STDLINE(ss_.str(),ACRed) ;
    return ;
  }
  
  if( theHWindow_ != 0 )
    delete theHWindow_ ;
  
  int planeID = plane.getPlaneID() ;
    
  stringstream name ;
  name.str("") ;
  name << "hWindow" << planeID ;
  gROOT->cd() ;
  theHWindow_ = new TH2F (name.str().c_str(),name.str().c_str(),300,-10,290,200,-10,190) ;
  
  DataClass data;
  data.setBranchAddress(theTree_);
  Cuts cuts(data);

  TAxis* xAxis = theHWindow_->GetXaxis() ;
  TAxis* yAxis = theHWindow_->GetYaxis() ; 
  
  float MaxCol, MaxRow, MinCol, MinRow ;
  
  MinRow = 1;
  MinCol = 1;
  
  MaxCol = data.getMaxCol(planeID, theTree_) ;
  MaxRow = data.getMaxRow(planeID, theTree_) ;

  //quella che scrivo viene uccisa

  //Irr diamond window
  /*if(planeID==9)
  {
     MinCol = 20;
     MaxCol = 38;
     MinRow = 44;
     MaxRow = 77;
  }*/
    
  //LC500 new Window
  /*if(planeID==9)
  {
     MinCol = 22;
     MaxCol = 44;
     MinRow = 26;
     MaxRow = 60;
  }*/
  
  //LC500 meeting giovedi scorso!
  /*if(planeID==9)
  {
     MinCol = 24;
     MaxCol = 50;
     MinRow = 1;
     MaxRow = 72;
  }*/
  
  //Plaquette diamond
  /*if(planeID==8)
  {
     MinCol = 69;
     MaxCol = 101;
     MinRow = 94;
     MaxRow = 131;
  }*/
  
  //Irr FBK 4E window
  /*if(planeID==8)
  {
     MinCol = 21;
     MaxCol = 34;
     MinRow = 5;
     MaxRow = 51;
  }*/
  
  //Irr FBK1E window
  /*if(planeID==8)
  {
     MinCol = 13;
     MaxCol = 41;
     MinRow = 21;
     MaxRow = 54;
  }*/
  
  //LIMPH
  /*if(planeID==8)
  {
     MinCol = 23;
     MaxCol = 32;
     MinRow = 52;
     MaxRow = 61;
  }*/
  
  //testChewie
  if(planeID==0 || planeID==1 || planeID==6 || planeID==7)
  {
     MinCol = 0;
     MaxCol = 207;
     MinRow = 0;
     MaxRow = 159;
  }
  
  if(planeID==2 || planeID==3 || planeID==4 || planeID==5)
  {
     MinCol = 0;
     MaxCol = 155;
     MinRow = 0;
     MaxRow = 159;
  }  
  
  if(planeID==8)
  {
     MinCol = 0;
     MaxCol = 51;
     MinRow = 0;
     MaxRow = 79;
  }
  
  if(planeID==9)
  {
     MinCol = 0;
     MaxCol = 51;
     MinRow = 0;
     MaxRow = 79;
  }
  
  int treeEntries = theTree_->GetEntries();
  int maxEvents = 400000;
  int nEvents;
  
  if( treeEntries > maxEvents ) nEvents = maxEvents;
  else nEvents = treeEntries;
  
  for(int i = 0;i < nEvents; ++i)
  {
    theTree_->GetEntry(i); 
    if( data.getHasHit(planeID)            &&
        data.getRow(planeID) >= (MinRow+1) && data.getCol(planeID) >= (MinCol+1) && 
	data.getRow(planeID) <= (MaxRow-1) && data.getCol(planeID) <= (MaxCol-1) )
    { 
        //std::cout << i << "\t" << planeID << "\t" << data.getCol(planeID) << "\t" << data.getRow(planeID) << std::endl;

      if( (data.getCol(planeID) - floor(data.getCol(planeID))) != 0 &&      
          (data.getRow(planeID) - floor(data.getRow(planeID))) == 0)
        {
          theHWindow_->Fill(floor(data.getCol(planeID)), data.getRow(planeID)) ;
          theHWindow_->Fill(ceil (data.getCol(planeID)), data.getRow(planeID)) ;
        }
        else if( (data.getCol(planeID) - floor(data.getCol(planeID))) == 0 &&  
	         (data.getRow(planeID) - floor(data.getRow(planeID))) != 0)
        {
          theHWindow_->Fill(data.getCol(planeID),floor(data.getRow(planeID))) ;
          theHWindow_->Fill(data.getCol(planeID),ceil (data.getRow(planeID))) ;
        }
        else theHWindow_->Fill(data.getCol(planeID),data.getRow(planeID)) ;    	 
    }
  }
}
