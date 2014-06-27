/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include "trackFinder.h"
#include <iostream>
#include <TVectorT.h>
#include <TMatrixTSym.h>
using namespace std;

#define MINIMUM_TRACK_POINTS 3
#define CHI2DOF_CUT 1e8

//ClassImp(trackFinder)

//================================================================
void trackFinder::findFirstAndLastTrackCandidates(Event* theEvent, Geometry* theGeometry)
{
    //Event::trackCandidatesDef                      &trackCandidates_      = theEvent->getTrackCandidates()       ;
    //Event::fittedTracksDef                         &tracksFitted_         = theEvent->getFittedTracks()          ;
    //Event::fittedTracksCovarianceDef               &covMat_               = theEvent->getFittedTracksCovariance();
    //Event::chi2VectorDef                           &chi2_                 = theEvent->getFittedTracksChi2()      ;
    Event::clustersMapDef                          &clusters              = theEvent->getClusters()              ;
    Event::clustersMapDef                          &alignedClusters       = theEvent->getAlignedClusters()       ;
    Event::trackCandidatesDef                      &alignedHitsCandidates = theEvent->getAlignedHitsCandidates() ;
    //std::vector<Event::alignedHitsCandidateMapDef> &alignedHitsCandidates = theEvent->getAlignedHitsCandidates() ;

    //cout << __PRETTY_FUNCTION__ << "First and Last track search" << endl;
    //cout << __PRETTY_FUNCTION__ << "Event number: " << theEvent->getTrigger() << endl;

    //trackCandidates_.clear();
    //tracksFitted_   .clear();
    //covMat_         .clear();
    //chi2_           .clear();

    alignedClusters .clear();
    alignedHitsCandidates.clear();

    if ( clusters.size()==0 ) return;

    if( minPoints_ < MINIMUM_TRACK_POINTS )
        minPoints_ = MINIMUM_TRACK_POINTS;

    for(Event::clustersMapDef::iterator dIt=clusters.begin(); dIt!=clusters.end(); ++dIt)
        if(maxPlanePoints_ > 0 && dIt->second.size() > maxPlanePoints_)
            return;

    //Since all tracking is done in global coordinate first create alignedClusters from local clusters
    //Event::clustersMapDef alignedClusters;
    for(Event::clustersMapDef::iterator dIt=clusters.begin(); dIt!=clusters.end(); ++dIt)
    {
        if ( theGeometry->getDetector(dIt->first)->isDUT() ) continue;
        //cout << __PRETTY_FUNCTION__ << dIt->second.size() << endl;
        for (Event::aClusterMapDef::iterator cluster=dIt->second.begin(); cluster!=dIt->second.end(); cluster++)
        {

            Event::aClusterDef& alignedCluster = (alignedClusters[ dIt->first ])[cluster->first];
            //cluster is a copy of the real clusters so I just change them to global coordinates
            alignedCluster["x"]      = cluster->second["x"]     ;
            alignedCluster["y"]      = cluster->second["y"]     ;
            alignedCluster["z"]      = 0;
            alignedCluster["xErr"]   = cluster->second["xErr"]  ;
            alignedCluster["yErr"]   = cluster->second["yErr"]  ;
            alignedCluster["zErr"]   = 0;
            alignedCluster["size"]   = cluster->second["size"]  ;
            alignedCluster["charge"] = cluster->second["charge"];
            alignedCluster["num"]    = cluster->first  ;

            //std::stringstream ssk;
            //ssk << dIt->first << ": hit in (" << alignedCluster["x"] << ", " << alignedCluster["y"] << ")";
            //STDLINE("In local coordinates before equalization: " + ssk.str(), ACGreen);

            alignedCluster["xyErr"]  = theGeometry->getDetector( dIt->first )->fromLocalToGlobal( &alignedCluster["x"], &alignedCluster["y"], &alignedCluster["z"], &alignedCluster["xErr"], &alignedCluster["yErr"], &alignedCluster["zErr"] ) ;
        }
    }

    for(Geometry::iterator det=theGeometry->begin(); det!=theGeometry->end(); det++)
    {
        int station = theGeometry->getDetectorStation(det->first);
        int module  = theGeometry->getDetectorModule(det->first);
        //int dataType = theGeometry->getDetectorDataType(det->first);

        //if(dataType == 1) //strip data
        if(station == 5 || station == 6 || station == 7)
        {
            for (Event::aClusterMapDef::iterator cluster=alignedClusters[getPlaneID(station, module)].begin(); cluster!=alignedClusters[getPlaneID(station, module)].end(); cluster++)
            {
                if (module%2 == 0) //first plaquette of the pair
                {
                                    //ensures data exists by using the find function (statment returns false if the data doesn't exist)                             ensures data exists by testing for NaN (returns false if stolen data is NaN)                                                                makes sure there are the same number of points on each plaq
                   if (alignedClusters[getPlaneID(station, module+1)].find(cluster->first) != alignedClusters[getPlaneID(station, module+1)].end() && !(alignedClusters[getPlaneID(station, module+1)][cluster->first]["y"]!=alignedClusters[getPlaneID(station, module+1)][cluster->first]["y"]) && alignedClusters[getPlaneID(station, module)].size()==alignedClusters[getPlaneID(station, module+1)].size())
                   {
                        /*cout << "  " << endl;
                        cout << "First" << endl;
                        //cout << (alignedClusters[getPlaneID(station, module+1)][cluster->first]["y"]!=(alignedClusters[getPlaneID(station, module+1)][cluster->first]["y"])) << endl;
                        //cout << "     Current: " <<  (alignedClusters[getPlaneID(station, module)].find(cluster->first)!=alignedClusters[getPlaneID(station, module)].end()) << endl;
                        //cout << "     Stolen:  " <<  (alignedClusters[getPlaneID(station, module+1)].find(cluster->first)!=alignedClusters[getPlaneID(station, module+1)].end()) << endl;
                        cout << "     x-value: " << alignedClusters[getPlaneID(station, module)][cluster->first]["x"] << endl;
                        cout << "     y-value: " << alignedClusters[getPlaneID(station, module)][cluster->first]["y"] << endl;
                        cout << "New  y-value: " << alignedClusters[getPlaneID(station, module+1)][cluster->first]["y"] << endl;*/

                        alignedClusters[getPlaneID(station, module)][cluster->first]["y"]     = alignedClusters[getPlaneID(station, module+1)][cluster->first]["y"];
                        alignedClusters[getPlaneID(station, module)][cluster->first]["yErr"]  = alignedClusters[getPlaneID(station, module+1)][cluster->first]["yErr"];
                        alignedClusters[getPlaneID(station, module)][cluster->first]["xyErr"] = 0;

                        //cout << "Stole y-value from partner module. y = " << alignedClusters[getPlaneID(station, module+1)][cluster->first]["y"] << " and yErr = " << alignedClusters[getPlaneID(station, module+1)][cluster->first]["yErr"] << endl;
                   }

                   else //Otherwise, delete the whole point (both strips)
                   {
                       /*cout << "  " << endl;
                       cout << "First" << endl;
                       cout << "Bad set - erased data" << endl;
                       //cout << (alignedClusters[getPlaneID(station, module+1)][cluster->first]["y"]!=(alignedClusters[getPlaneID(station, module+1)][cluster->first]["y"])) << endl;
                       //cout << "     Current: " <<  (alignedClusters[getPlaneID(station, module)].find(cluster->first)!=alignedClusters[getPlaneID(station, module)].end()) << endl;
                       //cout << "     Stolen:  " <<  (alignedClusters[getPlaneID(station, module+1)].find(cluster->first)!=alignedClusters[getPlaneID(station, module+1)].end()) << endl;
                       cout << "     x-value: " << alignedClusters[getPlaneID(station, module)][cluster->first]["x"] << endl;
                       cout << "     y-value: " << alignedClusters[getPlaneID(station, module)][cluster->first]["y"] << endl;
                       cout << "New  y-value: " << alignedClusters[getPlaneID(station, module+1)][cluster->first]["y"] << endl;

                       cout << "  " << endl;
                       cout << "Erase corresponding set" << endl;
                       //cout << (alignedClusters[getPlaneID(station, module-1)][cluster->first]["x"]!=(alignedClusters[getPlaneID(station, module-1)][cluster->first]["x"])) << endl;
                       //cout << "     Current: " <<  (alignedClusters[getPlaneID(station, module+1)].find(cluster->first)!=alignedClusters[getPlaneID(station, module)].end()) << endl;
                       //cout << "     Stolen:  " <<  (alignedClusters[getPlaneID(station, module)].find(cluster->first)!=alignedClusters[getPlaneID(station, module-1)].end()) << endl;
                       cout << "     x-value: " << alignedClusters[getPlaneID(station, module+1)][cluster->first]["x"] << endl;
                       cout << "New  x-value: " << alignedClusters[getPlaneID(station, module)][cluster->first]["x"] << endl;
                       cout << "     y-value: " << alignedClusters[getPlaneID(station, module+1)][cluster->first]["y"] << endl;*/

                       alignedClusters[getPlaneID(station, module)].erase(cluster->first);
                       alignedClusters[getPlaneID(station, module+1)].erase(cluster->first);
                    }
                }

                else //second plaquette of the pair
                {
                                     //ensures data exists by using the find function (statment returns false if the data doesn't exist)                             ensures data exists by testing for NaN (returns false if stolen data is NaN)                                                               make sure there are the same number of points on each plaq
                    if (alignedClusters[getPlaneID(station, module-1)].find(cluster->first) != alignedClusters[getPlaneID(station, module-1)].end() && !(alignedClusters[getPlaneID(station, module-1)][cluster->first]["x"]!=alignedClusters[getPlaneID(station, module-1)][cluster->first]["x"]) && alignedClusters[getPlaneID(station, module)].size()==alignedClusters[getPlaneID(station, module-1)].size())
                    {
                        /*cout << "  " << endl;
                        cout << "Second" << endl;
                        //cout << (alignedClusters[getPlaneID(station, module-1)][cluster->first]["x"]!=(alignedClusters[getPlaneID(station, module-1)][cluster->first]["x"])) << endl;
                        //cout << "     Current: " <<  (alignedClusters[getPlaneID(station, module)].find(cluster->first)!=alignedClusters[getPlaneID(station, module)].end()) << endl;
                        //cout << "     Stolen:  " <<  (alignedClusters[getPlaneID(station, module-1)].find(cluster->first)!=alignedClusters[getPlaneID(station, module-1)].end()) << endl;
                        cout << "     x-value: " << alignedClusters[getPlaneID(station, module)][cluster->first]["x"] << endl;
                        cout << "New  x-value: " << alignedClusters[getPlaneID(station, module-1)][cluster->first]["x"] << endl;
                        cout << "     y-value: " << alignedClusters[getPlaneID(station, module)][cluster->first]["y"] << endl;*/

                        alignedClusters[getPlaneID(station, module)][cluster->first]["x"]     = alignedClusters[getPlaneID(station, module-1)][cluster->first]["x"];
                        alignedClusters[getPlaneID(station, module)][cluster->first]["xErr"]  = alignedClusters[getPlaneID(station, module-1)][cluster->first]["xErr"];
                        alignedClusters[getPlaneID(station, module)][cluster->first]["xyErr"] = 0;

                        //cout << "Stole x-value from partner module. x = " << alignedClusters[getPlaneID(station, module-1)][cluster->first]["x"] << " and xErr = " << alignedClusters[getPlaneID(station, module-1)][cluster->first]["xErr"] << endl;
                    }

                    else //Otherwise, delete the whole point (both strips)
                    {
                        /*cout << "  " << endl;
                        cout << "Second" << endl;
                        cout << "Bad set - erased data" << endl;
                        //cout << (alignedClusters[getPlaneID(station, module-1)][cluster->first]["x"]!=(alignedClusters[getPlaneID(station, module-1)][cluster->first]["x"])) << endl;
                        //cout << "     Current: " <<  (alignedClusters[getPlaneID(station, module)].find(cluster->first)!=alignedClusters[getPlaneID(station, module)].end()) << endl;
                        //cout << "     Stolen:  " <<  (alignedClusters[getPlaneID(station, module-1)].find(cluster->first)!=alignedClusters[getPlaneID(station, module-1)].end()) << endl;
                        cout << "     x-value: " << alignedClusters[getPlaneID(station, module)][cluster->first]["x"] << endl;
                        cout << "New  x-value: " << alignedClusters[getPlaneID(station, module-1)][cluster->first]["x"] << endl;
                        cout << "     y-value: " << alignedClusters[getPlaneID(station, module)][cluster->first]["y"] << endl;

                        cout << "  " << endl;
                        cout << "Erase corresponding set" << endl;
                        //cout << (alignedClusters[getPlaneID(station, module-1)][cluster->first]["x"]!=(alignedClusters[getPlaneID(station, module-1)][cluster->first]["x"])) << endl;
                        //cout << "     Current: " <<  (alignedClusters[getPlaneID(station, module-1)].find(cluster->first)!=alignedClusters[getPlaneID(station, module)].end()) << endl;
                        //cout << "     Stolen:  " <<  (alignedClusters[getPlaneID(station, module)].find(cluster->first)!=alignedClusters[getPlaneID(station, module-1)].end()) << endl;
                        cout << "     x-value: " << alignedClusters[getPlaneID(station, module-1)][cluster->first]["x"] << endl;
                        cout << "     y-value: " << alignedClusters[getPlaneID(station, module-1)][cluster->first]["y"] << endl;
                        cout << "New  y-value: " << alignedClusters[getPlaneID(station, module)][cluster->first]["y"] << endl;*/

                        alignedClusters[getPlaneID(station, module)].erase(cluster->first);
                        alignedClusters[getPlaneID(station, module-1)].erase(cluster->first);
                    }
                }
            }
        }
        //            if (clustersMap_[getPlaneID(station, module)].size() > 1) clustersMap_[getPlaneID(station, module)].erase(clustersMap_[getPlaneID(station, module)].begin(), clustersMap_[getPlaneID(station, module)].end());
    }

    //STDLINE("New track", ACYellow);
    double xLoc, yLoc, zLoc;
    for(Geometry::iterator det=theGeometry->begin(); det!=theGeometry->end(); det++)
    {
        int station = theGeometry->getDetectorStation(det->first);
        int module  = theGeometry->getDetectorModule(det->first);
        //int dataType = theGeometry->getDetectorDataType(det->first);

        //if (dataType == 1) //strip data
        if(station == 5 || station == 6 || station == 7)
        {
            for (Event::aClusterMapDef::iterator clusterLocal = clusters[getPlaneID(station, module)].begin(); clusterLocal != clusters[getPlaneID(station, module)].end(); ++clusterLocal)
            {
//                std::stringstream ssl;
//                ssl << "Station: " << station << " module: " << module << " hit: " << clusterLocal->first <<  " in (" << clusters[getPlaneID(station, module)][clusterLocal->first]["x"] << ", " << clusters[getPlaneID(station, module)][clusterLocal->first]["y"] << ")";
//                STDLINE("Before returning to local: " + ssl.str(), ACRed);
                xLoc = alignedClusters[getPlaneID(station, module)][clusterLocal->first]["x"];
                yLoc = alignedClusters[getPlaneID(station, module)][clusterLocal->first]["y"];
                zLoc = alignedClusters[getPlaneID(station, module)][clusterLocal->first]["z"];
                theGeometry->getDetector(station, module)->fromGlobalToLocal(&xLoc, &yLoc, &zLoc);
                clusters[getPlaneID(station, module)][clusterLocal->first]["x"] = xLoc;
                clusters[getPlaneID(station, module)][clusterLocal->first]["y"] = yLoc;
/*
                if (module%2 == 0)
                {
                    clusters[getPlaneID(station, module)][clusterLocal->first]["y"] = yLoc;
                }
                else
                {
                    clusters[getPlaneID(station, module)][clusterLocal->first]["x"] = xLoc;
                }
*/
//                ssl.str("");
//                ssl << "Station: " << station << " module: " << module << " hit: " << clusterLocal->first <<  " in (" << clusters[getPlaneID(station, module)][clusterLocal->first]["x"] << ", " << clusters[getPlaneID(station, module)][clusterLocal->first]["y"] << ")";
//                STDLINE("After  returning to local: " + ssl.str(), ACGreen);
            }
        }
    }

    if ( alignedClusters.size() < minPoints_ ) return;

    Event::alignedHitsCandidateMapDef  alignedHitsCandidate ;
    //find first and last detector in space
    std::string firstDetector = alignedClusters.begin()->first;
    std::string lastDetector  = alignedClusters.begin()->first;
    for ( Event::clustersMapDef::iterator det=alignedClusters.begin(); det!=alignedClusters.end(); det++ )
    {
        //cout << __PRETTY_FUNCTION__ << "First: " << firstDetector << " first z: " << theGeometry->getDetector(firstDetector)->getZPositionTotal() << " detector: " << (*det).first << " z: " << theGeometry->getDetector((*det).first)->getZPositionTotal() << endl;
        if ( theGeometry->getDetector((*det).first)->getZPositionTotal() < theGeometry->getDetector(firstDetector)->getZPositionTotal() )
            firstDetector = (*det).first;
        if ( theGeometry->getDetector((*det).first)->getZPositionTotal() > theGeometry->getDetector(lastDetector)->getZPositionTotal()  )
            lastDetector  = (*det).first;
        //cout << __PRETTY_FUNCTION__ << "Hits: " << clusters[(*det).first].size() << endl;
    }
    if (firstDetector == lastDetector) return ;

    //ss_.str("");
    //ss_ << "clustersMap size " << alignedClusters.size();
    //ss_ << " aClusterMap size " << clusters[firstDetector].size();
    //STDLINE(ss_.str(),ACCyan);

    //std::vector<Event::alignedHitsCandidateMapDef> alignedHitsCandidates;
    Event::aClusterMapDef& aClusterMapFirst = alignedClusters[firstDetector];
    Event::aClusterMapDef& aClusterMapLast  = alignedClusters[lastDetector];
    for (Event::aClusterMapDef::iterator fc=aClusterMapFirst.begin(); fc!=aClusterMapFirst.end(); fc++)
    {
        double y1 = fc->second["y"];
        double x1 = fc->second["x"];
        double z1 = fc->second["z"];
        for (Event::aClusterMapDef::iterator lc=aClusterMapLast.begin(); lc!=aClusterMapLast.end(); lc++)
        {
            double y2 = lc->second["y"];
            double x2 = lc->second["x"];
            double z2 = lc->second["z"];
            unsigned int hitsLost = 0;
            alignedHitsCandidate.clear() ;
            trackFitter::SV4Def lineThroughFirstAndLast((x2-x1)/(z2-z1),(z2*x1-x2*z1)/(z2-z1),(y2-y1)/(z2-z1),(z2*y1-y2*z1)/(z2-z1));
            for(Event::clustersMapDef::iterator dIt=alignedClusters.begin(); dIt!=alignedClusters.end(); ++dIt)
            {
                bool   hitFound  = false;
                double bestDist2 = 0xffffffff;
                if ( dIt->first == lastDetector || dIt->first == firstDetector)
                    continue;
                for (Event::aClusterMapDef::iterator cluster=dIt->second.begin(); cluster!=dIt->second.end(); cluster++)
                {
                    double y = cluster->second["y"]   ;
                    double x = cluster->second["x"]   ;

                    double predX, predY, predZ;
                    theGeometry->getDetector(dIt->first)->getPredictedGlobal(lineThroughFirstAndLast,predX,predY,predZ);
                    double resX = x - predX;
                    double resY = y - predY;
                    double dist2 = resX*resX + resY*resY;

                    //is a hit?
                    if ( fabs( resX ) < xTol_ && fabs( resY ) < yTol_ )
                    {
                        if( !hitFound || (dist2 < bestDist2) )
                        {
                            //if (hitFound) STDLINE("got a new point!!",ACGreen)
                            alignedHitsCandidate[ dIt->first ] = cluster->second;
                            bestDist2 = dist2;
                            //ss_.str("");
                            //ss_ << "detector: " << dIt->first << " x: " << x << " y: " << y;
                            //STDLINE(ss_.str(),ACRed)
                            //std::cout << __PRETTY_FUNCTION__ << "Aligned hit added" << endl;
                        }
                        hitFound = true;
                    }
                }
                if ( !hitFound ) hitsLost++;
                if ( (theGeometry->getDetectorsNumber(true) - hitsLost ) < minPoints_ )
                {
                    //STDLINE("not enought points to make a track",ACRed)
                    break;
                }
            }
            //ss_.str("");
            //ss_ << "DUT num: "                  << theGeometry->getDetectorsNumber(true)
            //    << " hitsLost: "                << hitsLost
            //    << " hits found: "              << (theGeometry->getDetectorsNumber(true) - hitsLost)
            //    << " >= MINIMUM_TRACK_POINTS: " << minPoints_;
            //STDLINE(ss_.str(),ACGreen)

            if ( alignedHitsCandidate.size() >= (minPoints_-2) )
            {
                alignedHitsCandidate[ firstDetector ] = fc->second;
                alignedHitsCandidate[ lastDetector  ] = lc->second;

                //cout << __PRETTY_FUNCTION__ << "Track found for event number: " << theEvent->getTrigger() << endl;
                trackFitter::aFittedTrackDef aFittedTrack = theTrackFitter_->fitSingleTrack(alignedHitsCandidate, theGeometry);

                //                  ss_.str("");
                //                  ss_ << "chi2/DOF: " << (aFittedTrack.second);
                //                  STDLINE(ss_.str(),ACYellow)
                for(Event::clustersMapDef::iterator dIt=alignedClusters.begin(); dIt!=alignedClusters.end(); ++dIt)
                {
                    bool   hitFound  = false       ;
                    double bestDist2 = 0xffffffff;
                    if (alignedHitsCandidate.find(dIt->first) != alignedHitsCandidate.end() )
                        continue;
                    for (Event::aClusterMapDef::iterator cluster=dIt->second.begin(); cluster!=dIt->second.end(); cluster++)
                    {
                        double y    = cluster->second["y"]   ;
                        double x    = cluster->second["x"]   ;

                        double predX, predY, predZ;
                        theGeometry->getDetector(dIt->first)->getPredictedGlobal(aFittedTrack.first.first,predX,predY,predZ);
                        double resX = x - predX;
                        double resY = y - predY;
                        double dist2 = resX*resX + resY*resY;

                        //ss_.str("");
                        //ss_ << "resX: "  << fabs( resX ) << " < tolX: " << xTol_;
                        //ss_ << " resY: " << fabs( resY ) << " < tolY: " << yTol_;
                        //STDLINE(ss_.str(),ACPurple)

                        //is a hit?
                        if ( fabs( resX ) < xTol_ && fabs( resY ) < yTol_ )
                        {
                            if( !hitFound || (dist2 < bestDist2) )
                            {
                                //if (hitFound) STDLINE("got a new point!!",ACGreen)
                                alignedHitsCandidate[ dIt->first ] = cluster->second;
                                bestDist2 = dist2;
                                //ss_.str("");
                                //ss_ << "Found 1 more -> detector: " << dIt->first << " resX: " << resX << " resY: " << resY << " already have 1? " << hitFound;
                                //STDLINE(ss_.str(),ACRed)
                            }
                            hitFound = true;
                        }
                    }
                }
                alignedHitsCandidates.push_back(alignedHitsCandidate);
                /*
                aFittedTrack = theTrackFitter_->fitSingleTrack(alignedHitsCandidate, theGeometry);

                if ( aFittedTrack.second < chi2cut_ || (chi2cut_ < 0 && aFittedTrack.second < CHI2DOF_CUT) )
                {
                    trackCandidates_.push_back(alignedHitsCandidate);
                    tracksFitted_   .push_back(aFittedTrack.first.first);
                    covMat_         .push_back(aFittedTrack.first.second);
                    chi2_           .push_back(aFittedTrack.second);
                }
*/
            }
            //            }
        }

    }
    //cleanUpTracks(alignedHitsCandidates, alignedClusters, theEvent, theGeometry);
    //return trackCandidates_;
}

//================================================================
void trackFinder::findRoadSearchTrackCandidates(Event* theEvent, Geometry* theGeometry)
{
    //Event::trackCandidatesDef         &trackCandidates_ = theEvent->getTrackCandidates()      ;
    //Event::fittedTracksDef            &tracksFitted_    = theEvent->getFittedTracks()         ;
    //Event::fittedTracksCovarianceDef  &covMat_          = theEvent->getFittedTracksCovariance();
    //Event::chi2VectorDef              &chi2_            = theEvent->getFittedTracksChi2()     ;
    Event::clustersMapDef                          &clusters              = theEvent->getClusters()             ;
    Event::clustersMapDef                          &alignedClusters       = theEvent->getAlignedClusters()      ;
    std::vector<Event::alignedHitsCandidateMapDef> &alignedHitsCandidates = theEvent->getAlignedHitsCandidates();

    //cout << __PRETTY_FUNCTION__ << "Road track search" << endl;
    //cout << __PRETTY_FUNCTION__ << "Event number: " << theEvent->getTrigger() << endl;

    //trackCandidates_.clear();
    //tracksFitted_   .clear();
    //covMat_         .clear();
    //chi2_           .clear();

    alignedClusters .clear();
    alignedHitsCandidates.clear();

    //    std::cout << __PRETTY_FUNCTION__ << "---------------------------------------------------------------------" << std::endl;
    //    std::cout << __PRETTY_FUNCTION__ << "Event: " << theEvent->getTrigger() << std::endl;
    if ( clusters.size()==0 ) return;

    if( minPoints_ < MINIMUM_TRACK_POINTS )
        minPoints_ = MINIMUM_TRACK_POINTS;

    for(Event::clustersMapDef::iterator dIt=clusters.begin(); dIt!=clusters.end(); ++dIt)
        if(maxPlanePoints_ > 0 && dIt->second.size() > maxPlanePoints_)
            return;

    //Since all tracking is done in global coordinate first create alignedClusters from local clusters
    //Event::clustersMapDef alignedClusters;
    std::map<double,std::string> zSortedDetectorNames;
    for(Event::clustersMapDef::iterator dIt=clusters.begin(); dIt!=clusters.end(); ++dIt)
    {
        //cout << __PRETTY_FUNCTION__ << "Road Loop 1" << endl;
        if ( theGeometry->getDetector(dIt->first)->isDUT() ) continue;
        for (Event::aClusterMapDef::iterator cluster=dIt->second.begin(); cluster!=dIt->second.end(); cluster++)
        {
            //cout << __PRETTY_FUNCTION__ << "Road Loop 2" << endl;
            Event::aClusterDef& alignedCluster = alignedClusters[ dIt->first ][cluster->first];
            //cluster is a copy of the real clusters so I just change them to global coordinates
            alignedCluster["x"]      = cluster->second["x"]     ;
            alignedCluster["y"]      = cluster->second["y"]     ;
            alignedCluster["z"]      = 0;
            alignedCluster["xErr"]   = cluster->second["xErr"]  ;
            alignedCluster["yErr"]   = cluster->second["yErr"]  ;
            alignedCluster["zErr"]   = 0;
            alignedCluster["size"]   = cluster->second["size"]  ;
            alignedCluster["charge"] = cluster->second["charge"];
            alignedCluster["num"]    = cluster->first  ;
            alignedCluster["xyErr"]  = theGeometry->getDetector( dIt->first )->fromLocalToGlobal( &alignedCluster["x"], &alignedCluster["y"], &alignedCluster["z"], &alignedCluster["xErr"], &alignedCluster["yErr"], &alignedCluster["zErr"] ) ;
            zSortedDetectorNames[theGeometry->getDetector( dIt->first )->getZPositionTotal()] = dIt->first;
            //std::cout << __PRETTY_FUNCTION__ << "Cluster " <<  alignedCluster["num"] << " on station " << dIt->first << std::endl;
        }
    }

    bool allPlanes = true;
    for(unsigned int loop=0; loop<1; loop++)
    {
        //cout << __PRETTY_FUNCTION__ << "Road Loop 3" << endl;
        if( alignedClusters.size() < minPoints_ ) break;
        std::vector<std::string> orderedNames(alignedClusters.size(),"");
        unsigned int combinations = 1;
        unsigned int numberOfHits = 0;
        unsigned int pos = 0;
        for(std::map<double,std::string>::iterator it=zSortedDetectorNames.begin(); it!=zSortedDetectorNames.end(); it++)
        {
            //cout << __PRETTY_FUNCTION__ << "Road Loop 4" << endl;
            if(alignedClusters.find(it->second) != alignedClusters.end())
            {
                orderedNames[pos] = it->second;
                combinations *= alignedClusters[it->second].size();
                numberOfHits += alignedClusters[it->second].size();
                ++pos;
                //                std::cout << __PRETTY_FUNCTION__ << "Number of hits on plane: " << it->second << " = " << alignedClusters[it->second].size() << std::endl;
            }
        }
        //        std::cout << __PRETTY_FUNCTION__ << "Number of combinations: " << combinations << std::endl;
        //        std::cout << __PRETTY_FUNCTION__
        //                << "Number of planes with hits: " << alignedClusters.size()
        //                << " from a total of " << numberOfHits << " hits!" << std::endl;
        if(loop==1)
            allPlanes = false;
        //Bisogna considerare il caso in cui c'e' un hit random sul piano 0 e poi una traccia sugli altri!!!!
        //Bisogna fare il seeding anche da altri piani 0 va messo in un for sui piani
        //    for(unsigned int plane=0; plane < orderedNames.size(); plane++)
        //std::vector<Event::alignedHitsCandidateMapDef> alignedHitsCandidates;
        Event::alignedHitsCandidateMapDef alignedHitsCandidate;
        for(unsigned int plane=0; plane < 1; plane++)
        {
            //cout << __PRETTY_FUNCTION__ << "Road Loop 5" << endl;
            alignedHitsCandidate.clear();
            searchHits(alignedHitsCandidates, alignedHitsCandidate, alignedClusters, orderedNames, 0, theEvent, theGeometry, allPlanes);
        }
        //        std::cout << __PRETTY_FUNCTION__
        //                << "Number of tracks before cleaning: " << alignedHitsCandidates.size()
        //                << " from a total of " << numberOfHits << " hits!" << std::endl;

        //        int trackNumber = 0;
        //        for(std::vector<Event::alignedHitsCandidateMapDef>::iterator tracksIt=alignedHitsCandidates.begin(); tracksIt!=alignedHitsCandidates.end(); tracksIt++)
        //        {
        //            std::cout << __PRETTY_FUNCTION__ << "Track # " << trackNumber++ << " size: " << tracksIt->size() << std::endl;
        //            for(Event::alignedHitsCandidateMapDef::itera
        /*Event::trackCandidatesDef         &trackCandidates_ = theEvent->getTrackCandidates()      ;
        Event::fittedTracksDef            &tracksFitted_    = theEvent->getFittedTracks()         ;
        Event::fittedTracksCovarianceDef  &covMat_          = theEvent->getFittedTracksCovariance();
        Event::chi2VectorDef              &chi2_            = theEvent->getFittedTracksChi2()     ;*/
        //                std::cout << __PRETTY_FUNCTION__ << "Cluster " <<  it->second["num"] << " on station " << it->first << std::endl;
        //        }
        //        if(alignedHitsCandidates.size() == 0)
        //        {
        //            std::cout << __PRETTY_FUNCTION__ << "Event with 0 tracks!!!" << std::endl;
        //        }

        //fitSimpleTrackCandidates(theEvent, theGeometry);
        //cleanUpTracks(alignedHitsCandidates, alignedClusters, theEvent, theGeometry);
    }
    //    std::cout << __PRETTY_FUNCTION__ << "Number of tracks after cleaning:  " << trackCandidates_.size() << std::endl;
}

//================================================================
void trackFinder::fitKalmanTrackCandidates(Event* theEvent, Geometry* theGeometry)
{
  //Use simpleFit to establish initial estimates of track parameters
    fitSimpleTrackCandidates(theEvent, theGeometry);

  //Outputs of Simple Fit to iterate over
    Event::trackCandidatesDef        &trackCandidates = theEvent->getTrackCandidates()       ;
    Event::fittedTracksDef           &tracksFitted    = theEvent->getFittedTracks()          ;
    Event::fittedTracksCovarianceDef &covMat          = theEvent->getFittedTracksCovariance();
    Event::chi2VectorDef             &chi2            = theEvent->getFittedTracksChi2()      ;
    Event::clustersMapDef            &clusters        = theEvent->getClusters()              ;

  //Define iterators
    std::vector<Event::alignedHitsCandidateMapDef>::iterator trackCandidate = trackCandidates.begin();
    std::vector<Event::vectorDef>                 ::iterator track          = tracksFitted   .begin();
    std::vector<Event::matrixDef>                 ::iterator cov            = covMat         .begin();
    Event::chi2VectorDef                          ::iterator itChi2         = chi2           .begin();

    //cout << __PRETTY_FUNCTION__ << "Kalman Track Fit All" << endl;

  //Iterate over candidate tracks, fitted tracks, and corresponding covarience matrices
    for (; trackCandidate!=trackCandidates.end(); trackCandidate++, track++, cov++, itChi2++)
    {
        trackFitter::aFittedTrackDef aKalmanFittedTrack = theTrackFitter_->kalmanFitSingleTrack(*trackCandidate, *track, *cov, clusters, theGeometry);

        if ( aKalmanFittedTrack.second < chi2cut_ || (chi2cut_ < 0 && aKalmanFittedTrack.second < CHI2DOF_CUT) )
        {
            (*track)  = aKalmanFittedTrack.first.first ;
            (*cov)    = aKalmanFittedTrack.first.second;
            (*itChi2) = aKalmanFittedTrack.second      ;
        }
    }
}

//============================================================================
void trackFinder::fitSimpleTrackCandidates(Event* theEvent, Geometry* theGeometry)
{
    Event::trackCandidatesDef                      &trackCandidates_      = theEvent->getTrackCandidates()       ;
    Event::fittedTracksDef                         &tracksFitted_         = theEvent->getFittedTracks()          ;
    Event::fittedTracksCovarianceDef               &covMat_               = theEvent->getFittedTracksCovariance();
    Event::chi2VectorDef                           &chi2_                 = theEvent->getFittedTracksChi2()      ;

    std::vector<Event::alignedHitsCandidateMapDef>  &alignedHitsCandidates = theEvent->getAlignedHitsCandidates();
    Event::clustersMapDef                           &alignedClusters       = theEvent->getAlignedClusters();

    trackCandidates_.clear();
    tracksFitted_   .clear();
    covMat_         .clear();
    chi2_           .clear();

    //cout << __PRETTY_FUNCTION__ << "Simple Track Fit All" << endl;

    //cout << __PRETTY_FUNCTION__ << "AHC Size: " << alignedHitsCandidates.size() << endl;
    //cout << __PRETTY_FUNCTION__ << "AHC Size: " << alignedClusters.size() << endl;

    cleanUpTracks(alignedHitsCandidates, alignedClusters, theEvent, theGeometry);
}

//============================================================================
void trackFinder::searchHits(std::vector<Event::alignedHitsCandidateMapDef>& alignedHitsCandidates,
                             Event::alignedHitsCandidateMapDef& alignedHitsCandidate,
                             Event::clustersMapDef&             alignedClusters,
                             std::vector<std::string>&          orderedNames,
                             unsigned int                       currentDetector,
                             Event*                             theEvent,
                             Geometry*                          theGeometry,
                             bool                               allPlanes)
{
    //cout << "searchHits begin" << endl;
/*
    std::cout << "trackFinder::searchHits()\t";
    if(currentDetector < orderedNames.size())
        std::cout << "Det: " << orderedNames[currentDetector];

    std::cout << " number: " << currentDetector
            << " size: " << orderedNames.size()
            << " # hits: " << alignedHitsCandidate.size();
    if(alignedHitsCandidate.size() > 0)
        std::cout << " last hit: " << alignedHitsCandidate.begin()->first << " num: " << alignedHitsCandidate.begin()->second["num"];
    std::cout << std::endl;
*/
    //cout << "Current detector: " << currentDetector << " Size: " << orderedNames.size() << endl;
    if(currentDetector >= orderedNames.size())
    {
        //cout << "1" << endl;
        if(alignedHitsCandidate.size() >= minPoints_)
        {
            //cout << "2" << endl;
            alignedHitsCandidates.push_back(alignedHitsCandidate);
            //cout << "Track made for event number " << theEvent->getTrigger() << endl;
            //exit(0);
        }
        return;
    }
    std::string& currentDetectorName = orderedNames[currentDetector];
    Event::aClusterMapDef& detectorClusters = alignedClusters.find(currentDetectorName)->second;
    if(alignedHitsCandidate.size() == 0 || alignedHitsCandidate.size() == 1)
    {
        //cout << "3" << endl;
        for(Event::aClusterMapDef::iterator cluster=detectorClusters.begin(); cluster!=detectorClusters.end(); cluster++)
        {
            //cout << "Search Loop 1" << endl;
            //cout << "4" << endl;
            alignedHitsCandidate[currentDetectorName] = cluster->second;
            searchHits(alignedHitsCandidates, alignedHitsCandidate, alignedClusters, orderedNames, currentDetector+1, theEvent, theGeometry, allPlanes);
        }
    }
    else
    {
        //cout << "5" << endl;
        double predX, predY, predZ;
        if(alignedHitsCandidate.size() == 2)
        {
            //cout << "6" << endl;
            double x1 = alignedHitsCandidate.begin()->second["x"];
            double y1 = alignedHitsCandidate.begin()->second["y"];
            double z1 = alignedHitsCandidate.begin()->second["z"];
            double x2 = (++alignedHitsCandidate.begin())->second["x"];
            double y2 = (++alignedHitsCandidate.begin())->second["y"];
            double z2 = (++alignedHitsCandidate.begin())->second["z"];
            trackFitter::SV4Def lineThrough2Points((x2-x1)/(z2-z1),(z2*x1-x2*z1)/(z2-z1),(y2-y1)/(z2-z1),(z2*y1-y2*z1)/(z2-z1));
            theGeometry->getDetector(currentDetectorName)->getPredictedGlobal(lineThrough2Points,predX,predY,predZ);
        }
        else
        {
            //cout << "7" << endl;
            //cout << __PRETTY_FUNCTION__ << "Event number: " << theEvent->getTrigger() << endl;
            trackFitter::aFittedTrackDef aFittedTrack = theTrackFitter_->fitSingleTrack(alignedHitsCandidate, theGeometry);
            theGeometry->getDetector(currentDetectorName)->getPredictedGlobal(aFittedTrack.first.first,predX,predY,predZ);
        }

        for(Event::aClusterMapDef::iterator cluster=detectorClusters.begin(); cluster!=detectorClusters.end(); cluster++)
        {
            //cout << "8" << endl;
            if ( fabs( cluster->second["x"] - predX ) < xTol_ && fabs( cluster->second["y"] - predY ) < yTol_ )
            {
                //cout << "9" << endl;
                alignedHitsCandidate[currentDetectorName] = cluster->second;
            }
            else if(allPlanes)
            {    //cout << "10" << endl;
                continue;//If this is uncommented then a track is built only if it has all planes
            }
            //cout << "Search Loop 2" << endl;
            searchHits(alignedHitsCandidates, alignedHitsCandidate, alignedClusters, orderedNames, currentDetector+1, theEvent, theGeometry, allPlanes);
            Event::alignedHitsCandidateMapDef::iterator it = alignedHitsCandidate.find(currentDetectorName);
            if( it != alignedHitsCandidate.end())
            {
                //cout << "11" << endl;
                alignedHitsCandidate.erase(it);
            }
        }
    }
}

//============================================================================
void trackFinder::cleanUpTracks(std::vector<Event::alignedHitsCandidateMapDef>& alignedHitsCandidates,
                                Event::clustersMapDef& alignedClusters,
                                Event* theEvent,
                                Geometry* theGeometry)
{
    Event::trackCandidatesDef        &trackCandidates_ = theEvent->getTrackCandidates()       ;
    Event::fittedTracksDef           &tracksFitted_    = theEvent->getFittedTracks()          ;
    Event::fittedTracksCovarianceDef &covMat_          = theEvent->getFittedTracksCovariance();
    Event::chi2VectorDef             &chi2_            = theEvent->getFittedTracksChi2()      ;

    std::vector<Event::alignedHitsCandidateMapDef> alignedHitsCandidatesCopy = alignedHitsCandidates;
    Event::clustersMapDef alignedClustersCopy = alignedClusters;

    //cout << "Initial Size: " << alignedHitsCandidates.size() << " Copy Size: " << alignedHitsCandidatesCopy.size() << endl;

    //cout << "cleanTracks begin" << endl;

    /*
    if(alignedHitsCandidates.size() == 1 && (combinations == 1 || loop == 1))//No erasing necessary
    {
        trackFitter::aFittedTrackDef aFittedTrack = theTrackFitter_->fitSingleTrack(alignedHitsCandidates[0], theGeometry);

        if ( aFittedTrack.second < chi2cut_ || (chi2cut_ < 0 && aFittedTrack.second < CHI2DOF_CUT) )
        {
            trackCandidates_.push_back(alignedHitsCandidates[0]);
            tracksFitted_   .push_back(aFittedTrack.first.first);
            covMat_         .push_back(aFittedTrack.first.second);
            chi2_           .push_back(aFittedTrack.second);
        }
        break;
    }
*/
    //    else
    {
        while(alignedHitsCandidatesCopy.size() > 0)
        {
            unsigned int maxHits = 0;
            for(std::vector<Event::alignedHitsCandidateMapDef>::iterator tracksIt=alignedHitsCandidatesCopy.begin(); tracksIt!=alignedHitsCandidatesCopy.end(); tracksIt++)
            {
                //std::cout << __PRETTY_FUNCTION__ << "Track size: " << tracksIt->size() << std::endl;
                if(maxHits < tracksIt->size())
                    maxHits = tracksIt->size();
            }
            //                std::cout << __PRETTY_FUNCTION__ << "maxHits: " <<  maxHits << std::endl;
            std::map<double, unsigned int> maxHitsCandidates;
            unsigned int position = 0;
            for(std::vector<Event::alignedHitsCandidateMapDef>::iterator tracksIt=alignedHitsCandidatesCopy.begin(); tracksIt!=alignedHitsCandidatesCopy.end(); tracksIt++)
            {
                if(maxHits == tracksIt->size())
                {
                    //cout << "Event number: " << theEvent->getTrigger() << endl;
                    trackFitter::aFittedTrackDef aFittedTrack = theTrackFitter_->fitSingleTrack(*tracksIt, theGeometry);
                    trackCandidates_.push_back(*tracksIt);
                    tracksFitted_   .push_back(aFittedTrack.first.first);
                    covMat_         .push_back(aFittedTrack.first.second);
                    chi2_           .push_back(aFittedTrack.second);
                    position = trackCandidates_.size()-1;
                    if(maxHitsCandidates.find(chi2_[position]) == maxHitsCandidates.end())
                    {
                        maxHitsCandidates[chi2_[position]] = position;
                        //std::cout << __PRETTY_FUNCTION__ << "Hits: " << maxHits << "Chi2: " << chi2_[position] << " position # " << position << std::endl;
                    }
                    tracksIt = alignedHitsCandidatesCopy.erase(tracksIt)-1;
                }
            }
            unsigned int numberOfErasedHits;
            std::vector<bool> trackEraser(trackCandidates_.size(),false);
            for(std::map<double, unsigned int>::iterator tracksIt=maxHitsCandidates.begin(); tracksIt!=maxHitsCandidates.end(); tracksIt++)
            {
                numberOfErasedHits = 0;
                for(Event::alignedHitsCandidateMapDef::iterator it=trackCandidates_[tracksIt->second].begin(); it!=trackCandidates_[tracksIt->second].end();)
                {
                    Event::aClusterMapDef::iterator cluster;
                    if(alignedClustersCopy.find(it->first) != alignedClustersCopy.end() &&
                            (cluster = alignedClustersCopy[it->first].find((int)it->second["num"])) != alignedClustersCopy[it->first].end())
                    {
                        alignedClustersCopy[it->first].erase(cluster);
                        //                            std::cout << __PRETTY_FUNCTION__ << "Track hits on plane " << it->first
                        //                                    << " cluster # " << it->second["num"]
                        //                                    << " .Number of hits = " << trackCandidates_[tracksIt->second].size()
                        //                                    << std::endl;
                        if(alignedClustersCopy[it->first].size() == 0)
                        {
                            alignedClustersCopy.erase(it->first);
                        }
                        ++it;
                    }
                    else
                    {
                        //                           std::cout << __PRETTY_FUNCTION__ << "1 Erasing track cluster on " << it->first
                        //                                   << " cluster # " << it->second["num"]
                        //                                   << " .Number of hits left = " << trackCandidates_[tracksIt->second].size()-1
                        //                                   << std::endl;
                        trackCandidates_[tracksIt->second].erase(it++);
                        ++numberOfErasedHits;
                    }
                }
                if(numberOfErasedHits != 0)
                {
                    trackEraser[tracksIt->second] = true;
                    //std::cout << __PRETTY_FUNCTION__ << "Erased " << numberOfErasedHits << " hits!" << std::endl;
                }
            }
            for(unsigned int pos=0; pos<trackEraser.size(); pos++)
            {
                if(trackEraser[pos] == true)
                {
                    trackEraser     .erase(trackEraser.begin()+pos);
                    if ( trackCandidates_[pos].size() >= minPoints_ )
                    {
                        //                          std::cout << "reusing track with n hits: " << trackCandidates_[pos].size() << std::endl;
                        alignedHitsCandidatesCopy.push_back(*(trackCandidates_.begin()+pos));
                    }
                    //                      std::cout << __PRETTY_FUNCTION__ << "Erasing track" << std::endl;
                    trackCandidates_.erase(trackCandidates_.begin()+pos);
                    tracksFitted_   .erase(tracksFitted_.begin()+pos);
                    covMat_         .erase(covMat_.begin()+pos);
                    chi2_           .erase(chi2_.begin()+pos);
                    pos--;
                }
            }
            for(std::vector<Event::alignedHitsCandidateMapDef>::iterator tracksIt=alignedHitsCandidatesCopy.begin(); tracksIt!=alignedHitsCandidatesCopy.end(); tracksIt++)
            {
                for(Event::alignedHitsCandidateMapDef::iterator it=tracksIt->begin(); it!=tracksIt->end();)
                {
                    if(alignedClustersCopy.find(it->first) == alignedClustersCopy.end() ||
                            alignedClustersCopy[it->first].find((int)it->second["num"]) == alignedClustersCopy[it->first].end())
                    {
                        //                        std::cout << __PRETTY_FUNCTION__ << "2 Erasing track cluster on " << it->first
                        //                                << " cluster # " << it->second["num"]
                        //                                << " .Number of hits left = " << tracksIt->size()-1
                        //                                << std::endl;
                        tracksIt->erase(it++);
                    }
                    else
                    {
                        ++it;
                        //                        std::cout << __PRETTY_FUNCTION__ << "Still there" << it->first << " cluster # " << it->second["num"] << std::endl;
                    }
                }
                //                std::cout << __PRETTY_FUNCTION__ << "Track size: " << tracksIt->size() << std::endl;
                if(tracksIt->size() < minPoints_)
                    tracksIt = alignedHitsCandidatesCopy.erase(tracksIt)-1;
            }
        }
    }
    //    std::cout << "trackFinder::cleanUpTracks()" << "Chisqrcut: " << chi2cut_ << " track chisqr: " << chi2_[pos] << std::endl;
    //(chi2_[pos] < chi2cut_ || (chi2cut_ < 0 && chi2_[pos] < CHI2DOF_CUT))

    //    for(Event::trackCandidatesDef::iterator trackIt = trackCandidates_.begin(); trackIt != trackCandidates_.end(); trackIt++)
    //erasing tracks that don't pass the chisqr cut
    for(unsigned int pos=0; pos<trackCandidates_.size(); pos++)
    {
        if( (chi2cut_ < 0 && chi2_[pos] > CHI2DOF_CUT) || ( chi2cut_ >= 0 && chi2_[pos] > chi2cut_))
        {
            trackCandidates_.erase(trackCandidates_.begin()+pos);
            tracksFitted_   .erase(tracksFitted_.begin()+pos);
            covMat_         .erase(covMat_.begin()+pos);
            chi2_           .erase(chi2_.begin()+pos);
            pos--;
        }
    }
    //cout << "Final Size:   " << alignedHitsCandidates.size() << " Copy Size: " << alignedHitsCandidatesCopy.size() << endl;
}

//============================================================================
void trackFinder::findDUTCandidates(Event* theEvent, Geometry* theGeometry)
{
    Event::clustersMapDef            &clusters        = theEvent->getClusters()             ;
    Event::clustersHitsMapDef        &clusterHits     = theEvent->getClustersHits()         ;
    Event::trackCandidatesDef        &trackCandidates = theEvent->getTrackCandidates()      ;
    Event::fittedTracksDef           &tracks          = theEvent->getFittedTracks()         ;
    //    Event::fittedTrackCovarianceDef  &covMat          = theEvent->getFittedTrackCovariance();
    //    Event::residualsMapDef           &residuals       = theEvent->getFittedTrackResiduals() ;

    for(unsigned int tr=0; tr < tracks.size(); tr++)
    {
        for(Event::clustersMapDef::iterator dIt=clusters.begin(); dIt!=clusters.end(); ++dIt)
        {
            if ( !theGeometry->getDetector( dIt->first )->isDUT() ) continue;

            bool   hitFound = false       ;
            double bestDist2 = 0xffffffff;
            trackCandidates[tr].erase( (*dIt).first );

            for (Event::aClusterMapDef::iterator cluster=(*dIt).second.begin(); cluster!=(*dIt).second.end(); cluster++)
            {
                double       y       = (*cluster).second["y"];
                double       x       = (*cluster).second["x"];
                double       z       = 0;
                double       yErr    = (*cluster).second["yErr"];
                double       xErr    = (*cluster).second["xErr"];
                double       zErr    = 0;
                unsigned int size    = clusterHits[ (*dIt).first ][(*cluster).first].size();
                double       charge  = (*cluster).second["charge"];
                int          clusNum = (*cluster).first;

                double xyErr = theGeometry->getDetector( (*dIt).first )->fromLocalToGlobal( &x, &y, &z, &xErr, &yErr, &zErr ) ;
                double predX, predY, predZ;
                theGeometry->getDetector((*dIt).first)->getPredictedGlobal(tracks[tr],predX,predY,predZ);
                //double resX  = x - (z*tracks[tr][0] + tracks[tr][1]);
                //double resY  = y - (z*tracks[tr][2] + tracks[tr][3]);
                double resX  = x - predX;
                double resY  = y - predY;
                double dist2 = resX*resX + resY*resY;

                if ( ( xTol_ <= 0 || yTol_ <= 0 ) || ( fabs( resX ) < xTol_  && fabs( resY ) < yTol_ ) )
                {
                    if( !hitFound || (dist2 < bestDist2) )
                    {
                        trackCandidates[tr][ (*dIt).first ]["x"]      = x      ;
                        trackCandidates[tr][ (*dIt).first ]["y"]      = y      ;
                        trackCandidates[tr][ (*dIt).first ]["z"]      = z      ;
                        trackCandidates[tr][ (*dIt).first ]["xErr"]   = xErr   ;
                        trackCandidates[tr][ (*dIt).first ]["yErr"]   = yErr   ;
                        trackCandidates[tr][ (*dIt).first ]["zErr"]   = 0      ;
                        trackCandidates[tr][ (*dIt).first ]["xyErr"]  = xyErr  ;
                        trackCandidates[tr][ (*dIt).first ]["size"]   = size   ;
                        trackCandidates[tr][ (*dIt).first ]["charge"] = charge ;
                        trackCandidates[tr][ (*dIt).first ]["num"]    = clusNum;
                        bestDist2 = dist2;
                    }
                    hitFound = true;
                }
            }
        }
    }
}
//================================================================================
void trackFinder::execute(Event * theEvent, Geometry * theGeometry)
{
    (this->*subProcessOperation_)(theEvent,theGeometry);
}
//========================================================================================
void trackFinder::setTrackSearchParameters (double  xTol, double yTol, double chi2cut, double minPoints, double maxPlanePoints)
{
    xTol_           = xTol          ;
    yTol_           = yTol          ;
    chi2cut_        = chi2cut       ;
    minPoints_      = minPoints     ;
    maxPlanePoints_ = maxPlanePoints;
}

//========================================================================================
std::string  trackFinder::getLabel (void )
{
    return "Reconstructing tracks"      ;
}

//========================================================================================
void trackFinder::findAllFirstAndLast(Event* theEvent, Geometry* theGeometry)
{
    this->findFirstAndLastTrackCandidates(theEvent,theGeometry);
    this->findDUTCandidates              (theEvent,theGeometry);
}

//========================================================================================
void trackFinder::findAllRoadSearch(Event* theEvent, Geometry* theGeometry)
{
    this->findRoadSearchTrackCandidates(theEvent,theGeometry);
    this->findDUTCandidates            (theEvent,theGeometry);
}

//========================================================================================
void trackFinder::fitAllKalman(Event* theEvent, Geometry* theGeometry)
{
    this->fitKalmanTrackCandidates    (theEvent,theGeometry);
    this->findDUTCandidates           (theEvent,theGeometry);
}

//========================================================================================
void trackFinder::fitAllSimple(Event* theEvent, Geometry* theGeometry)
{
    this->fitSimpleTrackCandidates    (theEvent,theGeometry);
    this->findDUTCandidates           (theEvent,theGeometry);
}

//========================================================================================
std::string trackFinder::getPlaneID (int station, int plaquette)
{
    std::stringstream ss;
    ss << "Station: " << station << " - Plaq: " << plaquette;
    return ss.str();
}
