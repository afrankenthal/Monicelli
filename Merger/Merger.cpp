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

//////////////////////////////////////////////////////////////////////////////////
//                    WARNING
// We are assuming:
// 1) The file names must be like this: Run1_ . That is they start with Run, then there is the run number and then an underscore (int FilesMerger::makeFilesList(string runNumber,std::string dir))
// 2) The 3 stations must be named dut, downstream and upstream                                                                  (int FilesMerger::makeFilesList(string runNumber,std::string dir))
// 3) The files are contained in 3 subdirectories called CC_DUT/Burst Archive, CC_DS/Burst Archive, CC_US/Burst Archive          (int FilesMerger::makeFilesList(string runNumber,std::string dir))
// 4)The station numbers are dut => station = 4, upstream => station = 2 and downstream => station = 0                           (int FilesMerger::merge(void))
//
///////////////////////////////////////////////////////////////////////////////////

const string dutName        = "digital";
const string telescope1Name = "downstream";
const string telescope2Name = "upstream";
const string fed0Name       = "FED0";
const string fed1Name       = "FED1";
const string fed2Name       = "FED2";

int main(int argc, char **argv)
{

  if(getenv("MERGER_INPUT_DIR") == 0 || getenv("MERGER_OUTPUT_DIR") == 0)
  {
     cout << __PRETTY_FUNCTION__ << "You need to source setup.csh" << endl;
     exit(EXIT_FAILURE);
  }
  
  const string inputFilesDir  = getenv("MERGER_INPUT_DIR");
  const string outputFilesDir = getenv("MERGER_OUTPUT_DIR");

  int firstRunNumber, lastRunNumber;
  
  if(argc < 2 || argc >3)
  {
     cout << __PRETTY_FUNCTION__ << "\tUsage: ./Merger runNumber or ./Merger firstRunNumber lastRunNumber" << endl;
     exit(EXIT_FAILURE);
  }
  else if( argc == 2 )
  {
    firstRunNumber = atoi(argv[1]);
    lastRunNumber  = atoi(argv[1]);
  }
  else if( argc == 3 )
  {
    firstRunNumber = atoi(argv[1]);
    lastRunNumber  = atoi(argv[2]);
  }
  

  
  FilesMerger theFilesMerger;
  vector<string> stationNames;
  stationNames.push_back(telescope1Name);
  stationNames.push_back(telescope2Name);
  stationNames.push_back(dutName);
  stationNames.push_back(fed0Name);
  stationNames.push_back(fed1Name);
  stationNames.push_back(fed2Name);
  theFilesMerger.setStationNames(stationNames);

  for(int runNumber=firstRunNumber; runNumber<=lastRunNumber; runNumber++)
  {
    cout << __PRETTY_FUNCTION__ << "\tMerging run " << runNumber << endl;
    if( theFilesMerger.makeFilesList(runNumber,inputFilesDir) == -1) continue;
    
    theFilesMerger.makeStationBuffers();
    theFilesMerger.merge();
    theFilesMerger.writeMergedFiles(outputFilesDir,"");
  }
  
  return EXIT_SUCCESS;
}
