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
    gaus->SetLineColor(2);
    gaus->SetParNames("Mean", "Sigma", "Height");

    for(int i=0; i<4; i++)
    {
        if ( (mean - nRMS*rms) > min ) min = mean - nRMS*rms ;
        if ( (mean + nRMS*rms) < max ) max = mean + nRMS*rms;

        //histo->GetXaxis()->SetRangeUser(loEdge, hiEdge);
        gaus->SetRange(min, max);
        gaus->SetParameters(mean, rms, height);

        ss_.str("") ; ss_ << "Gaussian fit to " << histo->GetName() ;
        STDLINE(ss_.str(),ACYellow) ;
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

//===============================================================================
void fitter::chi2Fit(TH1* histo)
{
    //    double hiEdge  = histo->GetBinLowEdge( histo->FindLastBinAbove(0) );
    double min     = 0;
    double max     = 20;
    //    double loEdge  = histo->GetBinLowEdge( histo->FindFirstBinAbove(0));
    //    double mean    = histo->GetMean()         ;
    //    double rms     = histo->GetRMS()          ;
    //    double height  = histo->GetMaximum()      ;

    //ss_.str("") ;ss_ << "Setting range user: " << loEdge << " - " << hiEdge << " for " << histo->GetName() ; STDLINE(ss_.str(),ACWhite) ;

    //stringstream functionDefinition;
    //functionDefinition << "TMath::Power(x,

    TF1* chi2 = new TF1("chi2Func", "[1]*TMath::Power(x,[0]/2-1)*expo(0,-.5)/(TMath::Gamma([0]/2)*TMath::Power(2,[0]/2))", min, max) ;
    chi2->SetLineColor(4);
    chi2->SetParNames("DOF", "Amplitude");
    histo->Fit(chi2, "RLQ");
    /*
    for(int i=0; i<4; i++)
    {
      if ( (mean - nRMS*rms) > min ) min = mean - nRMS*rms ;
      if ( (mean + nRMS*rms) < max ) max = mean + nRMS*rms;

      //histo->GetXaxis()->SetRangeUser(loEdge, hiEdge);
      chi2->SetRange(min, max);
      chi2->SetParameters(mean, rms, height);

      histo->Fit("chi2Func", "RLQ");

      mean_ = chi2->GetParameter(0);
      sigma_= fabs( chi2->GetParameter(1) );

      if ( (mean_+sigma_*1.5) < hiEdge  ) hiEdge =  (mean_+sigma_*1.5);
      if ( (mean_-sigma_*1.5) > loEdge  ) loEdge =  (mean_-sigma_*1.5);

      //histo->GetXaxis()->SetRangeUser(loEdge, hiEdge);

      mean = histo->GetMean();
      rms  = fabs( histo->GetRMS() );

      chi2->SetRange(loEdge, hiEdge);
      chi2->SetParameters(mean, rms, height);
      histo->Fit("chi2Func", "RLQ");

      mean_ = chi2->GetParameter(0);
      sigma_= fabs( chi2->GetParameter(1) );
    }
*/
    //ss_.str(""); ss_  << "Chi2: " <<  gaus->GetChisquare()/gaus->GetNDF(); STDLINE(  ss_.str() ,ACGreen ) ;
}

//=================================================================
void fitter::linearFit(TH1*  histo, double *slope, double *q, double tolerance )
{
    double nsig = tolerance ;

    ss_.str("") ; ss_ << "Fitting "
                      << histo->GetTitle()
                      << " with a tolerance of "
                      << nsig
                      << " sigmas" ;
    STDLINE(ss_.str(),ACGreen) ;

    TH1D * t = (TH1D*)histo->Clone() ;
    TH1F t1("t","t",100,-2.5,2.5) ;
    t->Reset() ;

    for(int bin=1; bin<histo->GetNbinsX(); ++bin )
    {
        if( histo->GetBinContent(bin) == 0 &&
            histo->GetBinError  (bin) == 0 ) continue ;
        t1.Fill(histo->GetBinContent(bin)) ;
    }
    double mea = t1.GetMean() ;
    double rms = t1.GetRMS()  ;
    ss_.str("") ; ss_ << "Mean: " << mea << " +/- " << rms; STDLINE(ss_.str(),ACCyan) ;

    int lowB =  5000 ;
    int higB = -5000 ;
    for(int bin=1; bin<histo->GetNbinsX(); ++bin )
    {
        if( histo->GetBinContent(bin) == 0 &&
            histo->GetBinError(bin)   == 0 ) continue ;
        if( histo->GetBinContent(bin) > mea-rms*nsig &&
            histo->GetBinContent(bin) < mea+rms*nsig )
        {
            if( lowB > bin ) lowB = bin ;
            if( higB < bin ) higB = bin ;
//            t->SetBinContent(bin,histo->GetBinContent(bin)) ;
//            t->SetBinError  (bin,histo->GetBinError  (bin)) ;
        }
    }
    double low = histo->GetBinCenter(lowB) ;
    double hig = histo->GetBinCenter(higB) ;
    ss_.str("") ; ss_ << "Cut: from " << low << " to " << hig; STDLINE(ss_.str(),ACCyan) ;

    TF1  * f = new TF1("linearFitFunc", "x*[0]+[1]", low, hig) ;
    f->SetLineColor(2) ;

    f->SetParameters( *slope, *q );

    histo->GetYaxis()->SetRangeUser(-2.5,2.5) ;
//    t    ->GetYaxis()->SetRangeUser(-2.5,2.5) ;
    f->SetLineColor(4);
    f->SetParNames("Slope", "q");
    f->SetParameters(*slope,*q );

    histo->GetXaxis()->SetRangeUser(-low-.1*low,hig+.1*hig) ;
    histo->Fit("linearFitFunc","","",low,hig);

    delete t ;
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

        if ((int)fitResult == 0 || (int)fitResult == 4)
	  // fitResult == 0 --> fit converged and covariance matrix with positive determinant
	  // fitResult == 4 --> fit converged and covariance matrix with negative determinant
	  par = histo->GetFunction(calibrationFitFunctionName_)->GetParameters() ;
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

