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
