/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef CLUSTERIZER_H
#define CLUSTERIZER_H

#include "math.h"
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

//#include "process.h"

#include "Event.h"
#include "Geometry.h"
#include "EventHeader.h"
#include "subProcess.h"

class clusterizer : public subProcess<Event,Geometry>
{

public:
    clusterizer(void);
    ~clusterizer(void);

    void                       clear           (void                                    );
    //  void                       execute         (void                                    );
    Event::clustersHitsMapDef  findClusters    (Event * theEvent                        );
    Event::clustersMapDef      makeClusters    (Event * theEvent, Geometry * theGeometry);
    void                       clusterize      (Event * theEvent, Geometry * theGeometry);
    void                       execute         (Event * theEvent, Geometry * theGeometry);
    void                       setClustersBuilt(int     built                           ) {clustersBuilt_ = built;}
    bool                       clustersBuilt   (void                                    ) {return clustersBuilt_ ;}
    std::string                getLabel        (void                                    ) ;
    std::string                getName         (void                                    ) {return "clusterizer"        ;}
    void                       setHeader       (EventHeader * theHeader                 ) {theHeader->setClustersDone(clustersBuilt_);}
    void                       getChargeAsymmetryPlots(Geometry * theGeometry);
    void                       setUseEtaFunction(bool use                               ){useEtaFunction_ = use;}

private:

    std::string getPlaneID (int station, int plaquette);

    //        station             ROCID           X|Y       pars[]
    std::map<std::string,std::map<int,std::map<std::string,double* > > >chargeAsymmetryPars_;

    Event::clustersHitsMapDef clustersHitsMap_     ;
    Event::clustersMapDef     clustersMap_         ;

    Event*                    theEvent_            ;
    Geometry*                 theGeometry_         ;

    bool                      clustersBuilt_       ;
    bool                      useEtaFunction_      ;

    std::stringstream         ss_                  ;
};

#endif // CLUSTERIZER_H
