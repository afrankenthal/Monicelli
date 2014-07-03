#include "FilesMerger.h"
#include "Hit.h"
#include "Event.h"
#include "DataDecoder.h"
#include "PxEvent.h"
#include <TH1F.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <ctime>
#include <errno.h>
#include <dirent.h>
#include <cstdlib>
#include <stdint.h>
#include <algorithm>
#include <cmath>
#include <arpa/inet.h>

using namespace std;

//-----------------------------------------------------------------------
FilesMerger::FilesMerger(){
}

//-----------------------------------------------------------------------
FilesMerger::~FilesMerger(){
}

//-----------------------------------------------------------------------
int FilesMerger::makeFilesList(unsigned int runNumber,std::string dir){
  stringstream runN; runN.str("");
  runN << runNumber;
  return makeFilesList(runN.str(),dir);
}

//-----------------------------------------------------------------------
int FilesMerger::makeFilesList(string runNumber,std::string dir){
  std::multimap<std::string,std::string> tmpFilesList;
  filesList_.clear();

  runNumber_ = runNumber;
  if( stationNames_.size() == 0 )
  {
    cout << __PRETTY_FUNCTION__ << "You need to set the station names first!" << endl;
    return -1;
  }
  
  DIR* pDir;
  struct dirent* pDirEntries;
  for(unsigned int station=0; station<stationNames_.size(); station++)
  {
    //cout << __PRETTY_FUNCTION__<< stationNames_[station] << endl;
    string subDir;
    if(stationNames_[station] == "dut")
    {
      subDir = dir+"Raw/CC_DUT/Burst Archive";
    }
    else if(stationNames_[station] == "downstream")
    {
      subDir = dir+"Raw/CC_DS/Burst Archive";
    }
    else if(stationNames_[station] == "upstream")
    {
      subDir = dir+"Raw/CC_US/Burst Archive";
    }
    else if(stationNames_[station] == "digital")
    {
      subDir = dir+"Raw/CC_STIB_MASTER/Burst Archive";
    }
    else if(stationNames_[station].find("FED") != string::npos)
    {
      subDir = dir+"PxSuiteDataRenamed/";
    }
    pDir=opendir(subDir.c_str());
    if( !pDir )
    {
      cout << __PRETTY_FUNCTION__ << "\tOpendir() failure; can't open dir: " << subDir << endl;
      return -1;
    }
    errno=0;															  				       
    while( (pDirEntries=readdir(pDir)) )
    {								 
      string fileName = pDirEntries->d_name;
//  	cout << fileName << endl;
      if((int)fileName.find("Run"+runNumber+"_") == 0 && fileName.find(stationNames_[station]) != string::npos)
      {
    	 //cout << __PRETTY_FUNCTION__ << fileName << " station name: " << stationNames_[station]<< endl;
	 tmpFilesList.insert(pair<string,string>(stationNames_[station],subDir+"/"+fileName));
      }
    }																  				       
  
    if (errno){ 														       
      cout << __PRETTY_FUNCTION__ << "\tReaddir() failure; terminating" << endl;      
      return -1;														  	       
    }																  			       
    closedir(pDir);
  }
  vector<string> files;
  for(unsigned int station=0; station<stationNames_.size(); station++)
  {
    pair<multimap<string,string>::iterator,multimap<string,string>::iterator> stationRange;
    stationRange = tmpFilesList.equal_range(stationNames_[station]);
    files.clear();
    for(multimap<string,string>::iterator fileIt=stationRange.first; fileIt != stationRange.second; fileIt++)
    {
      files.push_back(fileIt->second);
    }
    sort( files.begin(), files.end() );
    for(vector<string>::iterator fileIt=files.begin(); fileIt!=files.end(); fileIt++)
    {
      filesList_.insert(pair<string,string>(stationNames_[station],*fileIt));
      cout << __PRETTY_FUNCTION__ << "Using file: " << *fileIt << endl;
    }
  }
  if(filesList_.size() == 0)
  {
    cout << __PRETTY_FUNCTION__ << filesList_.size() << endl;
    return -1;
  }
  return 0;
}

//-----------------------------------------------------------------------
int FilesMerger::makeStationBuffers(void){
  if(filesList_.size() == 0)
  {
    cout << __PRETTY_FUNCTION__ << "\tYou need to make the files list first!" << endl;
    return -1;
  }
  stationBuffers_.clear();
//  for(multimap<string,string>::iterator fileIt=filesList_.begin(); fileIt != filesList_.end(); fileIt++){
//    cout << __PRETTY_FUNCTION__ << fileIt->first << ":" << fileIt->second << endl;
//  }
  for(unsigned int station=0; station<stationNames_.size(); station++)
  {
    pair<multimap<string,string>::iterator,multimap<string,string>::iterator> stationRange;
    stationRange = filesList_.equal_range(stationNames_[station]);
    string stationBuffer = "";
    for(multimap<string,string>::iterator fileIt=stationRange.first; fileIt != stationRange.second; fileIt++)
    {
      //cout << __PRETTY_FUNCTION__ <<"Station: "<< fileIt->first << "-> File: " << fileIt->second << endl;
      ifstream file;
      file.open(fileIt->second.c_str());
      if(!file.is_open())
      {
        cout << __PRETTY_FUNCTION__ << "Can't open file: " << fileIt->second.c_str() << endl;
      }
      
      std::vector<char> data = std::vector<char>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
      int dataLength  = 0;
      
      //cout << __PRETTY_FUNCTION__ << "Data length begin: " << data.size() << endl;
      std::vector<char>::iterator dataIt = data.begin();
      if(stationNames_[station].find("FED") != string::npos)
        dataLength  = data.size();
      else 
      {
        unsigned char blockLengthC;
        int           blockLength = -1;//Tells you how many 8 bytes words are written in a block
      	while(blockLength != 0)
      	{
//    	  for(int f=0; f<10; f++){
      	  //file.read((char*)(&blockLengthC),sizeof(char));
      	  blockLengthC = *dataIt;
	  blockLength = (int)blockLengthC;
	  //cout << __PRETTY_FUNCTION__ << blockLength << endl;
	  if(blockLength > 0)
	  {
	    dataIt += blockLength*sizeof(unsigned long long)+1; 
	    dataLength += blockLength*sizeof(unsigned long long)+1; 
	    //file.seekg(dataLength);
	  }
      	}
      }
      file.seekg(0,ios::beg);
      char* charBuffer = new char[dataLength];
      //cout << __PRETTY_FUNCTION__ << "Data length: " << dataLength << endl;
      file.read(charBuffer,dataLength);
      stationBuffer.append(charBuffer,dataLength);
      delete charBuffer;
      file.close();
    }
    stationBuffers_[stationNames_[station]] = stationBuffer;
//    break;
  }
  return 0;
}

//-----------------------------------------------------------------------
int FilesMerger::merge(void)
{
  const int          wrapAround  = 1048576;//2^20
  const unsigned int maxEvents  = -1;
  const unsigned int maxSeconds = 100;
  unsigned char      blockLengthC;
  int                blockLength = -1;
  unsigned long long currentTimestamp = 0;
  DataDecoder        theDataDecoder;
  string             stationName;
  int                correction;
  map<string, vector<Event*> > stationsEvents;
  map<string, unsigned int>    eventCounters;
  std::map<uint64_t,PxEvent*> memory_;


  for(map<string,string>::iterator buffer=stationBuffers_.begin(); buffer!=stationBuffers_.end(); buffer++)
    eventCounters[buffer->first] = 0;

  theRun.reset();
  for(map<string,string>::iterator buffer=stationBuffers_.begin(); buffer!=stationBuffers_.end(); buffer++)
  {
    stationName = buffer->first;
    cout << __PRETTY_FUNCTION__ << "Building events for station " << stationName << endl;

    unsigned int    bufferPos      = 0;
    Event*          currentEvent   = 0;
    unsigned int    currentTrigger = 0;
    unsigned int    lastTrigger    = (unsigned int)-1;
    unsigned int    uniqueTrigger  = 0;
    uint64_t        currentTriggerBCO_;
    
    vector<Event*>* stationEvents  = 0;
    stationsEvents[buffer->first]  = vector<Event*>();
    stationEvents = &(stationsEvents[buffer->first]);
    int station = -1;
    if(      stationName == "dut" || stationName == "digital")     station = 4;
    else if( stationName == "upstream" )   station = 2;
    else if( stationName == "downstream" ) station = 0;
    else if( stationName == "FED0") station = 5;
    else if( stationName == "FED1") station = 6;
    else if( stationName == "FED2") station = 7;
    else
    {
      cout << __PRETTY_FUNCTION__ << "\tUnrecognized station name: " << stationName << endl;
      exit(EXIT_FAILURE);
    }
    
    if(station == 5 || station == 6 || station == 7)
    {
      const int    dataTriggerDelay = 57; 
      uint64_t     currentBCO_;
      unsigned int word;		     
      uint64_t     lastBCO = 0ULL;
      //struct stat statbuf;
      uint32_t     trigger;
      uint32_t     correctedTrigger = -1;
      uint64_t     previousTriggerBco = -1;
      std::cout << __PRETTY_FUNCTION__ << "File size: " << buffer->second.size() << std::endl;
      int len = buffer->second.size() - 8;
   
      
      int type, dataType;
      bool bcoIsComplete_ = false;
      
      int dataState = 3;
      int triggerState = 2;
      int whatTriggeredLongBCO = -1;
      int deltaBCOWhenTriggered = 0; 
      string dataName;
      
      //cout << "Name : " << buffer->second[0] <<  buffer->second[1] << buffer->second[2] <<  buffer->second[3]<< endl; 
      //cout << "Version: " << hex << (buffer->second.c_str()[4]) << dec << endl; 
      for ( int i=0; i<len; i+=sizeof(unsigned int) ) 
      //for ( int i=0; i<20000; i+=sizeof(unsigned int) ) 
      { 								  
        //word = ntohl(((unsigned int*)(buffer->second.c_str()))[i/4+8]);
        //word = ((unsigned int*)(buffer->second.c_str()))[i+8];
	//cout << hex << word << dec << endl;
	word = (((buffer->second[i+8])&0xff) << 24) + (((buffer->second[i+9])&0xff) << 16) + (((buffer->second[i+10])&0xff) << 8) + ((buffer->second[i+11])&0xff);
	//cout << hex << word << dec << endl;
	
        type	 = word & 0xf;
        dataType = (word>>4) & 0xf; 		   
      
        if     (type == 8 && dataType == 0x1) dataName = "Low BCO";
        else if(type == 8 && dataType == 0x2) dataName = "High BCO";
        else if(type == 8 && dataType == 0xa) dataName = "Low trigger";
        else if(type == 8 && dataType == 0xb) dataName = "High trigger";
        else if(type == 8 && dataType == 0xc) dataName = "Trigger type";
        else if((type & 0x1) == 1 )	      dataName = "Strip";
        else dataName = "Unknown type";
      
//        if(type == 8 && dataType == 0x1)
//            cout << "------- END EVENT ------" << endl << endl << "--------- BEGIN EVENT -----------" << endl; 
	//cout << hex << "Type: " << type << " dataType: " << dataType << " data name: " << dataName << dec << endl;
        if(dataState == 2 && triggerState == 0 && type == 8 && dataType == 1)
          cout << "***************************ERROR: I must have a data or a trigger after a bco is complete!" << endl;
        if( (type & 0x1) == 1 )//Strip 
        {
          if(!(dataState == 2 || dataState == 3))
            cout << "ERROR: Bco is incomplete! dataState=" << dataState << endl;
          dataState = 3;
          
	  
	  //int 	trig   ;
          Hit hit;

          hit.data      = word;
          unsigned long long rocType = 1;//Bits 63->60 are the ROC type => 0 pixels 1 strip
          unsigned long long longStation = station;
          unsigned long long triggerNumber = 0xfffff;
          unsigned long long reset = 0x00000000ffffffff;
          hit.data &= reset;
          hit.data += ((rocType & 0xf) << 60) + ((longStation & 0xf) << 56) + ((triggerNumber & 0xfffff) << 32);
          //hit.data += ((longStation & 0xf) << 56);
          unsigned long long tmpData = 0;
          unsigned long long orderedData = 0;
	  int                dataSize = sizeof(unsigned long long);
	  for(int byte=0; byte < dataSize; byte++)
          {
              tmpData = ((hit.data >> ((dataSize-byte-1)*8))& 0xff) << (byte*8);
              orderedData += tmpData;
          }
	  hit.data      = orderedData;
	  hit.plaq	= (word>>27) & 0x07;
          hit.cid	= (word>>24) & 0x07;
	  //hit.set	= (word>>12) & 0x1f;
          //hit.strip	= (word>>17) & 0x0f;
          hit.bco	= (word>>4 ) & 0xff;
          
	  hit.adc	= (word>>1 ) & 0x7;
          //hit.istrip	= computeSensorStrip(hit.chipid, hit.set, hit.strip);

          unsigned int lowBCO	= (currentBCO_ & 0xffULL);
          uint64_t     eventBCO = (currentBCO_ & 0xffffffffffffff00ULL) + hit.bco;
          if(lowBCO < hit.bco)
            eventBCO -= 0x100;
          if(whatTriggeredLongBCO == -1)
          { 
            whatTriggeredLongBCO = 0;
            currentBCO_ -= 20;
          }
          if((int)(eventBCO - currentBCO_) < 0) eventBCO += 0x100;
	  deltaBCOWhenTriggered = eventBCO - currentBCO_;

          PxEvent* event;
          if(memory_.find(eventBCO) == memory_.end() 
	  && memory_.find(eventBCO-1) == memory_.end() 
	  && memory_.find(eventBCO+1) == memory_.end() 
	  && memory_.find(eventBCO-2) == memory_.end()
	  && memory_.find(eventBCO+2) == memory_.end()
	  && memory_.find(eventBCO+3) == memory_.end()
	  && memory_.find(eventBCO-3) == memory_.end()
	  && memory_.find(eventBCO+4) == memory_.end()
	  && memory_.find(eventBCO-4) == memory_.end()
	  )
	  {
            //cout << hex << "NO MATCH -> Delta: " << eventBCO - currentBCO_ << " -- Trigger number " << currentTrigger_ << " Event BCO: " << eventBCO << dec << endl;
            memory_[eventBCO] = new PxEvent(eventBCO);
            event = memory_[eventBCO];
	  }
          else
          {
            //cout << hex << "!! MATCH -> Delta: " << eventBCO - currentBCO_ << " -- Trigger number " << currentTrigger_ << " Event BCO: " << eventBCO << dec << endl;
            //cout << "Matched BCO! For trigger Number " << currentTrigger_ << endl;
            if     (memory_.find(eventBCO  ) != memory_.end()) event = memory_[eventBCO];
            else if(memory_.find(eventBCO-1) != memory_.end()) event = memory_[eventBCO-1];
            else if(memory_.find(eventBCO+1) != memory_.end()) event = memory_[eventBCO+1];
            else if(memory_.find(eventBCO-2) != memory_.end()) event = memory_[eventBCO-2];
            else if(memory_.find(eventBCO+2) != memory_.end()) event = memory_[eventBCO+2];
            else if(memory_.find(eventBCO-3) != memory_.end()) event = memory_[eventBCO-3];
            else if(memory_.find(eventBCO+3) != memory_.end()) event = memory_[eventBCO+3];
            else if(memory_.find(eventBCO-4) != memory_.end()) event = memory_[eventBCO-4];
            else if(memory_.find(eventBCO+4) != memory_.end()) event = memory_[eventBCO+4];
	  }
      	  event->setRawHit(word);
      	  event->setDecodedHit(hit);
//    	  //  cout << "- Delta: " << deltaBCOWhenTriggered << "----- EVENT DATA BCO: " << hex << eventBCO << " - " << hit.bco << " curr bco: " << currentBCO_ << dec << " strip: " << hit.istrip << endl;  
      	  ////cout << "EVENT DATA Strip data-> Bco: " << hex << hit.bco << dec
      	  //	 << " istrip: " << hit.istrip
      	  //	 << " adc: " << hit.adc
      	  //	 << endl;
        }
        else if(type == 8) 
        {
   	  dataType  = (word>>4) & 0xf;		
   	  if(dataType == 1) 
   	  {
   	    if(dataState == 1)//LOW BCO
   	      cout << "ERROR: Can't be. I already am in dataState 1! dataState=" << dataState << endl;
   	    dataState = 1;
   	    triggerState = 0;

   	    lastBCO = currentBCO_;
   	    currentBCO_ = (uint64_t)((word >> 8) & 0xffffff);
   	  }								
   	  else if(dataType == 2)//HIGH BCO
   	  {
   	    if(dataState != 1)
   	      cout << "ERROR: I must have received the first part of the BCO already! dataState=" << dataState << endl;
   	    dataState = 2;
 	  	    whatTriggeredLongBCO = -1;

   	    currentBCO_ |= ((uint64_t)((word >> 8) & 0xffffff))<<24;
          //cout << "BCO: " << hex << currentBCO_ << dec << endl;
          //cout << endl << "BEGIN EVENT: " << hex << currentBCO_ << dec << endl;
          //std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
      	 }
      	 else if ( dataType == 0xa )//LOW TRIGGER
      	 {			       
      	   if(triggerState != 2 && triggerState != 0)
      	     cout << "ERROR: Impossible this must be the first part of the trigger! triggerState=" << triggerState << endl;
      	   triggerState = 1;
      	   trigger = (word>>16) & 0xffff;				       
      	   currentTriggerBCO_ = (word>>8) & 0xff;
      	   //cout << "Trigger Bco:" << currentTriggerBCO_ << " corrected: " << correctedTrigger << endl;				       
      	   if(false && ((int)(currentTriggerBCO_-previousTriggerBco) == 1 || (int)(currentTriggerBCO_-previousTriggerBco) == -255))
      	   {
      	     cout << "~~~~~~~~~~~~~~~~~~~~~~~~ corrected: " << correctedTrigger << endl;				       
      	     currentTriggerBCO_ = previousTriggerBco;
      	     triggerState = -1;
      	   }
      	   else
      	   {						    
      	     //++correctedTrigger;
      	   }	   
      	   //std::cout << "Trigger low = " << trignum_low << ", bco = " << hex << theStripEvent_.bco << std::endl;	     
      	 }							       
      	 else if( dataType == 0xb )//HIGH TRIGGER
      	 {	       
      	   if(triggerState == -1)
      	     continue;  	       
      	   if(triggerState != 1)
      	     cout << "ERROR: I must have received the first part of the trigger already! triggerState=" << triggerState << endl;
      	   triggerState = 2;

	   if(whatTriggeredLongBCO == -1)
	   { 
	     whatTriggeredLongBCO = 1;	       
	     currentBCO_ -= 63;
	   }

      	   trigger |= ((word>>8) & 0xffffff) << 16;
	   correctedTrigger = trigger;
      	   //cout << "Trigger " << trigger << " corrected: " << correctedTrigger << endl;				       
      	   //memory_[currentBCO_]->setTriggerNumber(trigger);
           //std::cout <<  "END EVENT------TRIGGER = " << currentTrigger_ << " Bco: " << hex << currentTriggerBCO_-dataTriggerDelay << " ---------------------" << dec<< endl << endl;  	     
      	   unsigned int lowBCO   = (currentBCO_ & 0xffULL);
      	   uint64_t   eventBCO = (currentBCO_ & 0xffffffffffffff00ULL) + currentTriggerBCO_;

           eventBCO -= dataTriggerDelay;
	   deltaBCOWhenTriggered = eventBCO - currentBCO_;
//           cout << "- Delta: " << deltaBCOWhenTriggered <<"--Trigger " << trigger << " lcorrected: " << correctedTrigger  << " Trig BCO: " << hex << eventBCO << " curr bco: " << currentBCO_ << dec << " WhatTriggeredBCO: " << whatTriggeredLongBCO << endl;				       
	   if((int)(eventBCO - currentBCO_) < -10) eventBCO += 0x100;
        
	   if(memory_.find(eventBCO-0xff) != memory_.end()){ eventBCO -= 0xff; cout << "Found event 0xff bco ago" << endl;}
           if(memory_.find(eventBCO-0x100) != memory_.end()){ eventBCO -= 0x100; cout << "Found event 0x100 bco ago" << endl;}
           if(memory_.find(eventBCO-0x101) != memory_.end()){ eventBCO -= 0x101; cout << "Found event 0x101 bco ago" << endl;}

           if(previousTriggerBco != -1)
	   {
	     if(abs((int)(eventBCO - previousTriggerBco)) <= 20)
	       cout << "Trig#: " << trigger << " Delta trigger bco: " << (int)(eventBCO - previousTriggerBco) << " Trig bco: " << eventBCO << " Prev bco: " << previousTriggerBco << endl;
	   }
           previousTriggerBco = eventBCO;
  	       
	   deltaBCOWhenTriggered = eventBCO - currentBCO_;
//           cout << "- Delta: " << deltaBCOWhenTriggered <<"--Trigger " << trigger << " lcorrected: " << correctedTrigger  << " Trig BCO: " << hex << eventBCO << " curr bco: " << currentBCO_ << dec << " WhatTriggeredBCO: " << whatTriggeredLongBCO << endl;				       

           PxEvent* event;
           //for(int i=-10; i<10; i++)
           //  if(memory_.find(eventBCO+i) != memory_.end())
           //	 cout << "FOUND " << i << endl;
           if(memory_.find(eventBCO) == memory_.end() 
	   && memory_.find(eventBCO-1) == memory_.end() 
	   && memory_.find(eventBCO+1) == memory_.end() 
	   && memory_.find(eventBCO-2) == memory_.end()
	   && memory_.find(eventBCO+2) == memory_.end()
	   && memory_.find(eventBCO+3) == memory_.end()
	   && memory_.find(eventBCO-3) == memory_.end()
	   && memory_.find(eventBCO+4) == memory_.end()
	   && memory_.find(eventBCO-4) == memory_.end()
	   && memory_.find(eventBCO+5) == memory_.end()
	   && memory_.find(eventBCO-5) == memory_.end()
	   //&& memory_.find(eventBCO+4) == memory_.end()
	   )
           {
//             cout << hex << "NO MATCH -> Delta: " << eventBCO - currentBCO_ << " -- Trigger number " << currentTrigger_ << " Event BCO: " << eventBCO << dec << endl;
             memory_[eventBCO] = new PxEvent(eventBCO);
             event = memory_[eventBCO];
           }
           else
           {
//             cout << hex << "!! MATCH -> Delta: " << eventBCO - currentBCO_ << " -- Trigger number " << currentTrigger_ << " Event BCO: " << eventBCO << dec << endl;
             //cout << "Matched BCO! For trigger Number " << currentTrigger_ << endl;
             if     (memory_.find(eventBCO  ) != memory_.end()) event = memory_[eventBCO];
             else if(memory_.find(eventBCO-1) != memory_.end()) event = memory_[eventBCO-1];
             else if(memory_.find(eventBCO+1) != memory_.end()) event = memory_[eventBCO+1];
             else if(memory_.find(eventBCO-2) != memory_.end()) event = memory_[eventBCO-2];
             else if(memory_.find(eventBCO+2) != memory_.end()) event = memory_[eventBCO+2];
             else if(memory_.find(eventBCO-3) != memory_.end()) event = memory_[eventBCO-3];
             else if(memory_.find(eventBCO+3) != memory_.end()) event = memory_[eventBCO+3];
             else if(memory_.find(eventBCO-4) != memory_.end()) event = memory_[eventBCO-4];
             else if(memory_.find(eventBCO+4) != memory_.end()) event = memory_[eventBCO+4];
             else if(memory_.find(eventBCO-5) != memory_.end()) event = memory_[eventBCO-5];
             else if(memory_.find(eventBCO+5) != memory_.end()) event = memory_[eventBCO+5];
           }  
  
           event->setTriggerBcoNumber(currentTriggerBCO_); 
           //event->setTriggerNumber(currentTrigger_); 
           event->setTriggerNumber(correctedTrigger); 
           if(event->getNumberOfHits() != 0)
           {

//         	 cout << "END OF EVENT ############# TRIG DATA BCO: " << currentTrigger_ << "-" << hex << eventBCO  << " - " << event->getBCONumber() 
//         	 << " | " << currentTriggerBCO_  
//         	 << " = " << event->getDecodedHit(0).bco 
//         	 << " -> delta = " << event->getDecodedHit(0).bco-currentTriggerBCO_ 
//         	 << dec 
//         	 << endl << endl;

           }
           //if(event->getTriggerNumber() > lastTriggerInTelescope_) break;    
      	 }							       
      	 else if ( dataType == 0x0c ) 
      	 {
      	   triggerState = 2;
           //cout << hex << "---++TYPE C BCO: " << ((word >> 8)&0xff) << " Tin 0: " << ((word >> 16)&0xff) << " Tin 1: " << ((word >> 24)&0xff) << dec << endl; 	 	    
      	   //if( ((word >> 16)&0xff) != 0)
           //  cout << hex << "---++TYPE C BCO: " << ((word >> 8)&0xff) << " Tin 0: " << ((word >> 16)&0xff) << " Tin 1: " << ((word >> 24)&0xff) << dec << endl; 	 	    
      	 }     
       }								 
     }
     cout << "--------------Last Trigger #: " << correctedTrigger << "-------------" << endl;
    }
    else
    {
      correction  = 0;
      bool wrapping = false;
      int  wrapCounter = 0;
      while( bufferPos < buffer->second.size() && eventCounters[stationName] <= maxEvents )
      {
        blockLengthC = buffer->second[bufferPos];
        bufferPos   += sizeof(char);
        blockLength  = (int)blockLengthC;
        //cout << __PRETTY_FUNCTION__ << blockLength << endl;
        for(int nWords=0; nWords<blockLength; nWords++)
        {
          if( nWords == 0 )
          {
            theDataDecoder.setTimestamp(buffer->second.substr(bufferPos,sizeof(unsigned long long)));
            currentTimestamp = theDataDecoder.getTimestamp();
          }
          else
          {
            theDataDecoder.setData(buffer->second.substr(bufferPos,sizeof(unsigned long long)));
            if( theDataDecoder.isData() )
            {
              //cout << __PRETTY_FUNCTION__ << "Station: " << station << endl;
              theDataDecoder.addStation(station);
              currentTrigger = theDataDecoder.decodeTrigger();
              if(stationName == "digital") --currentTrigger;

              if( !wrapping && currentTrigger > 3./4*wrapAround )
              {
        	wrapping = true;
        	cout << __PRETTY_FUNCTION__ << "Wrapping around! Trigger: " << currentTrigger << endl;
              }
              else if( wrapping && currentTrigger > wrapAround/4 && currentTrigger < wrapAround/2 )
              {
        	wrapping = false;
        	++wrapCounter;
        	cout << __PRETTY_FUNCTION__ << "Wrapped around!  Trigger: " << currentTrigger << endl;
              }
              
              if(wrapping && (int)(currentTrigger - wrapAround/2) < 0)
              {
        	currentTrigger += (wrapCounter+1)*wrapAround;
              }
              else
              {
        	currentTrigger += wrapCounter*wrapAround;
              }
              //cout << __PRETTY_FUNCTION__ << "Station: " << station << " Trigger: " << currentTrigger << endl;
              
              if( currentTrigger != lastTrigger )
              {
        	++eventCounters[stationName];
        	if( eventCounters[stationName] > maxEvents ) 
        	  break;
        	
        	currentEvent = &theRun.getEvent(currentTrigger);
        	if( lastTrigger > currentTrigger+20)// || (currentTrigger > 10 && lastTrigger < currentTrigger-10))
        	{
        	  cout << __PRETTY_FUNCTION__ 
        	       << "Plane: " << theDataDecoder.decodeHit().plaq 
        	       << " on station: " << stationName
        	       << " has bad trigger number: " << currentTrigger 
        	       << " w.r.t last trigger: " << lastTrigger 
        	       << endl;
        	}
        	lastTrigger = currentTrigger;
        	currentEvent->setTimestamp(currentTimestamp);
        	currentEvent->setTriggerNumber(uniqueTrigger);
        	currentEvent->setHardwareTriggerNumber(currentTrigger);
              }
              else
              {
        	if(currentTimestamp > currentEvent->getTimestamp())
        	{
        	  currentEvent->addTimestamp(currentTimestamp);
        	}
        	else if(currentTimestamp < currentEvent->getTimestamp())
        	{
        	  cout << __PRETTY_FUNCTION__ << "IMPOSSIBLE: current " 
        	       << currentTimestamp << "!= " 
        	       << currentEvent->getTimestamp() << " Event timestamp" << endl;
        	}
        	if(currentTimestamp - currentEvent->getTimestamp() >= maxSeconds)
        	{
        	  cout << __PRETTY_FUNCTION__ << "Strange timestamp: current " 
        	       << currentTimestamp << "!= " 
        	       << currentEvent->getTimestamp() << " Event timestamp" << endl;
        	}
              }
              currentEvent->addHit(theDataDecoder.decodeHit());
            }

          }
          bufferPos += sizeof(unsigned long long);
        }
        //if(currentTrigger >= 1000) break; 
      }
//    break;
    }
  }

  Event* currentEvent   = 0;
  for(std::map<uint64_t,PxEvent*>::iterator it=memory_.begin(); it!=memory_.end(); it++)
  {
    if(it->second->getTriggerNumber() != -1)
    {
      PxEvent& anEvent = *(it->second);
      anEvent.addTriggerNumberToHits();
      //cout << "Trigger number: " << anEvent.getTriggerNumber() << endl; 
      currentEvent = &theRun.getEvent(anEvent.getTriggerNumber());
      //currentEvent->setTimestamp(currentTimestamp);
      currentEvent->setTriggerNumber(anEvent.getTriggerNumber());
      currentEvent->setHardwareTriggerNumber(anEvent.getTriggerNumber());
      for(unsigned int e=0; e<anEvent.getNumberOfHits(); e++)
      {
        currentEvent->addHit(anEvent.getDecodedHit(e));
      }
    }
  }
  cout << __PRETTY_FUNCTION__ << "Done! Total number of events: " << theRun.getNumberOfEvents() << endl;

  return 0;
}

//-----------------------------------------------------------------------
int FilesMerger::writeMergedFiles(string filesDirectory,string fileName){
  if(theRun.getNumberOfEvents() == 0){
    cout << __PRETTY_FUNCTION__ << "There are no events in this run!" << endl;
    return -1;
  }
  if(fileName == "")
    fileName = "Run"+runNumber_+"_Merged.dat";
  cout << __PRETTY_FUNCTION__ << "Saving events in file: " << filesDirectory + "/" + fileName << endl;
  ofstream outFile((filesDirectory + "/" + fileName).c_str(),ios::binary);
  const long long ones = -1;
  unsigned long long timestamp;
  unsigned long long data;
  for(map<unsigned int, Event>::iterator eventsIt=theRun.getEvents().begin(); eventsIt!=theRun.getEvents().end(); eventsIt++){
    outFile.write((char*)&ones,sizeof(long long));
    timestamp = eventsIt->second.getTimestamp();
    outFile.write((char*)&timestamp,sizeof(unsigned long long));
    //cout << __PRETTY_FUNCTION__ << "Timestamp: " << timestamp << endl;
//    cout << __PRETTY_FUNCTION__ << "N hits: " << eventsIt->second.getNumberOfHits() << endl;
    for(unsigned int hit=0; hit<eventsIt->second.getNumberOfHits(); hit++)
    {
      data = eventsIt->second.getHit(hit).data;
      //cout << __PRETTY_FUNCTION__ << eventsIt->second.getHit(hit) << endl;
      outFile.write((char*)&data,sizeof(unsigned long long));
    }
  }
  outFile.close();
  return 0;
}

//-----------------------------------------------------------------------
int FilesMerger::readMergedFiles(string fileName)
{
  int maxNumberOfEvents = 30;
  cout << __PRETTY_FUNCTION__ << "\tReading file: " << fileName << endl;
  //Reading the file
  // 8 Bytes: all 1's
  // 8 Bytes: time stamp in computer seconds
  // All hits in event
  // the high byte reserved bits (63:56) indicate station number
  // the low 7 bytes of hit quad word are saved (55:0)
  //Quad Word: 
  //reserved(63:52) | trigger number(51:32) | plaquette(31:29) | data0/debug1(28) | chipid(27:23) | row(22:16) | col(15:10) | pulseheight(9:0)
  bool writeOutFile = true;
  ofstream outputFile;
  if(writeOutFile)
  {
    string outFileName = fileName;
    outputFile.open(outFileName.replace(outFileName.find(".dat"),4,".txt").c_str());
  }

  bool timeStamp = false;
  unsigned long long ones = -1;
  unsigned long long data;
  unsigned int       uniqueTrigger = 0;

  theRun.reset();
  
  ifstream inFile(fileName.c_str(),ios::binary);
  if(!inFile.is_open())
  {
    cout << "Can't open file: " << fileName << endl;
    return -1;
  }
  Event anEvent;
  DataDecoder theDataDecoder;
  
  while(!inFile.eof())
  {
    inFile.read((char*)(&data),sizeof(unsigned long long));
    //cout << "Data: " << (data>>32) << (data & 0xffffffff) << endl;
    if(data == ones)
    {
      outputFile << "fffffffffffffffffffffffffffffffffffff" << endl;
      timeStamp = true;
      continue;
    }
    if(timeStamp || inFile.eof())
    {
      ++uniqueTrigger;
      theDataDecoder.setTimestamp(data);
      anEvent.setTimestamp(theDataDecoder.getTimestamp());
      outputFile << dec << "Time: " << (data>>32) << (data & 0xffffffff) << endl;
      timeStamp = false;
      if(anEvent.getNumberOfHits() != 0)
      {
//        cout << __PRETTY_FUNCTION__ << "New event " << uniqueTrigger << " nHits =  " << anEvent.getNumberOfHits() << endl;
	theRun.addEvent(anEvent);
        anEvent.reset();
	if(theRun.getNumberOfEvents() >= (unsigned int)maxNumberOfEvents)
	  break;
      }
    }
    else
    {
      theDataDecoder.setData(data);
      if( theDataDecoder.isData() )
      {
        anEvent.setTriggerNumber(uniqueTrigger);
        anEvent.setHardwareTriggerNumber(theDataDecoder.decodeTrigger());
//      cout << __PRETTY_FUNCTION__ << "Trigger number: " << theDataDecoder.decodeTrigger() << endl;
//      cout << __PRETTY_FUNCTION__ 
//           << "data " << setw(8) << setfill('0') << hex
//           << ((data>>32) & 0xffffffff)
//           << setw(8) << setfill('0')
//           << (data & 0xffffffff) << dec
 //          << endl;
        anEvent.addHit(theDataDecoder.decodeHit());
        if(writeOutFile)
        {
          Hit tmpHit = theDataDecoder.decodeHit();
          outputFile << hex
        	     << "data: "
          	     << setw(8) << setfill('0')
        	     << ((data>>32) & 0xffffffff)
        	     << setw(8) << setfill('0')
        	     << (data & 0xffffffff)
        	     << " trig: "      << anEvent.getHardwareTriggerNumber()
        	     << " row: "       << tmpHit.row
        	     << " col: "       << tmpHit.col
        	     << " chip: "      << tmpHit.cid
        	     << " plaq: "      << tmpHit.plaq
        	     << " adc: "       << tmpHit.adc
        	     << " station: "   << tmpHit.station
        	     << endl;
         }
       }
     }
  }
  inFile.close();
  if(writeOutFile)
  {
    outputFile.close();
  }
  cout << __PRETTY_FUNCTION__ << "Final number of events: " << theRun.getNumberOfEvents() << endl; 
  return 0;
}

//-----------------------------------------------------------------------
int FilesMerger::readStationBuffers(void){
  //Reading the file
  // 1 Byte (unsigend char) = Number of 8 bytes words in the following block (max 183)
  // 8 Bytes: the first word in a block is the time stamp in computer seconds
  // 8 Bytes * nWords in a block-1: hits 
  unsigned char      blockLengthC;
  int                blockLength = -1;
  long long          data;
  int                trig;
  unsigned long long orderedData = 0;
  unsigned long long timeStamp;
  
  bool writeOutFile = true;
  
  ofstream outputFile;
  if(writeOutFile){
    outputFile.open("summary.txt");
  }


  DataDecoder theDataDecoder;
  Hit    tmpHit;
  Status tmpStatus;
  struct tm * timeinfo;
  for(map<string,string>::iterator buffer=stationBuffers_.begin(); buffer!=stationBuffers_.end(); buffer++){
    unsigned int bufferPos = 0;
//      for(unsigned int a=0; a<20; a++){
//        cout << __PRETTY_FUNCTION__ << hex << "Single: " << (unsigned int)buffer->second[a] << endl;
//      }
    while(bufferPos < buffer->second.size()){
//    for(int f=0; f<100; f++){
      if(writeOutFile){
    	outputFile << "-----------------------------------------------" << endl;
      }
      blockLengthC = buffer->second[bufferPos];
      bufferPos += sizeof(char);
      blockLength = (int)blockLengthC;
      if(writeOutFile){
    	outputFile << dec << blockLength << endl;
      }
      data = 0;
      for(int nWords=0; nWords<blockLength; nWords++){
//	cout << __PRETTY_FUNCTION__ << "Buffer pos: " << bufferPos << endl;
    	if(nWords==0){
    	  theDataDecoder.setTimestamp(buffer->second.substr(bufferPos,sizeof(unsigned long long)));
	  timeStamp = theDataDecoder.getTimestamp();

          timeinfo = localtime ( (time_t*)(&timeStamp) );
    	  if(writeOutFile){
  	      outputFile << dec << "Time: " << (timeStamp >> 32) << (timeStamp & 0xffffffff) 
	                 << " which correspond to: " << asctime (timeinfo)
			 << "--- Packet number: " << theDataDecoder.getPacketNumber()
			 << endl;
    	  }
    	}
    	else{
	  theDataDecoder.setData(buffer->second.substr(bufferPos,sizeof(unsigned long long)));
	  if(theDataDecoder.isData()){
	    data   = theDataDecoder.getData();
	    tmpHit = theDataDecoder.decodeHit();
	    trig   = theDataDecoder.decodeTrigger();
 	    if(writeOutFile){
 	      outputFile
	       << hex
	       << "data: "
	       << setw(8) << setfill('0')
	       << ((data>>32) & 0xffffffff)
	       << setw(8) << setfill('0')
	       << (data & 0xffffffff)
	       << " ordered:"
	       << setw(8) << setfill('0')
	       << ((orderedData>>32) & 0xffffffff)
	       << setw(8) << setfill('0')
	       << (orderedData & 0xffffffff)
 	       << dec
 	       << " Block: "	 << setw(3) << blockLength
 	       << " Time: "	 << (timeStamp >> 32) << (timeStamp & 0xffffffff)
 	       << " trig: "	 << setw(3) << trig
 	       << " row: "	 << setw(2) << tmpHit.row
 	       << " col: "	 << setw(2) << tmpHit.col
 	       << " chip: "	 << tmpHit.cid
 	       << " plaq: "	 << tmpHit.plaq
 	       << " adc: "	 << setw(3) << tmpHit.adc
 	       << " station: "   << tmpHit.station
 	       << endl;
 	    }
	  }
	  else
	  {
	    data      = theDataDecoder.getData();
	    tmpStatus = theDataDecoder.decodeStatus();
 	    if(writeOutFile){
 	      outputFile
        	<< "Data: "	      << std::hex << (tmpStatus.data >> 32) << (tmpStatus.data & 0xffffffff) << std::dec
        	<< " error : "        << tmpStatus.error
        	<< " repErr: "        << tmpStatus.repErr
        	<< " tokenInError: "  << tmpStatus.tokenInError
        	<< " tokenOutError: " << tmpStatus.tokenOutError
        	<< " stError: "       << tmpStatus.stError
        	<< " plaquette: "     << tmpStatus.plaquette
        	<< " tokenIn: "       << tmpStatus.tokenIn
        	<< " tokenOut: "      << tmpStatus.tokenOut
        	<< " trigger: "       << tmpStatus.trigger
        	<< " ultraBlack: "    << tmpStatus.ultraBlack
		<< endl;
	    }
	  }
    	}
        bufferPos += sizeof(unsigned long long);
      }
    }
    if(writeOutFile){
      outputFile.close();
    }
    break;
  }
  return 0;
}

//-----------------------------------------------------------------------
int FilesMerger::readFile(void){
  //Reading the file
  // 1 Byte (unsigend char) = Number of 8 bytes words in the following block (max 183)
  // 8 Bytes: the first word in a block is the time stamp in computer seconds
  // 8 Bytes * nWords in a block-1: hits 
  // All hits in event
  // the high byte reserved bits (63:56)
  // the low 7 bytes of hit quad word are saved (55:0)
  // Quad Word: 
  // reserved(63:52) | trigger number(51:32) | plaquette(31:29) | data0/debug1(28) | chipid(27:23) | row(22:16) | col(15:10) | pulseheight(9:0)
  struct Hit{
  	  int detId;
  	  int station;
  	  int plaq;
  	  int cid;
  	  int row;
  	  int col;
  	  int adc;
  };
  unsigned char      blockLengthC;
  int                blockLength = -1;
  long long          data;
  int                dataSize = 8;
  int                dataDebug;
  int                trig;
  unsigned long long orderedData = 0;
  unsigned long long tmpData;
  long long          timeStamp = 0;
  bool writeOutFile = true;
  
  ofstream outputFile;
  if(writeOutFile){
    outputFile.open("summary1.txt");
  }


  Hit tmpHit;											     
  ifstream file("test1.dat");
  if(!file.is_open()){
    cout << __PRETTY_FUNCTION__ << "Can't open file test1.dat" << endl;
    return -1;
  }
//  ifstream file("../LORE/Run286_DUT-01244583854-83360.dat");
  									     
  while(!file.eof() && blockLength != 0){							     
//  for(int f=0; f<100; f++){									     
    if(writeOutFile){										     
      outputFile << "-----------------------------------------------" << endl;			     
    }												     
    file.read((char*)&blockLengthC,sizeof(char));
    blockLength = (int)blockLengthC;								     
    if(writeOutFile){										     
      outputFile << dec << blockLength << endl; 						     
    }												     
    for(int nWords=0; nWords<blockLength; nWords++){					     
      file.read((char*)&data,sizeof(unsigned long long));
      if(nWords==0){										     
  	timeStamp = data;									     
  	if(writeOutFile){									     
//	    outputFile << dec << "Time: " << (timeStamp >> 32) << (timeStamp & 0xffffffff) << endl;  
  	}											     
      } 											     
      else{											     
  	orderedData = 0;									     
  	for(int byte=0;byte<dataSize;byte++){							     
  	  tmpData = ((data >> ((dataSize-byte-1)*8))& 0xff) << (byte*8);			     
  	  orderedData += tmpData;								     
  	}											     
  	tmpHit.adc    = orderedData & 0x3ff;							     
  	tmpHit.col    = (orderedData >> 10) & 0x3f;						     
  	tmpHit.row    = (orderedData >> 16) & 0x7f;						     
  	tmpHit.cid    = (orderedData >> 23) & 0xf;						     
  	dataDebug     = (orderedData >> 28) & 0x1;						     
  	tmpHit.plaq   = (orderedData >> 29) & 0x7;						     
  	trig	      = (orderedData >> 32) & 0xfffff;						     
	tmpHit.station= (orderedData >> 56) & 0xf;						     
  	if(writeOutFile){									     
  	  outputFile 										     
//		   << hex									     
//		   << "data: "									     
//		   << setw(8) << setfill('0')							     
//		   << ((data>>32) & 0xffffffff) 						     
//		   << setw(8) << setfill('0')							     
//		   << (data & 0xffffffff)							     
//		   << " ordered:"								     
//		   << setw(8) << setfill('0')							     
//		   << ((orderedData>>32) & 0xffffffff)						     
//		   << setw(8) << setfill('0')							     
//		   << (orderedData & 0xffffffff)						     
  	   << dec										     
  	   << "Block: "      << setw(3) << blockLength						     
  	   << " Time: "      << (timeStamp >> 32) << (timeStamp & 0xffffffff)			     
  	   << " trig: "      << setw(3) << trig 	   					     
  	   << " row: "       << setw(2) << tmpHit.row						     
  	   << " col: "       << setw(2) << tmpHit.col						     
  	   << " chip: "      << tmpHit.cid							     
  	   << " plaq: "      << tmpHit.plaq							     
  	   << " adc: "       << setw(3) << tmpHit.adc						     
  	   << " station: "   << tmpHit.station							     
  	   << endl;										     
  	}											     
      } 											     
    }												     
  }												     
  if(writeOutFile){										     
    outputFile.close(); 									     
  }												     
  return 0;
}
