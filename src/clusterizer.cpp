/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include "clusterizer.h"
#include <iostream>
#include <TFile.h>
#include <TF1.h>
#include <TH1.h>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>

using namespace std;


//===================================================================================
clusterizer::clusterizer(void) :
    clustersBuilt_ (false)
  , useEtaFunction_(false)
{
}

//===================================================================================
clusterizer::~clusterizer(void)
{
    //    STDLINE("clusterizer::~clusterizer",ACRed);
    for(std::map<std::string,std::map<int,std::map<std::string,double*> > >::iterator it=chargeAsymmetryPars_.begin(); it!=chargeAsymmetryPars_.end(); it++)
        for(std::map<int,std::map<std::string,double*> >::iterator it1=it->second.begin(); it1!=it->second.end(); it1++)
            for(std::map<std::string,double*>::iterator it2=it1->second.begin(); it2!=it1->second.end(); it2++)
                delete[] it2->second;

    //    STDLINE("clusterizer::~clusterizer",ACGreen);
}

//===================================================================================
struct pixelInfos
{
    double x;
    double y;
    double xPitch;
    double yPitch;
    double charge;
};

//===================================================================================
void clusterizer::clear(void )
{
    clustersHitsMap_.clear();
    clustersMap_    .clear();
}

//===================================================================================
Event::clustersHitsMapDef clusterizer::findClusters(Event *theEvent)
{
    if ( theEvent->getClustersHits().empty() )
    {
        //find clusters from vector
        this->clear();
        int clusterID=0;

        Event::plaqMapDef plaqMap = theEvent->getRawData();

        for(Event::plaqMapDef::iterator hits=plaqMap.begin(); hits!=plaqMap.end(); ++hits)
        {
            clusterID = 0;
            //            std::cout << __PRETTY_FUNCTION__ << " Plaq: " << (*hits).first << " n hits: " << (*hits).second.size() << std::endl;
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
                        if ( ( abs(cCol - hits->second[h]["col"]) <= 1 ) && ( abs(cRow - hits->second[h]["row"]) <= 1 ))
                        {
                            clusterHits.push_back( hits->second[h] );
                            /*                            STDLINE("push back!", ACYellow);
                            std::stringstream shs;
                            if ( abs(cCol - hits->second[h]["col"]) <= 1 )
                            {
                                shs << hits->first << ": got a cluster along columns " << cCol << " and " << hits->second[h]["col"] << " on row " << cRow;
                                STDLINE(shs.str(), ACWhite);
                            }
                            if ( abs(cRow - hits->second[h]["row"]) <= 1 )
                            {
                                shs << hits->first << ": got a cluster along rows " << cRow << " and " << hits->second[h]["row"] << " on column " << cCol;
                                STDLINE(shs.str(), ACWhite);
                            }
*/                            hits->second.erase( hits->second.begin()+h );
                            h--;
                        }
                    }
                }
                //                if (clusterHits.size() > 1)   //BAD THING, USED ONLY AS CHECK, TO BE REMOVED SOON!!!
                //                {
                //                    clustersHitsMap_[hits->first].erase(clusterID);
                //                    clusterID--;
                //                }
                ++clusterID;
            }
        }
        return clustersHitsMap_;
    }
    else
    {
        //STDLINE("Warning: clusters hits have been already found for this event", ACRed);
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
            //ss_.str("");
            //ss_ << "Loading calibrations for " << det->first << " -- rocID: " << roc->first << " roc ID: " << roc->second->getID();
            //STDLINE(ss_.str(),ACGreen);
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
                    STDLINE("ERROR: Can't find any Charge Asymmetry file named "
                            + calibrationsDir + what[1] + "_" + what[2] + ".root"
                            + " or " + calibrationFileName, ACRed);
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
    //    const double sqrt12 = sqrt(12.);
    this->clear();
    double pi = 3.1415;

    clustersHitsMap_ = findClusters(theEvent);
    //    std::cout << __PRETTY_FUNCTION__ << "Event" << std::endl;
    for ( Event::clustersHitsMapDef::iterator det=clustersHitsMap_.begin(); det!=clustersHitsMap_.end(); det++ )
    {
        Detector* detector = theGeometry->getDetector( det->first );
        for (Event::aClusterHitsMapDef::iterator cluster=det->second.begin(); cluster!=det->second.end(); cluster++)
        {
            if(cluster->second.size() == 0) continue;

            double& x      = clustersMap_[det->first][cluster->first]["x"];
            double& y      = clustersMap_[det->first][cluster->first]["y"];
            double& xErr   = clustersMap_[det->first][cluster->first]["xErr"];
            double& yErr   = clustersMap_[det->first][cluster->first]["yErr"];
            double& charge = clustersMap_[det->first][cluster->first]["charge"];
            unsigned int row = 0;
            unsigned int col = 0;
            charge = 0;
            double& size   = clustersMap_[det->first][cluster->first]["size"];
            size = cluster->second.size();

            std::vector<pixelInfos> pixels(cluster->second.size());
            int p = 0;
            ROC* roc = 0;
            for(Event::hitsDef::iterator hit=cluster->second.begin(); hit!=cluster->second.end(); hit++, p++)
            {
                pixels[p].x         = detector->getPixelCenterLocalX( (*hit)["col"] );
                pixels[p].y         = detector->getPixelCenterLocalY( (*hit)["row"] );
                pixels[p].xPitch    = detector->getPixelPitchLocalX ( (*hit)["col"] );
                pixels[p].yPitch    = detector->getPixelPitchLocalY ( (*hit)["row"] );
                //insert calibration
                row = (*hit)["row"];
                col = (*hit)["col"];
                roc = detector->convertPixelToROC(&row, &col);
                bool isCalibrated = roc->calibratePixel(row, col, (*hit)["adc"], (*hit)["charge"]);

                if(isCalibrated)
                    pixels[p].charge = abs( (*hit)["charge"] );
                else
                    //calibratePixel returns -999999 in case the pixel is not calibrated so I arbitrarily
                    //set a value that is typical of a minimum threshold
                    //                    pixels[p].charge = abs( (*hit)["charge"] );
                    //                    pixels[p].charge = 2000;
                    pixels[p].charge = abs( (*hit)["charge"] );

                charge += pixels[p].charge;
            }
            double chargeSharing  = 2;//20um charge sharing at 90 deg
            double xChargeSharing = 5*tan(fabs(detector->getYRotation(false))*pi/180);//7*sin(fabs(detector->getYRotation(false))*pi/180);//at 25 deg it gives ~+-20+30um charge sharing region which is all pixel
            double yChargeSharing = 5*tan(fabs(detector->getXRotation(false))*pi/180);//7*sin(fabs(detector->getXRotation(false))*pi/180);
            //if(detector->getXRotation(false) != 0 || detector->getYRotation(false) != 0)
            //    chargeSharing = 5;
            //if(detector->getXRotation(false) != 0)
            //    yChargeSharing = 5;
            //if(detector->getYRotation(false) != 0)
            //    xChargeSharing = 5;
            if(pixels.size() == 1)
            {

                x    = pixels[0].x;
                y    = pixels[0].y;
                xErr = (pixels[0].xPitch-2*xChargeSharing)/sqrt(12.);
                yErr = (pixels[0].yPitch-2*yChargeSharing)/sqrt(12.);
                //xErr = (pixels[0].xPitch-2*chargeSharing)/sqrt(12.);
                //yErr = (pixels[0].yPitch-2*chargeSharing)/sqrt(12.);
            }
            else if(pixels.size() == 2)
            {
                double center;
                double pitch;
                if(useEtaFunction_ && chargeAsymmetryPars_[det->first][roc->getID()]["X"] != 0 && chargeAsymmetryPars_[det->first][roc->getID()]["Y"] != 0)
                {
                    if(pixels[0].x == pixels[1].x)
                    {
                        x    = pixels[0].x;
                        xErr = (pixels[0].xPitch-2*xChargeSharing)/sqrt(12.);
                        if( pixels[0].y < pixels[1].y )
                        {
                            center = (pixels[0].y + pixels[0].yPitch/2. );
                            pitch = pixels[0].yPitch;
                            double eta = (pixels[0].charge - pixels[1].charge)/charge;
                            y = center + ((eta-chargeAsymmetryPars_[det->first][roc->getID()]["Y"][1])/chargeAsymmetryPars_[det->first][roc->getID()]["Y"][0])/10;
                            /*
                            double yW = (pixels[0].charge*(center - (yChargeSharing+chargeSharing)) + pixels[1].charge*(center + (yChargeSharing+chargeSharing)))/charge;
                            std::cout << det->first
                                      << " pixels[0].charge: " << pixels[0].charge
                                      << " pixels[1].charge: " << pixels[1].charge
                                      << " charge: " << charge
                                      << " eta: " << eta
                                      << " pixels[0].y: " << pixels[0].y
                                      << " pixels[1].y: " << pixels[1].y
                                      << " center: " << center
                                      << " yw: " << yW
                                      << " ye: " << y
                                      << " mY: " << chargeAsymmetryPars_[det->first]["Y"][0]
                                      << " mX: " << chargeAsymmetryPars_[det->first]["X"][0]
                                      << " delta: " << 10*(yW-y)
                                      << std::endl;
*/
                        }
                        else
                        {
                            center = (pixels[1].y + pixels[1].yPitch/2. );
                            double eta = (pixels[1].charge - pixels[0].charge)/charge;
                            y = center + ((eta-chargeAsymmetryPars_[det->first][roc->getID()]["Y"][1])/chargeAsymmetryPars_[det->first][roc->getID()]["Y"][0])/10;
                        }
                        yErr = 2.7/sqrt(12.)+sin(fabs(detector->getXRotation(false))*pi/180);
                    }
                    if(pixels[0].y == pixels[1].y)
                    {
                        y    = pixels[0].y;
                        yErr = (pixels[0].yPitch-2*yChargeSharing)/sqrt(12.);
                        if( pixels[0].x < pixels[1].x )
                        {
                            center = (pixels[0].x + pixels[0].xPitch/2. );
                            double eta = (pixels[0].charge - pixels[1].charge)/charge;
                            x = center + ((eta-chargeAsymmetryPars_[det->first][roc->getID()]["X"][1])/chargeAsymmetryPars_[det->first][roc->getID()]["X"][0])/10;
                        }
                        else
                        {
                            center = (pixels[1].x + pixels[1].xPitch/2. );
                            double eta = (pixels[1].charge - pixels[0].charge)/charge;
                            x = center + ((eta-chargeAsymmetryPars_[det->first][roc->getID()]["X"][1])/chargeAsymmetryPars_[det->first][roc->getID()]["X"][0])/10;
                        }
                        xErr = 2.7/sqrt(12.)+sin(fabs(detector->getYRotation(false))*pi/180);
                    }
                }
                else
                {
                    //Charge weighting method
                    //                    std::stringstream ssk;
                    if(pixels[0].x == pixels[1].x)
                    {
                        x    = pixels[0].x;
                        xErr = (pixels[0].xPitch-2*xChargeSharing)/sqrt(12.);
                        //                    if(detector->getYRotation(false) == 0)
                        //                        xErr = (pixels[0].xPitch)/sqrt(12.);
                        //                    else
                        //                        xErr = 0.55*(pixels[0].xPitch)/sqrt(12.);//!!!Copied from the yErr but don't have statistics
                        //xErr = pixels[0].xPitch/sqrt(12.);
                        //double c = (pixels[0].y + pixels[1].y)/2;
                        if( pixels[0].y < pixels[1].y )
                        {
                            center = (pixels[0].y + pixels[0].yPitch/2. );
                            pitch = pixels[0].yPitch;
                            y = (pixels[0].charge*(center - (yChargeSharing+chargeSharing)) + pixels[1].charge*(center + (yChargeSharing+chargeSharing)))/charge;
                            //                            ssk << "Plaquette " << det->first << ", cluster along  y: " <<  pixels[0].y << "|" << pixels[0].charge << "-" << pixels[1].y << "|" << pixels[1].charge << " -> " << y;
                            //                            STDLINE(ssk.str(), ACGreen);
                        }
                        else
                        {
                            center = (pixels[1].y + pixels[1].yPitch/2. );
                            pitch = pixels[1].yPitch;
                            y = (pixels[1].charge*(center - (yChargeSharing+chargeSharing)) + pixels[0].charge*(center + (yChargeSharing+chargeSharing)))/charge;
                            //                            ssk << "Plaquette " << det->first << ", cluster along  y: " <<  pixels[0].y << "-" << pixels[1].y << " -> " << y;
                            //                            STDLINE(ssk.str(), ACGreen);
                        }
                        //2.7/sqrt(12.) -> indetermination of the charge
                        //sin(fabs(detector->getXRotation(false))*pi/180) -> bigger charge sharing area
                        yErr = 2.7/sqrt(12.)+sin(fabs(detector->getXRotation(false))*pi/180);
                        //                        yErr = chargeSharing/(sqrt(12)*charge);

                    }
                    else if(pixels[0].y == pixels[1].y)//NEED to include the diagonals which must be treated separately
                    {
                        y    = pixels[0].y;
                        yErr = (pixels[0].yPitch-2*yChargeSharing)/sqrt(12.);
                        //if(detector->getXRotation(false) == 0)
                        //    yErr = (pixels[0].yPitch)/sqrt(12.);
                        //else
                        //    yErr = 0.55*(pixels[0].yPitch)/sqrt(12.);//25% of the cases are single pixels even when there is a rotation
                        //yErr = (pixels[0].yPitch)/sqrt(12.);
                        double center;
                        if( pixels[0].x < pixels[1].x )
                        {
                            center = (pixels[0].x + pixels[0].xPitch/2. );
                            x = (pixels[0].charge*(center - (xChargeSharing+chargeSharing)) + pixels[1].charge*(center + (xChargeSharing+chargeSharing)))/charge;
                            //                            ssk << "Plaquette " << det->first << ", cluster along  x: " <<  pixels[0].x << "-" << pixels[1].x << " -> " << x;
                            //                            STDLINE(ssk.str(), ACGreen);
                        }
                        else
                        {
                            center = (pixels[1].x + pixels[1].xPitch/2. );
                            x = (pixels[1].charge*(center - (xChargeSharing+chargeSharing)) + pixels[0].charge*(center + (xChargeSharing+chargeSharing)))/charge;
                            //                            ssk << "Plaquette " << det->first << ", cluster along  x: " <<  pixels[0].x << "-" << pixels[1].x << " -> " << x;
                            //                            STDLINE(ssk.str(), ACGreen);
                        }
                        //2.7/sqrt(12.) -> indetermination of the charge
                        //sin(fabs(detector->getYRotation(false))*pi/180) -> bigger charge sharing area
                        xErr = 2.7/sqrt(12.)+sin(fabs(detector->getYRotation(false))*pi/180);
                        //                        xErr = chargeSharing/(sqrt(12)*charge);

                    }
                    else    //this is for ***>>>>diagonal (yVet[0] != yVet[1] && xVet[0] != xVet[1]) <<<<<<<****
                    {       //cluster made of 2 hits that were discarded before the new changes
                        //STDLINE("Diagonal Cluster???", ACRed);
                        if( pixels[0].y < pixels[1].y )
                        {
                            center = (pixels[0].y + pixels[0].yPitch/2. );
                            y = (pixels[0].charge*(center - (yChargeSharing+chargeSharing)) + pixels[1].charge*(center + (yChargeSharing+chargeSharing)))/charge;
                        }
                        else
                        {
                            center = (pixels[1].y + pixels[1].yPitch/2. );
                            y = (pixels[1].charge*(center - (yChargeSharing+chargeSharing)) + pixels[0].charge*(center + (yChargeSharing+chargeSharing)))/charge;
                        }

                        if( pixels[0].x < pixels[1].x )
                        {
                            center = (pixels[0].x + pixels[0].xPitch/2. );
                            x = (pixels[0].charge*(center - (xChargeSharing+chargeSharing)) + pixels[1].charge*(center + (xChargeSharing+chargeSharing)))/charge;
                        }
                        else
                        {
                            center = (pixels[1].x + pixels[1].xPitch/2. );
                            x = (pixels[1].charge*(center - (xChargeSharing+chargeSharing)) + pixels[0].charge*(center + (xChargeSharing+chargeSharing)))/charge;
                        }

                        if( detector->getXRotation(false)!=0 )
                        {
                            yErr = 20./(6.5*sqrt(12.));
                            //yErr = 20./(4.6*sqrt(12.));
                        }
                        else  yErr = 2.887                ;

                        if( detector->getYRotation(false)!=0 )
                        {
                            xErr = 20./(6.5*sqrt(12.));
                            //xErr = 20./(4.6*sqrt(12.));
                        }
                        else  xErr = 2.887   ;
                    }
                }
            }
            /*
                if ( cluster->second.size() == 1 )
                {
                    row  = (*hit)["row"];
                    col  = (*hit)["col"];
                    x    = detector->getPixelCenterLocalX( col );
                    xErr = detector->getPixelPitchLocalX ( col )/sqrt12;
                    y    = detector->getPixelCenterLocalY( row );
                    yErr = detector->getPixelPitchLocalY ( row )/sqrt12;

                    //insert calibration
                    ROC *roc = detector->convertPixelToROC(&row, &col);
                    bool isCalibrated = roc->calibratePixel(row, col, (*hit)["adc"], (*hit)["charge"]);

                    if(isCalibrated)
                        charge += (*hit)["charge"];
                    else
                        //calibratePixel returns -999999 in case the pixel is not calibrated so I arbitrarily
                        //set a value that is typical of a minimum threshold
                        charge += 0;

                }
                else
*/
            /*
            if ( cluster->second.size() == 2 )
            {
                std::vector<double> xVet, yVet, adcV, adcE;
                std::vector< std::pair<double,double> > pitches;
                std::pair<double,double>                pitch  ;
                charge = 0;
                for(Event::hitsDef::iterator hit=cluster->second.begin(); hit!=cluster->second.end(); hit++ )
                {
                    y = detector->getPixelCenterLocalY( (*hit)["row"] );
                    x = detector->getPixelCenterLocalX( (*hit)["col"] );
                    pitch.first  = detector->getPixelPitchLocalX( (*hit)["col"] );
                    pitch.second = detector->getPixelPitchLocalY( (*hit)["row"] );
                    pitches.push_back(pitch);
                    xVet.push_back(x);
                    yVet.push_back(y);

                    //insert calibration
                    unsigned int row = (*hit)["row"];
                    unsigned int col = (*hit)["col"];
                    ROC *roc = detector->convertPixelToROC(&row, &col);
                    bool isCalibrated = roc->calibratePixel(row, col, (*hit)["adc"], (*hit)["charge"]);
                    if(isCalibrated)
                        adcV.push_back( abs( (*hit)["charge"] ) );
                    else
                        //calibratePixel returns -999999 in case the pixel is not calibrated so I arbitrarily
                        //set a value that is typical of a minimum threshold
                        adcV.push_back( 2000 );

                    charge += adcV[adcV.size()-1];
                }

                double chargeSharingSize = 2;//->Ortogonal to the beam
                if(detector->getXRotation(false)!=0 || detector->getYRotation(false)!=0 )
                    chargeSharingSize = 5;
                if ( cluster->second.size() == 2 )
                {
                    double adcSum = (adcV[0]+adcV[1]);
                    if ( xVet[0] == xVet[1] )//&& detector->getXRotation(false)!=0
                    {
                        x    = xVet[0];
                        xErr = pitches[0].first/sqrt(12.);

                        if( yVet[0] < yVet[1] )
                        {
                            double c = (yVet[0] + pitches[0].second/2. );
                            y = (adcV[0]/adcSum)*(c - chargeSharingSize) + (adcV[1]/adcSum)*(c + chargeSharingSize);
                        }
                        else
                        {
                            double c = (yVet[1] + pitches[1].second/2. );
                            y = (adcV[1]/adcSum)*(c - chargeSharingSize) + (adcV[0]/adcSum)*(c + chargeSharingSize);
                        }

                        if( detector->getXRotation(false)!=0 )
                        {
                            yErr = 20./(4.6*sqrt(12.));
                            //                                yErr = 1.5;
                            //yErr = 20./(6.5*sqrt(12.));
                        }
                        else
                            yErr = 2.887                ;

                    }
                    else if ( yVet[0] == yVet[1] )//&& detector->getYrotation(false)!=0 )
                    {
                        y    = yVet[0];
                        yErr = pitches[0].second/sqrt(12.);

                        if( xVet[0] < xVet[1] )
                        {
                            double c = (xVet[0] + pitches[0].first/2. );
                            x = (adcV[0]/adcSum)*(c - chargeSharingSize) + (adcV[1]/adcSum)*(c + chargeSharingSize);
                        }
                        else
                        {
                            double c = (xVet[1] + pitches[1].first/2. );
                            x = (adcV[1]/adcSum)*(c - chargeSharingSize) + (adcV[0]/adcSum)*(c + chargeSharingSize);
                        }

                        if( detector->getYRotation(false)!=0 )
                        {
                            xErr = 20./(4.6*sqrt(12.));
                            //                                xErr = 1.5;
                            //xErr = 20./(6.5*sqrt(12.));
                        }
                        else  xErr = 2.887;
                    }
                    //else continue;
                    else    //this is for ***>>>>diagonal (yVet[0] != yVet[1] && xVet[0] != xVet[1]) <<<<<<<****
                    {       //cluster made of 2 hits that were discarded before the new changes
                        if( yVet[0] < yVet[1] )
                        {
                            double c = (yVet[0] + pitches[0].second/2. );
                            y = (adcV[0]/adcSum)*(c - chargeSharingSize) + (adcV[1]/adcSum)*(c + chargeSharingSize);
                        }
                        else
                        {
                            double c = (yVet[1] + pitches[1].second/2. );
                            y = (adcV[1]/adcSum)*(c - chargeSharingSize) + (adcV[0]/adcSum)*(c + chargeSharingSize);
                        }

                        if( xVet[0] < xVet[1] )
                        {
                            double c = (xVet[0] + pitches[0].first/2. );
                            x = (adcV[0]/adcSum)*(c - chargeSharingSize) + (adcV[1]/adcSum)*(c + chargeSharingSize);
                        }
                        else
                        {
                            double c = (xVet[1] + pitches[1].first/2. );
                            x = (adcV[1]/adcSum)*(c - chargeSharingSize) + (adcV[0]/adcSum)*(c + chargeSharingSize);
                        }

                        if( detector->getXRotation(false)!=0 )
                        {
                            yErr = 20./(6.5*sqrt(12.));
                            //yErr = 20./(4.6*sqrt(12.));
                        }
                        else  yErr = 2.887                ;

                        if( detector->getYRotation(false)!=0 )
                        {
                            xErr = 20./(6.5*sqrt(12.));
                            //xErr = 20./(4.6*sqrt(12.));
                        }
                        else  xErr = 2.887   ;
                    }
                }
            }
                    if( cluster->second.size() == 4 )
                    {
                        double adcSum = (adcV[0] + adcV[1] + adcV[2] + adcV[3]);

                        if(xVet[0] == xVet[1] &&  xVet[2] == xVet[3])
                        {
                            if(yVet[0] == yVet[3] && yVet[1] == yVet[2])
                            {
                                y    = ( (adcV[0]+adcV[3])/adcSum )*yVet[0] + ( (adcV[1]+adcV[2])/adcSum )*yVet[1];
                                x    = ( (adcV[0]+adcV[1])/adcSum )*xVet[0] + ( (adcV[3]+adcV[2])/adcSum )*xVet[2];
                                xErr = (pitches[0].first  + pitches[2].first )/(4*sqrt(12.))          ;
                                yErr = (pitches[0].second + pitches[1].second)/(4*sqrt(12.))          ;
                            }
                            else if(yVet[0] == yVet[2] && yVet[1] == yVet[3])
                            {
                                y    = ( (adcV[0]+adcV[2])/adcSum )*yVet[0] + ( (adcV[1]+adcV[3])/adcSum )*yVet[1];
                                x    = ( (adcV[0]+adcV[1])/adcSum )*xVet[0] + ( (adcV[2]+adcV[3])/adcSum )*xVet[2];
                                xErr = (pitches[0].first  + pitches[2].first )/(4*sqrt(12.))          ;
                                yErr = (pitches[0].second + pitches[1].second)/(4*sqrt(12.))          ;
                            }
                            else continue;
                        }

                        else if(xVet[0] == xVet[3] &&  xVet[1] == xVet[2])
                        {
                            if(yVet[0] == yVet[1] && yVet[2] == yVet[3])
                            {
                                y    = ( (adcV[0]+adcV[1])/adcSum )*yVet[0] + ( (adcV[2]+adcV[3])/adcSum )*yVet[3];
                                x    = ( (adcV[0]+adcV[3])/adcSum )*xVet[0] + ( (adcV[1]+adcV[2])/adcSum )*xVet[2];
                                xErr = (pitches[0].first  + pitches[1].first )/(4*sqrt(12.))          ;
                                yErr = (pitches[0].second + pitches[3].second)/(4*sqrt(12.))          ;
                            }
                            else if(yVet[0] == yVet[2] && yVet[1] == yVet[3])
                            {
                                y    = ( (adcV[0]+adcV[2])/adcSum )*yVet[0] + ( (adcV[1]+adcV[3])/adcSum )*yVet[3];
                                x    = ( (adcV[0]+adcV[3])/adcSum )*xVet[0] + ( (adcV[1]+adcV[2])/adcSum )*xVet[2];
                                xErr = (pitches[0].first  + pitches[1].first )/(4*sqrt(12.))          ;
                                yErr = (pitches[0].second + pitches[3].second)/(4*sqrt(12.))          ;
                            }
                            else continue;
                        }

                        else if(xVet[0] == xVet[2] &&  xVet[1] == xVet[3])
                        {
                            if(yVet[0] == yVet[1] && yVet[2] == yVet[3])
                            {
                                y    = ( (adcV[0]+adcV[1])/adcSum )*yVet[0] + ( (adcV[2]+adcV[3])/adcSum )*yVet[2];
                                x    = ( (adcV[0]+adcV[2])/adcSum )*xVet[0] + ( (adcV[1]+adcV[3])/adcSum )*xVet[1];
                                xErr = (pitches[0].first  + pitches[1].first )/(4*sqrt(12.))          ;
                                yErr = (pitches[0].second + pitches[2].second)/(4*sqrt(12.))          ;
                            }
                            else if(yVet[0] == yVet[3] && yVet[1] == yVet[2])
                            {
                                y    = ( (adcV[0]+adcV[3])/adcSum )*yVet[0] + ( (adcV[1]+adcV[2])/adcSum )*yVet[2];
                                x    = ( (adcV[0]+adcV[2])/adcSum )*xVet[0] + ( (adcV[1]+adcV[3])/adcSum )*xVet[1];
                                xErr = (pitches[0].first  + pitches[1].first )/(4*sqrt(12.))          ;
                                yErr = (pitches[0].second + pitches[3].second)/(4*sqrt(12.))          ;
                            }
                            else continue;
                        }
                        else continue;
                    }
            }
*/
            else//all other cluster types
            {
                //cout << "Other cluster" << endl;
                //                STDLINE("Cluster size >= 2 ??", ACRed);
                row    = 0;
                col    = 0;
                x      = 0;
                y      = 0;
                xErr   = 0;
                yErr   = 0;
                charge = 0;
                for(Event::hitsDef::iterator hit=cluster->second.begin(); hit!=cluster->second.end(); hit++ )
                {
                    row = (*hit)["row"];
                    col = (*hit)["col"];
                    x    += detector->getPixelCenterLocalX( col );
                    xErr += detector->getPixelPitchLocalX ( col );
                    y    += detector->getPixelCenterLocalY( row );
                    yErr += detector->getPixelPitchLocalY ( row );

                    //insert calibration
                    ROC *roc = detector->convertPixelToROC(&row, &col);
                    bool isCalibrated = roc->calibratePixel(row, col, (*hit)["adc"], (*hit)["charge"]);

                    if(isCalibrated)
                        charge += (*hit)["charge"];  //Should be abs? Real charge. Should be find since clusters of 3+ are used just for efficiencies
                        //charge += abs((*hit)["charge"]);
                    else
                        //calibratePixel returns -999999 in case the pixel is not calibrated so I arbitrarily
                        //set a value that is typical of a minimum threshold
                        charge += 0; //Doesn't really matter, just for efficiencies
                        //charge += (*hit)["charge"];  //Should be abs? Not real charge - just adc
                        //charge += abs((*hit)["charge"]);
                }
                if(cluster->second.size() != 1)
                {
                    x    /= cluster->second.size();
                    y    /= cluster->second.size();
                    xErr /= cluster->second.size()*sqrt(12.);
                    yErr /= cluster->second.size()*sqrt(12.);
                }
                xErr /= sqrt(12.);
                yErr /= sqrt(12.);
            }
            //            std::stringstream ss; ss.str("");
            //            ss << "x: " << x << " y: " << y << " row: " << row << " col: " << col << " xErr: " << xErr << " yErr: " << yErr << " charge: " << charge << " size: " << size;
            //            STDLINE(ss.str(), ACRed);
            //detector->flipPositionLocal(&x, &y, &xErr, &yErr);
        }
    }

    clustersBuilt_ = true ;

    //    for (Event::clustersMapDef::iterator ig = clustersMap_.begin(); ig != clustersMap_.end(); ++ig)
    //    {
    //        STDLINE(ig->first, ACYellow);
    //    }

/*
    std::stringstream ssf;

    STDLINE("New event", ACYellow);
    for(Geometry::iterator det=theGeometry->begin(); det!=theGeometry->end(); det++)
    {
        int station = theGeometry->getDetectorStation(det->first);
        int module  = theGeometry->getDetectorModule(det->first);
        if(station == 5 || station == 6 || station == 7)
        {
            for (unsigned int it = 0; it < clustersMap_[getPlaneID(station, module)].size(); ++it)
            {
                ssf.str("");
                ssf << "Station: " << station << " module: " << module << " hit: " << it <<  " in (" << clustersMap_[getPlaneID(station, module)][it]["x"] << ", " << clustersMap_[getPlaneID(station, module)][it]["y"] << ")";
                STDLINE("Before cluster copy: " + ssf.str(), ACRed);
                if (module%2 == 0) //first module of the pair
                {
                    clustersMap_[getPlaneID(station, module)][it]["y"] = clustersMap_[getPlaneID(station, module+1)][it]["y"];
                    clustersMap_[getPlaneID(station, module)][it]["yErr"] = 90000/sqrt(12);
                    //                    clustersMap_[getPlaneID(stat, plaq)][it]["yErr"] = clustersMap_[getPlaneID(stat, plaq+1)][it]["yErr"];
                    //                    clustersMap_[getPlaneID(stat, plaq)][it]["xErr"] = 6/sqrt(12);
                    //                    clustersMap_[getPlaneID(stat, plaq)][it]["yErr"] = 6/sqrt(12);
                    //ssf << "Station: " << station << " module: " << module << ": getting cluster #" << it << " while from station: " << station << " module: " << module+1;
                    //if (it > 0) STDLINE(ssf.str(), ACWhite);
                }

                else //second plaquette of the pair
                {
                    clustersMap_[getPlaneID(station, module)][it]["x"] = clustersMap_[getPlaneID(station, module-1)][it]["x"];
                    clustersMap_[getPlaneID(station, module)][it]["xErr"] = 90000/sqrt(12);
                    //                    clustersMap_[getPlaneID(stat, plaq)][it]["xErr"] = clustersMap_[getPlaneID(stat, plaq-1)][it]["xErr"];
                    //                    clustersMap_[getPlaneID(stat, plaq)][it]["xErr"] = 6/sqrt(12);
                    //                    clustersMap_[getPlaneID(stat, plaq)][it]["yErr"] = 6/sqrt(12);
                    //                    ssf << "St. " << stat << " pl. " << plaq << ": getting cluster #" << it;
                    //                    if (it > 0)STDLINE(ssf.str(), ACWhite);
                }
                ssf.str("");
                ssf << "Station: " << station << " module: " << module << " hit: " << it <<  " in (" << clustersMap_[getPlaneID(station, module)][it]["x"] << ", " << clustersMap_[getPlaneID(station, module)][it]["y"] << ")";
                STDLINE("After  cluster copy: " + ssf.str(), ACGreen);
            }
        }
        //            if (clustersMap_[getPlaneID(stat, plaq)].size() > 1) clustersMap_[getPlaneID(stat, plaq)].erase(clustersMap_[getPlaneID(stat, plaq)].begin(), clustersMap_[getPlaneID(stat, plaq)].end());
    }
*/
    return  clustersMap_;
}
//=============================================================================
void clusterizer::clusterize(Event *theEvent, Geometry *theGeometry)
{
    //STDLINE("Staring clusterize", ACRed);
    this->makeClusters(theEvent,theGeometry);
    theEvent->setClustersHits( clustersHitsMap_ );
    theEvent->setClusters    ( clustersMap_     );
}

//=============================================================================
void clusterizer::execute(Event * theEvent, Geometry * theGeometry)
{
    this->clusterize  (theEvent,theGeometry);
}

//=============================================================================
std::string  clusterizer::getLabel (void )
{
    return "Reconstructing hit clusters";
}

std::string clusterizer::getPlaneID (int station, int plaquette)
{
    std::stringstream ss;
    ss << "Station: " << station << " - Plaq: " << plaquette;
    return ss.str();
}
