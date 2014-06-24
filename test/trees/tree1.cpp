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
