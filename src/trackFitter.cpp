/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include "trackFitter.h"
#include <TVectorT.h>
#include <TMatrixTSym.h>

//===============================================================================
trackFitter::trackFitter(void)
{
    count=0;
    STDLINE("empty constructor",ACWhite);
    debug_ = false;
    tracksFitted_ = false ;
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

int myCounter=0;
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
            ss_ << "x prime: "  << clusters_[(*tr).first][(int)hit["num"]]["x"] - theGeometry->getDetector( (*tr).first )->getXPosition()
                << "+-" << clusters_[(*tr).first][(int)hit["num"]]["xErr"]
                << " y prime: " << clusters_[(*tr).first][(int)hit["num"]]["y"] - theGeometry->getDetector( (*tr).first )->getYPosition()
                << "+-" << clusters_[(*tr).first][(int)hit["num"]]["yErr"]
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
    //STDLINE(ss_.str(),ACPurple)
//    double xPredLoc, yPredLoc, zPredLoc;
//    double xHitLoc , yHitLoc , zHitLoc ;
//    double xErrLoc , yErrLoc , zErrLoc ;
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
    //if (myCounter==1)
        //exit(0);
    ++myCounter;

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
    std::map<int,int> trackParsMap;
    trackParsMap[0] = 2;//x int.
    trackParsMap[1] = 0;//y int.
    trackParsMap[2] = 3;//x slope
    trackParsMap[3] = 1;//y slope
    for ( int i=0; i<4; i++ )
    {
        trackPars[trackParsMap[i]] = track[i];
    }
    for ( int i=0; i<4; i++ )
    {
        for ( int j=0; j<4; j++ )
        {
            estCov[trackParsMap[i]][trackParsMap[j]] = cov[i][j];
        }
    }

    //cout << __PRETTY_FUNCTION__ << "Initial: x int = " << trackPars[0] << " y int = " << trackPars[1] << " x slope = " << trackPars[2] << " y slope = " << trackPars[3] << endl;

  //Change loop so it runs in order of z instead of plaqID
    std::map<double, std::string> plaqByZ;
    for (Event::alignedHitsCandidateMapDef::const_iterator itTrackCandidate=trackCandidate.begin(); itTrackCandidate!=trackCandidate.end(); itTrackCandidate++)
    {
        std::string plaqID = itTrackCandidate->first;
        double z = itTrackCandidate->second.find("z")->second;

        plaqByZ[z] = plaqID;
    }

  //Now loop over all plaqs
    //std::map<double, string>::iterator itZ=plaqByZ.begin();
    for (std::map<double, std::string>::iterator itZ=plaqByZ.begin(); itZ!=plaqByZ.end(); itZ++)
    {
        std::string plaqID = itZ->second;

        if( theGeometry->getDetector( plaqID )->isDUT() ) continue;

      //Use Kalman fit method to improve fit

      //Define variables
        Detector* detector = theGeometry->getDetector(plaqID);
        TVectorT<double> sensorOrigin(4);
        TVectorT<double> upVector(4);
        TVectorT<double> rightVector(4);
        TVectorT<double> beamVector(4);
        TVectorT<double> h(4);
        TMatrixTSym<double> trackCov(4);
        TMatrixTSym<double> a(4);
        double offset;
        double multipleScattering = 4.37e-6;

      //Test residual
        //double xPred, yPred, zPred;
        //detector->getPredictedGlobal(track, xPred, yPred, zPred);
        //detector->fromGlobalToLocal(&xPred, &yPred, &zPred);
        //double resTest = trackCandidate.find(plaqID)->second.find("x")->second - xPred;

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

      //Normalize vectors
        upVector[0]    -= sensorOrigin[0]; upVector[1]    -= sensorOrigin[1]; upVector[2]    -= sensorOrigin[2];
        rightVector[0] -= sensorOrigin[0]; rightVector[1] -= sensorOrigin[1]; rightVector[2] -= sensorOrigin[2];
        beamVector[0]  -= sensorOrigin[0]; beamVector[1]  -= sensorOrigin[1]; beamVector[2]  -= sensorOrigin[2];

      //"Compute"
        double den = upVector[1]*rightVector[0]-upVector[0]*rightVector[1];
        offset = -sensorOrigin[0]*rightVector[0] - sensorOrigin[1]*rightVector[1]+
                  rightVector[2]*(-sensorOrigin[2]+(sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
                  sensorOrigin[1]*(upVector[0]*rightVector[2]-upVector[2]*rightVector[0])+
                  sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))/den);
        h[0] = rightVector[0]+rightVector[2]*(upVector[1]*rightVector[2]-upVector[2]*rightVector[1])/den;
        h[1] = rightVector[1]+rightVector[2]*(upVector[2]*rightVector[0]-upVector[0]*rightVector[2])/den;
        h[2] =(sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
               sensorOrigin[1]*(upVector[0]*rightVector[2]-rightVector[0]*upVector[2])+
               sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))*
              (upVector[1]*(rightVector[0]*rightVector[0]+rightVector[2]*rightVector[2])-
               rightVector[1]*(upVector[0]*rightVector[0]+upVector[2]*rightVector[2]))/(den*den);
        h[3] =(sensorOrigin[0]*(upVector[2]*rightVector[1]-upVector[1]*rightVector[2])+
               sensorOrigin[1]*(upVector[0]*rightVector[2]-upVector[2]*rightVector[0])+
               sensorOrigin[2]*(upVector[1]*rightVector[0]-upVector[0]*rightVector[1]))*
              (rightVector[0]*(upVector[1]*rightVector[1]+upVector[2]*rightVector[2])-
               upVector[0]*(rightVector[1]*rightVector[1]+rightVector[2]*rightVector[2]))/(den*den);
        trackCov[0][2] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
        trackCov[1][1] = sensorOrigin[2]*sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
        trackCov[1][3] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
        trackCov[2][0] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
        trackCov[2][2] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
        trackCov[3][1] = -sensorOrigin[2]*multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
        trackCov[3][3] = multipleScattering*multipleScattering/(beamVector[2]*beamVector[2]);
        for ( int i=0; i<4; i++ )
        {
            for ( int j=0; j<4; j++ )
            {
                a[i][j] = h[i]*h[j];
            }
        }

      //Residual of prediction
        double dist = clusters[plaqID][trackCandidate.find(plaqID)->second.find("cluster ID")->second].find("x")->second;//distance of the hit from origin of the sensor
        double distErr = trackCandidate.find(plaqID)->second.find("xErr")->second;
        double res = dist - trackPars*h - offset;

        //cout << __PRETTY_FUNCTION__
        //     << "dist: " << dist
        //     << " product: " << trackPars*h
        //     << " offset: " << offset
        //     << " predicted: " << xPred
        //     << " delta: " << xPred - trackPars*h
        //     << " residual: " << res << endl;

        //cout << __PRETTY_FUNCTION__ << "Theirs: " << res << " Ours: " << resTest << endl;

        //Calculate gain matrix
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
                cov[i][j] = estCov[trackParsMap[i]][trackParsMap[j]]+trackCov[trackParsMap[i]][trackParsMap[j]];
            }
        }
        if( plaqID == excludedDetector )//don't update track parameters for the excluded plaq, but still update covMat
        {
            excludedDetectorFound_ = true;
        }

        else
        {
            trackPars += res*k;
        }
    }

  //Switch trackPars back to our way
    for ( int i=0; i<4; i++ )
    {
        track[i] = trackPars[trackParsMap[i]];
    }

    //cout << __PRETTY_FUNCTION__ << "Final:   x int = " << track[1] << " y int = " << track[3] << " x slope = " << track[0] << " y slope = " << track[2] << endl;

    trackFitter::aFittedTrackDef aKalmanFittedTrack                  ;
    aKalmanFittedTrack.first.first = track                           ;
    aKalmanFittedTrack.first.second= cov                             ;
    aKalmanFittedTrack.second      = chi2                            ; //pixels
    //aFittedTrack.second            = chi2/(alignedHits.size() - 2)   ; //strips

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
    else if((trackIt2 = trackIt1->second.find("num")) == trackIt1->second.end())
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

    //    residualsMap_ = theEvent->getFittedTrackResiduals();
    //    pullMap_      = theEvent->getFittedTrackPulls()    ;

    //cout << " " << endl;
    //cout << __PRETTY_FUNCTION__ << "Making residuals" << endl;

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
    //for (unsigned int i=0; i < tracks.size(); ++i, cov++, trackFit++)
    {
        //std::vector<Event::vectorDef>    ::iterator trackFit   = tracksFitted   .begin();
        //std::vector<Event::matrixDef>    ::iterator cov        = covMat         .begin();
        //Event::alignedHitsCandidateMapDef::iterator det        = tracks[i]      .begin();

        //cout << __PRETTY_FUNCTION__ << "Pars fed to res maker: x int = " << (*trackFit)[0] << " y int = " << (*trackFit)[1] << " x slope = " << (*trackFit)[2] << " y slope = " << (*trackFit)[3] << endl;//good

        Event::alignedHitsCandidateMapDef                     ::iterator det        = (*track)          .begin();
        //Event::alignedHitsCandidateMapDef::iterator det        = tracks[i]      .begin();

        for(; det!=(*track).end(); det++)//det++, cov++, trackFit++)
        //for(; det!=tracks[i].end(); )//det++, cov++, trackFit++)
        {
            //cout << __PRETTY_FUNCTION__ << "Pars 1: x int = " << (*trackFit)[0] << " y int = " << (*trackFit)[1] << " x slope = " << (*trackFit)[2] << " y slope = " << (*trackFit)[3] << endl;//bad

            if(!selectedDetectors_.empty())
            {
                if( selectedDetectors_.find(det->first) ==  selectedDetectors_.end() )
                    continue;
            }

            //cout << __PRETTY_FUNCTION__ << "Pars 2: x int = " << (*trackFit)[0] << " y int = " << (*trackFit)[1] << " x slope = " << (*trackFit)[2] << " y slope = " << (*trackFit)[3] << endl;//bad

            if(debug_)
            {
                ss_.str("");

                ss_ << "Track n " << i << " - Excluded detector: " << det->first;
                STDLINE(ss_.str(),ACGreen);
            }

            //cout << __PRETTY_FUNCTION__ << "Pars 3: x int = " << (*trackFit)[0] << " y int = " << (*trackFit)[1] << " x slope = " << (*trackFit)[2] << " y slope = " << (*trackFit)[3] << endl;//bad

            trackFitter::aFittedTrackDef aFittedTrack;

            //cout << __PRETTY_FUNCTION__ << "Track: " << tracks[i] << " Pars:" << *trackFit << " Cov: " << *cov << " Det: " << det->first << endl;
            //cout << __PRETTY_FUNCTION__ << "Pars fed to track fitter: x int = " << (*trackFit)[0] << " y int = " << (*trackFit)[1] << " x slope = " << (*trackFit)[2] << " y slope = " << (*trackFit)[3] << endl;//bad

            if(fitMethodName_=="Kalman") aFittedTrack = this->kalmanFitSingleTrack(*track, *trackFit, *cov, clusters, theGeometry, det->first);
            //if(fitMethodName_=="Kalman") aFittedTrack = this->kalmanFitSingleTrack(tracks[i], *trackFit, *cov, clusters, theGeometry, det->first);
            else if (fitMethodName_=="Simple") aFittedTrack = this->fitSingleTrack(*track, theGeometry, det->first);
            //else if (fitMethodName_=="Simple") aFittedTrack = this->fitSingleTrack(tracks[i], theGeometry, det->first);
            else aFittedTrack = this->fitSingleTrack(*track, theGeometry, det->first);
            //else aFittedTrack = this->fitSingleTrack(tracks[i], theGeometry, det->first);

            //cout << __PRETTY_FUNCTION__ << "Pars after track fitter: x int = " << (*trackFit)[0] << " y int = " << (*trackFit)[1] << " x slope = " << (*trackFit)[2] << " y slope = " << (*trackFit)[3] << endl;//bad

            theEvent->addUnconstrainedFittedTrack(i,det->first,aFittedTrack.first.first,aFittedTrack.first.second,aFittedTrack.second);
            //theEvent->addUnconstrainedFittedTrack(i,det->first,aFittedTrack.first.first,aFittedTrack.first.second,aFittedTrack.second);

            //cout << __PRETTY_FUNCTION__ << "Pars after adding: x int = " << (*trackFit)[0] << " y int = " << (*trackFit)[1] << " x slope = " << (*trackFit)[2] << " y slope = " << (*trackFit)[3] << endl;//bad

            this->makeDetectorTrackResiduals(aFittedTrack.first.first, aFittedTrack.first.second,
                                             clusters                , trackCandidates                   ,
                                             theGeometry             , det->first               , i);

            //cout << __PRETTY_FUNCTION__ << "Pars after res: x int = " << (*trackFit)[0] << " y int = " << (*trackFit)[1] << " x slope = " << (*trackFit)[2] << " y slope = " << (*trackFit)[3] << endl;//bad
        }
        //cout << __PRETTY_FUNCTION__ << "Pars at end: x int = " << (*trackFit)[0] << " y int = " << (*trackFit)[1] << " x slope = " << (*trackFit)[2] << " y slope = " << (*trackFit)[3] << endl;//bad
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

