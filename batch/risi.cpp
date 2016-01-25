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
 
// g++ -o risi -I../include -I${BOOSTINC}/include/boost -L${BOOSTLIB} -lboost_regex -lboost_filesystem risi.cpp `root-config --cflags --libs` && ./risi

#include "alphanum.h"
#include <algorithm>
#include <iostream>
#include <vector>

#include "stdio.h"
#include "dirent.h" 
#include "stdlib.h"
#include "sys/stat.h"

#include "Event.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>

#include <TApplication.h>
#include <TFile.h>
#include <TRandom.h>
#include <TTree.h>

#include "MessageTools.h"
#include "fileEater.h" 

typedef std::map<std::string, std::string, doj::alphanum_less<std::string> > fileListDef ;

std::string rawDataPath = "/gwpool/users/uplegger/data/Merges/" ;

fileListDef getFileList(void)        ;
void        parseFiles (fileListDef) ;

//==================================================================
int main(int argc, char** argv)
{
  TApplication app("App",&argc, argv);
 
  parseFiles(getFileList()) ;
 
  app.Run() ;
  
  return 0 ;
}

//==================================================================
fileListDef getFileList() 
{
  fileListDef fileList ;  
  struct dirent *entry; 
  DIR* p_startingDirectory; 

  p_startingDirectory = opendir(rawDataPath.c_str()); 
                                       
  if (p_startingDirectory == NULL) 
  {
    perror("opendir");
  }
 
  while((entry = readdir(p_startingDirectory))) 
  {
    std::string fileName = entry->d_name ;
    if( fileName == "." || fileName == ".."            ) continue ;
    if( fileName.find("filepart") != std::string::npos ) continue ;
    fileList[fileName] = fileName ;
  }
  return fileList ;
 }
 
//==================================================================
void parseFiles(fileListDef fileList)
{
 static const boost::regex exp(".dat");
 
 for(fileListDef::iterator it=fileList.begin(); it!=fileList.end(); ++it)
 {
   fileEater * theFileEater_ = new fileEater() ;
   std::string inpFileName = rawDataPath + it->first ;
   std::string outFileName = "./dat/" + regex_replace(it->first, exp, ".root") ;
   std::stringstream ss ;
   ss << "Parsing " << inpFileName << " and saving to " << outFileName ;
   STDLINE(ss.str(),ACBlue) ;
   
   theFileEater_->setOutputTreePath("./dat") ;
   theFileEater_->openFile( inpFileName) ;
   theFileEater_->parse() ;
   delete theFileEater_ ;
 }
}
