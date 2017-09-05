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
#include <algorithm>
#include "trackFitter.h"
#include "MessageTools.h"

//===============================================================================
trackFitter::trackFitter(void)
{
    count=0;
    //STDLINE("Empty constructor",ACWhite);
    debug_        = true;
    tracksFitted_ = false ;
    nIterations_  = 0;
}

//===============================================================================
Event::fittedTracksDef trackFitter::fitTracks(const Event::trackCandidatesDef & tracks,
                                              Geometry                        * theGeometry,
                                              std::string                       excludedDetector )
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
trackFitter::aFittedTrackDef trackFitter::fitSingleTrack(const Event::alignedHitsCandidateMapDef & alignedHits,
                                                         Geometry                                * theGeometry,
                                                         std::string                               excludedDetector )
{
    TBenchmark * theBenchmark = new TBenchmark();
    theBenchmark->Start("fitSingleTrack")     ;

    ROOT::Math::SMatrix<double,4,4> AtVA      ;
    ROOT::Math::SMatrix<double,4,4> AtVAInv   ;
    ROOT::Math::SMatrix<double,2,2> sigmaXY   ;
    ROOT::Math::SMatrix<double,2,2> sigmaXYInv;
    ROOT::Math::SVector<double,4>   AtVxy     ;
    ROOT::Math::SVector<double,4>   pars      ;

    double chi2=0;
    excludedDetectorFound_ = false;

    for(Event::alignedHitsCandidateMapDef::const_iterator tr=alignedHits.begin(); tr!=alignedHits.end(); tr++)
    {
        if( theGeometry->getDetector( (*tr).first )->isDUT() ) continue;

        if( (*tr).first == excludedDetector )
        {
            excludedDetectorFound_ = true;
            continue;
        }
// Dario e Luigi: verificare se slope e intercette sono in ordine corretto rispetto al formalismo
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
//            ss_.str("");
//            ss_ << "Filling matrix";
//            STDLINE(ss_.str(),ACYellow);

            ss_.str("");
            ss_ << "Detector "
                << tr->first;
            ss_ << ": Rotation alpha: "
                << -(theGeometry->getDetector( (*tr).first )->getXRotation() +
                     theGeometry->getDetector( (*tr).first )->getXRotationCorrection())
                << " - beta: "
                << -(theGeometry->getDetector( (*tr).first )->getYRotation() +
                     theGeometry->getDetector( (*tr).first )->getYRotationCorrection())
                << " - gamma: "
                << -(theGeometry->getDetector( (*tr).first )->getZRotationCorrection());
            // STDLINE(ss_.str(),ACRed);


            ss_.str("");
            ss_ << " x prime: "
                << clusters_[(*tr).first][(int)hit["cluster ID"]]["x"] -
                   theGeometry->getDetector( (*tr).first )->getXPosition()
                << "+-"
                << clusters_[(*tr).first][(int)hit["cluster ID"]]["xErr"]
                << " y prime: "
                << clusters_[(*tr).first][(int)hit["cluster ID"]]["y"] -
                   theGeometry->getDetector( (*tr).first )->getYPosition()
                << "+-"
                << clusters_[(*tr).first][(int)hit["cluster ID"]]["yErr"]
                << " z center: "
                << theGeometry->getDetector( (*tr).first )->getZPositionTotal();
            // STDLINE(ss_.str(),ACPurple);

            ss_.str("");
            ss_ << " x: "  << hit["x"] << "+-" << hit["xErr"]
                << " y: "  << hit["y"] << "+-" << hit["yErr"]
                << " z: "  << hit["z"];
            // STDLINE(ss_.str(),ACPurple);
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
        // STDLINE(ss_.str(),ACRed);

        printMatrix("AtVAInv: ",AtVAInv);

//        assert(0);
//        ss_.str("");
//        ss_ << "faild count " << count++;
//        STDLINE(ss_.str(),ACPurple);

//        continue;
    }

    pars = AtVAInv*AtVxy;

    ss_.str("");
    ss_ << "alignedHits.size: " << alignedHits.size();
    std::map<std::string, std::pair<double, double> > resMap; //residuals for each plane used for successive iterations
    double xPredLoc, yPredLoc        ;
    double xHitLoc , yHitLoc, zHitLoc;
    double xErrLoc , yErrLoc, zErrLoc;
    for(Event::alignedHitsCandidateMapDef::const_iterator tr=alignedHits.begin(); tr!=alignedHits.end(); tr++)
    {
        if( theGeometry->getDetector( tr->first )->isDUT() )  continue;
        if( tr->first == excludedDetector )                   continue;

        Event::aClusterDef hit = tr->second;

        //global coordinates
        chi2 += pow( hit["x"] - pars[0]*hit["z"] - pars[1], 2 )/(hit["xErr"]*hit["xErr"]) +
                pow( hit["y"] - pars[2]*hit["z"] - pars[3], 2 )/(hit["yErr"]*hit["yErr"])  ;

        if (nIterations_ == 0)
            continue;

        theGeometry->getDetector(tr->first)->getPredictedLocal(pars, xPredLoc, yPredLoc);
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

            theGeometry->getDetector(tr->first)->getPredictedLocal(pars, xPredLoc, yPredLoc);
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
        // STDLINE(ss_.str(),ACYellow);

        ss_.str("");
        ss_ << "Fit Results"
            << " sx: " << pars[0]
            << " qx: " << pars[1]
            << " sy: " << pars[2]
            << " qy: " << pars[3];
        // STDLINE(ss_.str(), ACYellow);
    }

    trackFitter::aFittedTrackDef aFittedTrack;
    aFittedTrack.first.first = pars          ;
    aFittedTrack.first.second= AtVAInv         ;
    aFittedTrack.second      = chi2/(alignedHits.size()*2 - 4)          ; //pixels

    theBenchmark->Stop("fitSingleTrack") ;
    cout << __LINE__ << "] fitSingleTrack CPU time: " << theBenchmark->GetCpuTime("fitSingleTrack") << endl ;

    return aFittedTrack;
}

//===============================================================================
// Original document with formalism: ../documents/StripTelescopeAlignmentFormalism.pdf
trackFitter::aFittedTrackDef trackFitter::kalmanFitSingleTrack(const Event::alignedHitsCandidateMapDef & trackCandidate,
                                                                     Event::vectorDef                  & track         ,
                                                                     Event::matrixDef                  & cov           ,
                                                                     Event::clustersMapDef             & clusters      ,
                                                                     Geometry                          * theGeometry   )
{
    TBenchmark * theBenchmark = new TBenchmark();
    theBenchmark->Start("kalmanFitSingleTrack")     ;

    double chi2            = 0;

    //Use existing trackPars and covMat

    TVectorT   <double> trackPars(4);
    TMatrixTSym<double> estCov   (4);

    for ( int i=0; i<4; i++ )
    {
        trackPars[i] = track[i];
    }
    for ( int i=0; i<4; i++ )
    {
        for ( int j=0; j<4; j++ )
        {
            estCov[i][j] = cov[i][j]; // Dario e Luigi: moltiplicare per un fattore 10^4 (minimo)
        }
    }

    //Use maps to store estCov and trackPars for each plane to use them in the smoothing step

    std::map<std::string, TVectorT   <double> > trackParsMap;
    std::map<std::string, TMatrixTSym<double> > CkMap;
    std::map<std::string, TMatrixTSym<double> > Ckk_1Map;
    TMatrixT<double> A   (4,4);
    TMatrixT<double> At  (4,4);
    TMatrixT<double> temp(4,4);

    for (unsigned int plane = 0; plane < theKalmanPlaneInfo_.getKalmanFilterOrder().size(); plane++)
    {
        std::string plaqID = theKalmanPlaneInfo_.getKalmanFilterOrder().at(plane).second;

        TVectorT   <double> k (4);
        TMatrixTSym<double> B (4);

        TVectorT   <double> h         = theKalmanPlaneInfo_.getH        (plaqID);
        TMatrixTSym<double> trackCov  = theKalmanPlaneInfo_.getTrackCov (plaqID);
        TMatrixTSym<double> trackCovx = theKalmanPlaneInfo_.getTrackCovx(plaqID);
        TMatrixTSym<double> trackCovy = theKalmanPlaneInfo_.getTrackCovy(plaqID);
        double offset                 = theKalmanPlaneInfo_.getOffset   (plaqID);

        if ( theGeometry->getDetector(plaqID)->isStrip() )//|| theGeometry->getDetector(plaqID)->isDUT() ) //strip data
        {            
            if(trackCandidate.find(plaqID) != trackCandidate.end())
            {
                for ( int i=0; i<4; i++ )
                {
                    for ( int j=0; j<4; j++ )
                    {
                        B[i][j] = h[i]*h[j];
                    }
                }

                //Residual of prediction
                double distErr = clusters[plaqID][trackCandidate.find(plaqID)->second.find("cluster ID")->second].find("xErr")->second;
                double dist    = clusters[plaqID][trackCandidate.find(plaqID)->second.find("cluster ID")->second].find("x"   )->second;//distance of the hit from origin of the sensor
                double res     = dist - trackPars*h - offset;

                //Calculate gain matrix
                double r       = estCov.Similarity(h) + distErr*distErr;// h*c*h^t + sigma^2
                k              = (1/r)*(estCov*h);
                estCov        -= (1/r)*B.Similarity(estCov);// (1/rrr)*c*a*c^t
                CkMap[plaqID].ResizeTo(4,4);
                CkMap[plaqID]  = estCov;
                chi2          += (res*res/r)/(trackCandidate.size() - 2);
                trackPars     += res*k;
            }

            if(plane != (theKalmanPlaneInfo_.getKalmanFilterOrder().size()-1))
            {
                for ( int i=0; i<4; i++ )
                {
                    for ( int j=0; j<4; j++ )
                    {
                        estCov[i][j] += trackCov[i][j];
                    }
                }
            }

            trackParsMap[plaqID].ResizeTo(4  );
            trackParsMap[plaqID] = trackPars  ;
            Ckk_1Map    [plaqID].ResizeTo(4,4);
            Ckk_1Map    [plaqID] = estCov     ;
        }
        else //not strip data (pixels)
        {
            for ( int i=0; i<4; i++ )
            {
                for ( int j=0; j<4; j++ )
                {
                    estCov[i][j] += (trackCovx[i][j]+ trackCovy[i][j]);
                }
            }
        }
    }

    // Smoothing planes
    for (revIterDef rit = theKalmanPlaneInfo_.getKalmanFilterOrder().crbegin()+1;
                    rit!= theKalmanPlaneInfo_.getKalmanFilterOrder().crend();
                  ++rit)
    {
            std::string plane = (*rit).second ;

            if(trackCandidate.find(plane) == trackCandidate.end()) continue;

            TVectorT<double> h = theKalmanPlaneInfo_.getH     (plane) ;
            double offset      = theKalmanPlaneInfo_.getOffset(plane);

            Ckk_1Map[plane].Invert();

            A = CkMap[plane]*Ckk_1Map[plane];
            At.Transpose(A);
            Ckk_1Map[plane].Invert(); // Dario: might be better to copy the origina here;l Ckk matrix in a copy (more performant than two inversion)

            trackPars = trackParsMap[plane] + A*(trackPars-trackParsMap[plane]);

            temp = CkMap[plane] + A*(estCov-Ckk_1Map[plane])*At;

            for ( int i=0; i<4; i++ )
            {
                for ( int j=0; j<4; j++ )
                {
                    estCov[i][j] = temp[i][j];
                }
            }

            //chi2 update. dist is the distance of the hit from origin of the sensor
            double dist    = clusters[plane][trackCandidate.find(plane)->second.find("cluster ID")->second].find("x"   )->second;
            double distErr = clusters[plane][trackCandidate.find(plane)->second.find("cluster ID")->second].find("xErr")->second;
            double res     = dist - trackPars*h - offset;

            double r       = estCov.Similarity(h) + distErr*distErr;// h*c*h^t + sigma^2
            chi2          += (res*res/r)/(trackCandidate.size() - 2);
    }

    //Switch trackPars back to our array type
    for ( int i=0; i<4; i++ )
    {
        track[i] = trackPars[i];
        for ( int j=0; j<4; j++ )
        {
            cov[i][j] = estCov[i][j];
        }
    }

    trackFitter::aFittedTrackDef aKalmanFittedTrack;
    aKalmanFittedTrack.first.first  = track        ;
    aKalmanFittedTrack.first.second = cov          ;
    aKalmanFittedTrack.second       = chi2         ;

    theBenchmark->Stop("kalmanFitSingleTrack") ;
    cout << __LINE__ << "] kalmanFitSingleTrack CPU time: " << theBenchmark->GetCpuTime("kalmanFitSingleTrack") << endl ;

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

//===============================================================================
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
void trackFitter::makeDetectorTrackResiduals ( ROOT::Math::SVector<double,4>   & fittedTrack,
                                               Event::matrixDef                & covMat     ,
                                               const Event::clustersMapDef     & clusters   ,
                                               const Event::trackCandidatesDef & tracks     ,
                                               Geometry*                         theGeometry,
                                               std::string                       detector   ,
                                               int                               trackNum   )
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
    if( (clustersIt = clusters.find(detector)) == clusters.end() ||
        (clusterIt  = clustersIt->second.find(clusterNumber)) == clustersIt->second.end())
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

//    if((theGeometry->getDetectorModule(detector))%2==0)
//        cout<<__PRETTY_FUNCTION__<<" Simple Xpred before flip: "<<xp<<endl;

//    else cout<<__PRETTY_FUNCTION__<<" Simple Ypred before flip: "<<yp<<endl;

    // Flip operations are inlcuded in kalman fit so flip of x planes is not needed

//    if((det->isStrip())&&((theGeometry->getDetectorModule(detector))%2!=0))
//    {
        det->flipPositionLocal(&xm, &ym, &xmErr, &ymErr);
        det->flipPositionLocal(&xp, &yp, &xpErr, &ypErr);
//    }

    //for global resiudals, not to be done
    //    det->getPredictedGlobal(fittedTrack, xp, yp, zp);

    double resX  = xm-xp;
    double resY  = ym-yp;

    Detector::xyPair predPair = det->getTrackErrorsOnPlane( fittedTrack, covMat );

    double predsigxx = predPair.first  + pow( xmErr , 2 );
    double predsigyy = predPair.second + pow( ymErr , 2 );

    double pullX = resX/sqrt(predsigxx);
    double pullY = resY/sqrt(predsigyy);

//    cout << __PRETTY_FUNCTION__ << "predXErr: " << predPair.first << "predyErr: " << predPair.second
//                                << "mesXErr: " << xmErr << " mesYErr: " << ymErr<< endl;
//    if(!det->isDUT())
//    {

//        TVectorT<double> h = theKalmanPlaneInfo_.getH(detector) ;
//        double offset      = theKalmanPlaneInfo_.getOffset(detector);
//        TVectorT<double> trackPars(4);
//        for ( int i=0; i<4; i++ )
//        {
//            trackPars[i] = fittedTrack[i];
//        }

//        double res = cluster["x"]-(trackPars*h + offset);

//        if((theGeometry->getDetectorModule(detector))%2==0)
//        {
//            cout<<__PRETTY_FUNCTION__<<" Simple Xmes: "<<xm<<" Kalman Xmes: "<<cluster["x"]
//                                     <<" Simple Xpred: "<<xp<<" Kalman Xpred: "<<trackPars*h + offset
//                                     <<" Simple res: "<<resX<<" Kalman res: "<<res
//                                     <<" Difference: "<<resX-res<<endl;

//            resX  -= res;


//        }
//        else
//        {
//            cout<<__PRETTY_FUNCTION__<<" Simple Ymes: "<<ym<<" Kalman Ymes: "<<cluster["x"]
//                                     <<" Simple Ypred: "<<yp<<" Kalman Ypred: "<<trackPars*h + offset
//                                     <<" Simple res: "<<resY<<" Kalman res: "<<res
//                                     <<" Difference: "<<resY-res<<endl;

//            resY  -= res;
//        }

//    }

    residualsMap_[trackNum][detector] = std::make_pair(resX, resY);
    pullMap_     [trackNum][detector] = std::make_pair(pullX, pullY)   ;

    if(debug_)
    {
        // std::cout << "****************************************************************************************" << std::endl;
        ss_.str("");
        ss_ << "Detector: " << detector;
        // STDLINE(ss_.str(), ACGreen);
        ss_.str("");
        ss_ << "ResX: " << resX;
        ss_ << " ResY: " << resY;
        // STDLINE(ss_.str(), ACGreen);
        // std::cout << "****************************************************************************************" << std::endl;
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
                // STDLINE(ss_.str(),ACGreen);
            }

            trackFitter::aFittedTrackDef aFittedTrack;

            if(      fitMethodName_=="Kalman") aFittedTrack = this->kalmanFitSingleTrack(*track      ,
                                                                                         *trackFit   ,
                                                                                         *cov        ,
                                                                                          clusters   ,
                                                                                          theGeometry);
            else if (fitMethodName_=="Simple") aFittedTrack = this->fitSingleTrack      (*track,
                                                                                          theGeometry,
                                                                                          det->first );
            else                               aFittedTrack = this->fitSingleTrack      (*track,
                                                                                          theGeometry,
                                                                                          det->first );

            theEvent->addUnconstrainedFittedTrack(
                                                  i,
                                                  det->first,
                                                  aFittedTrack.first.first,
                                                  aFittedTrack.first.second,
                                                  aFittedTrack.second
                                                 );

            this->makeDetectorTrackResiduals(aFittedTrack.first.first, aFittedTrack.first.second,
                                             clusters                , trackCandidates          ,
                                             theGeometry             , det->first               , i);

        }
    }
    theEvent->setFittedTrackResiduals(residualsMap_);
    theEvent->setFittedTrackPulls    (pullMap_     );
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

ROOT::Math::SVector<double,4> trackFitter::calculateParCorrections (ROOT::Math::SVector<double,4>                       pars,
                                                                    Geometry                                          * geo ,
                                                                    std::map<std::string, std::pair<double, double> >   res )
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
