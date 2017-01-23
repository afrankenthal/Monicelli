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
 
#include "calibrationLoader.h"

#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>

// ### Hard coded values ###
#define SCALE_F 1000
#define FITMINPOINTS 10
#define MAXTHRESHOLD 10000 // [e-]
#define DYNAMICRANGE 200
// #########################

//=========================================================
calibrationLoader::calibrationLoader(fileEater *theFileEater, HManager *theHManager, fitter *theFitter) :
    theFileEater_        (theFileEater)
  , theHManager_         (theHManager)
  , theFitter_           (theFitter)
  , emptyTH1I_           (new TH1I())
  , calibrationsLoaded_  (false)
{
    emptyTH1I_->SetName("emptyCalibration");
    calibrations_.clear();
}

//=========================================================
calibrationLoader::~calibrationLoader(void)
{
//    STDLINE("calibrationLoader::~calibrationLoader",ACRed);
    delete emptyTH1I_;
//    STDLINE("calibrationLoader::~calibrationLoader",ACGreen);
}

//=========================================================
bool calibrationLoader::loadASCIIcalibrationFile(std::string fileName)
{
    STDLINE(std::string("Calibration file; " + fileName), ACPurple);
    // Open file
    std::ifstream  *inputFile   ;
    inputFile = new std::ifstream(fileName.c_str(), std::ifstream::in );

    if( !inputFile->is_open() )
    {
        STDLINE(std::string("Could not open file ") + fileName, ACRed);
        return false;
    }

    // Define regex expressions used to read information from calibration files.
    boost::cmatch what ;
    const boost::regex expIteration   ("Iteration\\s+(\\d+)\\s+\\D+\\s+reg\\s+=\\s+(\\d+)\\s*"       , boost::regex::perl);
    const boost::regex expRowColAdc   ("r\\s+(\\d+)\\s+c\\s+(\\d+)\\s+h\\s+(\\d+)\\s+a\\s+(\\d+)\\s*", boost::regex::perl);
    const boost::regex expRegisters   ("-+\\s*(\\d*)\\s*R*e*g*i*s*t*e*r*\\s*-+\\s*"                  , boost::regex::perl);
    const boost::regex expHeaderValues("(\\w+):\\s+(\\w+[.]*\\s*\\w*)\\s*"                           , boost::regex::perl);
    std::string line = "" ;

    int currentReg=0;

    // Clear the pixel data and header map definitions
    pixels_.clear();
    header_.clear();

    int nLines = 0 ;

    bool header = true;
    bool headerValues = false;
    while(inputFile->good())     // While there are lines left to read in the file...
    {
        std::getline(*inputFile,line);
        if(header && line.find("Iteration") != 0)
        {
            if(boost::regex_match(line.c_str(), what, expRegisters, boost::match_extra))     //fill the header
            {
                headerValues = true;
            }
            else if(headerValues)
            {
                if(boost::regex_match(line.c_str(), what, expHeaderValues, boost::match_extra))
                {
                    STDLINE(what[0],ACYellow);
                    header_[what[1]] = what[2];
                }
                else
                {
                    header       = false;
                    headerValues = false;
                }
            }
        }
        else
        {
            if(line.find("Iteration") != std::string::npos)
            {
                if( boost::regex_match(line.c_str(), what, expIteration, boost::match_extra) )
                {
                    currentReg = Utils::toInt(what[2])*ELECTRONS_NUMBER;  //reg
                }

            }
            else if(boost::regex_match(line.c_str(), what, expRowColAdc, boost::match_extra)) //fill pixel
            {
	      pixels_[Utils::toInt(what[1])][Utils::toInt(what[2])][currentReg] = std::make_pair(Utils::toInt(what[3]), Utils::toInt(what[4]));
            }
        }
        ++nLines ;
    }
    inputFile->close();
    delete inputFile;
    return true;
}

//=========================================================
bool calibrationLoader::loadROOTcalibrationFiles(std::string detector, ROC *roc, std::string fileName)
{
    STDLINE(std::string("Loading calibrations from " + fileName),ACRed);

    //When you open the file it gives this warning Warning in <TObjectTable::Remove>
    //that should be fixed in other root releases OR you are trying to delete something twice
    TFile * inputFile = TFile::Open(fileName.c_str(), "READ");
    TIter nextPlot(inputFile->GetListOfKeys());

    ss_.str("");
    ss_ << detector << " - ROC: " << roc->getID();
    if( calibrations_.count(detector) && calibrations_[detector].count(roc->getID()) )
        calibrations_[detector][roc->getID()].clear();

    std::string detRoc = ss_.str();

    while(TKey * plotKey = (TKey*)nextPlot())
    {
        TH1 * plot = (TH1*)plotKey->ReadObj();
        ss_.str("");
        ss_ << plot->GetName();
        //STDLINE(ss_.str(),ACRed);

        // Check if station, roc info match in title
        if( ss_.str().substr(0,detRoc.length()) != detRoc)
        {
            STDLINE(std::string("")+ss_.str()+" does not match "+detRoc, ACRed);
            return false;
        }

        //find row, column from plot name
        int row = 0;
        int col = 0;

        boost::cmatch matchedNums;
        static const boost::regex title_exp ("Station:\\s+(\\d+)\\s+-\\s+Plaq:\\s+(\\d+)\\s+-\\s+ROC:\\s+(\\d+)\\s+-\\s+r:\\s+(\\d+)\\s+-\\s+c:\\s+(\\d+)\\s*", boost::regex::perl);
        if(boost::regex_match( ss_.str().c_str(), matchedNums, title_exp, boost::match_extra) )
        {
            row = Utils::toInt(matchedNums[4]);
            col = Utils::toInt(matchedNums[5]);
//            ss_.str("");
//            ss_ << "Extracted r" << row << " c" << col << " from title " << plot->GetName();
//            STDLINE(ss_.str(), ACGreen);
        }
        else
        {
            STDLINE(std::string("ERROR ON ")+ss_.str(), ACRed);
            return false;
        }

        double* par = 0;
        //cov not used so stays NULL
        double* cov = 0;
        //ss_.str("") ; ss_ << theFitter_->getCalibrationFitFunctionName() ; STDLINE(ss_.str(),ACGreen) ;
        if(plot->GetFunction(theFitter_->getCalibrationFitFunctionName()))
        {
            if( plot->GetFunction(theFitter_->getCalibrationFitFunctionName())->GetNumberFreeParameters() != 0)
            {
             par  = plot->GetFunction(theFitter_->getCalibrationFitFunctionName())->GetParameters();
//             ss_.str("") ; ss_ << "par[0] " << par[0] ; STDLINE(ss_.str(),ACGreen) ;
//             ss_.str("") ; ss_ << "par[1] " << par[1] ; STDLINE(ss_.str(),ACGreen) ;
//             ss_.str("") ; ss_ << "par[2] " << par[2] ; STDLINE(ss_.str(),ACGreen) ;
//             ss_.str("") ; ss_ << "par[3] " << par[3] ; STDLINE(ss_.str(),ACGreen) ;
             roc->setCalibrationFunction(row, col, par, cov);
            }
        }

//LATEST        roc->setCalibrationFunction(row, col, par, cov, theFitter_->getCalibrationFitFunctionNPar());
        calibrations_[detector][roc->getID()][row][col].first = (TH1I*)plot;
        calibrations_[detector][roc->getID()][row][col].second = std::make_pair(par,cov); // ToROOT6
    }
    return true;
}

//====================================================================
bool calibrationLoader::loadAllCalibrationFiles()
{
    STDLINE("Loading all calibrations",ACRed);
    calibrations_.clear();
    Geometry * theGeometry = theFileEater_->getGeometry();

    std::string calibrationsDir = std::string(getenv("Monicelli_CalSample_Dir")) + "/";
    boost::cmatch what ;
    const boost::regex exp ("(\\w+).txt", boost::regex::perl);

    // Cycle through detectors contained in theGeometry
    for(Geometry::iterator det=theGeometry->begin(); det!=theGeometry->end(); det++)
    {
        // Cycle through ROCs in the detector
        for(Detector::iterator roc=(*det).second->begin(); roc!=(*det).second->end(); roc++ )
        {
            ss_.str("");
            ss_ << "Loading calibrations for " << det->first << " -- rocID: " << roc->first << " roc ID: " << roc->second->getID();
            STDLINE(ss_.str(),ACGreen);
            std::string calibrationFileRadix = "";
            if(boost::regex_match((*roc).second->getCalibrationFilePath().c_str(), what, exp, boost::match_extra))
                calibrationFileRadix = what[1];
            else
            {
                STDLINE("ERROR: Can't find the extension .txt in " + (*roc).second->getCalibrationFilePath(), ACRed);
                continue;
            }

            // Look for root file in root calibrations directory, load if loading from ROOT
            std::string path =  calibrationsDir + calibrationFileRadix + ".root";
            if( readROOT_ && boost::filesystem::exists(path) && loadROOTcalibrationFiles(det->first, roc->second, path))
            {
                STDLINE(std::string("Loaded ")+path,ACGreen) ;
            }
            else
            {
                path = calibrationsDir + (*roc).second->getCalibrationFilePath();
                if(this->loadASCIIcalibrationFile(path))   //Loads calibration file information into pixelData, header maps
                {
                    if(det->second->isDUT())
                        this->makeDUTHistograms(det->first, roc->second, true, true, writeASCII_);
                    else
                        this->makeHistograms(   det->first, roc->second, true, true, writeASCII_);
                }
		//else
                //    return false;
            }
            currentIteration_ += SCALE_F/det->second->getNumberOfROCs();
        }
    }
    makeChi2Histograms();
    calibrationsLoaded_ = true ;
    theGeometry->setCalibrationDone(true);

    return true;
}

//=========================================================
void  calibrationLoader::saveROOTcalibrationFiles(std::string fileDirectory)
{
    if(!calibrationsLoaded_)
    {
        STDLINE(std::string("Unable to save root calibrations: full calibrations not loaded."), ACRed);
        return;
    }

    boost::cmatch what ;
    const boost::regex exp ("(\\w+).txt", boost::regex::perl);
    Geometry * theGeometry = theFileEater_->getGeometry();
    std::string path = "";

    // Cycle through detectors contained in theGeometry and ROCs
    for(Geometry::iterator det=theGeometry->begin(); det!=theGeometry->end(); det++)
        for(Detector::iterator roc=det->second->begin(); roc!=det->second->end(); roc++ )
        {
            std::string calibrationFileRadix = "";
            STDLINE(roc->second->getCalibrationFilePath(), ACGreen);
            if(boost::regex_match(roc->second->getCalibrationFilePath().c_str(), what, exp, boost::match_extra))
                calibrationFileRadix = what[1];
            else
            {
                STDLINE(std::string("WARNING: Can't match the regular expression very likely because there is a strange character like - or + in the file name: ") + roc->second->getCalibrationFilePath() + ". You need to change the file name because I won't save it or load it!", ACRed);
                continue;
            }
            path = fileDirectory;
            path += std::string("/") + calibrationFileRadix + ".root";
            if(boost::filesystem::exists(path))
            {
                STDLINE(std::string("WARNING: File ") + path + " exists. Not overwriting...", ACRed);
                continue;
            }
            else
            {
                STDLINE(std::string("Saving calibrations to " + path), ACGreen);
            }
            // Open TFile
            TFile * outputTFile = TFile::Open(path.c_str(), "RECREATE");

            // Cycle through pixel rows and cols, write the histogram to file.
            if(calibrations_.find((*det).first) == calibrations_.end() || calibrations_[(*det).first].find((*roc).first) == calibrations_[(*det).first].end())
                continue;
            pixelPlotsMapDef * pixels = &calibrations_[(*det).first][(*roc).first];
            if( !pixels ) STDLINE("No pixels!!!",ACRed) ;
            for(calibrationLoader::pixelPlotsMapDef::iterator r=(*pixels).begin(); r!=(*pixels).end(); r++)
            {
                for(std::map<int, std::pair<TH1I*, fitter::fitResultDef> >::iterator c=(*r).second.begin(); c!=(*r).second.end(); c++)
                {
                    if( (*c).second.first == NULL ) STDLINE("No c!!!",ACRed) ;
                    (*c).second.first->Write();
                }
            }

            outputTFile->Close();
        }
}

//==================================================================
bool calibrationLoader::makeDUTHistograms(std::string detector, ROC *roc, bool fit,  bool writeGeometry, bool writeASCII)
{
    if( calibrations_.count(detector) && calibrations_[detector].count(roc->getID()) )
        calibrations_[detector][roc->getID()].clear();

    if(pixels_.empty())
    {
        STDLINE("WARNING: no pixels calibration file loaded",ACRed);
        return false;
    }

    int maxRows = roc->getNumberOfRows();
    int maxCols = roc->getNumberOfRows();

    ss_.str("") ;
    ss_ << "Calibrating ROC " << roc->getID() <<  " of detector " << detector ;
    STDLINE(ss_.str(),ACCyan) ;

    if(writeASCII) outputFile_.open(outputASCIIfile_.c_str(), std::ios_base::app);

    //bool isDUT = theFileEater_->getGeometry()->getDetector(detector)->isDUT();
    //if(isDUT)
    //{
    TH2F*  firstBinHisto = new TH2F("firstBinHisto_", "First bin distr", maxRows, 0, maxRows, maxCols, 0, maxCols);
    TH2F*  lastBinHisto  = new TH2F("lastBinHisto_" , "Last bin distr" , maxRows, 0, maxRows, maxCols, 0, maxCols);

    TH1I* calib   [maxRows][maxCols];
    TH1I* calibNew[maxRows][maxCols];

    TH1F* hPars[4];

    double plotsRangeAndBins[4][3];

    plotsRangeAndBins[0][2] =  1000 ;
    plotsRangeAndBins[0][1] =     0 ;
    plotsRangeAndBins[0][0] =   125 ;

    plotsRangeAndBins[1][2] =   500 ;
    plotsRangeAndBins[1][1] =     0 ;
    plotsRangeAndBins[1][0] =   125 ;

    plotsRangeAndBins[2][2] = 1E-04 ;
    plotsRangeAndBins[2][1] =     0 ;
    plotsRangeAndBins[2][0] =   125 ;

    plotsRangeAndBins[3][2] =     2 ;
    plotsRangeAndBins[3][1] =    -2 ;
    plotsRangeAndBins[3][0] =   125 ;

    int    binsX = 0;
    double Xmin  = 0;
    double Xmax  = 0;

    for(int p=0; p<4; p++)
    {
        binsX = (int)plotsRangeAndBins[p][0];
        Xmin  = plotsRangeAndBins[p][1];
        Xmax  = plotsRangeAndBins[p][2];

        ss_.str(); ss_ << "Parameter " << p;
        hPars[p] = new TH1F(ss_.str().c_str(), ss_.str().c_str(), binsX, Xmin, Xmax);
    }
    //}
    for(int r=0; r<maxRows; r++)
    {
        for(int c=0; c<maxCols; c++)
        {
            
            ss_.str(""); ss_ << "Service histo calibration of pixel at row: " << r << ", col " << c;
            calib[r][c] = new TH1I(ss_.str().c_str(), ss_.str().c_str(), 255, 0, 255*ELECTRONS_NUMBER);
            ss_.str("");
            ss_ << detector << " - ROC: " << roc->getID() << " - r: " << r << " - c: " << c;
            calibNew[r][c] = new TH1I(ss_.str().c_str(), ss_.str().c_str(), 255, 0, 255*ELECTRONS_NUMBER);
        }
    }

    int bin;
    for (calibrationLoader::pixelDataMapDef::iterator r=pixels_.begin(); r!=pixels_.end(); ++r)
    {
        for (std::map<int,aPixelDataMapDef>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
        {
            for(calibrationLoader::aPixelDataMapDef::iterator it2=(*c).second.begin(); it2!=(*c).second.end(); ++it2)
            {
                bin = calib[(*r).first][(*c).first]->Fill((*it2).first,(*it2).second.second);
                calib[(*r).first][(*c).first]->SetBinError(bin,2.5);
            }
        }
    }

    int nBins;
    for (calibrationLoader::pixelDataMapDef::iterator r=pixels_.begin(); r!=pixels_.end(); ++r)
    {
        for (std::map<int,aPixelDataMapDef>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
        {
            bin = 1;
            nBins = calib[(*r).first][(*c).first]->GetNbinsX();
            while(bin<=nBins && calib[(*r).first][(*c).first]->GetBinContent(bin)==0)
                bin++;
            firstBinHisto->Fill((*r).first,(*c).first,bin);

        }
    }

    for (calibrationLoader::pixelDataMapDef::iterator r=pixels_.begin(); r!=pixels_.end(); ++r)
    {
        for (std::map<int,aPixelDataMapDef>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
        {
            bin = calib[(*r).first][(*c).first]->GetNbinsX();
            while(bin>=1 && calib[(*r).first][(*c).first]->GetBinContent(bin)==0)
                bin--;
            lastBinHisto->Fill((*r).first,(*c).first,bin);
        }
    }

    double precADC   ;
    double currentADC;
    int    lastBin   ;
    int    firstBin  ;
    int    precBin   ;
    for (calibrationLoader::pixelDataMapDef::iterator r=pixels_.begin(); r!=pixels_.end(); ++r)
    {
        for (std::map<int,aPixelDataMapDef>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
        {
            lastBin  = (int)lastBinHisto ->GetBinContent(lastBinHisto ->GetXaxis()->FindBin((*r).first),lastBinHisto ->GetYaxis()->FindBin((*c).first));
            firstBin = (int)firstBinHisto->GetBinContent(firstBinHisto->GetXaxis()->FindBin((*r).first),firstBinHisto->GetYaxis()->FindBin((*c).first));
            precADC  = calib[(*r).first][(*c).first]->GetBinContent(lastBin);
            precBin = lastBin-1;
            for(int b=lastBin-1; b>=firstBin; b--)
            {
                currentADC = calib[(*r).first][(*c).first]->GetBinContent(b);
                if (std::abs(precADC-currentADC)>30*(precBin-b)) continue;
                else
                {
                    if (precADC == 0) continue;
                    calibNew[(*r).first][(*c).first]->SetBinContent(b,precADC);
                    calibNew[(*r).first][(*c).first]->SetBinError(b,2.5);
                    precADC = currentADC;
                    precBin = b;
                }
            }
        }
    }


    double* pars;
    if(fit)
    {
        fitter::fitResultDef fitR;

        double minBin,maxBin;

        for (calibrationLoader::pixelDataMapDef::iterator r=pixels_.begin(); r!=pixels_.end(); ++r)
        {
	  for (std::map<int,aPixelDataMapDef>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
            {
	      firstBin = (int)firstBinHisto->GetBinContent(firstBinHisto ->GetXaxis()->FindBin((*r).first),firstBinHisto ->GetYaxis()->FindBin((*c).first));
	      lastBin  = (int)lastBinHisto ->GetBinContent(lastBinHisto  ->GetXaxis()->FindBin((*r).first),lastBinHisto  ->GetYaxis()->FindBin((*c).first));
	      minBin = calibNew[(*r).first][(*c).first]->GetMinimum(1);
	      maxBin = calibNew[(*r).first][(*c).first]->GetBinContent(calibNew[(*r).first][(*c).first]->GetMaximumBin());
	      
	      if ((calibNew[(*r).first][(*c).first]->GetEntries() >= FITMINPOINTS) &&
		  (calibNew[(*r).first][(*c).first]->GetBinCenter((int)firstBinHisto->GetBinContent(firstBinHisto->GetXaxis()->FindBin((*r).first),firstBinHisto->GetYaxis()->FindBin((*c).first))) < MAXTHRESHOLD) &&
		  (maxBin-minBin > DYNAMICRANGE))
		{
		  fitR = theFitter_->calibrationFit(calibNew[(*r).first][(*c).first],
						    // 10000,
						    calibNew[(*r).first][(*c).first]->GetBinCenter(firstBin),
						    25000,
						    NULL);
		  pars = fitR.first;
		}
	      else continue;
	      
	      if (fitR.first == NULL)
		{
		  ss_.str("") ;
		  ss_ << "WARNING: first fit failed for detector " << detector << " - ROC: " << roc->getID() << " at row " << (*r).first << ", col " << (*c).first ;
		}
	      else
		{
		  for (int p = 0; p < 4; p++)
		    {
		      hPars[p]->Fill(pars[p]);
		    }
		}
            }
        }
	
        double rightPars[4];
        for(int p=0; p<4; p++) //Fit a second time with initial parameters equal to mean of the parameters found previously
        {
            rightPars[p] = hPars[p]->GetMean();
            ss_.str(""); ss_ << "Parameter " << p << ": " << rightPars[p];
            STDLINE(ss_.str(),ACGreen);
        }

        for (calibrationLoader::pixelDataMapDef::iterator r=pixels_.begin(); r!=pixels_.end(); ++r)
        {
	  for (std::map<int,aPixelDataMapDef>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
            {
	      firstBin = (int)firstBinHisto->GetBinContent(firstBinHisto ->GetXaxis()->FindBin((*r).first),firstBinHisto ->GetYaxis()->FindBin((*c).first));
	      lastBin  = (int)lastBinHisto ->GetBinContent(lastBinHisto  ->GetXaxis()->FindBin((*r).first),lastBinHisto  ->GetYaxis()->FindBin((*c).first));
	      minBin = calibNew[(*r).first][(*c).first]->GetMinimum(1);
	      maxBin = calibNew[(*r).first][(*c).first]->GetBinContent(calibNew[(*r).first][(*c).first]->GetMaximumBin());
	      
	      if ((calibNew[(*r).first][(*c).first]->GetEntries() >= FITMINPOINTS) &&
		  (calibNew[(*r).first][(*c).first]->GetBinCenter((int)firstBinHisto->GetBinContent(firstBinHisto->GetXaxis()->FindBin((*r).first),firstBinHisto->GetYaxis()->FindBin((*c).first))) < MAXTHRESHOLD) &&
		  (maxBin-minBin > DYNAMICRANGE))
		{
		  fitR = theFitter_->calibrationFit(calibNew[(*r).first][(*c).first],
						    // 10000,
						    calibNew[(*r).first][(*c).first]->GetBinCenter(firstBin),
						    25000,
						    rightPars);
		}
	      else fitR.first = NULL;
	      
	      if (calibNew[(*r).first][(*c).first]->GetEntries() !=0 && fitR.first == NULL)
                {
		  STDLINE("",ACWhite);
		  ss_.str("") ;
		  ss_ << "WARNING: fit failed for detector " << detector << " - ROC: " << roc->getID() << " at row " << (*r).first << ", col " << (*c).first ;
		  STDLINE(ss_.str(),ACRed) ;
                }
	      
                calibrations_[detector][roc->getID()][(*r).first][(*c).first].second = fitR;

                if (writeGeometry)
                   roc->setCalibrationFunction((*r).first, (*c).first, fitR.first, fitR.second);

                if(writeASCII)
                {
                    if( fitR.first!=NULL )
                    {
                        outputFile_ << detector << " " << roc->getID() << " " << (*r).first << " " << (*c).first << " ";
                        for(int p=0; p < 4; p++)
                        {
                            outputFile_ << fitR.first[p] << " ";
                        }
                        outputFile_ << std::endl;
                    }
                }
             calibrations_[detector][roc->getID()][(*r).first][(*c).first].first = calibNew[(*r).first][(*c).first];

            }
        }
    }

    if(writeASCII) outputFile_.close();

    for(int r=0; r<maxRows; r++)
    {
        for(int c=0; c<maxCols; c++)
        {

            calib[r][c]->Delete();
        }
    }

    firstBinHisto->Delete();
    lastBinHisto->Delete();

    for(int p=0;p<4;p++)
        hPars[p]->Delete();

    STDLINE("Done",ACCyan) ;
    return true;

}

//==================================================================
bool calibrationLoader::makeHistograms(std::string detector, ROC *roc, bool fit,  bool writeGeometry, bool writeASCII)
{
    if(pixels_.empty())
    {
        STDLINE("WARNING: no pixels calibration file loaded",ACRed);
        return false;
    }

    if( calibrations_.count(detector) > 0 && calibrations_[detector].count(roc->getID()) > 0 )
        calibrations_[detector][roc->getID()].clear();

    ss_.str("") ;
    ss_ << "Calibrating ROC " << roc->getID() <<  " of detector " << detector ;
    STDLINE(ss_.str(),ACCyan) ;

    if(writeASCII) outputFile_.open(outputASCIIfile_.c_str(), std::ios_base::app);

    //Cycle through each pixel on the ROC
    for (calibrationLoader::pixelDataMapDef::iterator r=pixels_.begin(); r!=pixels_.end(); ++r)
    {
        for (std::map<int,aPixelDataMapDef>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
        {
            //if( cc++ > 10) {continue;}
            ss_.str("");
            ss_ << detector << " - ROC: " << roc->getID() << " - r: " << (*r).first << " - c: " << (*c).first;
            TH1I * calib = new TH1I(ss_.str().c_str(), ss_.str().c_str(), 255, 0, 255*ELECTRONS_NUMBER);
            calib->GetXaxis()->SetTitle("V_cal (electrons number)");
            calib->GetYaxis()->SetTitle("ADC counts");
            calib->GetYaxis()->SetRangeUser(0,750);
            calib->SetDirectory(0);
            for (calibrationLoader::aPixelDataMapDef::iterator it2=(*c).second.begin(); it2!=(*c).second.end(); ++it2)
            {
                int bin = calib->Fill( (*it2).first, (*it2).second.second );
                calib->SetBinError(bin,2.4);//sqrt((*it2).second.second));
            }
            calibrations_[detector][roc->getID()][(*r).first][(*c).first].first = calib;
            //ss_.str("") ; ss_ << CALIBRATIONS << "/Results/Gain/" << detector->first ;

            //theHManager_->storeCustomTObject(calib,ss_.str()) ;
            if(fit)
            {
                fitter::fitResultDef fitR = theFitter_->calibrationFit(calib,
                                                                       //2000,
                                                                       //50000,
                                                                        2000,
                                                                       35000,
                                                                       NULL);
                if(calib->GetEntries() != 0 && fitR.first == NULL /*|| fitR.second == NULL*/)
                {
                    std::cout << std::endl ; // This interrupts the STSNAP without CR issued by the theFitter_->calibrationFit call above
                    ss_.str("") ;
                    ss_ << "WARNING: fit failed for detector " << detector << " - ROC: " << roc->getID() << " at row " << (*r).first << ", col " << (*c).first ;
                    //STDLINE(ss_.str(),ACRed) ;
                }
                calibrations_[detector][roc->getID()][(*r).first][(*c).first].second = fitR;


                //if(fitR.first == NULL /*|| fitR.second == NULL*/)
                    //STDLINE("Fit is null",ACRed);
                if(writeGeometry)
                {
                    //ss_.str(""); ss_ << "par[0]: " << par[0] << " par[1]: " << par[1];
                    //if ((*r).first == (*c).first) STDLINE(ss_.str(),ACPurple);
                    roc->setCalibrationFunction((*r).first, (*c).first, fitR.first, fitR.second);
                }
                if(writeASCII)
                {
                    if( fitR.first!=NULL )
                    {
                        outputFile_ << detector << " " << roc->getID() << " " << (*r).first << " " << (*c).first << " ";
                        for(int p=0; p < theFitter_->getCalibrationFitFunctionNPar(); p++) //FIXME THIS IS WRONG IF THE FUNCTION IN THE FITTER IS DIFFERENT THAN THE ONE IN THE FILE
                        {
                            outputFile_ << fitR.first[p] << " ";
                        }
                        outputFile_ << std::endl;
                    }
                }
            }
        }
    }

    if(writeASCII) outputFile_.close();

    STDLINE("Done",ACCyan) ;
    return true;
}

//=================================================================
void calibrationLoader::writeGeometry(std::string detector, ROC *roc )
{
    for(pixelPlotsMapDef::iterator r=calibrations_[detector][roc->getID()].begin(); r!=calibrations_[detector][roc->getID()].end(); r++)
        for(std::map<int, std::pair<TH1I*,fitter::fitResultDef> >::iterator c=(*r).second.begin(); c!=(*r).second.end(); c++)
            roc->setCalibrationFunction((*r).first, (*c).first, c->second.second.first, c->second.second.second);
}

//==================================================================
TH1* calibrationLoader::getHistogram(std::string detectorID, int rocID, int row, int col)
{
    if(        calibrations_                        .find(detectorID) != calibrations_.end()
            && calibrations_[detectorID]            .find(rocID)      != calibrations_[detectorID].end()
            && calibrations_[detectorID][rocID]     .find(row)        != calibrations_[detectorID][rocID].end()
            && calibrations_[detectorID][rocID][row].find(col)        != calibrations_[detectorID][rocID][row].end() )
    {
        return calibrations_[detectorID][rocID][row][col].first;
    }
    else
        return emptyTH1I_;
}

//==================================================================
void calibrationLoader::listHeader(void)
{
    if( header_.empty() )
    {
        ss_.str("") ;
        ss_ << "Header is empty! " ;
        STDLINE(ss_.str(),ACRed)   ;
    }

    for (headerMapDef::iterator it=header_.begin(); it!=header_.end(); ++it)
    {
        ss_.str("") ;
        ss_ << (*it).first << " : \t " << (*it).second ;
        STDLINE(ss_.str(),ACCyan);
    }
}

//====================================================================
bool calibrationLoader::execute(void)
{
    currentIteration_ = 0;
    return this->loadAllCalibrationFiles();
}

//====================================================================
std::string calibrationLoader::getLabel(void)
{
    return "Loading calibration";
}

//====================================================================
int calibrationLoader::getMaxIterations(void)
{
    Geometry * geometry = theFileEater_->getGeometry();
    int det = geometry->getDetectorsNumber();
    return det*SCALE_F;
}

//====================================================================
void calibrationLoader::makeChi2Histograms(void)
{
    Geometry * theGeometry = theFileEater_->getGeometry();
    for(Geometry::iterator detector=theGeometry->begin(); detector!=theGeometry->end(); detector++)
        for(Detector::iterator roc=detector->second->begin(); roc!=detector->second->end(); roc++ )
        {
            if(calibrations_.find(detector->first) == calibrations_.end() || calibrations_[detector->first].find(roc->second->getID()) == calibrations_[detector->first].end())
                continue;
            int maxRows = roc->second->getNumberOfRows();
            int maxCols = roc->second->getNumberOfCols();
            ss_.str("");
            ss_ << "Chi2 - "   << detector->first << " - ROC: " << roc->second->getID() ;
            TH1I* chi2Hist = new TH1I(ss_.str().c_str(), ss_.str().c_str(), 1000, 0, 4);
            ss_.str("");
            ss_ << "Chi2 2D- " << detector->first << " - ROC: " << roc->second->getID() ;
            TH2F* chi2Scat = new TH2F(ss_.str().c_str(), ss_.str().c_str(), maxRows, 0, maxRows, maxCols, 0, maxCols);
            chi2Scat->SetMaximum(2) ;
            ss_.str("") ; ss_ << CALIBRATIONS << "/Results/1D" ; // << detector->first ;
            theHManager_->storeCustomTObject(chi2Hist,ss_.str()) ;
            ss_.str("") ; ss_ << CALIBRATIONS << "/Results/2D" ; // << detector->first ;
            theHManager_->storeCustomTObject(chi2Scat,ss_.str()) ;
            for(pixelPlotsMapDef::iterator r=calibrations_[detector->first][roc->second->getID()].begin(); r!=calibrations_[detector->first][roc->second->getID()].end(); r++)
                for(std::map<int, std::pair<TH1I*,fitter::fitResultDef> >::iterator c=r->second.begin(); c!=r->second.end(); c++)
                {
                    TF1* funk = 0;
                    if( (funk = c->second.first->GetFunction("calibrationFitFunc")) && funk != 0)
                    {
                        int ndf = funk->GetNDF();
                        if( ndf > 0 )
                        {
                            double chi2 = funk->GetChisquare() / ndf ;
                            chi2Hist->Fill(chi2) ;
                            chi2Scat->SetBinContent(r->first,c->first,chi2) ;
                        }
                    }
                }
        }
}
