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
 
#include <iostream>

#include <TApplication.h>
#include <TFile.h>
#include <TRandom.h>
#include <TTree.h>

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
  //create a Tree file tree1.root
  
  //create the file, the Tree and a few branches
  TFile f("tree1.root","recreate");
  TTree t1("t1","a simple Tree with simple variables");
  Float_t px, py, pz;
  Double_t random;
  Int_t ev;
  t1.Branch("px",&px,"px/F");
  t1.Branch("py",&py,"py/F");
  t1.Branch("pz",&pz,"pz/F");
  t1.Branch("random",&random,"random/D");
  t1.Branch("ev",&ev,"ev/I");
  
  //fill the tree
  for (Int_t i=0;i<10000;i++) 
  {
    gRandom->Rannor(px,py);
    pz = px*px + py*py;
    random = gRandom->Rndm();
    ev = i;
    t1.Fill();
  }
  
  //save the Tree header. The file will be automatically closed
  //when going out of the function scope
  t1.Write();
}
