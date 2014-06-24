/****************************************************************************
** Authors: Lorenzo Uplegger, Luigi Vigani
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#ifndef __StripTestReader_H__
#define __StripTestReader_H__

#include "Detector.h"
#include "TTree.h"
#include <stdint.h>
#include <map>

class EventReader;
class Event;
class EventHeader;
class Geometry;
class TFile;
class TTree;
class PxEvent;
class TH1F;
class TH2F;

#define MAX_TRACKS   10
#define MAX_HITS     20
#define MAX_CLUSTERS 20

typedef struct 
{
  unsigned int trigger;
  unsigned int numberOfTracks;
  unsigned int numberOfHits[MAX_TRACKS];
  double       chi2        [MAX_TRACKS];
  double       xSlope      [MAX_TRACKS];
  double       ySlope      [MAX_TRACKS];
  double       xIntercept  [MAX_TRACKS];
  double       yIntercept  [MAX_TRACKS];
  double       xError      [MAX_TRACKS];
  double       yError      [MAX_TRACKS];
} TrackEvent;

typedef struct
{
  unsigned int       trigger	  ; 
  unsigned int       stripTrigger   [MAX_HITS];
  unsigned int       triggerBco     [MAX_HITS];  
  unsigned long long longBco        [MAX_HITS];
  int                numberOfHits;
  int 	             chan	    [MAX_HITS];
  int 	             chipid	    [MAX_HITS];
  int 	             set	    [MAX_HITS];
  int 	             strip	    [MAX_HITS];
  int                istrip	    [MAX_HITS];
  int 	             bco	    [MAX_HITS]; 
  int 	             adc	    [MAX_HITS]; 
  int                numberOfClusters;
  int                clusterSize    [MAX_CLUSTERS]; 
  int                clusterAdc	    [MAX_CLUSTERS]; 
  int                clusterCharge  [MAX_CLUSTERS]; 
  double             clusters	    [MAX_CLUSTERS]; 
  double             measuredX      [MAX_CLUSTERS]; 
  double             measuredY      [MAX_CLUSTERS]; 
  double             measuredLocalX [MAX_CLUSTERS]; 
  double             measuredLocalY [MAX_CLUSTERS]; 
  double             projectedX     [MAX_CLUSTERS]; 
  double             projectedY     [MAX_CLUSTERS]; 
} StripEvent;

class StripTestReader
{
public:
   StripTestReader(EventReader* reader);
  ~StripTestReader(void);
  
  void analyzeEvent(unsigned int event) ;
  void getStripEvents();

  int computeSensorStrip(int chip,int set,int strip);

private: 
  Detector      stripPlane_ ;
  Event       * theEvent_   ;
  EventHeader * theHeader_  ;
  Geometry    * theGeometry_;
   
  TFile*     theFile_	   ;
  TTree*     theTrackTree_ ;
  TTree*     theStripTree_ ;
  TrackEvent theTrackEvent_;
  StripEvent theStripEvent_;
   
  std::map<uint64_t,PxEvent*> memory_;
  bool                        bcoIsComplete_;
  uint64_t                    currentBCO_;
  uint64_t                    currentTriggerBCO_;
  uint32_t                    currentTrigger_;
  int                         runNumber_;
  int                         pxSuiteRunNumber_;

  TH2F* hCorrelation_;
  TH1F* hProjectedErrorX_;
  TH1F* hProjectedErrorY_;
  TH1F* hResidual_;
  TH1F* hResidualY_;
  TH2F* h2DClusterSizeVsAdc_;
  TH2F* h2DClusterSizeVsCharge_;
  TH2F* h2DClusterSizeVsChargeMatched_;
  TH2F* h2DResidualXVsX_;
  TH2F* h2DResidualXVsY_;
  TH1F* hResidualXVsX_;
  TH1F* hResidualXVsY_;
  TH1F* hResidualNormXVsX_;
  TH1F* hResidualNormXVsY_;
  TH1F* hResidualSize1_;
  TH1F* hResidualSize1Adc7_;
  TH1F* hResidualSize1Adc6_;
  TH1F* hResidualSize1Adc5_;
  TH1F* hResidualSize1Adc4_;
  TH1F* hResidualSize1Adc3_;
  TH1F* hResidualSize1Adc2_;
  TH1F* hResidualSize1Adc1_;
  TH1F* hResidualSize1Adc0_;
  TH1F* hResidualSize2_;
  TH1F* hResidualSize3_;
  TH1F* hMeasuredProfile_;
  TH1F* hStripProfile_;
  TH1F* hClusterProfile_;
  TH1F* hBeamProfileX_;
  TH1F* hBeamProfileY_;
  TH1F* hAssociatedTriggers_;
  TH1F* hAdc_;
  TH1F* hNumberOfHits_;
  TH1F* hNumberOfClusters_;
  TH1F* hClusterSize_;
  TH1F* hClusterSizeMatched_;
  TH1F* hDeltaBco_;
  TH1F* hChargeAsymmetry_;
  TH1F* hChargeAsymmetryNorm_;
  TH2F* h2DChargeAsymmetry_;
  TH1F* hEfficiency_;
  TH1F* hEfficiencyX_;
  TH1F* hEfficiencyVsTrigger_;
  TH1F* hAdcX_;
  TH1F* hEfficiencyNorm_;
  TH1F* hEfficiencyXNorm_;
  TH1F* hAdcXNorm_;
  TH1F* hEfficiencyVsTriggerNorm_;
  int lastTriggerInTelescope_;


};

#endif
