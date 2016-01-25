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
 
#include "alignedHitsCandidate.h"

#define MINIMUM_TRACK_POINTS 3
#define WINDOW               0.005

//ClassImp(alignedHitsCandidate)

alignedHitsCandidate::alignedHitsCandidate(HManager *theHManager)
{
    theHManager_ = theHManager;
}
//===============================================================================
alignedHitsCandidate::alignedHitsCandidate(Event *theEvent)
{
    theClustersMap_ = theEvent->getClusters();
}
//================================================================
int alignedHitsCandidate::findTrackCandidates(Event *theEvent, Geometry *theGeometry, std::string *histoTypeX, std::string *histoTypeY)
{
   std::map< std::string, TH1D*> histoDevX;
   std::map< std::string, TH1D*> histoDevY;
   Event::clustersMapDef &clusters = theEvent->getClusters();

   alignedHitsCandidateMapDef  alignedHitsCandidate ;
   if ( theEvent->getClusters().size()==0 ) return 0;
   //find first and last detector in space
   std::string firstDetector = clusters.begin()->first;
   std::string lastDetector  = clusters.begin()->first;
   for ( Event::clustersMapDef::iterator det=clusters.begin(); det!=clusters.end(); det++ )
   {
       if ( theGeometry->getDetector((*det).first)->getZposition() <
            theGeometry->getDetector(firstDetector)->getZposition() ) firstDetector = (*det).first;
       if ( theGeometry->getDetector((*det).first)->getZposition() >
            theGeometry->getDetector(lastDetector)->getZposition()  ) lastDetector  = (*det).first;
   }
   if (firstDetector == lastDetector) return 0;
   ss_.str("");
   ss_ << "clustersMap size " << clusters.size();
   ss_ << " aClusterMap size " << clusters[firstDetector].size();
   STDLINE(ss_.str(),ACCyan)
   Event::aClusterMapDef aClusterMapFirst = clusters[firstDetector];
   for (Event::aClusterMapDef::iterator fc=aClusterMapFirst.begin(); fc!=aClusterMapFirst.end(); fc++)
   {
       double y1    = (*fc).second["y"]   ;
       double x1    = (*fc).second["x"]   ;
       double z1    = 0                   ;
       double yErr1 = (*fc).second["yErr"];
       double xErr1 = (*fc).second["xErr"];
       double zErr1 = 0                   ;
       theGeometry->getDetector( firstDetector )->fromDetectorToGlobal( &x1, &y1, &z1, &xErr1, &yErr1, &zErr1) ;

       ss_.str("");
       ss_ << "first detector is: " << firstDetector << " point x: " << x1 << " y: " << y1 << " z: " << z1;
       STDLINE(ss_.str(),ACBlue)
       Event::aClusterMapDef aClusterMapLast = clusters[lastDetector];
       for (Event::aClusterMapDef::iterator lc=aClusterMapLast.begin(); lc!=aClusterMapLast.end(); lc++)
       {
           double y2    = (*lc).second["y"]   ;
           double x2    = (*lc).second["x"]   ;
           double z2    = 0                   ;
           double yErr2 = (*lc).second["yErr"];
           double xErr2 = (*lc).second["xErr"];
           double zErr2 = 0                   ;
           theGeometry->getDetector( lastDetector )->fromDetectorToGlobal( &x2, &y2, &z2, &xErr2, &yErr2, &zErr2) ;

           ss_.str("");
           ss_ << "last detector is: " << lastDetector << " point x: " << x2 << " y: " << y2 << " z: " << z2;
           STDLINE(ss_.str(),ACBlue)
           ss_.str("");
           ss_ << " acceptance error is x: " << theGeometry->getDetector(lastDetector)->getXpositionErr()*5;
           ss_ << " y: " << theGeometry->getDetector(lastDetector)->getYpositionErr()*5;
           STDLINE(ss_.str(),ACBlue)
           if( fabs(x1-x2) < theGeometry->getDetector(lastDetector)->getXpositionErr()*5 &&
               fabs(y1-y2) < theGeometry->getDetector(lastDetector)->getYpositionErr()*5 )
           {
              STDLINE("got track seed!!",ACYellow)
              unsigned int hitsLost = 0;
              alignedHitsCandidate.clear() ;
              for(Event::clustersMapDef::iterator dIt=clusters.begin(); dIt!=clusters.end(); ++dIt)
              {
                  bool hitFound = false        ;
                  for (Event::aClusterMapDef::iterator cluster=(*dIt).second.begin(); cluster!=(*dIt).second.end(); cluster++)
                  {
                     double y    = (*cluster).second["y"]   ;
                     double x    = (*cluster).second["x"]   ;
                     double z    = 0                        ;
                     double yErr = (*cluster).second["yErr"];
                     double xErr = (*cluster).second["xErr"];
                     double zErr = 0                        ;
                     theGeometry->getDetector( (*dIt).first )->fromDetectorToGlobal( &x, &y, &z, &xErr, &yErr, &zErr ) ;

                     if ( histoDevX.count((*dIt).first)==0 )//if he doesn't find the plaq histogram in the map he create it
                     {
                        histoDevX[(*dIt).first] =  new TH1D((*dIt).first.c_str(),(*dIt).first.c_str(),2000,-10,+10);
                        histoDevX[(*dIt).first]->SetDirectory(0);
                        *histoTypeX = theHManager_->storeCustomTObject( histoDevX[(*dIt).first], "Xresiduals" );
                        histoDevY[(*dIt).first] =  new TH1D((*dIt).first.c_str(),(*dIt).first.c_str(),2000,-10,+10);
                        histoDevY[(*dIt).first]->SetDirectory(0);
                        *histoTypeY = theHManager_->storeCustomTObject( histoDevY[(*dIt).first], "Yresiduals" );
                        STDLINE("Filling " + (*dIt).first,ACWhite);
                     }
                     if ((*dIt).first==lastDetector)
                     {
                        histoDevX[(*dIt).first]->Fill( 9 );
                        histoDevY[(*dIt).first]->Fill( 9 );
                     }
                     else if ((*dIt).first==firstDetector)
                     {
                        histoDevX[(*dIt).first]->Fill( -9 );
                        histoDevY[(*dIt).first]->Fill( -9 );
                     }
                     else
                     {
                        histoDevX[(*dIt).first]->Fill( x-(x1+(x2-x1)*(z-z1)/(z1-z2)));
                        histoDevY[(*dIt).first]->Fill( x-(x1+(x2-x1)*(z-z1)/(z1-z2)));
                     }

                     if ( fabs( x-(x1+(x2-x1)*(z-z1)/(z1-z2)) ) < theGeometry->getDetector((*dIt).first)->getXpositionErr()*5 &&
                          fabs( y-(y1+(y2-y1)*(z-z1)/(z1-z2)) ) < theGeometry->getDetector((*dIt).first)->getYpositionErr()*5   )
                     {
                         STDLINE("got a point!!",ACGreen)
                         alignedHitsCandidate[ (*dIt).first ]["x"]    = x    ;
                         alignedHitsCandidate[ (*dIt).first ]["y"]    = y    ;
                         alignedHitsCandidate[ (*dIt).first ]["z"]    = z    ;
                         alignedHitsCandidate[ (*dIt).first ]["xErr"] = xErr ;
                         alignedHitsCandidate[ (*dIt).first ]["yErr"] = yErr ;
                         alignedHitsCandidate[ (*dIt).first ]["zErr"] = 0    ;
                         hitFound = true;
                     }
                  }
                  if ( !hitFound ) hitsLost++;
                  if ( hitsLost > ( clusters.size() - MINIMUM_TRACK_POINTS ) )
                  {
                      STDLINE("not enought points to make a track",ACRed)
                      break;
                  }
              }
              if ( hitsLost < ( clusters.size() - MINIMUM_TRACK_POINTS ) )
                                          trackCandidates_.push_back(alignedHitsCandidate);
           }
       }
   }
   return trackCandidates_.size();
}
//================================================================
int alignedHitsCandidate::size(void)
{
    return -1;
}
//================================================================
alignedHitsCandidate::alignedHitsCandidateMapDef alignedHitsCandidate::getAlignedCombination(std::string exludedPlaneId)
{
   if( trackCandidates_.size()==0 ) exit(0);
   trackCandidatesDef trackCandidates = trackCandidates_;
   alignedHitsCandidateMapDef::iterator it = trackCandidates[0].find(exludedPlaneId);
   STDLINE("before erasing",ACYellow)
   trackCandidates[0].erase(it)               ;
   STDLINE("after erasing",ACBlue)
   return trackCandidates[0];
}
