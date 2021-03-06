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
 
// g++ -o tgraph -I../include tgraph.cpp `root-config --cflags --libs` && ./tgraph

#include <iostream> 
#include <map> 
#include <sstream> 
#include <string> 


#include <TApplication.h>
#include <TBrowser.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TFolder.h>
#include <TH1D.h>
#include <TFrame.h>
#include <TGraphErrors.h>
#include <TObject.h>
#include <TRandom.h>
#include <TRint.h>
#include <TROOT.h>
#include <TTree.h>

void  	     doIt	     (void		      ) ;

//==================================================================
int main(int argc, char** argv)
{
  TApplication app("Tgraph",&argc, argv);
 
  doIt() ;

  app.Run() ;
  
  return 0 ;
}

//==================================================================
void doIt()
{
   //Draw a graph with error bars
   // To see the output of this macro, click begin_html <a href="gif/gerrors.gif">here</a>. end_html
   //Author: Rene Brun
   
   TCanvas *c1 = new TCanvas("c1","A Simple Graph with error bars",200,10,700,500);

   c1->SetFillColor(42);
   c1->SetGrid();
   c1->GetFrame()->SetFillColor(21);
   c1->GetFrame()->SetBorderSize(12);

   const Int_t n = 10;
   Float_t x[n]  = {-0.22, 0.05, 0.25, 0.35, 0.5, 0.61,0.7,0.85,0.89,0.95};
   Float_t y[n]  = {1,2.9,5.6,7.4,9,9.6,8.7,6.3,4.5,1};
   Float_t ex[n] = {.05,.1,.07,.07,.04,.05,.06,.07,.08,.05};
   Float_t ey[n] = {.8,.7,.6,.5,.4,.4,.5,.6,.7,.8};
   TGraphErrors *gr = new TGraphErrors(n,x,y,ex,ey);
   gr->SetTitle("TGraphErrors Example");
   gr->SetMarkerColor(4);
   gr->SetMarkerStyle(21);
   gr->Draw("ALP");

   c1->Update();
   
   TFile * file = new TFile("tgraph.root","recreate") ;

   TFolder * canvasFolder = new TFolder("CanvasFolder", "Canvas Folder") ;

   canvasFolder->Add(c1) ;

   file->Add(canvasFolder) ;
   
   file->Write() ;
   
   file->Close() ;
   
   std::cout << "File saved" << std::endl ;
}
