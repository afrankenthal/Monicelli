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
 
#ifndef _clusterizer_h_
#define _clusterizer_h_

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
