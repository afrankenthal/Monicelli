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

   bool growFakeTree (std::string   inputFileName , 
                      std::string   outputTreePath,
                      std::string   outputTreeName, 
                      Event       * theEvent      , 
                      Geometry    * theGeometry    );

   bool growFakeTree2(int           trackNum      ,
                      double        xErr          ,
                      double        yErr          ,
                      std::string   outputTreePath,
                      std::string   outputTreeName, 
                      Event       * theEvent      , 
                      Geometry    * theGeometry    );

   void setErrors    (double        xErr_um, 
                      double        yErr_um        ) {
                                                      xErr_ = xErr_um; 
                                                      yErr_ = yErr_um;
                                                     }

private:

    double            xErr_;
    double            yErr_;

    std::stringstream ss_  ;
};

#endif // BEAMSIMULATOR_H
