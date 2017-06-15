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

// @@@ Hard coded parameters @@@
#define SCALE_F           1000
#define FITMINPOINTS        10
#define MAXTHRESHOLD     10000 // [e-]
#define DYNAMICRANGE       200
#define ELECTRONS_NUMBER   350 // [e- / Vcal]
// ============================

//=========================================================
calibrationLoader::calibrationLoader(fileEater      * theFileEater     ,
                                     HManager       * theHManager      ,
                                     fitter         * theFitter        ,
                                     geometryLoader * theGeometryLoader,
                                     QProgressBar   * parseProgressBar ) :
    theFileEater_        (theFileEater     )
  , theHManager_         (theHManager      )
  , theFitter_           (theFitter        )
  , theGeometryLoader_   (theGeometryLoader)
  , emptyTH1I_           (new TH1I()       )
  , calibrationsLoaded_  (false            )
  , parseProgressBar_    (parseProgressBar )
{
  emptyTH1I_->SetName("emptyCalibration");
  calibrations_.clear();
}

//=========================================================
calibrationLoader::~calibrationLoader(void)
{
  delete emptyTH1I_;
}

//=========================================================
bool calibrationLoader::loadASCIIcalibrationFile(std::string fileName)
{
  STDLINE(std::string("Calibration file: " + fileName), ACPurple);
  
  std::ifstream  *inputFile;
  inputFile = new std::ifstream(fileName.c_str(), std::ifstream::in );
  
  if (!inputFile->is_open())
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
  while (inputFile->good())     // While there are lines left to read in the file...
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
          currentReg = Utils::toInt(what[2])*ELECTRONS_NUMBER;
        }
      }
      else if(boost::regex_match(line.c_str(), what, expRowColAdc, boost::match_extra))
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
  if( calibrations_.count(detector)               && 
      calibrations_[detector].count(roc->getID()) )
      calibrations_[detector][roc->getID()].clear();
  
  std::string detRoc = ss_.str();
  
  while (TKey * plotKey = (TKey*)nextPlot())
  {
    TH1 * plot = (TH1*)plotKey->ReadObj();
    ss_.str("");
    ss_ << plot->GetName();
    
    // Check if station, roc info match in title
    if( ss_.str().substr(0,detRoc.length()) != detRoc)
    {
//      STDLINE(std::string("")+ss_.str()+" does not match "+detRoc, ACRed);
      continue ;
//      return false;
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
    }
    else
    {
      STDLINE(std::string("ERROR ON ")+ss_.str(), ACRed);
      return false;
    }
    
    double* par = 0;
    double* cov = 0;
    if (plot->GetFunction(theFitter_->getCalibrationFitFunctionName()))
    {
      if( plot->GetFunction(theFitter_->getCalibrationFitFunctionName())->GetNumberFreeParameters() != 0)
      {
        par  = plot->GetFunction(theFitter_->getCalibrationFitFunctionName())->GetParameters();
        roc->setCalibrationFunction(row, col, par, cov);
      }
    }
    
    calibrations_[detector][roc->getID()][row][col].first = (TH1I*)plot;
    calibrations_[detector][roc->getID()][row][col].second = std::make_pair(par,cov); // ToROOT6
  }

//  inputFile->Close() ;

  return true;
}

//====================================================================
bool calibrationLoader::loadAllCalibrationFiles()
{
  STDLINE("Loading all calibrations",ACRed);
  calibrations_.clear();
  Geometry * theGeometry = theFileEater_->getGeometry();

  theGeometryLoader_->setCalibrationMaps();
  
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
      {
    	calibrationFileRadix = what[1];
      }
      else
      {
        STDLINE("ERROR: Can't find the extension .txt in " + (*roc).second->getCalibrationFilePath(), ACRed);
        continue;
      }
      
      // Look for root file in root calibrations directory, load if loading from ROOT
      std::string path =  calibrationsDir + calibrationFileRadix + ".root";

      if( readROOT_ && boost::filesystem::exists(path) && loadROOTcalibrationFiles(det->first, roc->second, path))
      {
        STDLINE(std::string("Loaded: ")+path,ACGreen) ;
      }
      else
      {
        path = calibrationsDir + (*roc).second->getCalibrationFilePath();
        if(this->loadASCIIcalibrationFile(path))   //Loads calibration file information into pixelData, header maps
        {
          if(det->second->isDUT())
          {
              ss_.str("");
              ss_ << "Make fit and plots for det:"
                  << det->first
                  << " -- rocID: "
                  << roc->first
                  << " roc ID: "
                  << roc->second->getID()
                  << " which is a DUT";
              STDLINE(ss_.str(),ACCyan);
              this->makeDUTHistograms(det->first, roc->second, true, true, writeASCII_);
          }
          else
          {
              ss_.str("");
              ss_ << "Make fit and plots for det:"
                  << det->first
                  << " -- rocID: "
                  << roc->first
                  << " roc ID: "
                  << roc->second->getID()
                  << " which is a telescope element";
              STDLINE(ss_.str(),ACBlue);
              this->makeHistograms(   det->first, roc->second, true, true, writeASCII_);
          }
        }
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
void calibrationLoader::removeCalibrationFiles(std::string fileDirectory)
{
    boost::cmatch what ;
    const boost::regex exp ("(\\w+).txt", boost::regex::perl);
    Geometry * theGeometry = theFileEater_->getGeometry();
    std::string path = "";

    int totalLoops = 0 ;
    for(Geometry::iterator det=theGeometry->begin(); det!=theGeometry->end(); det++)
    {
      for(Detector::iterator roc=det->second->begin(); roc!=det->second->end(); roc++ )
      {
          totalLoops ++ ;
      }
    }

    parseProgressBar_->reset();
    parseProgressBar_->setMaximum(totalLoops);

    int loop = 0 ;
    for(Geometry::iterator det=theGeometry->begin(); det!=theGeometry->end(); det++)
    {
      for(Detector::iterator roc=det->second->begin(); roc!=det->second->end(); roc++ )
      {
        std::string calibrationFileRadix = "";
        if(boost::regex_match(roc->second->getCalibrationFilePath().c_str(), what, exp, boost::match_extra))
        {
          calibrationFileRadix = what[1];
        }
        else
        {
          STDLINE(std::string("WARNING: Can't match the regular expression very likely because there is a strange character like - or + in the file name: ") + roc->second->getCalibrationFilePath() + ". You need to change the file name because I won't save it or load it!", ACRed);
          continue;
        }
        path = fileDirectory;
        path += std::string("/") + calibrationFileRadix + ".root";

        loop++ ;
        parseProgressBar_->setValue(loop) ;

        if( !this->checkFileStatus(path) ) {continue;}

        std::string me = getenv("USER") ;
        ss_.str("") ; ss_ << "/tmp/" << me ;

        if( !this->checkFileStatus(ss_.str()) )
        {
            STDLINE(string("mkdir ")+ss_.str(),ACPurple) ;
            if( !system((string("mkdir ")+ss_.str()).c_str()))
            {
                STDLINE(string("Failed to mkdir ")+ss_.str(),ACPurple) ;
                exit(0) ;
            }
        }
        ss_.str("") ; ss_ << "mv " << path << " /tmp/" << me << "/.";
        STDLINE(ss_.str(),ACGreen) ;
        system(ss_.str().c_str()) ;
      }
    }
    STDLINE("Files have actually been moved to a temporary area",string(ACCyan)+string(ACReverse)) ;
}

//=========================================================
bool calibrationLoader::checkFileStatus(std::string path)
{
    struct stat fileStatus;
    int returnStatus = stat(path.c_str(), &fileStatus);
    if( returnStatus == ENOENT )
    {
        ss_.str("") ; ss_ << "WARNING: Path to " << path << " does not exist, or path is an empty string." ;
        STDLINE(ss_.str(),ACRed) ;
        return false ;
    }
    else if( returnStatus == ENOTDIR )
    {
        ss_.str("") ; ss_ << "WARNING: A component of the path of " << path << " is not a directory.";
        STDLINE(ss_.str(),ACRed) ;
        return false ;
    }
    else if( returnStatus == ELOOP )
    {
        ss_.str("") ; ss_ << "WARNING: Too many symbolic links encountered while traversing the path to " << path;
        STDLINE(ss_.str(),ACRed) ;
        return false ;
    }
    else if( returnStatus == EACCES )
    {
        ss_.str("") ; ss_ << "WARNING: Permission denied to read " << path;
        STDLINE(ss_.str(),ACRed) ;
        return false ;
    }
    else if( returnStatus == ENAMETOOLONG )
    {
        ss_.str("") ; ss_ << "WARNING: Cannot read " << path;
        STDLINE(ss_.str(),ACRed) ;
        return false ;
    }
    else if( returnStatus == -1  )
    {
        ss_.str("") ; ss_ << "WARNING: Could not find calibration file "
                          << "'"
                          << path
                          << "'!";
        STDLINE(ss_.str(),ACRed) ;
        return false ;
    }
    return true ;
}
//=========================================================
void calibrationLoader::restoreCalibrationFiles(std::string fileDirectory)
{
    boost::cmatch what ;
    const boost::regex exp ("(\\w+).txt", boost::regex::perl);
    Geometry * theGeometry = theFileEater_->getGeometry();
    std::string path = "";

    int totalLoops = 0 ;
    for(Geometry::iterator det=theGeometry->begin(); det!=theGeometry->end(); det++)
    {
      for(Detector::iterator roc=det->second->begin(); roc!=det->second->end(); roc++ )
      {
          totalLoops ++ ;
      }
    }

    parseProgressBar_->reset();
    parseProgressBar_->setMaximum(totalLoops);

    int loop = 0 ;
    for(Geometry::iterator det=theGeometry->begin(); det!=theGeometry->end(); det++)
    {
      for(Detector::iterator roc=det->second->begin(); roc!=det->second->end(); roc++ )
      {
        std::string calibrationFileRadix = "";
        if(boost::regex_match(roc->second->getCalibrationFilePath().c_str(), what, exp, boost::match_extra))
        {
          calibrationFileRadix = what[1];
        }
        else
        {
          STDLINE(std::string("WARNING: Can't match the regular expression very likely because there is a strange character like - or + in the file name: ") + roc->second->getCalibrationFilePath() + ". You need to change the file name because I won't save it or load it!", ACRed);
          continue;
        }
        std::string me = getenv("USER") ;
        path = std::string("/tmp/") +
               me                   +
               "/"                  +
               calibrationFileRadix +
               ".root";

        loop++ ;
        parseProgressBar_->setValue(loop) ;

        if( !this->checkFileStatus(path) ) {continue;}

        ss_.str("") ; ss_ << "mv /tmp/"
                          << me
                          << "/"
                          << calibrationFileRadix
                          << ".root "
                          << fileDirectory
                          << "/." ;
        STDLINE(ss_.str(),ACGreen) ;

        system(ss_.str().c_str()) ;
      }
    }
    STDLINE("Calibration files have been restored.",string(ACCyan)+string(ACReverse)) ;
}

//=========================================================
void  calibrationLoader::saveROOTcalibrationFiles(std::string fileDirectory)
{
  if (!calibrationsLoaded_)
  {
    STDLINE(std::string("Unable to save root calibrations: full calibrations not loaded."), ACRed);
    return;
  }

  boost::cmatch what ;
  const boost::regex exp ("(\\w+).txt", boost::regex::perl);
  Geometry * theGeometry = theFileEater_->getGeometry();
  std::string path = "";
  
  int totalLoops = 0 ;
  for(Geometry::iterator det=theGeometry->begin(); det!=theGeometry->end(); det++)
  {
    for(Detector::iterator roc=det->second->begin(); roc!=det->second->end(); roc++ )
    {
        totalLoops ++ ;
    }
  }

  parseProgressBar_->reset();
  parseProgressBar_->setMaximum(totalLoops);

  // Cycle through detectors contained in theGeometry and ROCs
  int loop = 0 ;
  for(Geometry::iterator det=theGeometry->begin(); det!=theGeometry->end(); det++)
  {
    for(Detector::iterator roc=det->second->begin(); roc!=det->second->end(); roc++ )
    {
      std::string calibrationFileRadix = "";
      if(boost::regex_match(roc->second->getCalibrationFilePath().c_str(), what, exp, boost::match_extra))
      {
        calibrationFileRadix = what[1];
      }
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
      loop++ ;
      parseProgressBar_->setValue(loop) ;
      // Open TFile
      TFile * outputTFile = TFile::Open(path.c_str(), "RECREATE");

      // Cycle through pixel rows and cols, write the histogram to file.
      if(calibrations_.find((*det).first) == calibrations_.end() ||
         calibrations_[(*det).first].find((*roc).first) == calibrations_[(*det).first].end())
      {
        continue;
      }

      pixelPlotsMapDef * pixels = &calibrations_[(*det).first][(*roc).first];
      if( !pixels ) STDLINE("No pixels!!!",ACRed) ;
      for(calibrationLoader::pixelPlotsMapDef::iterator r=(*pixels).begin(); r!=(*pixels).end(); r++)
      {
        for(std::map<int, std::pair<TH1I*, fitter::fitResultDef> >::iterator c=(*r).second.begin(); c!=(*r).second.end(); c++)
        {
          if( (*c).second.first == NULL )
          {
              STDLINE("No c!!!",ACRed) ;
          }
          else
          {
              (*c).second.first->Write();
          }
        }
      }

      TH2F * pixelsS = chisquaresS_[(*det).first][(*roc).first];
      if( !pixelsS ) STDLINE("No chi2 plots!!!",ACRed) ;
      pixelsS->Write();

//      std::stringstream ss; ss << "Saving " << pixelsS->GetTitle() ; STDLINE(ss.str(),ACWhite) ;
      TH1I * pixelsH = chisquaresH_[(*det).first][(*roc).first];
      if( !pixelsH ) STDLINE("No chi2 plots!!!",ACRed) ;
      pixelsH->Write();

      outputTFile->Close();
    }
  }
  STDLINE("Calibration histograms saved...", ACGreen) ; 
}

//==================================================================
bool calibrationLoader::makeDUTHistograms(std::string detector, ROC *roc, bool fit,  bool writeGeometry, bool writeASCII)
{
  if ( calibrations_.count(detector) && calibrations_[detector].count(roc->getID()) )
       calibrations_[detector][roc->getID()].clear();

  if(pixels_.empty())
  {
    STDLINE("WARNING: no pixels calibration file loaded",ACRed);
    return false;
  }
  
  int maxRows = roc->getNumberOfRows();
  int maxCols = roc->getNumberOfRows();
  
  ss_.str("") ;
  ss_ << "Calibrating ROC "
      << roc->getID()
      <<  " of detector "
      << detector
      << " with function: "
      << roc->getCalibrationFunctionType();
  STDLINE(ss_.str(),ACCyan) ;

  if(writeASCII) outputFile_.open(outputASCIIfile_.c_str(), std::ios_base::app);

  static double plotsRangeAndBins[4][3];

  static bool firstDUTFit_ = true ;
  if( firstDUTFit_ )
  {
    firstBinHisto_ = new TH2F("firstBinHisto_", "First bin distr", maxRows, 0, maxRows,
                                                                   maxCols, 0, maxCols);
    lastBinHisto_  = new TH2F("lastBinHisto_" , "Last bin distr" , maxRows, 0, maxRows,
                                                                   maxCols, 0, maxCols);
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
    firstDUTFit_  = false ;
  }
  else
  {
      firstBinHisto_->Reset() ;
      lastBinHisto_ ->Reset() ;
  }

  theFitter_->setFitFunctionType(roc->getCalibrationFunctionType());
//  STDLINE(theFitter_->getFitFunctionType(),string(ACCyan)+string(ACReverse)) ;

  TH1I* calib   [maxRows][maxCols];
  TH1I* calibNew[maxRows][maxCols];
  
  TH1F* hPars[4];
  
  
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
  
  for(int r=0; r<maxRows; r++)
  {
    for(int c=0; c<maxCols; c++)
    {
      ss_.str(""); ss_ << "Service histo calibration of pixel at row: " << r << ", col " << c;
      calib   [r][c] = new TH1I(ss_.str().c_str(), ss_.str().c_str(), 255, 0, 255*ELECTRONS_NUMBER);
      ss_.str("");
      ss_ << detector << " - ROC: " << roc->getID() << " - r: " << r << " - c: " << c;
      calibNew[r][c] = new TH1I(ss_.str().c_str(), ss_.str().c_str(), 255, 0, 255*ELECTRONS_NUMBER);
    }
  }
  
  int bin;
  for (calibrationLoader::pixelDataMapDef::iterator r=pixels_.begin(); r!=pixels_.end(); ++r)
  {
    int row = (*r).first ;
    for (std::map<int,aPixelDataMapDef>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
    {
      int col = (*c).first ;
      for(calibrationLoader::aPixelDataMapDef::iterator it2=(*c).second.begin(); it2!=(*c).second.end(); ++it2)
      {
        bin = calib[row][col]->Fill((*it2).first,(*it2).second.second);
              calib[row][col]->SetBinError(bin,2.5);
      }
    }
  }
  
  int nBins;
  for (calibrationLoader::pixelDataMapDef::iterator r=pixels_.begin(); r!=pixels_.end(); ++r)
  {
    int row = (*r).first ;
    for (std::map<int,aPixelDataMapDef>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
    {
      int col = (*c).first ;
      bin = 1;
      nBins = calib[row][col]->GetNbinsX();
      while(bin<=nBins && calib[row][col]->GetBinContent(bin)==0)
    	bin++;
      firstBinHisto_->Fill(row,col);
      
    }
  }
  
  for (calibrationLoader::pixelDataMapDef::iterator r=pixels_.begin(); r!=pixels_.end(); ++r)
  {
    int row = (*r).first ;
    for (std::map<int,aPixelDataMapDef>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
    {
      int col = (*c).first ;
      bin = calib[row][col]->GetNbinsX();
      while(bin>=1 && calib[row][col]->GetBinContent(bin)==0)
    	bin--;
      lastBinHisto_->Fill(row,col,bin);
    }
  }
  
  double precADC   ;
  double currentADC;
  int    lastBin   ;
  int    firstBin  ;
  for (calibrationLoader::pixelDataMapDef::iterator r=pixels_.begin(); r!=pixels_.end(); ++r)
  {
    int row = (*r).first ;
    for (std::map<int,aPixelDataMapDef>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
    {
      int col  = (*c).first ;
      lastBin  = (int)lastBinHisto_ ->GetBinContent(lastBinHisto_ ->GetXaxis()->FindBin(row),
                                                    lastBinHisto_ ->GetYaxis()->FindBin(col));
      firstBin = (int)firstBinHisto_->GetBinContent(firstBinHisto_->GetXaxis()->FindBin(row),
                                                    firstBinHisto_->GetYaxis()->FindBin(col));
      precADC  = calib[row][col]->GetBinContent(lastBin);
      for(int b=lastBin-1; b>=firstBin; b--)
      {
        currentADC = calib[row][col]->GetBinContent(b);
        if (precADC == 0) continue;
        calibNew[row][col]->SetBinContent(b,precADC);
        calibNew[row][col]->SetBinError(b,2.5);
        precADC = currentADC;
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
          int row = (*r).first ;
          for (std::map<int,aPixelDataMapDef>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
          {
              int col  = (*c).first ;
              firstBin = (int)firstBinHisto_->GetBinContent(firstBinHisto_->GetXaxis()->FindBin(row),
                                                            firstBinHisto_->GetYaxis()->FindBin(col));
              lastBin  = (int)lastBinHisto_ ->GetBinContent(lastBinHisto_ ->GetXaxis()->FindBin(row),
                                                            lastBinHisto_ ->GetYaxis()->FindBin(col));
              minBin = calibNew[row][col]->GetMinimum(1);
              maxBin = calibNew[row][col]->GetBinContent(calibNew[row][col]->GetMaximumBin());

              if ((calibNew[row][col]->GetEntries() >= FITMINPOINTS)                                                                            &&
                  (calibNew[row][col]->GetBinCenter((int)firstBinHisto_->GetBinContent(firstBinHisto_->GetXaxis()->FindBin(row),
                                                                                       firstBinHisto_->GetYaxis()->FindBin(col))) < MAXTHRESHOLD) &&
                  (maxBin-minBin > DYNAMICRANGE))
              {
                  fitR = theFitter_->calibrationFit(calibNew[row][col],
                                                    // 10000,
                                                    calibNew[row][col]->GetBinCenter(firstBin),
                                                    22000,
                                                    NULL);
                  pars = fitR.first;
              }
              else
              {
                  continue;
              }

              if (fitR.first == NULL)
              {
                  ss_.str("") ;
                  ss_ << "WARNING: first fit failed for detector "
                      << detector
                      << " - ROC: "
                      << roc->getID()
                      << " at row "
                      << (*r).first
                      << ", col "
                      << (*c).first ;
                  STDLINE(ss_.str(),ACRed) ;
              }
              else
              {
                  for (int p = 0; p <theFitter_->getCalibrationFitFunctionNPar(); p++)
                  {
                      hPars[p]->Fill(pars[p]);
                  }
              }
          }
      }

      double rightPars[4];
      for(int p=0; p<theFitter_->getCalibrationFitFunctionNPar() ; p++) // Fit a second time with initial parameters equal
      {                                                                 // to the mean of the parameters found previously
          rightPars[p] = hPars[p]->GetMean();
          ss_.str(""); ss_ << "Parameter " << p << ": " << rightPars[p];
          STDLINE(ss_.str(),ACGreen);
      }

      for (calibrationLoader::pixelDataMapDef::iterator r =pixels_.begin();
                                                        r!=pixels_.end();
                                                      ++r)
      {
          int row = (*r).first ;
          for (std::map<int,aPixelDataMapDef>::iterator c =(*r).second.begin();
                                                        c!=(*r).second.end();
                                                      ++c)
          {
              int col = (*c).first ;
              firstBin = (int)firstBinHisto_->GetBinContent(firstBinHisto_->GetXaxis()->FindBin(row),
                                                            firstBinHisto_->GetYaxis()->FindBin(col));
              lastBin  = (int)lastBinHisto_ ->GetBinContent(lastBinHisto_ ->GetXaxis()->FindBin(row),
                                                            lastBinHisto_ ->GetYaxis()->FindBin(col));
              minBin   = calibNew[row][col]->GetMinimum(1);
              maxBin   = calibNew[row][col]->GetBinContent(calibNew[row][col]->GetMaximumBin());

              if ((calibNew[row][col]->GetEntries() >= FITMINPOINTS) &&
                  (
                   calibNew[row][col]->GetBinCenter(
                                                    (int)firstBinHisto_->GetBinContent(
                                                                                       firstBinHisto_->GetXaxis()->FindBin(row),
                                                                                       firstBinHisto_->GetYaxis()->FindBin(col)
                                                                                     )
                                                   )
                  ) < MAXTHRESHOLD &&
                  (maxBin-minBin > DYNAMICRANGE)
                 )
              {
                  fitR = theFitter_->calibrationFit(
                                                    calibNew[row][col],
                                                    calibNew[row][col]->GetBinCenter(firstBin),
                                                    22000,
                                                    rightPars
                                                   );
              }
              else
              {
                  fitR.first = NULL;
              }

              if (calibNew[row][col]->GetEntries() !=0 && fitR.first == NULL)
              {
                  ss_.str("") ;
                  ss_ << "WARNING: fit failed for detector "
                      << detector
                      << " - ROC: "
                      << roc->getID()
                      << " at row "
                      << row
                      << ", col "
                      << col ;
                  STDLINE(ss_.str(),ACRed) ;
              }

              calibrations_[detector][roc->getID()][row][col].second = fitR;

              if (writeGeometry)
              {
                  ss_.str("") ;
                  ss_ << "Writing geometry for "
                      << detector
                      << " - ROC: "
                      << roc->getID()
                      << " at row "
                      << (*r).first
                      << ", col "
                      << (*c).first ;
//                  STDLINE(ss_.str(),ACYellow) ;
                  roc->setCalibrationFunction(row, col, fitR.first, fitR.second);
//                  STDLINE("Done",ACGreen) ;
              }

              if(writeASCII)
              {
                  ss_.str("") ;
                  ss_ << "Writing ASCII for "
                      << detector
                      << " - ROC: "
                      << roc->getID()
                      << " at row "
                      << row
                      << ", col "
                      << col ;
                  STDLINE(ss_.str(),ACYellow) ;
                  if( fitR.first!=NULL )
                  {
                      outputFile_ << detector
                                  << " "
                                  << roc->getID()
                                  << " "
                                  << row
                                  << " "
                                  << col
                                  << " ";
                      for(int p=0; p < 4; p++)
                      {
                          outputFile_ << fitR.first[p] << " ";
                      }
                      outputFile_ << std::endl;
                  }
              }
              calibrations_[detector][roc->getID()][row][col].first = calibNew[row][col];
          }
      }
  }
  
  if(writeASCII)
  {
      outputFile_.close();
      STDLINE(string("Calibration written out in ASCII format in ")+outputASCIIfile_,ACCyan) ;
  }
  
  for(int r=0; r<maxRows; r++)
  {
      for(int c=0; c<maxCols; c++)
      {

          calib[r][c]->Delete();
      }
  }
  
//  firstBinHisto->Delete();
//  lastBinHisto ->Delete();
  
  for (int p = 0; p < 4; p++) hPars[p]->Delete();
  
  return true;
}

//==================================================================
bool calibrationLoader::makeHistograms(std::string detector, ROC *roc, bool fit,  bool writeGeometry, bool writeASCII)
{
  bool alreadyFit = false ;

  if(pixels_.empty())
  {
    STDLINE("WARNING: no pixels calibration file loaded",ACRed);
    return false;
  }

  if( calibrations_.count(detector) > 0 &&
      calibrations_[detector].count(roc->getID()) > 0 )
  {
      calibrations_[detector][roc->getID()].clear();
  }

  std::string type = roc->getCalibrationFunctionType() ;
  theFitter_->setFitFunctionType(type);

  ss_.str("") ;
  ss_ << "Calibrating ROC "
      << roc->getID()
      <<  " of detector "
      << detector
      << " with function: "
      << type ;
  STDLINE(ss_.str(),ACCyan) ;

  if(writeASCII) outputFile_.open(outputASCIIfile_.c_str(), std::ios_base::app);

  //Cycle through each pixel on the ROC
  for (calibrationLoader::pixelDataMapDef::iterator r=pixels_.begin(); r!=pixels_.end(); ++r)
  {
    int row = (*r).first ;
    for (std::map<int,aPixelDataMapDef>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
    {
      int col = (*c).first ;
      //if( cc++ > 10) {continue;}
      ss_.str("");
      ss_ << detector
          << " - ROC: "
          << roc->getID()
          << " - r: "
          << row
          << " - c: "
          << col;
      TH1I * calib = (TH1I*)gROOT->FindObject(ss_.str().c_str()) ;
      if( !calib )
      {
          alreadyFit = false ;
          calib = new TH1I(ss_.str().c_str(),
                           ss_.str().c_str(), 255, 0, 255*ELECTRONS_NUMBER);
          calib->GetXaxis()->SetTitle("V_cal (electrons number)");
          calib->GetYaxis()->SetTitle("ADC counts");
          calib->GetYaxis()->SetRangeUser(0,750);
          calib->SetDirectory(0);
      }
      else
      {
          alreadyFit = true ;
      }
      if( alreadyFit ) continue ;
//      STDLINE(ss_.str(),ACYellow) ;
      for (calibrationLoader::aPixelDataMapDef::iterator it2 =(*c).second.begin();
                                                         it2!=(*c).second.end();
                                                       ++it2)
      {
          int bin = calib->Fill( (*it2).first, (*it2).second.second );
          calib->SetBinError(bin,2.4*4); //sqrt((*it2).second.second));
      }
      calibrations_[detector][roc->getID()][row][col].first = calib;
      //ss_.str("") ; ss_ << CALIBRATIONS << "/Results/Gain/" << detector->first ;

      //theHManager_->storeCustomTObject(calib,ss_.str()) ;
      if(fit)
      {
        fitter::fitResultDef fitR = theFitter_->calibrationFit(
                                                               calib,
                                                               0,
                                                               30000,
                                                               NULL
                                                              );
        if(calib->GetEntries() != 0 && fitR.first == NULL /*|| fitR.second == NULL*/)
        {
          ss_.str("") ;
          ss_ << "WARNING: fit failed for detector "
              << detector
              << " - ROC: "
              << roc->getID()
              << " at row "
              << row
              << ", col "
              << col ;
          STDLINE(ss_.str(),ACRed) ;
        }
        calibrations_[detector][roc->getID()][row][col].second = fitR;


        if(writeGeometry)
        {
          roc->setCalibrationFunction(row, col, fitR.first, fitR.second);
        }
        if(writeASCII)
        {
          if( fitR.first!=NULL )
          {
            outputFile_ << detector
                        << " "
                        << roc->getID()
                        << " "
                        << row
                        << " "
                        << col
                        << " ";
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
  for(pixelPlotsMapDef::iterator r=calibrations_[detector][roc->getID()].begin(); 
  				 r!=calibrations_[detector][roc->getID()].end(); 
        			 r++
     )
  {
    for(std::map<int, std::pair<TH1I*,fitter::fitResultDef> >::iterator c =(*r).second.begin();
                                                                        c!=(*r).second.end();
                                                                        c++
        )
    {
      roc->setCalibrationFunction(
                                  (*r).first,
                                  (*c).first,
                                  c->second.second.first,
                                  c->second.second.second
                                 );
    }
  }
}

//==================================================================
TH1* calibrationLoader::getHistogram(std::string detectorID, int rocID, int row, int col)
{
    if(   calibrations_ 		       .find(detectorID) != calibrations_.end()
       && calibrations_[detectorID]	       .find(rocID)	 != calibrations_[detectorID].end()
       && calibrations_[detectorID][rocID]     .find(row)	 != calibrations_[detectorID][rocID].end()
       && calibrations_[detectorID][rocID][row].find(col)	 != calibrations_[detectorID][rocID][row].end() 
      )
    {
      return calibrations_[detectorID][rocID][row][col].first;
    }
    else
    {
      return emptyTH1I_;
    }
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
  {
    for(Detector::iterator roc=detector->second->begin(); roc!=detector->second->end(); roc++ )
    {
      if(calibrations_.find(detector->first) == calibrations_.end() || 
         calibrations_[detector->first].find(roc->second->getID()) == calibrations_[detector->first].end())
          continue;
      int maxRows = roc->second->getNumberOfRows();
      int maxCols = roc->second->getNumberOfCols();
      ss_.str("");
      ss_ << "Chi2 - "   << detector->first << " - ROC: " << roc->second->getID() ;
      TH1I* chi2Hist = new TH1I(ss_.str().c_str(), ss_.str().c_str(), 200, 0, 20);
      ss_.str("");
      ss_ << "Chi2 2D- " << detector->first << " - ROC: " << roc->second->getID() ;
      TH2F* chi2Scat = new TH2F(ss_.str().c_str(), ss_.str().c_str(), maxCols, 0, maxCols,
                                                                      maxRows, 0, maxRows);
      ss_.str("") ; ss_ << CALIBRATIONS << "/Results/1D" ; // << detector->first ;
      theHManager_->storeCustomTObject(chi2Hist,ss_.str()) ;
      ss_.str("") ; ss_ << CALIBRATIONS << "/Results/2D" ; // << detector->first ;
      theHManager_->storeCustomTObject(chi2Scat,ss_.str()) ;
      for(pixelPlotsMapDef::iterator r =calibrations_[detector->first][roc->second->getID()].begin();
                                     r!=calibrations_[detector->first][roc->second->getID()].end();
                                     r++
         )
      {
        for(std::map<int, std::pair<TH1I*,fitter::fitResultDef> >::iterator c =r->second.begin(); 
                                                                            c!=r->second.end(); c++)
        {
          TF1* funk = 0;
          if( (funk = c->second.first->GetFunction("calibrationFitFunction")) && funk != 0)
          {
            int ndf = funk->GetNDF();
            if( ndf > 0 )
            {
              double chi2 = funk->GetChisquare() / ndf ;
              chi2Hist->Fill(chi2) ;
              chi2Scat->SetBinContent(c->first+1,r->first+1,chi2) ;
            }
          }
          else
          {
              static bool first = true ;
              if( first )
              {
                  first = false ;
                  STDLINE("No calibration function found! (Message will NOT be repeated...)",ACRed) ;
              }
          }
        }
      }  
      chisquaresH_[detector->first][roc->second->getID()] = chi2Hist ;
      chisquaresS_[detector->first][roc->second->getID()] = chi2Scat ;
    }
  }
}
//================================================================================
TH1I* calibrationLoader::get1DChi2(std::string   detectorID,
                                   int           rocID      )

{
    return chisquaresH_[detectorID][rocID] ;
}
//================================================================================
TH2F* calibrationLoader::get2DChi2(std::string   detectorID,
                                   int           rocID      )

{
    return chisquaresS_[detectorID][rocID] ;
}
//================================================================================
vector<TH1I*> calibrationLoader::getAll1DChi2(void)

{
    vector<TH1I*> theH ;
    for(calibrationPlotsMapDefH::iterator it =chisquaresH_.begin();
                                          it!=chisquaresH_.end();
                                          it++)
    {
        for(map<int, TH1I*>::iterator jt =it->second.begin();
                                      jt!=it->second.end();
                                      jt++)
        {
            theH .push_back(jt->second) ;
        }
    }
    return theH ;
}
//================================================================================
vector<TH2F*> calibrationLoader::getAll2DChi2(void)

{
    vector<TH2F*> theH ;
    for(calibrationPlotsMapDefS::iterator it =chisquaresS_.begin();
                                          it!=chisquaresS_.end();
                                          it++)
    {
        for(map<int, TH2F*>::iterator jt =it->second.begin();
                                      jt!=it->second.end();
                                      jt++)
        {
            theH .push_back(jt->second) ;
        }
    }
    return theH ;
}
