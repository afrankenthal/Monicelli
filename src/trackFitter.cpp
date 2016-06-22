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
 
#include "trackFitter.h"
#include <TVectorT.h>
#include <TMatrixTSym.h>

//===============================================================================
trackFitter::trackFitter(void)
{
    count=0;
    STDLINE("Empty constructor",ACWhite);
    debug_        = false;
    tracksFitted_ = false ;
    nIterations_  = 0;
}

//===============================================================================
Event::fittedTracksDef trackFitter::fitTracks(const Event::trackCandidatesDef&  tracks, Geometry *theGeometry, std::string excludedDetector )
{
    fittedTracks_.clear();
    chi2_.clear()        ;
    covMat_.clear()      ;

    for (unsigned int i=0; i < tracks.size(); ++i)
    {
        ss_.str("");
        ss_ << "Track n " << i << " - Excluded detector: " << excludedDetector;
        if(debug_) STDLINE(ss_.str(),ACGreen);
        trackFitter::aFittedTrackDef aFittedTrack = this->fitSingleTrack(tracks[i], theGeometry, excludedDetector);
        fittedTracks_.push_back( aFittedTrack.first.first  );
        covMat_.push_back( aFittedTrack.first.second )      ;
        std::cout << __PRETTY_FUNCTION__ << covMat_[1](0,0) << std::endl;
        chi2_.push_back( aFittedTrack.second )              ;

        if( excludedDetectorFound_ ) this->makeDetectorTrackResiduals(aFittedTrack.first.first, aFittedTrack.first.second,
                                                                      clusters_               , tracks                   ,
                                                                      theGeometry             , excludedDetector         , i);
        //assert(0);
        //if ( i+1 == tracks.size() ) count++;
    }

    tracksFitted_ = true ;

    return fittedTracks_;
}

//===============================================================================
trackFitter::aFittedTrackDef trackFitter::fitSingleTrack(const Event::alignedHitsCandidateMapDef& alignedHits, Geometry* theGeometry, std::string excludedDetector )
{
    ROOT::Math::SMatrix<double,4,4> AtVA      ;
    ROOT::Math::SMatrix<double,4,4> AtVAInv   ;
    ROOT::Math::SMatrix<double,2,2> sigmaXY   ;
    ROOT::Math::SMatrix<double,2,2> sigmaXYInv;
    ROOT::Math::SVector<double,4>   AtVxy     ;
    ROOT::Math::SVector<double,4>   pars      ;

    double chi2=0;
    excludedDetectorFound_ = false;

    //cout << __PRETTY_FUNCTION__ << "Simple Track Fit" << endl;

    for(Event::alignedHitsCandidateMapDef::const_iterator tr=alignedHits.begin(); tr!=alignedHits.end(); tr++)
    {
        if( theGeometry->getDetector( (*tr).first )->isDUT() ) continue;

        if( (*tr).first == excludedDetector )
        {
            excludedDetectorFound_ = true;
            continue;
        }

        Event::aClusterDef hit = tr->second;
        double x     = hit["x"];
        double y     = hit["y"];
        double z     = hit["z"];
        sigmaXY(0,0) = hit["xErr"] * hit["xErr"];
        sigmaXY(0,1) = hit["xyErr"];
//        sigmaXY(0,1) = 0;
        sigmaXY(1,0) = hit["xyErr"];
//        sigmaXY(1,0) = 0;
        sigmaXY(1,1) = hit["yErr"] * hit["yErr"];

        int ifail;
        sigmaXYInv = sigmaXY.Inverse(ifail);

        // AtVA
        for(int r=0; r<4; r++)
            for(int c=0; c<4; c++)
                AtVA(r,c) += sigmaXYInv(r/2,c/2)*pow(z,(2-(r%2+c%2)));

        // AtVxy
        for(int c=0; c<4; c++)
            AtVxy[c] += pow(z,1-c%2)*(x*sigmaXYInv(0,c/2)+y*sigmaXYInv(c/2,1));

        if(debug_)
        {
            ss_.str("");
            ss_ << "Filling matrix";
            //STDLINE(ss_.str(),ACYellow);

            ss_.str("");
            ss_ << "Detector "  << tr->first;
            ss_ << ": Rotation alpha: " << -(theGeometry->getDetector( (*tr).first )->getXRotation() + theGeometry->getDetector( (*tr).first )->getXRotationCorrection())
                << " - beta: "  << -(theGeometry->getDetector( (*tr).first )->getYRotation() + theGeometry->getDetector( (*tr).first )->getYRotationCorrection())
                << " - gamma: " << -(theGeometry->getDetector( (*tr).first )->getZRotationCorrection());
            STDLINE(ss_.str(),ACRed);


            ss_.str("");
            ss_ << "x prime: "  << clusters_[(*tr).first][(int)hit["cluster ID"]]["x"] - theGeometry->getDetector( (*tr).first )->getXPosition()
                << "+-" << clusters_[(*tr).first][(int)hit["cluster ID"]]["xErr"]
                << " y prime: " << clusters_[(*tr).first][(int)hit["cluster ID"]]["y"] - theGeometry->getDetector( (*tr).first )->getYPosition()
                << "+-" << clusters_[(*tr).first][(int)hit["cluster ID"]]["yErr"]
                << " z center: " << theGeometry->getDetector( (*tr).first )->getZPositionTotal();
            STDLINE(ss_.str(),ACPurple);

            ss_.str("");
            ss_ << "x: "   << hit["x"] << "+-" << hit["xErr"]
                << " y: "  << hit["y"] << "+-" << hit["yErr"]
                << " z: "  << hit["z"];
            STDLINE(ss_.str(),ACPurple);
        }
    }

    int fail = 0;
    AtVAInv = AtVA.Inverse(fail);
    if ( fail )
    {
        STDLINE("WARNING: Matrix inversion failed!",ACRed);

        printMatrix("AtVA: ", AtVA);
        double det;

        AtVA.Det2(det);
        ss_.str("");
        ss_ << "det: " << std::setprecision(15) << det
               ;
        STDLINE(ss_.str(),ACRed);

        printMatrix("AtVAInv: ",AtVAInv);

        //assert(0);
        ss_.str("");
        //ss_ << "faild count " << count++;
        //STDLINE(ss_.str(),ACPurple);

        //continue;
    }

    pars = AtVAInv*AtVxy;

    ss_.str("");
    ss_ << "alignedHits.size: " << alignedHits.size();
    std::map<std::string, std::pair<double, double> > resMap; //residuals for each plane used for successive iterations
    double xPredLoc, yPredLoc ;
    double xHitLoc, yHitLoc, zHitLoc;
    double xErrLoc, yErrLoc, zErrLoc;
    for(Event::alignedHitsCandidateMapDef::const_iterator tr=alignedHits.begin(); tr!=alignedHits.end(); tr++)
    {
        if( theGeometry->getDetector( tr->first )->isDUT() )  continue;
        if( tr->first == excludedDetector )                   continue;

        Event::aClusterDef hit = tr->second;

        //global coordinates
        chi2 += pow( hit["x"] - pars[0]*hit["z"] - pars[1], 2 )/(hit["xErr"]*hit["xErr"]) +
                pow( hit["y"] - pars[2]*hit["z"] - pars[3], 2 )/(hit["yErr"]*hit["yErr"])  ;

       //local coordinates
/*       xPredLoc = pars[0]*hit["z"] + pars[1];
       yPredLoc = pars[2]*hit["z"] + pars[3];
       zPredLoc = theGeometry->getDetector( tr->first )->getZPosition();
       theGeometry->getDetector( tr->first )->fromGlobalToLocal(&xPredLoc, &yPredLoc, &zPredLoc);
       xHitLoc = hit["x"];
       yHitLoc = hit["y"];
       zHitLoc = hit["z"];
       xErrLoc = hit["xErr"];
       yErrLoc = hit["yErr"];
       zErrLoc = hit["zErr"];
       theGeometry->getDetector( tr->first )->fromGlobalToLocal(&xHitLoc, &yHitLoc, &zHitLoc, &xErrLoc, &yErrLoc, &zErrLoc);
       chi2 += pow(xHitLoc - xPredLoc, 2 )/(xErrLoc*xErrLoc) +
               pow(yHitLoc - yPredLoc, 2 )/(yErrLoc*yErrLoc)  ;
*/
        if (nIterations_ == 0)
            continue;

//        xPredLoc = pars[0]*hit["z"] + pars[1];
//        yPredLoc = pars[2]*hit["z"] + pars[3];
        theGeometry->getDetector(tr->first)->getPredictedLocal(pars, xPredLoc, yPredLoc);
//        zPredLoc = theGeometry->getDetector( tr->first )->getZPosition();
//        theGeometry->getDetector( tr->first )->fromGlobalToLocal(&xPredLoc, &yPredLoc, &zPredLoc);
        xHitLoc = hit["x"];
        yHitLoc = hit["y"];
        zHitLoc = hit["z"];
        xErrLoc = hit["xErr"];
        yErrLoc = hit["yErr"];
        zErrLoc = hit["zErr"];
        theGeometry->getDetector( tr->first )->fromGlobalToLocal(&xHitLoc, &yHitLoc, &zHitLoc, &xErrLoc, &yErrLoc, &zErrLoc);
        if (theGeometry->getDetectorModule(tr->first)%2 == 0)
            resMap[tr->first] = std::make_pair(xHitLoc - xPredLoc, xErrLoc); // ToROOT6
        else
            resMap[tr->first] = std::make_pair(yHitLoc - yPredLoc, yErrLoc); // ToROOT6
    }

    for (int iter = 0; iter < nIterations_; ++iter)
    {
        ROOT::Math::SVector<double,4> corrections = calculateParCorrections(pars, theGeometry, resMap);
        pars += corrections;

        chi2 = 0;

        for(Event::alignedHitsCandidateMapDef::const_iterator tr=alignedHits.begin(); tr!=alignedHits.end(); tr++)
        {
            if( theGeometry->getDetector( tr->first )->isDUT() )  continue;
            if( tr->first == excludedDetector )                   continue;

            Event::aClusterDef hit = tr->second;

            //global coordinates
            chi2 += pow( hit["x"] - pars[0]*hit["z"] - pars[1], 2 )/(hit["xErr"]*hit["xErr"]) +
                    pow( hit["y"] - pars[2]*hit["z"] - pars[3], 2 )/(hit["yErr"]*hit["yErr"])  ;

//            if (iter == nIterations_-1)
//                continue;

//            xPredLoc = pars[0]*hit["z"] + pars[1];
//            yPredLoc = pars[2]*hit["z"] + pars[3];
            theGeometry->getDetector(tr->first)->getPredictedLocal(pars, xPredLoc, yPredLoc);
//            zPredLoc = theGeometry->getDetector( tr->first )->getZPosition();
//            theGeometry->getDetector( tr->first )->fromGlobalToLocal(&xPredLoc, &yPredLoc, &zPredLoc);
            xHitLoc = hit["x"];
            yHitLoc = hit["y"];
            zHitLoc = hit["z"];
            xErrLoc = hit["xErr"];
            yErrLoc = hit["yErr"];
            zErrLoc = hit["zErr"];
            theGeometry->getDetector( tr->first )->fromGlobalToLocal(&xHitLoc, &yHitLoc, &zHitLoc, &xErrLoc, &yErrLoc, &zErrLoc);
            if (theGeometry->getDetectorModule(tr->first)%2 == 0)
                resMap[tr->first] = std::make_pair(xHitLoc - xPredLoc, xErrLoc); // ToROOT6
            else
                resMap[tr->first] = std::make_pair(yHitLoc - yPredLoc, yErrLoc); // ToROOT6
        }
    }

    if(debug_)
    {
        ss_.str("");
        ss_ << " chi2: " << chi2;
        STDLINE(ss_.str(),ACYellow);

        ss_.str("");
        ss_ << "Fit Results sx: "  << pars[0]
            << " qx: " << pars[1]
            << " sy: " << pars[2]
            << " qy: " << pars[3];
        STDLINE(ss_.str(), ACYellow);
    }

    //cout << __PRETTY_FUNCTION__ << "Final: x int = " << pars[1] << " y int = " << pars[3] << " x slope = " << pars[0] << " y slope = " << pars[2] << endl;

    trackFitter::aFittedTrackDef aFittedTrack;
    aFittedTrack.first.first = pars          ;
    aFittedTrack.first.second= AtVAInv         ;
    aFittedTrack.second      = chi2/(alignedHits.size()*2 - 4)          ; //pixels
//    aFittedTrack.second      = chi2/(alignedHits.size() - 2)          ; //strips

    return aFittedTrack;
}

//===============================================================================
trackFitter::aFittedTrackDef trackFitter::kalmanFitSingleTrack(const Event::alignedHitsCandidateMapDef& trackCandidate, Event::vectorDef& track, Event::matrixDef& cov, Event::clustersMapDef& clusters, Geometry* theGeometry, std::string excludedDetector )
{
    excludedDetectorFound_ = false;
    double chi2 = 0;

    //cout << __PRETTY_FUNCTION__ << "Kalman Track Fit" << endl;

  //Use existing trackPars and covMat
    TVectorT<double> trackPars(4);
    TMatrixTSym<double> estCov(4);
    for ( int i=0; i<4; i++ )
    {
        trackPars[i] = track[i];
    }
    for ( int i=0; i<4; i++ )
    {
        for ( int j=0; j<4; j++ )
        {
            estCov[i][j] = cov[i][j];
        }
    }
    /*estCov[0][0] = 1e-8;
    estCov[1][1] = 100;
    estCov[2][2] = 1e-8;
    estCov[3][3] = 100;*/

    //cout << __PRETTY_FUNCTION__ << "Initial: x int = " << trackPars[1] << " y int = " << trackPars[3] << " x slope = " << trackPars[0] << " y slope = " << trackPars[2] << endl;
    //cout << __PRETTY_FUNCTION__ << "Initial Chi 2: " << chi2 << endl;
    for ( int i=0; i<4; i++ )
    {
        //std::cout << __PRETTY_FUNCTION__ << "Initial Track par " << i << ": " << fitpar[i] << std::endl;
        //std::cout << __PRETTY_FUNCTION__ << "Initial covMat line " << i << ": " << estCov[i][0] << " "<< estCov[i][1] << " "<<estCov[i][2] << " "<< estCov[i][3] <<std::endl;
    }
    //std::cout << __PRETTY_FUNCTION__ << "Initial covMat " << ": " << estCov[0][0] << " "<< estCov[1][1] << " "<<estCov[2][2] << " "<< estCov[3][3] <<std::endl;

  //Change loop so it runs in order of z instead of plaqID
    std::map<double, std::string> plaqByZ;
    for (Event::alignedHitsCandidateMapDef::const_iterator itTrackCandidate=trackCandidate.begin(); itTrackCandidate!=trackCandidate.end(); itTrackCandidate++)
    {
        std::string plaqID = itTrackCandidate->first;
        //double x = itTrackCandidate->second.find("x")->second;
        //double y = itTrackCandidate->second.find("y")->second;
        double z = itTrackCandidate->second.find("z")->second;

        //Detector* detector = theGeometry->getDetector(plaqID);
        //detector->fromLocalToGlobal(&x, &y, &z);

        plaqByZ[z] = plaqID;
    }

    double firstZ = plaqByZ.begin()->first;
    //cout << __PRETTY_FUNCTION__ << "firstZ: " << firstZ << endl;

  //Now loop over all plaqs
    //std::map<double, string>::iterator itZ=plaqByZ.begin();
    //for (std::map<double, std::string>::reverse_iterator itZ=plaqByZ.rbegin(); itZ!=plaqByZ.rend(); itZ++)
    for (std::map<double, std::string>::iterator itZ=plaqByZ.begin(); itZ!=plaqByZ.end(); itZ++)
    {
        std::string plaqID = itZ->second;
        double zValue = itZ->first;
        //double zValue = clusters[plaqID][trackCandidate.find(plaqID)->second.find("cluster ID")->second].find("z")->second;
        //double xValue = clusters[plaqID][trackCandidate.find(plaqID)->second.find("cluster ID")->second].find("x")->second;
        //double yValue = clusters[plaqID][trackCandidate.find(plaqID)->second.find("cluster ID")->second].find("y")->second;
        //cout << __PRETTY_FUNCTION__ << " x: " << xValue << endl;
        //cout << __PRETTY_FUNCTION__ << " y: " << yValue << endl;
        //cout << __PRETTY_FUNCTION__ << "plaq: " << plaqID << " z: " << itZ->first << endl;

        if( theGeometry->getDetector( plaqID )->isDUT() ) continue;

      //Use Kalman fit method to improve fit

      //Define variables
        Detector* detector = theGeometry->getDetector(plaqID);
        TVectorT<double> sensorOrigin(4);
        TVectorT<double> upVector(4);
        TVectorT<double> rightVector(4);
        TVectorT<double> beamVector(4);
        TVectorT<double> h(4);
        TVectorT<double> hx(4);
        TVectorT<double> hy(4);
        TVectorT<double> k(4);
        TVectorT<double> kx(4);
        TVectorT<double> ky(4);
        TMatrixTSym<double> trackCov(4);
        TMatrixTSym<double> trackCovx(4);
        TMatrixTSym<double> trackCovy(4);
        TMatrixTSym<double> a(4);
        TMatrixTSym<double> ax(4);
        TMatrixTSym<double> ay(4);
        double multipleScattering = 0;//4.37e-6;
        double dataType = trackCandidate.find(plaqID)->second.find("dataType")->second;

      //Test residual
        //double xPred, yPred, zPred;
        //detector->getPredictedGlobal(track, xPred, yPred, zPred);
        //detector->fromGlobalToLocal(&xPred, &yPred, &zPred);
        //double resxTest = trackCandidate.find(plaqID)->second.find("x")->second - xPred;
        //double resyTest = trackCandidate.find(plaqID)->second.find("y")->second - yPred;

      //Define local coordinates
        sensorOrigin[0] = 0; sensorOrigin[1] = 0; sensorOrigin[2] = 0;
        upVector[0]     = 0; upVector[1]     = 1; upVector[2]     = 0;
        rightVector[0]  = 1; rightVector[1]  = 0; rightVector[2]  = 0;
        beamVector[0]   = 0; beamVector[1]   = 0; beamVector[2]   = 1;

      //Change to global coordinates
        detector->fromLocalToGlobal(&sensorOrigin[0], &sensorOrigin[1], &sensorOrigin[2]);
        detector->fromLocalToGlobal(&upVector[0],     &upVector[1],     &upVector[2]);
        detector->fromLocalToGlobal(&rightVector[0],  &rightVector[1],  &rightVector[2]);
        detector->fromLocalToGlobal(&beamVector[0],   &beamVector[1],   &beamVector[2]);

      //Get z value of point in global
        //detector->fromLocalToGlobal(&xValue, &yValue, &zValue);
        //cout << __PRETTY_FUNCTION__ << "zValue after global: " << zValue << endl;
        zValue -= firstZ;
        //cout << __PRETTY_FUNCTION__ << "zValue after subtraction: " << zValue << endl;
        //cout << __PRETTY_FUNCTION__ << "plaq: " << plaqID << " z: " << zValue << endl;// << " sensorOrigin: " << sensorOrigin[2] << endl;

      //Normalize vectors
        upVector[0]    -= sensorOrigin[0]; upVector[1]    -= sensorOrigin[1]; upVector[2]    -= sensorOrigin[2];
        rightVector[0] -= sensorOrigin[0]; rightVector[1] -= sensorOrigin[1]; rightVector[2] -= sensorOrigin[2];
        beamVector[0]  -= sensorOrigin[0]; beamVector[1]  -= sensorOrigin[1]; beamVector[2]  -= sensorOrigin[2];


      //"Compute"
        //Uses 'our' element assignment. Inlcludes xy correlation for pixels.
        if ( dataType==1 ) //strip data
        {
            double den = upVector[1]*rightVector[0]-upVector[0]*rightVector[1];
            double offset = -sensorOrigin[0]*rightVector[0] - sensorOrigin[1]*rightVector[1]+
                      rightVector[2]*(-sensorOrigin[2]+(sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
                      sensorOrigin[1]*(upVector[0]*rightVector[2]-upVector[2]*rightVector[0])+
                      sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))/den);
            h[1] = rightVector[0]+rightVector[2]*(upVector[1]*rightVector[2]-upVector[2]*rightVector[1])/den;
            h[3] = rightVector[1]+rightVector[2]*(upVector[2]*rightVector[0]-upVector[0]*rightVector[2])/den;
            h[0] =(sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
                   sensorOrigin[1]*(upVector[0]*rightVector[2]-rightVector[0]*upVector[2])+
                   sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))*
                  (upVector[1]*(rightVector[0]*rightVector[0]+rightVector[2]*rightVector[2])-
                   rightVector[1]*(upVector[0]*rightVector[0]+upVector[2]*rightVector[2]))/(den*den);
            h[2] =(sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
                   sensorOrigin[1]*(upVector[0]*rightVector[2]-upVector[2]*rightVector[0])+
                   sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))*
                  (rightVector[0]*(upVector[1]*rightVector[1]+upVector[2]*rightVector[2])-
                   upVector[0]*(rightVector[1]*rightVector[1]+rightVector[2]*rightVector[2]))/(den*den);

            /*trackCov[0][0] = zValue*zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
                        trackCov[1][0] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
                        trackCov[2][2] = zValue*zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
                        trackCov[3][2] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
                        trackCov[0][1] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
                        trackCov[1][1] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
                        trackCov[2][3] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
                        trackCov[3][3] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);*/

            trackCov[1][1] = zValue*zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[1][0] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[3][3] = zValue*zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[3][2] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[0][1] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[0][0] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[2][3] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[2][2] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);


            /*trackCov[1][1] = sensorOrigin[2]*sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[1][0] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[3][3] = sensorOrigin[2]*sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[3][2] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[0][1] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[0][0] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[2][3] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[2][2] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);*/

            for ( int i=0; i<4; i++ )
            {
                for ( int j=0; j<4; j++ )
                {
                    a[i][j] = h[i]*h[j];
                }
            }

          //Residual of prediction
            double dist    = clusters[plaqID][trackCandidate.find(plaqID)->second.find("cluster ID")->second].find("x")->second;//distance of the hit from origin of the sensor
            double distErr = clusters[plaqID][trackCandidate.find(plaqID)->second.find("cluster ID")->second].find("xErr")->second;
            double res = dist - trackPars*h - offset;

          //Calculate gain matrix
            double r = estCov.Similarity(h) + distErr*distErr;// h*c*h^t + sigma^2
            k = (1/r)*(estCov*h);
            estCov -= (1/r)*a.Similarity(estCov);// (1/rrr)*c*a*c^t
            //cout << __PRETTY_FUNCTION__ << "Strip chi addition:  " << (res*res/r) << endl;
            chi2 += (res*res/r)/(trackCandidate.size() - 2);
            //cout << __PRETTY_FUNCTION__ << "New Strip chi:  " << chi2 << endl;

            //cout << __PRETTY_FUNCTION__ << trackCandidate.size() << endl;
            //cout << __PRETTY_FUNCTION__ << "Strip:  " << res - resxTest << endl;
            //cout << __PRETTY_FUNCTION__ << "Strip:  " << res << endl;

            if( plaqID == excludedDetector )//don't update track parameters for the excluded plaq
            {
                excludedDetectorFound_ = true;
            }
            else
            {
                trackPars += res*k;
                for ( int i=0; i<4; i++ )
                {
                    for ( int j=0; j<4; j++ )
                    {
                        estCov[i][j] += trackCov[i][j];
                    }
                }

            }
            //std::cout << __PRETTY_FUNCTION__ << "Strip trackC " << ": " << trackCov[0][0] << " "<< trackCov[1][1] << " "<< trackCov[2][2] << " "<< trackCov[3][3] <<std::endl;
            //std::cout << __PRETTY_FUNCTION__ << "Strip covMat " << ": " << estCov[0][0] << " "<< estCov[1][1] << " "<<estCov[2][2] << " "<< estCov[3][3] <<std::endl;
        }
        else //not strip data (pixels)
        {
            continue;

            double den = upVector[1]*rightVector[0]-upVector[0]*rightVector[1];
            double offsetx = -sensorOrigin[0]*rightVector[0] - sensorOrigin[1]*rightVector[1]+
                      rightVector[2]*(-sensorOrigin[2]+(sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
                      sensorOrigin[1]*(upVector[0]*rightVector[2]-upVector[2]*rightVector[0])+
                      sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))/den);
            double offsety = -sensorOrigin[0]*upVector[0] - sensorOrigin[1]*upVector[1]+
                      upVector[2]*(-sensorOrigin[2]+(sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
                      sensorOrigin[1]*(upVector[0]*rightVector[2]-upVector[2]*rightVector[0])+
                      sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))/den);
            hx[1]  = rightVector[0]+rightVector[2]*(upVector[1]*rightVector[2]-upVector[2]*rightVector[1])/den;
            hy[1] = upVector[0]+upVector[2]*(upVector[1]*rightVector[2]-upVector[2]*rightVector[1])/den;
            hx[3]  = rightVector[1]+rightVector[2]*(upVector[2]*rightVector[0]-upVector[0]*rightVector[2])/den;
            hy[3] = upVector[1]+upVector[2]*(upVector[2]*rightVector[0]-upVector[0]*rightVector[2])/den;
            hx[0]  = (sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
                   sensorOrigin[1]*(upVector[0]*rightVector[2]-rightVector[0]*upVector[2])+
                   sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))*
                  (upVector[1]*(rightVector[0]*rightVector[0]+rightVector[2]*rightVector[2])-
                   rightVector[1]*(upVector[0]*rightVector[0]+upVector[2]*rightVector[2]))/(den*den);
            hy[0] = (-sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])
                     -sensorOrigin[1]*(upVector[0]*rightVector[2]-rightVector[0]*upVector[2])
                     -sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))*
                   (rightVector[1]*(upVector[0]*upVector[0]+upVector[2]*upVector[2])-
                    upVector[1]*(upVector[0]*rightVector[0]+upVector[2]*rightVector[2]))/(den*den);
            hx[2]  = (sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
                   sensorOrigin[1]*(upVector[0]*rightVector[2]-upVector[2]*rightVector[0])+
                   sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))*
                  (rightVector[0]*(upVector[1]*rightVector[1]+upVector[2]*rightVector[2])-
                   upVector[0]*(rightVector[1]*rightVector[1]+rightVector[2]*rightVector[2]))/(den*den);
            hy[2] = (-sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])
                    -sensorOrigin[1]*(upVector[0]*rightVector[2]-upVector[2]*rightVector[0])
                    -sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))*
                   (upVector[0]*(upVector[1]*rightVector[1]+upVector[2]*rightVector[2])-
                    rightVector[0]*(upVector[1]*upVector[1]+upVector[2]*upVector[2]))/(den*den);


            /*trackCovx[0][0] = zValue*zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[1][0] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[0][1] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[1][1] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);

            trackCovx[2][2] = zValue*zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[3][2] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[2][3] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[3][3] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);

            trackCovy[2][0] = zValue*zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[3][0] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[2][1] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[3][1] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);

            trackCovy[1][3] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[1][2] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[0][3] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[0][2] = zValue*zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);*/

            trackCovx[1][1] = zValue*zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[1][0] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[0][1] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[0][0] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);

            trackCovx[3][3] = zValue*zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[3][2] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[2][3] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[2][2] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);

            trackCovy[3][1] = zValue*zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[3][0] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[2][1] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[2][0] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);

            trackCovy[0][2] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[1][2] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[0][3] = -zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[1][3] = zValue*zValue*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);

            /*trackCovx[1][1] = sensorOrigin[2]*sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[1][0] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[0][1] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[0][0] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);

            trackCovx[3][3] = sensorOrigin[2]*sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[3][2] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[2][3] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovx[2][2] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);

            trackCovy[3][1] = sensorOrigin[2]*sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[3][0] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[2][1] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[2][0] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);

            trackCovy[0][2] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[1][2] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[0][3] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCovy[1][3] = sensorOrigin[2]*sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);*/

            //cout << __PRETTY_FUNCTION__ << trackCovy[1][3] << " " <<trackCovy[1][2] << " " <<trackCovy[2][0] << " " << endl;

            for ( int i=0; i<4; i++ )
            {
                for ( int j=0; j<4; j++ )
                {
                    ax[i][j] = hx[i]*hx[j];
                }
            }
            for ( int i=0; i<4; i++ )
            {
                for ( int j=0; j<4; j++ )
                {
                    ay[i][j] = hy[i]*hy[j];
                }
            }

          //Residual of prediction
            double x    = clusters[plaqID][trackCandidate.find(plaqID)->second.find("cluster ID")->second].find("x")->second;
            double xErr = clusters[plaqID][trackCandidate.find(plaqID)->second.find("cluster ID")->second].find("xErr")->second;
            double resx = x - trackPars*hx - offsetx;

            //Calculate gain matrix for x
            double rx = estCov.Similarity(hx) + xErr*xErr;
            kx = (1/rx)*(estCov*hx);
            estCov -= (1/rx)*ax.Similarity(estCov);
            //cout << __PRETTY_FUNCTION__ << "Pixel chi x addition:  " << (resx*resx/rx) << endl;
            chi2 += (resx*resx/rx)/(trackCandidate.size()*2 - 4);

            if( plaqID == excludedDetector )//don't update track parameters for the excluded plaq
            {
                    excludedDetectorFound_ = true;
            }
            else
            {
                trackPars += resx*kx;
                for ( int i=0; i<4; i++ )
                {
                    for ( int j=0; j<4; j++ )
                    {
                        estCov[i][j] += trackCovx[i][j];
                    }
                }

            }

            double y    = clusters[plaqID][trackCandidate.find(plaqID)->second.find("cluster ID")->second].find("y")->second;
            double yErr = clusters[plaqID][trackCandidate.find(plaqID)->second.find("cluster ID")->second].find("yErr")->second;
            double resy = y - trackPars*hy - offsety;

            //Calculate gain matrix for y
            double ry = estCov.Similarity(hy) + yErr*yErr;
            ky = (1/ry)*(estCov*hy);
            estCov -= (1/ry)*ay.Similarity(estCov);
            //cout << __PRETTY_FUNCTION__ << "Pixel chi y addition:  " << (resy*resy/ry) << endl;
            chi2 += (resy*resy/ry)/(trackCandidate.size()*2 - 4);

            if( plaqID == excludedDetector )//don't update track parameters for the excluded plaq
            {
                    excludedDetectorFound_ = true;
            }
            else
            {
                trackPars += resy*ky;
                for ( int i=0; i<4; i++ )
                {
                    for ( int j=0; j<4; j++ )
                    {
                        estCov[i][j] += trackCovy[i][j];
                    }
                }

            }

            //std::cout << __PRETTY_FUNCTION__ << "Pixel trackC " << ": " << trackCovx[0][0] << " "<< trackCovx[1][1] << " "<< trackCovx[2][2] << " "<< trackCovx[3][3] <<std::endl;
            //std::cout << __PRETTY_FUNCTION__ << "Pixel covMat " << ": " << estCov[0][0] << " "<< estCov[1][1] << " "<<estCov[2][2] << " "<< estCov[3][3] <<std::endl;
            //cout << __PRETTY_FUNCTION__ << "New Pixel chi2:  " << chi2 << endl;
            //cout << __PRETTY_FUNCTION__ << "Pixel:  " << resx - resxTest << " | " << resy - resyTest << endl;
            //cout << __PRETTY_FUNCTION__ << "Pixel:  " << resx << " | " << resy << endl;
        }

/*       //Uses 'our' element assignment. Inlcludes xy correlation for pixels.
        if ( dataType==1 )
        {
            double den = upVector[1]*rightVector[0]-upVector[0]*rightVector[1];
            offset = -sensorOrigin[0]*rightVector[0] - sensorOrigin[1]*rightVector[1]+
                      rightVector[2]*(-sensorOrigin[2]+(sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
                      sensorOrigin[1]*(upVector[0]*rightVector[2]-upVector[2]*rightVector[0])+
                      sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))/den);
            h[1] = rightVector[0]+rightVector[2]*(upVector[1]*rightVector[2]-upVector[2]*rightVector[1])/den;
            h[3] = rightVector[1]+rightVector[2]*(upVector[2]*rightVector[0]-upVector[0]*rightVector[2])/den;
            h[0] =(sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
                   sensorOrigin[1]*(upVector[0]*rightVector[2]-rightVector[0]*upVector[2])+
                   sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))*
                  (upVector[1]*(rightVector[0]*rightVector[0]+rightVector[2]*rightVector[2])-
                   rightVector[1]*(upVector[0]*rightVector[0]+upVector[2]*rightVector[2]))/(den*den);
            h[2] =(sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
                   sensorOrigin[1]*(upVector[0]*rightVector[2]-upVector[2]*rightVector[0])+
                   sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))*
                  (rightVector[0]*(upVector[1]*rightVector[1]+upVector[2]*rightVector[2])-
                   upVector[0]*(rightVector[1]*rightVector[1]+rightVector[2]*rightVector[2]))/(den*den);
            trackCov[1][1] = sensorOrigin[2]*sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[1][0] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[3][3] = sensorOrigin[2]*sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[3][2] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[0][1] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[0][0] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[2][3] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[2][2] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
        }
        else
        {
            double den = upVector[1]*rightVector[0]-upVector[0]*rightVector[1];
            offset = -sensorOrigin[0]*rightVector[0] - sensorOrigin[1]*rightVector[1]+
                      rightVector[2]*(-sensorOrigin[2]+(sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
                      sensorOrigin[1]*(upVector[0]*rightVector[2]-upVector[2]*rightVector[0])+
                      sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))/den);
            h[1] = rightVector[0]+rightVector[2]*(upVector[1]*rightVector[2]-upVector[2]*rightVector[1])/den;
            h[3] = rightVector[1]+rightVector[2]*(upVector[2]*rightVector[0]-upVector[0]*rightVector[2])/den;
            h[0] =(sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
                   sensorOrigin[1]*(upVector[0]*rightVector[2]-rightVector[0]*upVector[2])+
                   sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))*
                  (upVector[1]*(rightVector[0]*rightVector[0]+rightVector[2]*rightVector[2])-
                   rightVector[1]*(upVector[0]*rightVector[0]+upVector[2]*rightVector[2]))/(den*den);
            h[2] =(sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
                   sensorOrigin[1]*(upVector[0]*rightVector[2]-upVector[2]*rightVector[0])+
                   sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))*
                  (rightVector[0]*(upVector[1]*rightVector[1]+upVector[2]*rightVector[2])-
                   upVector[0]*(rightVector[1]*rightVector[1]+rightVector[2]*rightVector[2]))/(den*den);
            trackCov[0][0] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[1][0] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[0][1] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[1][1] = sensorOrigin[2]*sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);

            trackCov[0][2] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[1][2] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[0][3] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[1][3] = sensorOrigin[2]*sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);

            trackCov[2][0] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[2][1] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[3][0] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[3][1] = sensorOrigin[2]*sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);

            trackCov[3][3] = sensorOrigin[2]*sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[3][2] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[2][3] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
            trackCov[2][2] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
        }
*/
/*        for ( int i=0; i<4; i++ )
        {
            for ( int j=0; j<4; j++ )
            {
                a[i][j] = h[i]*h[j];
            }
        }

      //Residual of prediction
        double dist    = clusters[plaqID][trackCandidate.find(plaqID)->second.find("cluster ID")->second].find("x")->second;//distance of the hit from origin of the sensor
        double distErr = clusters[plaqID][trackCandidate.find(plaqID)->second.find("cluster ID")->second].find("xErr")->second;//distance of the hit from origin of the sensor

        //double distErr = trackCandidate.find(plaqID)->second.find("xErr")->second;
        double res = dist - trackPars*h - offset;*/

        //cout << __PRETTY_FUNCTION__
        //     << "dist: " << dist
        //     << " product: " << trackPars*h
        //     << " offset: " << offset
        //     << " predicted: " << xPred
        //     << " delta: " << xPred - trackPars*h
        //     << " residual: " << res << endl;

        //cout << __PRETTY_FUNCTION__ << "Theirs: " << res << " Ours: " << resTest << endl;

/*      //Calculate gain matrix
        double r = estCov.Similarity(h) + distErr*distErr;// h*c*h^t + sigma^2
        TVectorT<double> k(4);
        k = (1/r)*(estCov*h);
        estCov = estCov - (1/r)*a.Similarity(estCov);// (1/rrr)*c*a*c^t
        chi2 += res*res/r;

      //Update track parameters
        for ( int i=0; i<4; i++ )
        {
            for ( int j=0; j<4; j++ )
            {
                cov[i][j] = estCov[i][j]+trackCov[i][j];
            }
        }
        if( plaqID == excludedDetector )//don't update track parameters for the excluded plaq
        {
            excludedDetectorFound_ = true;
        }
        else
        {
            trackPars += res*k;
            for ( int i=0; i<4; i++ )
            {
                for ( int j=0; j<4; j++ )
                {
                    cov[i][j] = estCov[i][j]+trackCov[i][j];
                }
            }

        }*/
    }

  //Switch trackPars back to our array type
    for ( int i=0; i<4; i++ )
    {
        //std::cout << __PRETTY_FUNCTION__ << "Track par " << i << ": " << trackPars[i] << std::endl;
        //std::cout << __PRETTY_FUNCTION__ << "Final covMat line " << i << ": " << estCov[i][0] << " "<< estCov[i][1] << " "<< estCov[i][2] << " "<< estCov[i][3] <<std::endl;
        track[i] = trackPars[i];
        for ( int j=0; j<4; j++ )
        {
            cov[i][j] = estCov[i][j];
        }
    }

    //cout << __PRETTY_FUNCTION__ << "Final:   x int = " << track[1] << " y int = " << track[3] << " x slope = " << track[0] << " y slope = " << track[2] << endl;

    trackFitter::aFittedTrackDef aKalmanFittedTrack                     ;
    aKalmanFittedTrack.first.first  = track                             ;
    aKalmanFittedTrack.first.second = cov                               ;
    aKalmanFittedTrack.second       = chi2                              ;
    //cout << __PRETTY_FUNCTION__ << "Total chi2:  " << chi2 << endl;
    //aKalmanFittedTrack.second       = chi2/(trackCandidate.size()*2 - 4); //pixels
    //aFittedTrack.second             = chi2/(alignedHits.size() - 2)   ; //strips

    return aKalmanFittedTrack;
}

//===============================================================================
void trackFitter::printMatrix(std::string sm, Event::matrixDef & matrix)
{
    std::cout << sm << std::endl ;
    for(int r=0; r<4; ++r)
    {
        for(int c=0; c<4; ++c)
        {
            std::cout << std::setw(24) << std::setprecision(18) << matrix(r,c) ;
        }
        std::cout << std::endl ;
    }
    std::cout << std::endl ;
}

void trackFitter::printMatrix(std::string sm,  ROOT::Math::SMatrix<long double,4,4> & matrix)
{
    std::cout << sm << std::endl ;
    for(int r=0; r<4; ++r)
    {
        for(int c=0; c<4; ++c)
        {
            std::cout << std::setw(24) << std::setprecision(18) << matrix(r,c) ;
        }
        std::cout << std::endl ;
    }
    std::cout << std::endl ;
}
//================================================================
void trackFitter::makeDetectorTrackResiduals ( ROOT::Math::SVector<double,4>   &fittedTrack,
                                               Event::matrixDef                &covMat     ,
                                               const Event::clustersMapDef     &clusters   ,
                                               const Event::trackCandidatesDef &tracks     ,
                                               Geometry*                        theGeometry,
                                               std::string                      detector   ,
                                               int                              trackNum   )
{

    //cout << __PRETTY_FUNCTION__ << "Calculating residuals" << endl;

    int clusterNumber;
    Event::alignedHitsCandidateMapDef::const_iterator trackIt1;
    Event::clusterCoordinateDef::const_iterator       trackIt2;
    if( (trackIt1 = tracks[trackNum].find(detector)) == tracks[trackNum].end())
    {
        residualsMap_[trackNum].erase(detector);
        pullMap_     [trackNum].erase(detector);
        return;
    }
    else if((trackIt2 = trackIt1->second.find("cluster ID")) == trackIt1->second.end())
        return;

    clusterNumber = (int)trackIt2->second;

    Detector* det = theGeometry->getDetector( detector );
    Event::clustersMapDef::const_iterator clustersIt;
    Event::aClusterMapDef::const_iterator clusterIt;
    if( (clustersIt = clusters.find(detector)) == clusters.end() || (clusterIt = clustersIt->second.find(clusterNumber)) == clustersIt->second.end())
        return;

    Event::aClusterDef cluster = clusterIt->second;

    double xm    = cluster["x"];
    double ym    = cluster["y"];
    double xmErr = cluster["xErr"];
    double ymErr = cluster["yErr"];
//    double zm    = 0;
//    double zmErr = 0;
//    det->fromLocalToGlobal(&xm,&ym,&zm,&xmErr,&ymErr,&zmErr);

    double xp     = 0;
    double yp     = 0;
//    double zp     = 0;
    double xpErr  = 0;
    double ypErr  = 0;
    //double zpErr  = 0;

    // For Local residuals
    det->getPredictedLocal(fittedTrack, xp, yp);

    det->flipPositionLocal(&xm, &ym, &xmErr, &ymErr);
    det->flipPositionLocal(&xp, &yp, &xpErr, &ypErr);

    //for global resiudals, not to be done
//    det->getPredictedGlobal(fittedTrack, xp, yp, zp);

    double resX  = ( xm - xp );
    double resY  = ( ym - yp );

    //cout << __PRETTY_FUNCTION__ << "resX: " << resX << " resY: " << resY << endl;

    Detector::xyPair predPair = det->getTrackErrorsOnPlane( fittedTrack, covMat );

    double predsigxx = predPair.first  + pow( xmErr , 2 );
    double predsigyy = predPair.second + pow( ymErr , 2 );

    double pullX = resX/sqrt(predsigxx);
    double pullY = resY/sqrt(predsigyy);


    residualsMap_[trackNum][detector] = std::make_pair(resX, resY);
    pullMap_     [trackNum][detector] = std::make_pair(pullX, pullY)   ;

    if(debug_)
    {
        std::cout << "****************************************************************************************" << std::endl;
        ss_.str("");
        ss_ << "Detector: " << detector;
        STDLINE(ss_.str(), ACGreen);
        ss_.str("");
        ss_ << "ResX: " << resX;
        ss_ << " ResY: " << resY;
        STDLINE(ss_.str(), ACGreen);
        std::cout << "****************************************************************************************" << std::endl;
    }
    /*
    xm    = cluster["x"];
    ym    = cluster["y"];
    double zm    = 0;
    xmErr = cluster["xErr"];
    ymErr = cluster["yErr"];
    double zmErr = 0;
    det->test(&xm,&ym,&zm,&xmErr,&ymErr,&zmErr);
*/
//    xp = 0;
//    yp = 0;
//    double zp = 0;
    //std::cout << "[makeDetectorTrackResiduals()]" << "Number of clusters: " << clusters.size() << std::endl;
//    det->getPredictedGlobal(fittedTrack, xp, yp, zp);

}

//================================================================
void trackFitter::makeFittedTracksResiduals(Event *theEvent, Geometry *theGeometry)
{
    residualsMap_.clear() ;
    pullMap_     .clear() ;

    Event::clustersMapDef                  & clusters                 = theEvent->getClusters()              ;
    Event::trackCandidatesDef              & trackCandidates          = theEvent->getTrackCandidates()       ;
    Event::fittedTracksDef                 & tracksFitted             = theEvent->getFittedTracks()          ;
    Event::fittedTracksCovarianceDef       & covMat                   = theEvent->getFittedTracksCovariance();

    std::vector<Event::alignedHitsCandidateMapDef>        ::iterator track      = trackCandidates.begin();
    std::vector<Event::vectorDef>                         ::iterator trackFit   = tracksFitted   .begin();
    std::vector<Event::matrixDef>                         ::iterator cov        = covMat         .begin();
    int i = 0;

    for (; track!=trackCandidates.end(); track++, cov++, trackFit++, ++i)
    {
        Event::alignedHitsCandidateMapDef::iterator det = (*track).begin();

        for(; det!=(*track).end(); det++)//det++, cov++, trackFit++)
        {
            if(!selectedDetectors_.empty())
            {
                if( selectedDetectors_.find(det->first) ==  selectedDetectors_.end() )
                {
                        continue;
                }
            }

            if(debug_)
            {
                ss_.str("");

                ss_ << "Track n " << i << " - Excluded detector: " << det->first;
                STDLINE(ss_.str(),ACGreen);
            }

            trackFitter::aFittedTrackDef aFittedTrack;

            if(      fitMethodName_=="Kalman") aFittedTrack = this->kalmanFitSingleTrack(*track, *trackFit, *cov, clusters, theGeometry, det->first);
            else if (fitMethodName_=="Simple") aFittedTrack = this->fitSingleTrack(*track, theGeometry, det->first);
            else                               aFittedTrack = this->fitSingleTrack(*track, theGeometry, det->first);


            theEvent->addUnconstrainedFittedTrack(i,det->first,aFittedTrack.first.first,aFittedTrack.first.second,aFittedTrack.second);


            this->makeDetectorTrackResiduals(aFittedTrack.first.first, aFittedTrack.first.second,
                                             clusters                , trackCandidates                   ,
                                             theGeometry             , det->first               , i);

        }
    }
    theEvent->setFittedTrackResiduals(residualsMap_);
    theEvent->setFittedTrackPulls(pullMap_         );
}

//================================================================
void  trackFitter::clear(void )
{
    residualsMap_     .clear();
    pullMap_          .clear();
    fittedTracks_     .clear();
    covMat_           .clear();
    chi2_             .clear();
    selectedDetectors_.clear();
}
//=================================================================
void trackFitter::execute(Event * theEvent, Geometry * theGeometry)
{
    (this->*subProcessOperation_)(theEvent,theGeometry);
}
//==================================================================
std::string trackFitter::getLabel(void )
{
    if(subProcessOperation_ == &trackFitter::makeFittedTracksResiduals)  return "Computing unconstrained residuals";
    if(subProcessOperation_ == &trackFitter::makeFittedTrackDeviations)  return "Computing constrained residuals"  ;
    else                                                                 return  NO_PROCESS_LABEL_                 ;
}

//========================================================================================
/*void trackFitter::setTrackFitParameters (double  xTol, double yTol, double chi2cut, double minPoints, double maxPlanePoints)
{
    xTol_           = xTol          ;
    yTol_           = yTol          ;
    chi2cut_        = chi2cut       ;
    minPoints_      = minPoints     ;
    maxPlanePoints_ = maxPlanePoints;

}*/

//===============================================================================
void trackFitter::makeFittedTrackDeviations(Event * theEvent, Geometry*)
{
    //residualsMap_ = theEvent->getFittedTrackDeviations();
    residualsMap_.clear();
    Event::trackCandidatesDef &trackCandidates = theEvent->getTrackCandidates();
    Event::fittedTracksDef    &fittedTracks    = theEvent->getFittedTracks()   ;

    if ( trackCandidates.size() != fittedTracks.size() )
    {
        ss_.str("");
        ss_ << "track candidates size: " << trackCandidates.size()
            << " fitted tracks   size: " << fittedTracks.size()    ;
        STDLINE(ss_.str(),ACYellow)
    }

    for( unsigned int i=0; i < trackCandidates.size(); i++ )
    {
        for(Event::alignedHitsCandidateMapDef::iterator pit = trackCandidates[i].begin(); pit != trackCandidates[i].end()  ; pit++)
        {
            double resX = ( (*pit).second["x"] - fittedTracks[i][0]*(*pit).second["z"] - fittedTracks[i][1] ) ;
            double resY = ( (*pit).second["y"] - fittedTracks[i][2]*(*pit).second["z"] - fittedTracks[i][3] ) ;
            if(debug_)
            {
                ss_.str("");
                ss_ << "track: " << i << " detector: " << (*pit).first << " ResX: " << resX << " ResY: " << resY;
                STDLINE(ss_.str(),ACYellow);
            }
            residualsMap_[i][(*pit).first] = std::make_pair(resX, resY);
        }
    }
    //STDLINE("",ACYellow)
    theEvent->setFittedTrackDeviations(residualsMap_);
}
//=======================================================================================

ROOT::Math::SVector<double,4> trackFitter::calculateParCorrections (ROOT::Math::SVector<double,4> pars, Geometry * geo, std::map<std::string, std::pair<double, double> > res)
{
    ROOT::Math::SMatrix<double,4,4> AtVA      ;
    ROOT::Math::SMatrix<double,4,4> AtVAInv   ;
    ROOT::Math::SVector<double,4>   AtVmq     ;
    ROOT::Math::SVector<double,4>   dmq       ;//x_loc derivatives with inverse error
    double D = 0;//denominator
    double N = 0;//numerator

    ROOT::Math::SVector<double,4>   corr;//corrections

    for (std::map<std::string, std::pair<double, double> >::iterator it = res.begin(); it != res.end(); ++it)
    {
        int fail;
        Detector::matrix33Def RM = geo->getDetector(it->first)->getRotationMatrix().Inverse(fail);
        double z = geo->getDetector(it->first)->getZPosition();
/*        std::cout << "Matrix for plane " << it->first << std::endl;
        for (int r = 0; r < 3; ++r)
        {
            for (int c = 0; c < 3; ++c)
                printf("%7.3f ", RM(r,c));
            std::cout << std::endl;
        }
        std::cout << std::endl;*/

        if (geo->getDetectorModule(it->first)%2 == 0)
        {
            N = (pars(0)*z + pars(1))*(RM(1,1) - pars(2)*RM(2,1)) -
                (pars(2)*z + pars(3))*(RM(0,1) - pars(0)*RM(2,1));
            D = (RM(0,0) - pars(0)*RM(2,0))*(RM(1,1) - pars(2)*RM(2,1)) -
                (RM(1,0) - pars(2)*RM(2,0))*(RM(0,1) - pars(0)*RM(2,1));
            dmq(0) = ((z*(RM(1,1) - pars(2)*RM(2,1)) + RM(2,1)*(pars(2)*z + pars(3)))*D -
                 (-RM(2,0)*(RM(1,1) - pars(2)*RM(2,1)) + RM(2,1)*(RM(1,0) - pars(2)*RM(2,0)))*N)/(D*D);
            dmq(1) = (RM(1,1) - pars(2)*RM(2,1))/(D);
            dmq(2) = ((-z*(RM(0,1) - pars(0)*RM(2,1)) -RM(2,1)*(pars(0)*z + pars(1)))*D -
                 (RM(2,0)*(RM(0,1) - pars(0)*RM(2,1)) - RM(2,1)*(RM(0,0) - pars(0)*RM(2,0)))*N)/(D*D);
            dmq(3) = -(RM(0,1) - pars(0)*RM(2,1))/(D);
        }
        else
        {
            N = (pars(2)*z + pars(3))*(RM(0,0) - pars(0)*RM(2,0)) -
                (pars(0)*z + pars(1))*(RM(1,0) - pars(2)*RM(2,0));
            D = (RM(1,1) - pars(2)*RM(2,1))*(RM(0,0) - pars(0)*RM(2,0)) -
                (RM(0,1) - pars(0)*RM(2,1))*(RM(1,0) - pars(2)*RM(2,0));
            dmq(2) = ((z*(RM(0,0) - pars(0)*RM(2,0)) + RM(2,0)*(pars(0)*z + pars(1)))*D -
                 (-RM(2,1)*(RM(0,0) - pars(0)*RM(2,0)) + RM(2,0)*(RM(0,1) - pars(0)*RM(2,1)))*N)/(D*D);
            dmq(3) = (RM(0,0) - pars(0)*RM(2,0))/(D);
            dmq(0) = ((-z*(RM(1,0) - pars(2)*RM(2,0)) -RM(2,0)*(pars(2)*z + pars(3)))*D -
                 (RM(2,1)*(RM(1,0) - pars(2)*RM(2,0)) - RM(2,0)*(RM(1,1) - pars(2)*RM(2,1)))*N)/(D*D);
            dmq(1) = -(RM(1,0) - pars(2)*RM(2,0))/(D);
        }

        for (int r = 0; r < 4; ++r)
        {
            for (int c = 0; c < 4; ++c)
            {
                AtVA(r,c) += dmq(r)*dmq(c)/(res[it->first].second*res[it->first].second);
            }
        }

        for (int ch = 0; ch < 4; ++ch)
        {
            AtVmq(ch) += dmq(ch)*res[it->first].first/(res[it->first].second*res[it->first].second);
        }
    }

    int fail2;
    AtVAInv = AtVA.Inverse(fail2);

    corr = AtVAInv*AtVmq;
    return corr;
}
//=======================================================================================
