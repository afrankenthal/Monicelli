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
 
#ifndef ALIGNER_H
#define ALIGNER_H

#include <iostream>
#include <map>
#include <math.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2D.h>
#include <TRandom.h>
#include <TCanvas.h>
#include <TMatrixD.h>
#include <TVectorD.h>

#include <Math/SMatrix.h>
#include <Math/SVector.h>

//#include "Event.h"
#include "fileEater.h"
//#include "Geometry.h"
//#include "HManager.h"
//#include "MessageTools.h"
//#include "multiProcess.h"
#include "trackFitter.h"

class aligner : public multiProcess<aligner,bool>
{
  public :
     aligner(fileEater* theFileEater, HManager* theHManager);
    ~aligner(void);

    enum{nAlignPars=6};//0=alpha, 1=beta, 2=gamma, 3=deltaTx, 4=deltaTy, 5=deltaTz
    typedef struct detectorResults
    {
        double alpha   ;
        double beta    ;
        double gamma   ;
        double deltaTx ;
        double deltaTy ;
        double deltaTz ;
    } detectorResults, detectorResultsDef ;

    typedef std::map<std::string, detectorResultsDef>  alignmentResultsDef;

    bool                     align               (void                                                 );
    bool                     alignDUT            (void                                                 );
    bool                     calculateCorrections(std::string detectorName,
                                                  std::vector<double>&                               deltaPars,
                                                  ROOT::Math::SMatrix<double,nAlignPars,nAlignPars>& aTvaall,
                                                  ROOT::Math::SVector<double,nAlignPars>&            aTvinvrall,
                                                  Detector::matrix33Def&                             fRinv);
    void                     makeAlignMatrices   (ROOT::Math::SMatrix<double,nAlignPars,nAlignPars>& aTvaall,
                                                  ROOT::Math::SVector<double,nAlignPars>&            aTvinvrall,
                                                  ROOT::Math::SVector<double,4>&                     trackPars,
                                                  Detector::matrix33Def&                             fRinv,
                                                  double z,
                                                  double predX,
                                                  double predY,
                                                  double den,
                                                  double sigmaX,
                                                  double sigmaY,
                                                  double residualX,
                                                  double residualY                                     );
    void                      makeAlignMatricesStripsX   (ROOT::Math::SMatrix<double,nAlignPars,nAlignPars>& AtVA,
                                                        ROOT::Math::SVector<double,nAlignPars>&            AtVAInvR,
                                                        ROOT::Math::SVector<double,4>&                     trackPars,
                                                        Detector::matrix33Def&                             fRInv,
                                                        double z,
                                                        double predX,
                                                        double den,
                                                        double sigmaX,
                                                        double residualX
                                                        );
    void                      makeAlignMatricesStripsY   (ROOT::Math::SMatrix<double,nAlignPars,nAlignPars>& AtVA,
                                                        ROOT::Math::SVector<double,nAlignPars>&            AtVAInvR,
                                                        ROOT::Math::SVector<double,4>&                     trackPars,
                                                        Detector::matrix33Def&                             fRInv,
                                                        double z,
                                                        double predY,
                                                        double den,
                                                        double sigmaY,
                                                        double residualY
                                                        );
    bool                     execute             (void                                                 );
    int                      getMaxIterations    (void                                                 );
    alignmentResultsDef      getAlignmentResults (void                                                 ){return alignmentResults_;}
    HManager::stringVDef     getAlignmentPlots   (void                                                 ){return outputPlots_     ;}
    unsigned int             getFixParMap        (std::string detector                                 ){return parMap_[detector];}
    std::string              getLabel            (void                                                 );
    std::string              getName             (void                                                 ){return "aligner"        ;}

    void                     setAlignmentPreferences(int          maxtrial,
                                                     int          strategy,
                                                     double       chi2cut            = -1,
                                                     int          maxClusterSize     = -1,
                                                     int          minTrackPoints     = -1,
                                                     int          maxTracks          = -1,
                                                     bool         noDiagonalClusters = true,
                                                     std::string  DUT                = "",
                                                     int          nEvents            = -1);
    void                     setFixParMap       (std::string detector, int code                       ){parMap_[detector]=code                 ;}
    void                     clearFixParMap     (void                                                 ){parMap_.clear()                        ;}
    void                     setAlignmentFitMethodName   (std::string alignmentFitMethod              ){alignmentFitMethod_=alignmentFitMethod ;}
    //std::string              getAlignmentFitMethodName   (void                                        ){return fitMethodName_                  ;}
    void                     setNumberOfIterations       (int                   iterations            ){nIterations_ = iterations              ;}
    void                     setKalmanPlaneInfo          (KalmanPlaneInfo      kalmanInfo             ){theKalmanPlaneInfo_ = kalmanInfo    ;}

private :
     fileEater                                  * theFileEater_    ;
     HManager                                   * theHManager_     ;
     alignmentResultsDef                          alignmentResults_;
     HManager::stringVDef                         outputPlots_     ;
     std::map< std::string, unsigned int >        parMap_          ;
     KalmanPlaneInfo                              theKalmanPlaneInfo_;

     int          maxtrial_          ;
     int          strategy_          ;
     double       chi2cut_           ;
     int          maxClusterSize_    ;
     int          maxTracks_         ;
     int          minTrackPoints_    ;
     int          nEvents_           ;
     bool         noDiagonalClusters_;
     std::string  DUT_               ;
     std::string  alignmentFitMethod_;
     int          nIterations_       ;


     std::stringstream ss_ ;
} ;

#endif // ALIGNER_H
