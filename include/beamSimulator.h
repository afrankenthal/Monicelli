/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef BEAMSIMULATOR_H
#define BEAMSIMULATOR_H

#include "math.h"
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <TBranch.h>
#include <TFile.h>
#include <TFolder.h>
#include <TH2I.h>
#include <TRandom.h>
#include <TROOT.h>
#include <TTree.h>
#include <vector>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>

#include "Event.h"
#include "Geometry.h"

class beamSimulator
{

public:
    beamSimulator(void) {;}
   ~beamSimulator(void) {;}

   bool growFakeTree (std::string inputFileName, std::string outputTreePath ,
                      std::string outputTreeName, Event *theEvent, Geometry *theGeometry);

   bool growFakeTree2(int trackNum, double xErr, double yErr, std::string outputTreePath ,
                      std::string outputTreeName, Event *theEvent, Geometry *theGeometry  );

   void setErrors   (double xErr_um, double yErr_um ) {xErr_=xErr_um; yErr_ = yErr_um;}

private:

    double xErr_;
    double yErr_;

    std::stringstream                ss_              ;
};

#endif // BEAMSIMULATOR_H
