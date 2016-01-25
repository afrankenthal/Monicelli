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
 
#include "FilesMerger.h"
#include <map>
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

const bool writeOutFile = true;

//const string filesDirectory = "../../2012_04_April/UTCA/";
const string filesDirectory = "../../2012_04_April/Merged/";
const string dutName        = "dut";
const string telescope1Name = "downstream";
const string telescope2Name = "upstream";


//map<string,string> mergeStationFiles(multimap<string,string> files);



int main(int argc, char **argv)
{
  string runNumber = "1";
  FilesMerger theFilesMerger;
  theFilesMerger.readMergedFiles(filesDirectory+"Run"+runNumber+"_Merged.dat");
//  theFilesMerger.writeMergedFiles("Run"+runNumber+"_Read.dat");
  return EXIT_SUCCESS;
  vector<string> stationNames;
//  stationNames.push_back(telescope1Name);
//  stationNames.push_back(telescope2Name);
  stationNames.push_back(dutName);
  theFilesMerger.setStationNames(stationNames);
  theFilesMerger.makeFilesList(runNumber,filesDirectory);
  theFilesMerger.makeStationBuffers();
  theFilesMerger.readStationBuffers();
  return 0;
}
