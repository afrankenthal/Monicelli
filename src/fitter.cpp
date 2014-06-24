/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
/*
#include <iomanip>
#include <TMath.h>
#include <TMatrixD.h>
#include <TMatrixDSym.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TVirtualFitter.h>

#include "Geometry.h"
#include "MessageTools.h"
*/

#include "fitter.h"
#include "ROC.h"

#include <math.h>

#include <TF1.h>
#include <TH1.h>

//================================================================================
fitter::fitter(void) :
    mean_                      (0)
  , sigma_                     (0)
  , calibrationFitFunctionName_("calibrationFitFunction")
  , calibrationFitFunction_    (new TF1(calibrationFitFunctionName_, ROC::calibrationFitFunction, 0, 800000,4))
{
}

//================================================================================
fitter::~fitter(void)
{
    delete calibrationFitFunction_;
}

//================================================================================
void fitter::gaussFit(TH1*  histo)
{
   double hiEdge  = histo->GetBinLowEdge( histo->FindLastBinAbove(0) );
   double max     = hiEdge;
   double loEdge  = histo->GetBinLowEdge( histo->FindFirstBinAbove(0));
   double min     = loEdge;
   double mean    = histo->GetMean()         ;
   double rms     = histo->GetRMS()          ;
   double height  = histo->GetMaximum()      ;
   double nRMS    = 2                        ;

   //ss_.str("") ;ss_ << "Setting range user: " << loEdge << " - " << hiEdge << " for " << histo->GetName() ; STDLINE(ss_.str(),ACWhite) ;

   TF1 *gaus = new TF1("gausFitFunc", "[2]*TMath::Gaus(x,[0],[1])", min, max) ;
   gaus->SetLineColor(4);
   gaus->SetParNames("Mean", "Sigma", "Height");

   for(int i=0; i<4; i++)
   {
     if ( (mean - nRMS*rms) > min ) min = mean - nRMS*rms ;
     if ( (mean + nRMS*rms) < max ) max = mean + nRMS*rms;

     //histo->GetXaxis()->SetRangeUser(loEdge, hiEdge);
     gaus->SetRange(min, max);
     gaus->SetParameters(mean, rms, height);

     histo->Fit("gausFitFunc", "RLQ");

     mean_ = gaus->GetParameter(0);
     sigma_= fabs( gaus->GetParameter(1) );

     if ( (mean_+sigma_*1.5) < hiEdge  ) hiEdge =  (mean_+sigma_*1.5);
     if ( (mean_-sigma_*1.5) > loEdge  ) loEdge =  (mean_-sigma_*1.5);

     //histo->GetXaxis()->SetRangeUser(loEdge, hiEdge);

     mean = histo->GetMean();
     rms  = fabs( histo->GetRMS() );

     gaus->SetRange(loEdge, hiEdge);
     gaus->SetParameters(mean, rms, height);
     histo->Fit("gausFitFunc", "RLQ");

     mean_ = gaus->GetParameter(0);
     sigma_= fabs( gaus->GetParameter(1) );
   }

   //ss_.str(""); ss_  << "Chi2: " <<  gaus->GetChisquare()/gaus->GetNDF(); STDLINE(  ss_.str() ,ACGreen ) ;
}

//===============================================================================
void fitter::gaussFit(TH1*  histo, double mean, double userRMS, double nRMS)
{
   double max    = histo->GetBinLowEdge( histo->FindLastBinAbove() );
   double height = histo->GetMaximum()      ;
   double rms = 0;
   if ( nRMS     <= 0 ) nRMS = 2;
   if ( userRMS  <= 0 ) rms = histo->GetRMS() ;
   else                 rms = userRMS         ;

   ss_.str("") ; ss_ << "For " << histo->GetName() << " mean: " << mean << " userRMS: " << userRMS << " nRMS: " << nRMS << " rms: " << rms; STDLINE(ss_.str(),ACWhite) ;

   double min = mean-nRMS*rms;

   if ( (mean - nRMS*rms) > min ) min = mean - nRMS*rms ;
   if ( (mean + nRMS*rms) < max ) max = (mean + nRMS*rms);

   TF1 *gaus = new TF1("gausFitFunc", "[2]*TMath::Gaus(x,[0],[1])",min , max) ;
   gaus->SetParNames("Mean", "Sigma", "Height"  );
   gaus->SetLineColor(2);
   ss_.str("") ; ss_ << " min: " << min << " max: " << max ; STDLINE(ss_.str(),ACWhite) ;
   gaus->SetParameters(mean, rms, height);

   histo->Fit("gausFitFunc", "RLQ");

   mean_ = gaus->GetParameter(0);
   sigma_= fabs( gaus->GetParameter(1) );

   ss_.str("");
   ss_  << "Chi2: " <<  gaus->GetChisquare()/gaus->GetNDF();
   STDLINE(  ss_.str() ,ACGreen ) ;
}

//=================================================================
void fitter::linearFit(TH1*  histo, double *slope, double *q, double xmin, double xmax)
{
   TF1 *line = new TF1("linearFitFunc", "x*[0]+[1]", -1000, +1000) ;
   line->SetLineColor(4);
   line->SetParNames("Slope", "q");
   line->SetParameters(*slope,*q );


   histo->Fit("linearFitFunc","Q","",xmin,xmax);
}

//==============================================================================
fitter::fitResultDef fitter::calibrationFit(TH1 *   histo,
                                            double  xmin,
                                            double  xmax,
                                            double* pars)
{
    double* par = 0;
    double* cov = 0;
    if(histo->GetEntries() != 0)
    {
        calibrationFitFunction_->SetLineColor(4);
        calibrationFitFunction_->SetParNames("0", "1", "2", "3");
        //line->SetParameters(400,300,0.00004,-0.1);
        if(pars==0)
            calibrationFitFunction_->SetParameters(400,300,0.00004,-0.1);
        else
            calibrationFitFunction_->SetParameters(pars);

        calibrationFitFunction_->SetRange(xmin,xmax);
        //*TEMP*/ TFitResultPtr r = histo->Fit(calibrationFitFunction_,"0+SQR","",xmin,xmax);
        TFitResultPtr fitResult = histo->Fit(calibrationFitFunction_,"SQR","",xmin,xmax);

        if((int)fitResult == 0)
            par  = histo->GetFunction(calibrationFitFunctionName_)->GetParameters() ;
        else
        {
            ss_.str("");
            ss_ << "WARNING: fit failed returned error: " << (int)fitResult;
            STDLINE(ss_.str(), ACRed);
        }
    }
    //std::cout << "Fitter par: " << par << " cal funk pars: " << calibrationFitFunction_->GetParameters() << std::endl;
    return std::make_pair(par,cov);

}

//==============================================================================
int fitter::getCalibrationFitFunctionNPar(void)
{
    return calibrationFitFunction_->GetNpar();
}

