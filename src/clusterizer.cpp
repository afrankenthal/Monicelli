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
 
#include "clusterizer.h"
#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TF1.h>
#include <TH1.h>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>


// @@@ Hard coded parameters @@@ 
#define TESTDIVIDE false     // Use this flag in case of normal incidence on DUT and in case there are problems in aligning the DUT
#define ONLYdoubleHITS false // Use this flag in order to align on cluster size 2 only

// ============================


using namespace std;


//===================================================================================
clusterizer::clusterizer(void) : clustersBuilt_ (false), useEtaFunction_(false)
{
    usePartitions_    = false;
    theGeometry_      = NULL ;
}

//===================================================================================
clusterizer::~clusterizer(void)
{
  for(std::map<std::string,std::map<int,std::map<std::string,double*> > >::iterator it=chargeAsymmetryPars_.begin(); it!=chargeAsymmetryPars_.end(); it++)
    for(std::map<int,std::map<std::string,double*> >::iterator it1=it->second.begin(); it1!=it->second.end(); it1++)
      for(std::map<std::string,double*>::iterator it2=it1->second.begin(); it2!=it1->second.end(); it2++)
	delete[] it2->second;
}

//===================================================================================
struct pixelInfos
{
  double x;
  double y;
  double xPitch;
  double yPitch;
  double charge;
  int dataType;
};

//===================================================================================
void clusterizer::clear(void)
{
  clustersHitsMap_.clear();
  clustersMap_    .clear();
}

//===================================================================================
void clusterizer::getPartitionsInfos(vector<int> partitionsPoints, bool usePartitions, Detector *theCurrentDUT, string theCurrentSector)
{
    thePoints_          = partitionsPoints;
    usePartitions_      = usePartitions;
    theCurrentDUT_      = theCurrentDUT;
    theCurrentSector_   = theCurrentSector;
}

//===================================================================================
void clusterizer::repartClusters (Event::clustersHitsMapDef & clustersHitsMap)
{
    Event::aClusterHitsMapDef* DUTClusterHitsMapDef = &clustersHitsMap[theCurrentDUT_->getID()];

    for(Event::aClusterHitsMapDef::iterator cluster = DUTClusterHitsMapDef->begin(); cluster!=DUTClusterHitsMapDef->end(); cluster++)
    {
        bool hitsInRegion = false;

        for(Event::hitsDef::iterator hits = cluster->second.begin(); hits!= cluster->second.end(); hits++)
        {
            int hCol = (*hits)["col"];
            int hRow = (*hits)["row"];

            if(isInRegion(hCol,hRow))
            {
                hitsInRegion = true;
                break;
            }
        }


        if(!hitsInRegion)
        {
            //ss_.str(""); ss_<<"Erasing cluster: "<<cluster->first<<" From DUT: "<<theCurrentDUT_->getID()<<endl; STDLINE(ss_.str(),ACGreen);
            DUTClusterHitsMapDef->erase(cluster->first);
        }
    }
}
//===================================================================================
bool clusterizer::isInRegion(int col, int row)
{
    int DUTCols = theCurrentDUT_->getNumberOfCols();
    int DUTRows = theCurrentDUT_->getNumberOfRows();

    map<string, vector<int> > sectorMap;

    sectorMap["A"].push_back(thePoints_.at(3));
    sectorMap["A"].push_back(thePoints_.at(1));
    sectorMap["A"].push_back(thePoints_.at(0));
    sectorMap["A"].push_back(thePoints_.at(2));

    sectorMap["B"].push_back(thePoints_.at(1));
    sectorMap["B"].push_back(thePoints_.at(5));
    sectorMap["B"].push_back(thePoints_.at(0));
    sectorMap["B"].push_back(thePoints_.at(2));

    sectorMap["C"].push_back(thePoints_.at(1));
    sectorMap["C"].push_back(thePoints_.at(5));
    sectorMap["C"].push_back(thePoints_.at(4));
    sectorMap["C"].push_back(thePoints_.at(0));

    sectorMap["D"].push_back(thePoints_.at(3));
    sectorMap["D"].push_back(thePoints_.at(1));
    sectorMap["D"].push_back(thePoints_.at(4));
    sectorMap["D"].push_back(thePoints_.at(0));

    int minCol = sectorMap[theCurrentSector_].at(0);
    int maxCol = sectorMap[theCurrentSector_].at(1);
    int minRow = sectorMap[theCurrentSector_].at(2);
    int maxRow = sectorMap[theCurrentSector_].at(3);

    // "+1" takes in account hits next to the frontier ones

    if(minCol!= 0)       minCol-=1;
    if(maxCol!= DUTCols) maxCol+=1;
    if(minRow!= 0)       minRow-=1;
    if(maxRow!= DUTRows) maxRow+=1;

    if((col > minCol)&&
       (col < maxCol)&&
       (row > minRow)&&
       (row < maxRow))
        return true;

    else return false;

}

//===================================================================================
Event::clustersHitsMapDef clusterizer::findClusters(Event *theEvent)
{
    if ((theEvent->getClustersHits().empty())||(!usePartitions_))
    {
        this->clear();
        int clusterID=0;

        Event::plaqMapDef plaqMap = theEvent->getRawData();

        for(Event::plaqMapDef::iterator hits=plaqMap.begin(); hits!=plaqMap.end(); ++hits)
        {
            clusterID = 0;

            while( hits->second.size() != 0 )
            {
                Event::hitsDef& clusterHits = clustersHitsMap_[hits->first][clusterID];
                clusterHits.push_back( hits->second[0] );
                hits->second.erase( hits->second.begin() );
                for(unsigned int c=0; c < clusterHits.size(); c++ )
                {
                    int& cRow = clusterHits[c]["row"];
                    int& cCol = clusterHits[c]["col"];

                    for(unsigned  int h=0; h < hits->second.size(); h++ )
                    {
                        int& hRow = hits->second[h]["row"];
                        int& hCol = hits->second[h]["col"];

                        if ((abs(cCol - hCol) <= 1 ) && ( abs(cRow - hRow) <= 1 ))
                        {
                            clusterHits.push_back( hits->second[h] );
                            hits->second.erase( hits->second.begin()+h );
                            h--;
                        }
                    }
                }
                ++clusterID;
            }
        }
        return clustersHitsMap_;
    }
    else
    {
        return theEvent->getClustersHits();
    }
}

//=============================================================================
void clusterizer::getChargeAsymmetryPlots(Geometry* theGeometry)
{
  std::map<std::string,std::string> monicelliPlaneToPlane;
  monicelliPlaneToPlane["Station: 0 - Plaq: 0"] = "Telescope_Downstream0";
  monicelliPlaneToPlane["Station: 0 - Plaq: 1"] = "Telescope_Downstream1";
  monicelliPlaneToPlane["Station: 0 - Plaq: 2"] = "Telescope_Downstream2";
  monicelliPlaneToPlane["Station: 0 - Plaq: 3"] = "Telescope_Downstream3";
  monicelliPlaneToPlane["Station: 2 - Plaq: 0"] = "Telescope_Upstream0";
  monicelliPlaneToPlane["Station: 2 - Plaq: 1"] = "Telescope_Upstream1";
  monicelliPlaneToPlane["Station: 2 - Plaq: 2"] = "Telescope_Upstream2";
  monicelliPlaneToPlane["Station: 2 - Plaq: 3"] = "Telescope_Upstream3";
  monicelliPlaneToPlane["Station: 4 - Plaq: 2"] = "Dut2";
  monicelliPlaneToPlane["Station: 4 - Plaq: 1"] = "Dut1";
  monicelliPlaneToPlane["Station: 4 - Plaq: 0"] = "Dut0";

  std::string calibrationsDir = std::string(getenv("Monicelli_CalSample_Dir")) + "/";
  boost::cmatch what ;
  const boost::regex exp ("(\\w+)_(\\d+).txt", boost::regex::perl);

  // Cycle through detectors contained in theGeometry
  for(Geometry::iterator det=theGeometry->begin(); det!=theGeometry->end(); det++)
    {
      // Cycle through ROCs in the detector
      for(Detector::iterator roc=det->second->begin(); roc!=det->second->end(); roc++ )
        {
	  if(!boost::regex_match(roc->second->getCalibrationFilePath().c_str(), what, exp, boost::match_extra))
            {
	      STDLINE("ERROR: Can't find the extension .txt in " + roc->second->getCalibrationFilePath(), ACRed);
	      return;
            }

	  // Look for root file in root calibrations directory, load if loading from ROOT
	  std::string calibrationFileName =  calibrationsDir + what[1] + "_" + what[2] + "_ChargeAsymmetry.root";
	  if(!boost::filesystem::exists(calibrationFileName))
            {
	      calibrationFileName =  calibrationsDir + what[1] + "_ChargeAsymmetry.root";
	      if(!boost::filesystem::exists(calibrationFileName))
                {
//		  STDLINE("ERROR: Can't find any Charge Asymmetry file named "
//			  + calibrationsDir + what[1] + "_" + what[2] + ".root"
//			  + " or " + calibrationFileName, ACRed);
		  continue;
                }
            }
	  
	  ss_.str("");
	  ss_ << "Loading calibrations for " << det->first << " -- rocID: " << roc->first << " from " << calibrationFileName;
	  STDLINE(ss_.str(),ACGreen);
	  TFile* file = TFile::Open(calibrationFileName.c_str(),"READ");
	  for(int xy=0; xy<2; xy++)
            {
	      std::string xyLetter = "";
	      if     (xy==0) xyLetter = "X";
	      else if(xy==1) xyLetter = "Y";
	      std::string histoName = "h1D" + xyLetter + "cellChargeAsimmetry_" + monicelliPlaneToPlane[det->first];
	      TH1* histo = (TH1*)file->Get(histoName.c_str());
	      double* pars = 0;
	      if(histo->GetFunction("line") != 0)
                {
		  pars = new double[histo->GetFunction("line")->GetNpar()];
		  for(int p=0; p<histo->GetFunction("line")->GetNpar();p++)
		    pars[p] = histo->GetFunction("line")->GetParameter(p);
		  chargeAsymmetryPars_[det->first][roc->second->getID()][xyLetter] = pars;
                }
            }
	  file->Close();
        }
    }

  useEtaFunction_ = true;
}

//=============================================================================
Event::clustersMapDef clusterizer::makeClusters(Event* theEvent, Geometry* theGeometry)
{
    this->clear();
    double pi = 3.1415;

    clustersHitsMap_ = findClusters(theEvent);

    if(usePartitions_) repartClusters(clustersHitsMap_);

    for (Event::clustersHitsMapDef::iterator det = clustersHitsMap_.begin(); det != clustersHitsMap_.end(); det++)
    {
        Detector* detector = theGeometry->getDetector(det->first);
        for (Event::aClusterHitsMapDef::iterator cluster = det->second.begin(); cluster != det->second.end(); cluster++)
        {
            if (cluster->second.size() == 0) continue;


            // #####################################
            // # Remove all clusters but of size 2 #
            // #####################################
            //davide_solo_hit_doppi_in_x_per_Strip
            if ((ONLYdoubleHITS == true) && ((cluster->second.size() <= 1) || (cluster->second.size() > 2))) continue;
            if ((ONLYdoubleHITS == true) && (cluster->second.size() == 2))
            {
                Event::hitsDef::iterator hit0 = cluster->second.begin();
                Event::hitsDef::iterator hit1 = cluster->second.begin()+1;
                if ((*hit0)["col"] != (*hit1)["col"] && (*hit0)["row"] != (*hit1)["row"]) continue;
            }

            unsigned int row = 0;
            unsigned int col = 0;
            double& x        = clustersMap_[det->first][cluster->first]["x"];
            double& y        = clustersMap_[det->first][cluster->first]["y"];
            double& xErr     = clustersMap_[det->first][cluster->first]["xErr"];
            double& yErr     = clustersMap_[det->first][cluster->first]["yErr"];
            double& charge   = clustersMap_[det->first][cluster->first]["charge"];
            double& dataType = clustersMap_[det->first][cluster->first]["dataType"];
            double& size     = clustersMap_[det->first][cluster->first]["size"];
            double& stub     = clustersMap_[det->first][cluster->first]["stub"];

            charge = 0;
            stub   = 0;
            size   = cluster->second.size();

            std::vector<pixelInfos> pixels(cluster->second.size());
            int p = 0;
            ROC* roc = 0;


//            bool hasSmallCenterPixelHit = false;
//            if (det->first == "Station: 4 - Plaq: 1") {
//                for (Event::hitsDef::iterator hit = cluster->second.begin(); hit != cluster->second.end(); hit++) {
//                    //std::cout << "[ANDRE] DUT1 COL " << ((*hit)["col"]-9)%18 << ", ROW " << (*hit)["row"]%12 << std::endl;
//                    if ((((*hit)["col"]-9)%18 == 8 || ((*hit)["col"]-9)%18 == 9) && ((*hit)["row"]%12 == 5 || (*hit)["row"]%12 == 6)) {
//                        hasSmallCenterPixelHit = true;
//                        break;
//                    }
////                    if  (((*hit)["col"]-9)%18 != 8 && ((*hit)["col"]-9)%18 != 9 && ((*hit)["col"]-9)%18 !=7 && ((*hit)["col"]-9)%18 !=10)
////                        isSmallPixel = true;
////                    else
////                       if ((*hit)["row"]%12 != 4 && (*hit)["row"]%12 != 5 && (*hit)["row"]%12 != 6 && (*hit)["row"]%12 != 7)
////                           isSmallPixel = true;
//                }
//            }
//            if (!hasSmallCenterPixelHit && det->first == "Station: 4 - Plaq: 1" && size > 1)
//            {
//                std::cout << "DUT 1: event doesn't have any small center pixel hits in size-2 clusters or larger, continuing..." << std::endl;
//                continue;
//            }
//            // removing all size 2 clusters or higher for now
//            if (size > 1 && det->first == "Station: 4 - Plaq: 1") continue;

//            // removing all but one pixel
//            if (size == 1 && det->first == "Station: 4 - Plaq: 1") {
//                Event::hitsDef::iterator hit = cluster->second.begin();
//                if (((*hit)["col"]-9)%18 == 8 && (*hit)["row"]%12 == 5) {
//                    // ok
//                }
//                else
//                    continue;
//            }

//            // removing 12 non-central pixels in 50x50
//            if (size == 1 && det->first == "Station: 4 - Plaq: 1") {
//                Event::hitsDef::iterator hit = cluster->second.begin();
//                if ((((*hit)["col"]-9)%18 == 7 && (*hit)["row"]%12 == 4) ||
//                    (((*hit)["col"]-9)%18 == 7 && (*hit)["row"]%12 == 7) ||
//                    (((*hit)["col"]-9)%18 == 10 && (*hit)["row"]%12 == 4) ||
//                    (((*hit)["col"]-9)%18 == 10 && (*hit)["row"]%12 == 7)) {
//                    //std::cout << "DUT 1: eliminating 4 diagonal small pixels..." << std::endl;
//                    continue;
//                }
//                else if ((((*hit)["col"]-9)%18 == 8 && (*hit)["row"]%12 == 7) ||
//                         (((*hit)["col"]-9)%18 == 9 && (*hit)["row"]%12 == 7) ||
//                         (((*hit)["col"]-9)%18 == 8 && (*hit)["row"]%12 == 4) ||
//                         (((*hit)["col"]-9)%18 == 9 && (*hit)["row"]%12 == 4)) {
//                         //std::cout << "DUT 1: eliminating 4 vertical small pixels..." << std::endl;
//                         continue;
//                }
//                else if ((((*hit)["col"]-9)%18 == 7 && (*hit)["row"]%12 == 5) ||
//                         (((*hit)["col"]-9)%18 == 7 && (*hit)["row"]%12 == 6) ||
//                         (((*hit)["col"]-9)%18 == 9 && (*hit)["row"]%12 == 5) ||
//                         (((*hit)["col"]-9)%18 == 9 && (*hit)["row"]%12 == 6)) {
//                         //std::cout << "DUT 1: eliminating 4 lateral small pixels..." << std::endl;
//                         continue;
//                }
//            }

            //>> wsi ONLY make clusters if it has hits in short-pitch pixels
//            if( det->first=="Station: 4 - Plaq: 1" && size>1)
//            {
//                //std::ofstream myfile;
//                //myfile.open("mypos.txt",ios::app);
//                //myfile<<"wsi\t";
//                bool inShortPixel = false;
//                for (Event::hitsDef::iterator hit = cluster->second.begin(); hit != cluster->second.end(); hit++)
//                {
//                    col = (*hit)["col"];
//                    //row = (*hit)["row"];
//                    if (col%4==0 || col%4==3)
//                    {
//                        inShortPixel = true;
//                        break;
//                    }
//                    //myfile<<"("<<col<<","<<row<<") ";
//                }
//                //myfile<<"\n";
//                //myfile.close();
//                if (!inShortPixel) continue;
//            }
            //<< wsi 20/12/17

            for (Event::hitsDef::iterator hit = cluster->second.begin(); hit != cluster->second.end(); hit++, p++)
            {
                pixels[p].x         = detector->getPixelCenterLocalX( (*hit)["col"] );
                pixels[p].y         = detector->getPixelCenterLocalY( (*hit)["row"] );
                pixels[p].xPitch    = detector->getPixelPitchLocalX ( (*hit)["col"] );
                pixels[p].yPitch    = detector->getPixelPitchLocalY ( (*hit)["row"] );
                pixels[p].dataType  = (*hit)["dataType"];
                stub                = (*hit)["stub"];

                if (det->first=="Station: 4 - Plaq: 1") {
//                    std::cout << "[ANDRE BEFORE] row: " << (*hit)["row"] << ", "
//                              << " detector->getPixelPitchLocalY " << pixels[p].yPitch << std::endl;
//                    std::cout << "[ANDRE BEFORE] row: " << (*hit)["row"] << ", "
//                              << " detector->getPixelCenterLocalY " << pixels[p].y << std::endl;
                }

                row = (*hit)["row"];
                col = (*hit)["col"];
                roc = detector->convertPixelToROC(&row, &col);

                if (det->first=="Station: 4 - Plaq: 1") {
//                    std::cout << "[ANDRE AFTER] row: " << row << ", "
//                              << " detector->getPixelPitchLocalY " << pixels[p].yPitch << std::endl;
//                    std::cout << "[ANDRE AFTER] col: " << col << ", "
//                              << " detector->getPixelCenterLocalY " << pixels[p].y << std::endl;
                }

                bool isDut = false;

                if (
                        det->first == "Station: 4 - Plaq: 0"
                        || det->first == "Station: 4 - Plaq: 1"
                        || det->first == "Station: 4 - Plaq: 2"
                        || det->first == "Station: 4 - Plaq: 3"
                        || det->first == "Station: 1 - Plaq: 0"
                        || det->first == "Station: 1 - Plaq: 1"
                        )
                    isDut = true;

                roc->calibratePixel(row, col, (*hit)["adc"], (*hit)["charge"], isDut);

                pixels[p].charge = abs( (*hit)["charge"] );

                charge += pixels[p].charge;
                dataType = pixels[p].dataType;
            }

            double chargeSharing, tiltedChargeSharing,xChargeSharing,yChargeSharing;
            if(dataType==0) //Pixel Data
            {

                chargeSharing       = 0.4; // 20um charge sharing at 90 deg for pixel
                tiltedChargeSharing = 7.5; // 20um charge sharing at 25 deg for pixel
                xChargeSharing = 4*tan(fabs(detector->getYRotation(false))*pi/180) + chargeSharing ;//5*tan(fabs(detector->getYRotation(false))*pi/180)//7*sin(fabs(detector->getYRotation(false))*pi/180); //at 25 deg it gives ~+-20+30um charge sharing region which is all pixel
                yChargeSharing = 4*tan(fabs(detector->getXRotation(false))*pi/180) + chargeSharing; //5*tan(fabs(detector->getXRotation(false))*pi/180)//7*sin(fabs(detector->getXRotation(false))*pi/180);

                // ####################
                // # Cluster size = 1 #
                // ####################
                if (pixels.size() == 1)
                {
                    x = pixels[0].x;
                    y = pixels[0].y;

                    if (fabs(detector->getYRotation(false)) > 10) xErr = tiltedChargeSharing / sqrt(12.);
                    else                                          xErr = (pixels[0].xPitch - 2*chargeSharing) / sqrt(12.);

                    if (fabs(detector->getXRotation(false)) > 10) yErr = tiltedChargeSharing / sqrt(12.);
                    else                                          yErr = (pixels[0].yPitch - 2*chargeSharing) / sqrt(12.);
                }
                // ####################
                // # Cluster size = 2 #
                // ####################
                else if (pixels.size() == 2)
                {
                    double center;

                    // ###################
                    // # Cluster along y #
                    // ###################
                    if (pixels[0].x == pixels[1].x)
                    {
                        // ##############################################
                        // # Assign to DUT the coordinate of the divide #
                        // ##############################################
                        if (TESTDIVIDE == true &&
                                (det->first == "Station: 4 - Plaq: 0" || det->first == "Station: 4 - Plaq: 1" || det->first == "Station: 4 - Plaq: 2"))
                        {
                            x = pixels[0].x;
                            // xErr = pixels[0].xPitch / 2. / sqrt(12.);

                            // ################
                            // # Fixed errors #
                            // ################
                            xErr = 0.3;
                            yErr = 0.3;

                            if (pixels[0].yPitch == pixels[1].yPitch)
                            {
                                y = (pixels[0].y + pixels[1].y) / 2.;
                                // yErr = pixels[0].yPitch / 2. / sqrt(12.);
                            }
                            else if (pixels[0].yPitch > pixels[1].yPitch)
                            {
                                if (pixels[0].y > pixels[1].y) y = pixels[1].y + pixels[1].yPitch / 2.;
                                else                           y = pixels[1].y - pixels[1].yPitch / 2.;
                                // yErr = pixels[1].yPitch / 2. / sqrt(12.);
                            }
                            else
                            {
                                if (pixels[0].y > pixels[1].y) y = pixels[0].y - pixels[0].yPitch / 2.;
                                else                           y = pixels[0].y + pixels[0].yPitch / 2.;
                                // yErr = pixels[0].yPitch / 2. / sqrt(12.);
                            }
                        }
                        else
                        {
                            x = pixels[0].x;

                            if (fabs(detector->getYRotation(false)) > 10) xErr = tiltedChargeSharing / sqrt(12.);
                            else                                          xErr = (pixels[0].xPitch - 2*chargeSharing) / sqrt(12.);

                            if (pixels[0].y < pixels[1].y)
                            {
                                center = (pixels[0].y + pixels[0].yPitch/2.);
                                y = (pixels[0].charge*(center - (yChargeSharing+chargeSharing)) + pixels[1].charge*(center + (yChargeSharing+chargeSharing))) / charge;
                            }
                            else
                            {
                                center = (pixels[1].y + pixels[1].yPitch/2.);
                                y = (pixels[1].charge*(center - (yChargeSharing+chargeSharing)) + pixels[0].charge*(center + (yChargeSharing+chargeSharing)))/charge;
                            }

                            if (fabs(detector->getXRotation(false)) > 10) yErr = 1.35;
                            else                                          yErr = 0.65;
                        }
                    }
                    // ###################
                    // # Cluster along x #
                    // ###################
                    else if (pixels[0].y == pixels[1].y)
                    {
                        // ##############################################
                        // # Assign to DUT the coordinate of the divide #
                        // ##############################################
                        if (TESTDIVIDE == true &&
                                (det->first == "Station: 4 - Plaq: 0" || det->first == "Station: 4 - Plaq: 1" || det->first == "Station: 4 - Plaq: 2"))
                        {
                            y = pixels[0].y;
                            // yErr = pixels[0].yPitch / 2. / sqrt(12.);

                            // ################
                            // # Fixed errors #
                            // ################
                            xErr = 0.3;
                            yErr = 0.3;

                            if (pixels[0].xPitch == pixels[1].xPitch)
                            {
                                x = (pixels[0].x + pixels[1].x) / 2.;
                                // xErr = pixels[0].xPitch / 2. / sqrt(12.);
                            }
                            else if (pixels[0].xPitch > pixels[1].xPitch)
                            {
                                if (pixels[0].x > pixels[1].x) x = pixels[1].x + pixels[1].xPitch / 2.;
                                else                           x = pixels[1].x - pixels[1].xPitch / 2.;
                                // xErr = pixels[1].xPitch / 2. / sqrt(12.);
                            }
                            else
                            {
                                if (pixels[0].x > pixels[1].x) x = pixels[0].x - pixels[0].xPitch / 2.;
                                else                           x = pixels[0].x + pixels[0].xPitch / 2.;
                                // xErr = pixels[0].xPitch / 2. / sqrt(12.);
                            }
                        }
                        else
                        {
                            y = pixels[0].y;

                            if (fabs(detector->getXRotation(false)) > 10) yErr = tiltedChargeSharing / sqrt(12.);
                            else                                          yErr = (pixels[0].yPitch - 2*chargeSharing) / sqrt(12.);

                            if (pixels[0].x < pixels[1].x)
                            {
                                center = (pixels[0].x + pixels[0].xPitch/2.);
                                x = (pixels[0].charge*(center - (xChargeSharing+chargeSharing)) + pixels[1].charge*(center + (xChargeSharing+chargeSharing)))/charge;
                            }
                            else
                            {
                                center = (pixels[1].x + pixels[1].xPitch/2.);
                                x = (pixels[1].charge*(center - (xChargeSharing+chargeSharing)) + pixels[0].charge*(center + (xChargeSharing+chargeSharing)))/charge;
                            }

                            if (fabs(detector->getYRotation(false)) > 10) xErr = 1.35;
                            else                                          xErr = 0.65;
                        }
                    }
                    // ####################
                    // # Diagonal cluster #
                    // ####################
                    else
                    {
                        if (pixels[0].y < pixels[1].y)
                        {
                            center = (pixels[0].y + pixels[0].yPitch/2.);
                            y = (pixels[0].charge*(center - (yChargeSharing+chargeSharing)) + pixels[1].charge*(center + (yChargeSharing+chargeSharing)))/charge;
                        }
                        else
                        {
                            center = (pixels[1].y + pixels[1].yPitch/2.);
                            y = (pixels[1].charge*(center - (yChargeSharing+chargeSharing)) + pixels[0].charge*(center + (yChargeSharing+chargeSharing)))/charge;
                        }

                        if( pixels[0].x < pixels[1].x)
                        {
                            center = (pixels[0].x + pixels[0].xPitch/2.);
                            x = (pixels[0].charge*(center - (xChargeSharing+chargeSharing)) + pixels[1].charge*(center + (xChargeSharing+chargeSharing)))/charge;
                        }
                        else
                        {
                            center = (pixels[1].x + pixels[1].xPitch/2.);
                            x = (pixels[1].charge*(center - (xChargeSharing+chargeSharing)) + pixels[0].charge*(center + (xChargeSharing+chargeSharing)))/charge;
                        }

                        if (detector->getXRotation(false) != 0) yErr = 1.4;
                        else                                    yErr = 2.887;

                        if (detector->getYRotation(false) != 0) xErr = 1.4;
                        else                                    xErr = 2.887;
                    }
                }
                // ###############################
                // # Cluster size greater than 2 #
                // ###############################
                else
                {
                    row    = 0;
                    col    = 0;
                    x      = 0;
                    y      = 0;
                    xErr   = 0;
                    yErr   = 0;
                    charge = 0;

                    for (Event::hitsDef::iterator hit = cluster->second.begin(); hit != cluster->second.end(); hit++)
                    {
                        row = (*hit)["row"];
                        col = (*hit)["col"];
                        x    += detector->getPixelCenterLocalX( col );
                        xErr += detector->getPixelPitchLocalX ( col );
                        y    += detector->getPixelCenterLocalY( row );
                        yErr += detector->getPixelPitchLocalY ( row );

                        ROC *roc = detector->convertPixelToROC(&row, &col);

                        bool isDut = false;

                        if (det->first == "Station: 4 - Plaq: 0" || det->first == "Station: 4 - Plaq: 1" || det->first == "Station: 4 - Plaq: 2") isDut = true;
                        bool isCalibrated = roc->calibratePixel(row, col, (*hit)["adc"], (*hit)["charge"], isDut);


                        if (isCalibrated == true)
                            charge += (*hit)["charge"]; // Should be abs? Real charge. Should be fine since clusters of 3+ are used just for efficiencies
                        else
                            charge += 0; // Doesn't really matter, just for efficiencies
                    }

                    x    /= cluster->second.size();
                    y    /= cluster->second.size();
                    xErr /= cluster->second.size() * sqrt(12.);
                    yErr /= cluster->second.size() * sqrt(12.);
                }
            }
            else if(dataType==1) //Strip Data -> Still to be improved: Errors, cluster of size > 2
            {

                // Strip planes are coupled:
                // even planes measure x coordinate and are tilted by -15 deg around Y
                // odd planes measure y and therefore tilt does not affect measurements
                // the measured coordinate is always called "x", y is assigned to be the center of the strip

                // ####################
                // # Cluster size = 1 #
                // ####################
                if (pixels.size() == 1)
                {
                    x = pixels[0].x;
                    y = pixels[0].y;

                    xErr = pixels[0].xPitch / (2. * sqrt(12.));
                    yErr = 3800             / sqrt(12.); //length of the overlap area of two strip plane in tens of microns
                }
                // #################################################################
                // # Cluster size = 2 ---> Using strip only double cluster along x #
                // #################################################################
                else if (pixels.size() == 2)
                {
                    y = pixels[0].y;
                    yErr = 3800 / sqrt(12.); //length of the overlap area of two strip plane in tens of microns

                    // ##############################################
                    // # Assign to DUT the coordinate of the divide #
                    // ##############################################
                    if ((TESTDIVIDE == true)
                        &&(det->first == "Station: 4 - Plaq: 0" || det->first == "Station: 4 - Plaq: 1" || det->first == "Station: 4 - Plaq: 2"))
                    {
                        // ################
                        // # Fixed errors #
                        // ################
                        xErr = 0.3;
                        yErr = 0.3;

                        if (pixels[0].xPitch == pixels[1].xPitch) x = (pixels[0].x + pixels[1].x) / 2.;

                        else if (pixels[0].xPitch > pixels[1].xPitch)
                        {
                            if (pixels[0].x > pixels[1].x) x = pixels[1].x + pixels[1].xPitch / 2.;
                            else                           x = pixels[1].x - pixels[1].xPitch / 2.;
                        }
                        else
                        {
                            if (pixels[0].x > pixels[1].x) x = pixels[0].x - pixels[0].xPitch / 2.;
                            else                           x = pixels[0].x + pixels[0].xPitch / 2.;
                        }
                    }
                    else
                    {

                        xErr = pixels[0].xPitch / (2. * sqrt(12.));
//                          xErr = 10.*(pixels[0].xPitch / (2. * sqrt(12.)));

                        if (pixels[0].x < pixels[1].x)
                            x = pixels[0].x + ((pixels[1].charge)/(pixels[1].charge + pixels[0].charge))*pixels[0].xPitch;
                        else
                            x = pixels[1].x + ((pixels[0].charge)/(pixels[1].charge + pixels[0].charge))*pixels[1].xPitch;

                    }
                }
                // ####################
                // # Cluster size > 2 #
                // ####################
                else
                {
                    row    = 0;
                    col    = 0;
                    x      = 0;
                    y      = 0;
                    xErr   = 0;
                    yErr   = 0;
                    charge = 0;

                    for (Event::hitsDef::iterator hit = cluster->second.begin(); hit != cluster->second.end(); hit++)
                    {
                        row = (*hit)["row"];
                        col = (*hit)["col"];
                        x    += detector->getPixelCenterLocalX( col );
                        xErr += detector->getPixelPitchLocalX ( col );
                        y    += detector->getPixelCenterLocalY( row );
                        yErr += detector->getPixelPitchLocalY ( row );

                        ROC *roc = detector->convertPixelToROC(&row, &col);

                        bool isDut = false;

                        if (det->first == "Station: 4 - Plaq: 0" || det->first == "Station: 4 - Plaq: 1" || det->first == "Station: 4 - Plaq: 2") isDut = true;
                        bool isCalibrated = roc->calibratePixel(row, col, (*hit)["adc"], (*hit)["charge"], isDut);


                        if (isCalibrated == true)
                            charge += (*hit)["charge"]; // Should be abs? Real charge. Should be fine since clusters of 3+ are used just for efficiencies
                        else
                            charge += 0; // Doesn't really matter, just for efficiencies
                    }

                    x    /= cluster->second.size();
                    y    /= cluster->second.size();
                    xErr /= cluster->second.size() * sqrt(12.);
                    yErr /= cluster->second.size() * sqrt(12.);
                }
            }
        }
    }
    clustersBuilt_ = true;
    return clustersMap_;
}

//=============================================================================
void clusterizer::clusterize(Event* theEvent, Geometry* theGeometry)
{
  this->makeClusters(theEvent,theGeometry);
  theEvent->setClustersHits( clustersHitsMap_ );
  theEvent->setClusters    ( clustersMap_     );
}

//=============================================================================
void clusterizer::execute(Event* theEvent, Geometry* theGeometry)
{
  this->clusterize(theEvent,theGeometry);
}

//=============================================================================
std::string clusterizer::getLabel(void)
{
  return "Reconstructing hit clusters";
}

//=============================================================================
std::string clusterizer::getPlaneID(int station, int plaquette)
{
  std::stringstream ss;
  ss << "Station: " << station << " - Plaq: " << plaquette;
  return ss.str();
}
