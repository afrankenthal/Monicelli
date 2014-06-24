#ifndef HManager_h
#define HManager_h

#include <vector>
#include <string>
#include <TString.h>
#include "DataClass.h"
#include "Cuts.h"
#include "Window.h"
#include "Plane.h"
#include "CommonVariables.h"

class TH1F;
class TH2F;
class TTree;
class TFile;
class TGraphAsymmErrors;
class TGraph;
//class Plane;

class HManager
{
 public:
  HManager (TTree* tree=0);
//  HManager (const HManager& hManager);
//  HManager& operator=(const HManager& hManager);
  ~HManager(void);
  
  void openFile         (TString fileName  );
  void writeAndCloseFile(void              );
  void closeFile        (void              );
  void setHistos        (TH2F* histos,int i);
  void book             (void              );
  void fill             (void              );
  void setupTree        (TTree* tree       );
  void endJob           (void              );
  void setWindow        (TTree* tree       );
  
 private:
  int allTrackProj            (Plane& planeN);
  int planeHits               (Plane& planeN);
  int planeWindowHits         (Plane& planeN);
  int clusterSizeDistribution (Plane& planeN);
  int charge                  (Plane& planeN);
  int predictedErrs           (Plane& planeN);
  int eventResiduals          (Plane& planeN);
  int chargeResidualPlot      (Plane& planeN);    
  int planeEfficiency         (Plane& planeN);
  int cellEfficiency          (Plane& planeN);  

  TFile*    outFile_         ;
  TTree*    theTree_         ;
  DataClass theData_         ;
  Cuts      theCuts_         ;
  Plane     planes_[NPLANES] ;
  TH2F*     calibHistos2D_[4];  
  std::vector<std::string> stationNames;
  std::vector<std::string> planeNames  ; 
  std::vector<std::string> planeIDs    ;
  int nEvents_;

  TH2F* hAllTrackProjXY_       [NPLANES];
  TH2F* hAllTrackProjPix_      [NPLANES];
  TH2F* hPlaneHitsXY_          [NPLANES];
  TH2F* hPlaneHitsPix_         [NPLANES];
  TH2F* hPlaneWindowHits_      [NPLANES];
  //TH2F* h2DNegCharge_              [NPLANES];
  TH2F* h2DChargeXResidual1Pxl_[NPLANES];
  TH2F* h2DChargeYResidual1Pxl_[NPLANES];
  TH2F* h2DChargeXResidual2Pxl_[NPLANES];
  TH2F* h2DChargeYResidual2Pxl_[NPLANES];
  TH2F* h2DEfficiency_         [NPLANES];
  TH2F* h2DInefficiency_       [NPLANES];
  TH2F* h2DEfficiencyDenom_    [NPLANES];
  TH2F* h2DCellEfficiencyDenom_[NPLANES];
  TH2F* h2DCellEfficiency_     [NPLANES];
  TH2F* h2DCellInefficiency_   [NPLANES];
  TH2F* h2DCellCharge_         [NPLANES];
  TH2F* h2DCellChargeDenom_    [NPLANES];

  TH1F* hClusterSize_          [NPLANES];
  TH1F* h1PxlCharge_           [NPLANES];
  TH1F* h2PxlCharge_           [NPLANES];
  TH1F* hXresidOnePxl_         [NPLANES];   
  TH1F* hYresidOnePxl_         [NPLANES];
  TH1F* hXresidTwoPxl_         [NPLANES];    
  TH1F* hYresidTwoPxl_         [NPLANES];   
  TH1F* hEfficiency_           [NPLANES];
  TH1F* hEfficiencyDenom_      [NPLANES];
  TH1F* hPredictedXpError_     [NPLANES];
  TH1F* hPredictedYpError_     [NPLANES];
  TH1F* hNTrack;           
  TH1F* hChi2_;		           
  TH1F* hChi2Excl_;  

};
#endif
