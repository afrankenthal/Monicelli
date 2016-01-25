/*===============================================================================
 * Monicelli: the FERMILAB MTEST geometry builder and track reconstruction tool
 * 
 * Copyright (C) 2014 
 *
 * Authors:
 *
 * Dario Menasce      (INFN) 
 * Luigi Moroni       (INFN)
 * Jennifer Ngadiuba  (INFN)
 * Stefano Terzo      (INFN)
 * Lorenzo Uplegger   (FNAL)
 * Luigi Vigani       (INFN)
 *
 * INFN: Piazza della Scienza 3, Edificio U2, Milano, Italy 20126
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ================================================================================*/
 
#include "RectangularWindow.h"
#include "Plane.h"
#include "Cuts.h"
#include "DataClass.h"
#include <iostream>
#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>
#include "TCanvas.h"

using namespace std;

/*-----------------------------------------------------------------------------------*/
RectangularWindow::RectangularWindow(TTree* tree) : Window(tree)
{
}

/*-----------------------------------------------------------------------------------*/
RectangularWindow::~RectangularWindow(void)
{
}

/*-----------------------------------------------------------------------------------*/
bool RectangularWindow::checkWindow(float col, float row) const
{
  if( (col >= lowLeftX_) && (col <= upRightX_) && (row >= lowLeftY_) && (row <= upRightY_) )
    return true;
  else    
    return false;
}

/*-----------------------------------------------------------------------------------*/
void RectangularWindow::calculateWindow(Plane& plane, TTree* tree)
{
  if(tree != 0)
    theTree_ = tree;
  else if(theTree_ == 0)
  {
    cout << __PRETTY_FUNCTION__ << "ERROR: Don't have any tree set up!" << endl;
    return;
  }
  DataClass data;
  data.setBranchAddress(theTree_);
  Cuts cuts(data);
  
  unsigned int planeID = plane.getPlaneID();
  int const nEvents    = theTree_->GetEntries();

  int    nHits 	    = 0;
  double xp  	    = 0.0;
  double yp         = 0.0;
  double deltaXMean = 0.0;
  double deltaYMean = 0.0;

  TH1F hXBound("hXBound", "X boundaries" , 1000, -50, 50);   
  TH1F hYBound("hYBound", "Y boundaries" , 1000, -50, 50);   
    
  for(int i = 0; i < nEvents; i++)   // Apply cuts 
  {
    theTree_->GetEntry(i);
    if(cuts.hitTrackProj(planeID))
    { 		      
       hXBound.Fill(data.getXl(planeID));
       hYBound.Fill(data.getYl(planeID));
    }
    if(cuts.hasHits(planeID))
    {
      xp = plane.calcXProjection(data);
      yp = plane.calcYProjection(data);
      deltaXMean += data.getRow(planeID)*0.1  - xp ;  
      deltaYMean += data.getCol(planeID)*0.15 + yp ;  
      ++nHits;
    }
  }
  
  deltaXMean /= nHits;
  deltaYMean /= nHits;

//  planeN.setDeltaXMean(deltaXMean);
//  planeN.setDeltaYMean(deltaYMean);

//  cout << __PRETTY_FUNCTION__  << "\n DeltaX : " << plane.getDeltaXMean() << " DeltaY : " << plane.getDeltaYMean() << endl;  

  setSize(hXBound, hYBound);

}

/*-----------------------------------------------------------------------------------*/
void RectangularWindow::setSize(double lowLeftX, double lowLeftY, double upRightX, double upRightY)
{
  if ((lowLeftX < upRightX) && ( lowLeftY < upRightY ))
  {   
    lowLeftX_ = lowLeftX;
    lowLeftY_ = lowLeftY;
    upRightX_ = upRightX;
    upRightY_ = upRightY;
  }   
  else 
  {
    cout << __PRETTY_FUNCTION__ << "Invalid RectangularWindow Size!!" << endl;
  }

}

/*-----------------------------------------------------------------------------------*/
void RectangularWindow::setSize(const TH1F* hXBound, const TH1F* hYBound )
{
  int minXbin = -1;
  int maxXbin = -1;
  int minYbin = -1;
  int maxYbin = -1;

  for(int bin = 1; bin < hXBound->GetNbinsX(); bin++)
  {
    if(minXbin < 0 && hXBound->GetBinContent(bin) != 0)
    {
      minXbin = bin;
    }
    if(hXBound->GetBinContent(bin) != 0){
      maxXbin = bin;
    }
  }
  for(int bin = 1; bin < hYBound->GetNbinsX(); bin++)
  {
    if(minYbin < 0 && hYBound->GetBinContent(bin) != 0)
    {
      minYbin = bin;
    }
    if(hYBound->GetBinContent(bin) != 0){
      maxYbin = bin;
    }
  }

  double deltaX = hXBound->GetBinCenter(maxXbin) - hXBound->GetBinCenter(minXbin);
  double deltaY = hYBound->GetBinCenter(maxYbin) - hYBound->GetBinCenter(minYbin);

  setSize(hXBound->GetBinCenter(minXbin)+0.1*deltaX, hYBound->GetBinCenter(minYbin)+0.1*deltaY
                    ,hXBound->GetBinCenter(maxXbin)-0.1*deltaX, hYBound->GetBinCenter(maxYbin)-0.1*deltaY);
  cout << __PRETTY_FUNCTION__
       << " Xmin: " << hXBound->GetBinCenter(minXbin)+0.05*deltaX
       << " Xmax: " << hXBound->GetBinCenter(maxXbin)-0.05*deltaX
       << " Ymin: " << hYBound->GetBinCenter(minYbin)+0.05*deltaY
       << " Ymax: " << hYBound->GetBinCenter(maxYbin)-0.05*deltaY;
 
}

/*-----------------------------------------------------------------------------------*/
void RectangularWindow::setSize(const TH1F& hXBound, const TH1F& hYBound )
{
  setSize(&hXBound,&hYBound);
}

/*------------------------------------------------------------------------------------*/
const double& RectangularWindow::getLLX()
{
  return lowLeftX_;
}

/*------------------------------------------------------------------------------------*/
const double& RectangularWindow::getLLY()
{
  return lowLeftY_;
}

/*------------------------------------------------------------------------------------*/
const double& RectangularWindow::getURX()
{
  return upRightX_;
}

/*------------------------------------------------------------------------------------*/
const double& RectangularWindow::getURY()
{
  return upRightY_;
}

/*------------------------------------------------------------------------------------*/
const double RectangularWindow::setOffset(double offset)
{
  return 0;
}

/*------------------------------------------------------------------------------------*/
const double RectangularWindow::setMaskWidth(double width)
{
  if( (width > 0) && (width < 2*( upRightX_ - lowLeftX_ )) && (width < 2*( upRightY_ - lowLeftY_ )))
  {
    double upRightX = upRightX_ - width; 
    double upRightY = upRightY_ - width;
    double lowLeftX = lowLeftX_ + width;
    double lowLeftY = lowLeftY_ + width;
    setSize(lowLeftX, lowLeftY, upRightX, upRightY);
    return 0;
  }
  else 
    return -1;	
}
/*--------------------------------------------------------------------------------------*/

