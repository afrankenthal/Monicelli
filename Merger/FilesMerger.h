#ifndef _FilesMerger_h_
#define _FilesMerger_h_

#include "Run.h"
#include <string>
#include <vector>
#include <map>

class FilesMerger{
 public:
  FilesMerger();
  ~FilesMerger();
  void setStationNames   (const std::vector<std::string>& names){stationNames_ = names;}
  int  makeFilesList     (std::string  runNumber,std::string dir);
  int  makeFilesList     (unsigned int runNumber,std::string dir);
  int  makeStationBuffers(void);
  int  readStationBuffers(void);
  int  readFile          (void);
  int  merge             (void);
  int  readMergedFiles   (std::string fileName);
  int  writeMergedFiles  (std::string filesDirectory,std::string fileName = "");
  Run& getRun            (void){return theRun;}
  const std::multimap<std::string,std::string>& getFilesList     (void){return filesList_;}
  std::map<std::string,std::string>&            getStationBuffers(void){return stationBuffers_;}
 private:
  std::string                            runNumber_;
  std::multimap<std::string,std::string> filesList_;
  std::map<std::string,std::string>      stationBuffers_;
  std::vector<std::string>               stationNames_;
  Run                                    theRun;
};

#endif
