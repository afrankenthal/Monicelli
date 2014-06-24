// g++ -o Landau Landau.cc `root-config --cflags --libs` -lRooFit -lRooFitCore -lFoam -lMinuit && ./Landau

#include "RooCFunction1Binding.h" 
#include "RooCFunction3Binding.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooGlobalFunc.h"
#include "RooPlot.h"
#include "RooLandau.h"
#include "RooRealVar.h"
#include "RooTFnBinding.h" 

#include "Math/DistFunc.h"
#include "TApplication.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TRandom.h"

using namespace RooFit;
using namespace std; 

TApplication * app_ ;

void extractCharge(double  charge) ;
void run          (int     argc, 
                   char ** argv  ) ;

TH1F    * generatedLandau_ ;
TH1F    * impactPoint_     ;
TH1F    * unitGPlot_       ;
TH2F    * sharing_         ;
TH2F    * erfcPlot_        ;
TRandom * random_          ;
double    diffusion_       ;
double    threshold_       ;
double    effNoise_        ;
  
//================================================================================
int main( int argc, char ** argv )
{
  app_ = new TApplication("plot",&argc, argv);

  argc = app_->Argc() ;
  argv = app_->Argv() ;
  
//  app_->SetReturnFromRun(true); 

  run(argc, argv) ;
  
  app_->Run(true); 

  return 0;
}

//================================================================================
void run(int argc, char ** argv)
{
  int entries = 100000 ;
  
  diffusion_ = 5.0 ;
  threshold_ = 10000 ; // electrons
  effNoise_  = 5000 ;

  RooRealVar x     ("x"     ,"x"     , 0   ,100000      ) ;
  RooRealVar mean  ("mean"  ,"mean"  ,20000,18000 ,22000) ;
  RooRealVar sigma ("sigma" ,"sigma" ,7500 ,1000  ,4000 ) ;
  RooLandau  landau("landau","landau",x    ,mean  ,sigma) ;

  generatedLandau_ = new TH1F("generatedLandau","Generated Landau"   ,100 ,   0  ,100000  ) ;
  impactPoint_     = new TH1F("impactPoint"    ,"Impact point"       ,1000, -50  ,    50  ) ;
  unitGPlot_       = new TH1F("unitGPlot"      ,"Normalized gaussian",1000, -15  ,    15  ) ;
  sharing_         = new TH2F("sharingPlot"    ,"Sharing"            ,200 , -50  ,    50
                                                                     ,200 ,  -1.1,     1.1) ;
  erfcPlot_        = new TH2F("erfcPlot"       ,"Erfc"               ,200 , -50  ,    50
                                                                     ,200 ,   0  ,     1.1) ;
  random_          = new TRandom() ;

  // Generate some events 
  RooDataSet* data = landau.generate(x,entries) ;

  const RooArgSet  * dataSet = data->get() ;
  RooRealVar       * xdata   = dynamic_cast<RooRealVar *>(dataSet->find(x.GetName())) ;

  landau.fitTo(*data) ;
  
  cout << "Name: " << dataSet->GetName() << " Size: " << data->numEntries() << endl ;
  for(int i=0; i<data->numEntries(); ++i)
  {
    data->get(i) ;
    extractCharge(xdata->getVal()) ;
  }
  
  // Plot data and pdf on frame
  RooPlot* frame = x.frame(Title("RooLandau pdf model")) ;
  data ->plotOn(frame) ;
  landau.plotOn(frame) ;

  TCanvas* c = new TCanvas("Landau","Landau",800,800) ;
  c->Divide(3,2) ;
  c->cd(1) ; gPad->SetLeftMargin(0.15) ; frame->GetYaxis()->SetTitleOffset(1.6) ; frame           ->Draw() ;
  c->cd(2) ; gPad->SetLeftMargin(0.15) ; frame->GetYaxis()->SetTitleOffset(1.6) ; generatedLandau_->Draw() ;
  c->cd(3) ; gPad->SetLeftMargin(0.15) ; frame->GetYaxis()->SetTitleOffset(1.6) ; impactPoint_    ->Draw() ;
  c->cd(4) ; gPad->SetLeftMargin(0.15) ; frame->GetYaxis()->SetTitleOffset(1.6) ; unitGPlot_      ->Draw() ;
  c->cd(5) ; gPad->SetLeftMargin(0.15) ; frame->GetYaxis()->SetTitleOffset(1.6) ; erfcPlot_       ->Draw() ;
  c->cd(6) ; gPad->SetLeftMargin(0.15) ; frame->GetYaxis()->SetTitleOffset(1.6) ; sharing_        ->Draw() ;

}

//===============================================================
void extractCharge(double charge) 
{

  double x       = random_->Rndm()*100 - 50    ;
  double impactP = random_->Gaus(x,diffusion_) ;
  double unitG   = -x / diffusion_ ;
  double erfc    = TMath::Erfc(unitG) / 2. ;
  
  double chargeL = charge * (1. - erfc) ;
  double chargeR = charge *       erfc  ;
  
  chargeL += random_->Gaus(0,effNoise_) ;
  chargeR += random_->Gaus(0,effNoise_) ;

  if( chargeL < threshold_ || chargeR < threshold_ ) return ;

  double a       = (chargeL - chargeR) / (chargeL + chargeR) ;
  
  generatedLandau_->Fill(chargeL + chargeR) ;
  impactPoint_    ->Fill(x     ) ;
  unitGPlot_      ->Fill(unitG ) ;
  erfcPlot_       ->Fill(x,erfc) ;
  sharing_        ->Fill(x,a   ) ;
  
//  cout << "Erfc: " << erfc << " " << 1-erfc << endl ;
}
