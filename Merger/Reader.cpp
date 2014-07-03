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
