/****************************************************************************
 ** Author: Lorenzo Uplegger
 **
 ** FERMILAB
 ****************************************************************************/
#include "StripTestReader.h"
#include "EventReader.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#define BUFLEN 1024
#include "PxEvent.h"

using namespace std;

std::string itos(double n)
{
   const int max_size = std::numeric_limits<int>::digits10 + 1 /*sign*/ + 1 /*0-terminator*/;
   char buffer[max_size] = {0};
   sprintf(buffer, "%f", n);
   return std::string(buffer);
}

//------------------------------------------------------------------------------------
StripTestReader::StripTestReader(EventReader* reader) : 
  stripPlane_("Station 8 - Plane 0",false)
{
  
  //int eventsToProcess = 500;
  //double thresholdConverter[8] = {1,1,1,1,1,1,1,1};
  double globalX = 0;
  double globalY = 0;
  double globalZ = 0;
  
  unsigned int eventsToProcess;
  vector<double> thresholdConverter;
  double Z = 450000; 
  double deltaX;
  double deltaY;
  double alpha;
  double beta;
  double gamma;
  
  runNumber_ = reader->getRunNumber();
  if(runNumber_ == 3340)
  {
    eventsToProcess = 88000;//150000;
    thresholdConverter.push_back(32);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(64);
    thresholdConverter.push_back(80);
    thresholdConverter.push_back(96);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(128);
    thresholdConverter.push_back(144);
    deltaX = 12752;
    deltaY = 46057;
    alpha  = 0;
    beta   = 15.6;
    gamma  = 0.85;
    pxSuiteRunNumber_ = 136;
  }  
  else if(runNumber_ == 3348)
  {
    eventsToProcess = 88000;//90000;
    thresholdConverter.push_back(32);
    thresholdConverter.push_back(44);
    thresholdConverter.push_back(56);
    thresholdConverter.push_back(68);
    thresholdConverter.push_back(80);
    thresholdConverter.push_back(92);
    thresholdConverter.push_back(104);
    thresholdConverter.push_back(116);
    deltaX = 7456;
    deltaY = 46057;
    alpha  = 0;
    beta   = 20.78;
    gamma  = 0.895;
    pxSuiteRunNumber_ = 142;
  }
  else if(runNumber_ == 3368)
  {
    eventsToProcess = 88000;//93900;
    thresholdConverter.push_back(30);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(60);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(80);
    thresholdConverter.push_back(90);
    thresholdConverter.push_back(100);
    thresholdConverter.push_back(110);
    deltaX = 12650;
    deltaY = 46187;
    alpha  = 0;
    beta   = 15.7;
    gamma  = 0.88;
    pxSuiteRunNumber_ = 170;
  }
  else if(runNumber_ == 3370)
  {
    eventsToProcess = 88000;
    thresholdConverter.push_back(30);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(60);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(80);
    thresholdConverter.push_back(90);
    thresholdConverter.push_back(100);
    thresholdConverter.push_back(110);
    deltaX = 17636.5;
    deltaY = 46057;
    alpha  = 0;
    beta   = 9.1;
    gamma  = 0.87;
    pxSuiteRunNumber_ = 172;
  }
   else if(runNumber_ == 3413)
   {
     eventsToProcess = 60000;
     thresholdConverter.push_back(32);
     thresholdConverter.push_back(48);
     thresholdConverter.push_back(60);
     thresholdConverter.push_back(70);
     thresholdConverter.push_back(80);
     thresholdConverter.push_back(90);
     thresholdConverter.push_back(100);
     thresholdConverter.push_back(110);
     deltaX = 28379;
     deltaY = 47657;
     alpha  = 0;
     beta   = 0;
     gamma  = 0.97;//0.6;
     pxSuiteRunNumber_ = 259;
     Z = 630000;
   }
  else if(runNumber_ == 3429)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(32);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(60);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(80);
    thresholdConverter.push_back(90);
    thresholdConverter.push_back(100);
    thresholdConverter.push_back(110);
    deltaX = 16969.9;
    deltaY = 47090;
    alpha  = 0;
    beta   = -0.5;
    gamma  = -0.965;//0.6;
    pxSuiteRunNumber_ = 282;
    Z = -70000;
  }
  else if(runNumber_ == 3430)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(32);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(60);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(80);
    thresholdConverter.push_back(90);
    thresholdConverter.push_back(100);
    thresholdConverter.push_back(110);
    deltaX = 16973.7;
    deltaY = 47090;
    alpha  = 0;
    beta   = -0.5;
    gamma  = -0.98;//0.6;
    pxSuiteRunNumber_ = 283;
    Z = -70000;
  }
  else if(runNumber_ == 3432)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(32);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(60);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(80);
    thresholdConverter.push_back(90);
    thresholdConverter.push_back(100);
    thresholdConverter.push_back(110);
    deltaX = 16973.7;
    deltaY = 47090;
    alpha  = 0;
    beta   = -0.5;
    gamma  = -0.98;//0.6;
    pxSuiteRunNumber_ = 285;
    Z = -70000;
  }
  else if(runNumber_ == 3434)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(32);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(60);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(80);
    thresholdConverter.push_back(90);
    thresholdConverter.push_back(100);
    thresholdConverter.push_back(110);
    deltaX = 16973.7;
    deltaY = 47090;
    alpha  = 0;
    beta   = -0.5;
    gamma  = -0.98;//0.6;
    pxSuiteRunNumber_ = 294;//285;
    Z = -70000;
  }
  else if(runNumber_ == 3435)
  {
    //eventsToProcess = 925;
    eventsToProcess = 60000;
    thresholdConverter.push_back(32);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 16973.7;
    deltaY = 47090;
    alpha  = 0;
    beta   = -0.5;
    gamma  = -0.98;//0.6;
    pxSuiteRunNumber_ = 295;
    Z = -70000;
  }
  else if(runNumber_ == 3439)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(32);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 16973.7;
    deltaY = 47090;
    alpha  = 0;
    beta   = -0.5;
    gamma  = -0.98;//0.6;
    pxSuiteRunNumber_ = 299;
    Z = -70000;
  }
  else if(runNumber_ == 3441)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(34);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 16973.7;
    deltaY = 47090;
    alpha  = 0;
    beta   = -0.5;
    gamma  = -0.98;//0.6;
    pxSuiteRunNumber_ = 300;
    Z = -70000;
  }
  else if(runNumber_ == 3442)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(36);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 16973.7;
    deltaY = 47090;
    alpha  = 0;
    beta   = -0.5;
    gamma  = -0.98;//0.6;
    pxSuiteRunNumber_ = 301;
    Z = -70000;
  }
  else if(runNumber_ == 3443)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(38);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 16973.7;
    deltaY = 47090;
    alpha  = 0;
    beta   = -0.5;
    gamma  = -0.98;//0.6;
    pxSuiteRunNumber_ = 302;
    Z = -70000;
  }
  else if(runNumber_ == 3444)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(40);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 16973.7;
    deltaY = 47090;
    alpha  = 0;
    beta   = -0.5;
    gamma  = -0.98;//0.6;
    pxSuiteRunNumber_ = 303;
    Z = -70000;
  }
  else if(runNumber_ == 3445)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(42);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 16973.7;
    deltaY = 47090;
    alpha  = 0;
    beta   = -0.5;
    gamma  = -0.98;//0.6;
    pxSuiteRunNumber_ = 304;
    Z = -70000;
  }
  else if(runNumber_ == 3448)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(32);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 15613;
    deltaY = 46920;
    alpha  = 0;
    beta   = 3.88;
    gamma  = -0.76;//0.6;
    pxSuiteRunNumber_ = 307;
    Z = -70000;
  }
  else if(runNumber_ == 3449)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(35);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 15613;
    deltaY = 46920;
    alpha  = 0;
    beta   = 3.88;
    gamma  = -0.76;//0.6;
    pxSuiteRunNumber_ = 308;
    Z = -70000;
  }
  else if(runNumber_ == 3450)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(40);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 15613;
    deltaY = 46920;
    alpha  = 0;
    beta   = 3.88;
    gamma  = -0.76;//0.6;
    pxSuiteRunNumber_ = 309;
    Z = -70000;
  }
  else if(runNumber_ == 3451)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(32);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 13698.5;
    deltaY = 46920;
    alpha  = 0;
    beta   = 8;
    gamma  = -0.71;//0.6;
    pxSuiteRunNumber_ = 310;
    Z = -70000;
  }
  else if(runNumber_ == 3452)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(35);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 13698.5;
    deltaY = 46920;
    alpha  = 0;
    beta   = 8;
    gamma  = -0.71;//0.6;
    pxSuiteRunNumber_ = 311;
    Z = -70000;
  }
  else if(runNumber_ == 3456)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(40);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 13698.5;
    deltaY = 46920;
    alpha  = 0;
    beta   = 8;
    gamma  = -0.71;//0.6;
    pxSuiteRunNumber_ = 315;
    Z = -70000;
  }
  else if(runNumber_ == 3457)
  {
    eventsToProcess = 35000;
    thresholdConverter.push_back(32);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 11714;
    deltaY = 46920;
    alpha  = 0;
    beta   = 15.4;
    gamma  = -0.59;//0.6;
    pxSuiteRunNumber_ = 316;
    Z = -70000;
  }
  else if(runNumber_ == 3460)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(35);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 11714;
    deltaY = 46920;
    alpha  = 0;
    beta   = 15.4;
    gamma  = -0.59;//0.6;
    pxSuiteRunNumber_ = 322;
    Z = -70000;
  }
  else if(runNumber_ == 3461)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(32);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 11712;
    deltaY = 46920;
    alpha  = 0;
    beta   = 15.5;
    gamma  = -0.60;//0.6;
    pxSuiteRunNumber_ = 323;
    Z = -70000;
  }
  else if(runNumber_ == 3462)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(35);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 11712;
    deltaY = 46920;
    alpha  = 0;
    beta   = 15.5;
    gamma  = -0.60;//0.6;
    pxSuiteRunNumber_ = 324;
    Z = -70000;
  }
  else if(runNumber_ == 3463)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(40);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 11712;
    deltaY = 46920;
    alpha  = 0;
    beta   = 15.5;
    gamma  = -0.60;//0.6;
    pxSuiteRunNumber_ = 325;
    Z = -70000;
  }
  else if(runNumber_ == 3464)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(32);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 9378.4;
    deltaY = 46920;
    alpha  = 0;
    beta   = 15.82;
    gamma  = 0.05;//0.6;
    pxSuiteRunNumber_ = 326;
    Z = -70000;
  }
  else if(runNumber_ == 3466)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(35);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 9378.4;
    deltaY = 46920;
    alpha  = 0;
    beta   = 15.82;
    gamma  = 0.05;//0.6;
    pxSuiteRunNumber_ = 328;
    Z = -70000;
  }
  else if(runNumber_ == 3469)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(40);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 9378.4;
    deltaY = 46920;
    alpha  = 0;
    beta   = 15.82;
    gamma  = 0.05;//0.6;
    pxSuiteRunNumber_ = 331;
    Z = -70000;
  }
  else if(runNumber_ == 3470)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(32);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 8381.5;
    deltaY = 46920;
    alpha  = 0;
    beta   = 20.54;
    gamma  = 0.025;//0.6;
    pxSuiteRunNumber_ = 332;
    Z = -70000;
  }
  else if(runNumber_ == 3473)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(35);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 8381.5;
    deltaY = 46920;
    alpha  = 0;
    beta   = 20.54;
    gamma  = 0.025;//0.6;
    pxSuiteRunNumber_ = 334;
    Z = -70000;
  }
  else if(runNumber_ == 3474)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(40);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 8381.5;
    deltaY = 46920;
    alpha  = 0;
    beta   = 20.54;
    gamma  = 0.025;//0.6;
    pxSuiteRunNumber_ = 335;
    Z = -70000;
  }
  else if(runNumber_ == 3475)
  {
    eventsToProcess = 60000;
    thresholdConverter.push_back(32);
    thresholdConverter.push_back(48);
    thresholdConverter.push_back(58);
    thresholdConverter.push_back(70);
    thresholdConverter.push_back(84);
    thresholdConverter.push_back(98);
    thresholdConverter.push_back(112);
    thresholdConverter.push_back(130);
    deltaX = 8381.5;
    deltaY = 46920;
    alpha  = 0;
    beta   = 20.54;
    gamma  = 0.025;//0.6;
    pxSuiteRunNumber_ = 336;
    Z = -70000;
  }
  

  stripPlane_.setNumberOfROCs(1,1);
  stripPlane_.addROC(0,0,0);
  stripPlane_.getROC(0)->setStandardPixPitch(90000/10.,60/10.);
  stripPlane_.getROC(0)->setNumberOfRows(1);
  stripPlane_.getROC(0)->setNumberOfCols(640);
  if(runNumber_ < 3394 || runNumber_ > 3424)
    stripPlane_.setXBackFlipped(true);
  else 
    stripPlane_.setXBackFlipped(false);  
  stripPlane_.setYBackFlipped(false);
  stripPlane_.setXPosition(deltaX/10);
  stripPlane_.setYPosition(deltaY/10);
  stripPlane_.setZPosition(Z/10);
  stripPlane_.setXRotation(alpha);
  stripPlane_.setYRotation(beta);
  //stripPlane_.setZRotation(gamma);
  stripPlane_.setZRotationCorrection(gamma);


  stripPlane_.setupVariables();

  double testGlobalX1 = stripPlane_.getPixelCenterLocalX(1);
  double testGlobalX2 = stripPlane_.getPixelCenterLocalX(0);
  double testGlobalY  = stripPlane_.getPixelCenterLocalY(0);
  double testGlobalZ  = 0;
  stripPlane_.fromLocalToGlobal(&testGlobalX1,&testGlobalY,&testGlobalZ);
  testGlobalY = stripPlane_.getPixelCenterLocalY(0);
  testGlobalZ = 0;
  stripPlane_.fromLocalToGlobal(&testGlobalX2,&testGlobalY,&testGlobalZ);
  cout 
  << "X1= "  << testGlobalX1 
  << " X2= " << testGlobalX2 
  << " Y= "  << testGlobalY 
  << " Z= "  << testGlobalZ 
  << endl;
  //exit(0);
  theHeader_   = reader->getEventHeaderPointer() ;
  theEvent_    = reader->getEventPointer      () ;
  theGeometry_ = reader->getGeometryPointer   () ;
  
  stringstream fileName;
  //fileName << "/data/TestBeam/2013_11_November/Strip//Run" << runNumber_ << "_Converted.root";
  fileName << "/data/TestBeam/2014_01_January/Strip//Run" << runNumber_ << "_Converted.root";
  //fileName << "/Users/uplegger/Programming/Monicelli/templates/Output/Run" << runNumber_ << "_Converted.root";

  PxEvent anEvent_;
  theFile_ = TFile::Open(fileName.str().c_str(),"RECREATE");
  theTrackTree_ = new TTree ("T992_Tracks", "The reconstructed telescope tracks");
  theStripTree_ = new TTree ("T992_Strips", "The reconstructed strip points"    );
  theTrackTree_->Branch("trackEvent", &theTrackEvent_, "trigger/i:numberOfTracks/i:numberOfHits[10]/i:chi2[10]/D:xSlope[10]/D:ySlope[10]/D:xIntercept[10]/D:yIntercept[10]/D:xError[10]/D:yError[10]/D");
  //theStripTree_->Branch("EventBranch", "PxEvent", &anEvent_, 16000, 0);
  theStripTree_->Branch("stripEvent", &theStripEvent_, "trigger/i:stripTrigger[20]/i:triggerBco[20]/i:longBco[20]/l:numberOfHits/I:chan[20]/I:chipid[20]/I:set[20]/I:strip[20]/I:istrip[20]/I:bco[20]/I:adc[20]/I:numberOfClusters/I:clusterSize[20]/I:clusterAdc[20]/I:clusterCharge[20]/I:clusters[20]/D:measuredLocalX[20]/D:mesuredLocalY[20]/D:measuredX[20]/D:mesuredY[20]/D:projectedX[20]/D:projectedY[20]/D");
  //theStripTree_->Branch("stripEvent", &theStripEvent_, "data/I:chan/I:chipid/I:set/I:strip/I:stripGlobal/I:bco/I:adc/I:runNumber/I:dataType/I:type/I:longBco/L:trigger/L");
  std::cout << "branched!\n";

  // Retrieve from file the number of stored events  
  unsigned int numberOfEvents = reader->getNumberOfEvents() ;
  
  std::stringstream ss;
  ss.str(""); 
  ss << "Found " << numberOfEvents << " events on file" ;
  STDLINE(ss.str(),ACYellow) ;
  STDLINE("       ",ACYellow) ;
  for(unsigned int event=0; event<eventsToProcess && event<numberOfEvents; ++event)
  //for(unsigned int event=0; event<10; ++event)
  {
    reader->readEvent(event) ;
    this->analyzeEvent(event) ;   
  }
  cout << "Done with telescope events " << endl;
  
  this->getStripEvents();
  cout << "Done with strip events " << endl;
  uint64_t currentBco = 0;
  vector<uint64_t> listToErase;
  cout << "Merging events... " << endl;
  for(std::map<uint64_t,PxEvent*>::iterator it=memory_.begin(); it!=memory_.end(); it++)
  {
    if(currentBco != it->first)
    {
      if(abs((int)(it->first-currentBco)) <= 0)
      {
        //cout << "bco: " << currentBco << " ev bco: " << it->first << " trigger: " << memory_[currentBco]->getTriggerNumber() << "  ev trig: " << it->second->getTriggerNumber() << endl;
        if(it->second->getTriggerNumber() == -1)
        {
          listToErase.push_back(it->first);
          //cout << "consecutive!" << endl;
          //cout << "bco: " << memory_[currentBco] << " trigger: " << memory_[currentBco]->getTriggerNumber() << " number of hits: " << memory_[currentBco]->getNumberOfHits() << endl;
          //cout << "bco: " << it->first << " trigger: " << it->second->getTriggerNumber() << " number of hits: " << it->second->getNumberOfHits() << endl;
          *(memory_[currentBco]) += *(it->second);
          //cout << "bco: " << memory_[currentBco] << " trigger: " << memory_[currentBco]->getTriggerNumber() << " number of hits: " << memory_[currentBco]->getNumberOfHits() << endl;
        }
      }
      currentBco = it->first;
    }	    
  }
  cout << "Erasing events... " << endl;
  for(vector<uint64_t>::iterator it=listToErase.begin(); it!=listToErase.end(); it++)
    memory_.erase(*it);
  cout << "filling strip tree... with #events " << memory_.size() << endl;
  map<int, int> orderedStrips;
  int    clusterNumber = 0;
  int    previousStrip = 0;
  int    tmpClusterHit = 0;
  bool   newCluster;
  for(std::map<uint64_t,PxEvent*>::iterator it=memory_.begin(); it!=memory_.end(); it++)
  {
    if(it->second->getTriggerNumber() != -1)
    {
      anEvent_ = *(it->second);
      theStripEvent_.numberOfHits = anEvent_.getNumberOfHits();
      theStripEvent_.trigger	  = anEvent_.getTriggerNumber();
      orderedStrips.clear();
      for(unsigned int e=0; e<MAX_HITS; e++)
      {
        if(e<anEvent_.getNumberOfHits())
        {
          theStripEvent_.chan	     [e] = anEvent_.getDecodedHit(e).chan;
          theStripEvent_.chipid      [e] = anEvent_.getDecodedHit(e).chipid;
          theStripEvent_.set	     [e] = anEvent_.getDecodedHit(e).set;
          theStripEvent_.strip       [e] = anEvent_.getDecodedHit(e).strip;
          theStripEvent_.istrip      [e] = anEvent_.getDecodedHit(e).istrip;
          theStripEvent_.bco	     [e] = anEvent_.getDecodedHit(e).bco;
          theStripEvent_.adc	     [e] = anEvent_.getDecodedHit(e).adc;
          theStripEvent_.stripTrigger[e] = anEvent_.getTriggerNumber();
          theStripEvent_.triggerBco  [e] = anEvent_.getTriggerBcoNumber();
          theStripEvent_.longBco     [e] = anEvent_.getBCONumber();
          orderedStrips[theStripEvent_.istrip[e]] = e;
        }
        else
        {
          theStripEvent_.chan	     [e] = 0;
          theStripEvent_.chipid      [e] = 0;
          theStripEvent_.set	     [e] = 0;
          theStripEvent_.strip       [e] = 0;
          theStripEvent_.istrip      [e] = 0;
          theStripEvent_.bco	     [e] = 0;
          theStripEvent_.adc	     [e] = 0;
          theStripEvent_.stripTrigger[e] = 0;
          theStripEvent_.triggerBco  [e] = 0;
          theStripEvent_.longBco     [e] = 0;
        }
      }
      newCluster = true;
      clusterNumber = -1;
      tmpClusterHit = 0;
      previousStrip = -10;
      for(map<int, int>::iterator it=orderedStrips.begin(); it!=orderedStrips.end(); it++)
      {
        if(it->first-previousStrip != 1)
        {
          if(clusterNumber >= 0 )
          {
            theStripEvent_.clusters[clusterNumber]       /= theStripEvent_.clusterSize[clusterNumber];
            theStripEvent_.measuredLocalX[clusterNumber] /= theStripEvent_.clusterCharge[clusterNumber];//theStripEvent_.clusterSize[clusterNumber];
            theStripEvent_.measuredLocalY[clusterNumber] = stripPlane_.getPixelCenterLocalY(0);
			//clusterCharge                                /= theStripEvent_.clusterSize[clusterNumber];
            globalX = theStripEvent_.measuredLocalX[clusterNumber];
            globalY = stripPlane_.getPixelCenterLocalY(0);
            globalZ = 0;
	    stripPlane_.fromLocalToGlobal(&globalX,&globalY,&globalZ);
            theStripEvent_.measuredLocalX[clusterNumber] *= 10;
            theStripEvent_.measuredLocalY[clusterNumber] *= 10;
            theStripEvent_.measuredX[clusterNumber] = globalX*10;
            theStripEvent_.measuredY[clusterNumber] = globalY*10;
          }
          clusterNumber++;
          theStripEvent_.clusters[clusterNumber]       = it->first;
          theStripEvent_.clusterAdc[clusterNumber]     = theStripEvent_.adc[it->second];//thresholdConverter[theStripEvent_.adc[it->second]];
          theStripEvent_.clusterCharge[clusterNumber]  = thresholdConverter[theStripEvent_.adc[it->second]];
	  theStripEvent_.measuredLocalX[clusterNumber] = stripPlane_.getPixelCenterLocalX(it->first)*thresholdConverter[theStripEvent_.adc[it->second]];
          theStripEvent_.clusterSize[clusterNumber] = 1;
          previousStrip = it->first;
        }
        else
        {
          theStripEvent_.clusters[clusterNumber]       += it->first;
          theStripEvent_.clusterAdc[clusterNumber]     += theStripEvent_.adc[it->second];//thresholdConverter[theStripEvent_.adc[it->second]];
          theStripEvent_.clusterCharge[clusterNumber]  += thresholdConverter[theStripEvent_.adc[it->second]];//thresholdConverter[theStripEvent_.adc[it->second]];
	  theStripEvent_.measuredLocalX[clusterNumber] += stripPlane_.getPixelCenterLocalX(it->first)*thresholdConverter[theStripEvent_.adc[it->second]];
          theStripEvent_.clusterSize[clusterNumber]++;
          previousStrip = it->first;
        }
      }
      if(clusterNumber >= 0)
      {
        theStripEvent_.clusters[clusterNumber]       /= theStripEvent_.clusterSize[clusterNumber];
        theStripEvent_.measuredLocalX[clusterNumber] /= theStripEvent_.clusterCharge[clusterNumber];//theStripEvent_.clusterSize[clusterNumber];
        theStripEvent_.measuredLocalY[clusterNumber] = stripPlane_.getPixelCenterLocalY(0);
		//clusterCharge                                /= theStripEvent_.clusterSize[clusterNumber];
        globalX = theStripEvent_.measuredLocalX[clusterNumber];
        globalY = stripPlane_.getPixelCenterLocalY(0);
        globalZ = 0;
	    stripPlane_.fromLocalToGlobal(&globalX,&globalY,&globalZ);
        theStripEvent_.measuredLocalX[clusterNumber] *= 10;
        theStripEvent_.measuredLocalY[clusterNumber] *= 10;
        theStripEvent_.measuredX[clusterNumber] = globalX*10;
        theStripEvent_.measuredY[clusterNumber] = globalY*10;
	//cout << "Old: " << theStripEvent_.measuredX[clusterNumber] << " new: " << -globalX << " delta: " << theStripEvent_.measuredX[clusterNumber]+globalX << endl;
      }
      theStripEvent_.numberOfClusters = clusterNumber + 1;
      theStripTree_->Fill();
      /*
      cout << "Event with trigger: " << theStripEvent_.trigger << endl;
      for(int e=0; e<theStripEvent_.numberOfHits; e++)
      {
        cout << dec 
          << " " << theStripEvent_.trigger
          << " " << theStripEvent_.istrip     [e]
          << " " << theStripEvent_.adc        [e]
          << " " << theStripEvent_.bco        [e]
          << " " << theStripEvent_.triggerBco [e]
          << " " << theStripEvent_.numberOfClusters
          << " " << theStripEvent_.clusters   [e]
          << " " << theStripEvent_.clusterSize[e]
          << endl;
      }
      */
    }
  }
  //exit(0);
  //    for(int hits=0; hits<theStripTree_->GetEntries(); hits++)
  //    {
  //      theStripTree_->GetEntry(hits);
  //	if(theStripEvent_.numberOfHits > 0)
  //	{
  //	  cout << "Reading: " << theStripEvent_.istrip << endl;
  //	}
  //    }
  //  exit(0);
  cout << "filling histos... " << endl;
  string titleExtra = " Beta=" + itos(beta) + " Thr=" + itos(thresholdConverter[0]) + " Run=" + itos(runNumber_);
  hStripProfile_         	= new TH1F("HStripProfile",	     	    ("Hits Strip Profile" + titleExtra).c_str(),      	    640, 0, 639);
  hClusterProfile_       	= new TH1F("HClusterProfile",	     	    ("Clusters Strip Profile" + titleExtra).c_str(),  	    640, 0, 639);
  hBeamProfileX_         	= new TH1F("HBeamProfileX",	     	    ("Beam Profile X" + titleExtra).c_str(),	    	    200, -25000, 25000);
  hBeamProfileY_         	= new TH1F("HBeamProfileY",	     	    ("Beam Profile Y" + titleExtra).c_str(),	    	    200, -25000, 25000);
  hMeasuredProfile_      	= new TH1F("HMeasuredProfile",       	    ("Cluster Measured Profile" + titleExtra).c_str(),	    200, -25000, 25000);
  h2DClusterSizeVsAdc_   	= new TH2F("H2DClusterSizeVsAdc",    	    ("Cluster Size Vs Adc" + titleExtra).c_str(),     	     20, 1, 21, 24, 0, 24);
  h2DClusterSizeVsCharge_	= new TH2F("H2DClusterSizeVsCharge", 	    ("Cluster Size Vs Charge" + titleExtra).c_str(),  	     20, 1, 21, 40, 0, 200);
  h2DClusterSizeVsChargeMatched_= new TH2F("H2DClusterSizeVsChargeMatched", ("Cluster Size Vs Charge Matched" + titleExtra).c_str(), 20, 1, 21, 40, 0, 200);
  hAdc_                  	= new TH1F("HAdc",		     	    ("Adc" + titleExtra).c_str(), 		    	     10, 0, 10);
  hNumberOfHits_         	= new TH1F("HNumberOfHits",	     	    ("Number of Hits" + titleExtra).c_str(),	    	     20, 0, 20);
  hNumberOfClusters_     	= new TH1F("HNumberOfClusters",      	    ("Number of Clusters" + titleExtra).c_str(),      	     20, 0, 20);
  hClusterSize_          	= new TH1F("HClusterSize",	     	    ("Cluster Size" + titleExtra).c_str(),	    	     10, 0, 10);
  hClusterSizeMatched_   	= new TH1F("HClusterSizeMatched",    	    ("Cluster Size Matched" + titleExtra).c_str(),    	     10, 0, 10);
  hDeltaBco_             	= new TH1F("HDeltaBco", 	     	    ("Delta Bco" + titleExtra).c_str(),		    	     10, -5,5);
  hAssociatedTriggers_   	= new TH1F("HAssociatedTriggers",    	    ("Associated Triggers" + titleExtra).c_str(),     	    511, -255, 255);
  hResidualXVsX_         	= new TH1F("HResidualXvsX",	     	    ("Residual X vs X" + titleExtra).c_str(),	    	    100, -15000, 15000);
  hResidualXVsY_         	= new TH1F("HResidualXvsY",	     	    ("Residual X vs Y" + titleExtra).c_str(),	    	    100, -15000, 15000);
  hResidualNormXVsX_     	= new TH1F("HResidualNormXvsX",      	    ("Residual Norm X vs X" + titleExtra).c_str(),    	    100, -15000, 15000);
  hResidualNormXVsY_     	= new TH1F("HResidualNormXvsY",      	    ("Residual Norm X vs Y" + titleExtra).c_str(),    	    100, -15000, 15000);
  h2DResidualXVsX_       	= new TH2F("H2DResidualXvsX",	     	    ("Residual X vs X" + titleExtra).c_str(),	    	    100, -15000, 15000, 100, -500, 500);
  h2DResidualXVsY_       	= new TH2F("H2DResidualXvsY",	     	    ("Residual X vs Y" + titleExtra).c_str(),	    	    100, -15000, 15000, 100, -500, 500);  
  hResidualY_            	= new TH1F("HResidualY",	     	    ("ResidualY" + titleExtra).c_str(),		    	    100, -15000, 15000);
  hResidual_             	= new TH1F("HResidual", 	     	    ("Residual" + titleExtra).c_str(),		    	    101, -151.5, 151.5);
  hResidualSize1_        	= new TH1F("HResidualSize1",	     	    ("Residual Size 1" + titleExtra).c_str(),	    	    101, -151.5, 151.5);
  hResidualSize1Adc7_           = new TH1F("HResidualSize1Adc7",	    ("Residual Size 1 Adc 7" + titleExtra).c_str(),	    101, -151.5, 151.5);
  hResidualSize1Adc6_           = new TH1F("HResidualSize1Adc6",	    ("Residual Size 1 Adc 6" + titleExtra).c_str(),	    101, -151.5, 151.5);
  hResidualSize1Adc5_           = new TH1F("HResidualSize1Adc5",	    ("Residual Size 1 Adc 5" + titleExtra).c_str(),	    101, -151.5, 151.5);
  hResidualSize1Adc4_           = new TH1F("HResidualSize1Adc4",	    ("Residual Size 1 Adc 4" + titleExtra).c_str(),	    101, -151.5, 151.5);
  hResidualSize1Adc3_           = new TH1F("HResidualSize1Adc3",	    ("Residual Size 1 Adc 3" + titleExtra).c_str(),	    101, -151.5, 151.5);
  hResidualSize1Adc2_           = new TH1F("HResidualSize1Adc2",	    ("Residual Size 1 Adc 2" + titleExtra).c_str(),	    101, -151.5, 151.5);
  hResidualSize1Adc1_           = new TH1F("HResidualSize1Adc1",	    ("Residual Size 1 Adc 1" + titleExtra).c_str(),	    101, -151.5, 151.5);
  hResidualSize1Adc0_           = new TH1F("HResidualSize1Adc0",	    ("Residual Size 1 Adc 0" + titleExtra).c_str(),	    101, -151.5, 151.5);
  hResidualSize2_        	= new TH1F("HResidualSize2",	     	    ("Residual Size 2" + titleExtra).c_str(),	    	    101, -151.5, 151.5);
  hResidualSize3_        	= new TH1F("HResidualSize3",	     	    ("Residual Size 3" + titleExtra).c_str(),	    	    101, -151.5, 151.5);
  hCorrelation_          	= new TH2F("HCorrelation",	     	    ("Hits Correlation" + titleExtra).c_str(),	    	    100, -25000, 25000, 100, -25000, 25000);
  hProjectedErrorX_      	= new TH1F("HProjectedErrorX",       	    ("Projected Error X" + titleExtra).c_str(),	    	    100, 0, 30);
  hProjectedErrorY_      	= new TH1F("HProjectedErrorY",       	    ("Projected Error Y" + titleExtra).c_str(),	    	    100, 0, 30);
  hChargeAsymmetry_      	= new TH1F("HChargeAsymmetry",       	    ("Charge Asymmetry" + titleExtra).c_str(),	    	     21, -50, 50);
  hChargeAsymmetryNorm_  	= new TH1F("HChargeAsymmetryNorm",   	    ("Charge Asymmetry Norm" + titleExtra).c_str(),   	     21, -50, 50);
  h2DChargeAsymmetry_    	= new TH2F("H2DChargeAsymmetry",     	    ("2D Charge Asymmetry" + titleExtra).c_str(),     	     21, -50, 50, 21, -1.1, 1.1);
  hEfficiency_                  = new TH1F("HEfficiency",	            ("Efficiency" + titleExtra).c_str(),	              1,  -0.5, 0.5);
  hEfficiencyX_                 = new TH1F("HEfficiencyX",	            ("Efficiency Along X" + titleExtra).c_str(),	     60, -30, 30);
  hEfficiencyVsTrigger_         = new TH1F("HEfficiencyVsTrigger",	    ("Efficiency Vs Trigger" + titleExtra).c_str(),	    100,  0, 500000);
  hAdcX_                        = new TH1F("HAdcX",	                    ("Adc Along X" + titleExtra).c_str(),	             60, -30, 30);
  hEfficiencyNorm_              = new TH1F("HEfficiencyNorm",		    ("Efficiency Norm" + titleExtra).c_str(),		      1,  -0.5, 0.5);
  hEfficiencyXNorm_             = new TH1F("HEfficiencyXNorm",		    ("Efficiency Along X Norm" + titleExtra).c_str(),	     60, -30, 30);
  hEfficiencyVsTriggerNorm_     = new TH1F("HEfficiencyVsTriggerNorm",	    ("Efficiency Vs Trigger Norm" + titleExtra).c_str(),    100,  0, 500000);
  hAdcXNorm_                    = new TH1F("HAdcXNorm",	                    ("Adc Along X Norm" + titleExtra).c_str(),	             60, -30, 30);
  
  for(int pixelEvent=0; pixelEvent<theTrackTree_->GetEntries(); pixelEvent++)
  {
    theTrackTree_->GetEntry(pixelEvent);
    for(unsigned int track=0; track<theTrackEvent_.numberOfTracks; track++)
    {
      hBeamProfileX_    ->Fill((theTrackEvent_.xSlope[track]*Z+theTrackEvent_.xIntercept[track]*10));
      hBeamProfileY_    ->Fill((theTrackEvent_.ySlope[track]*Z+theTrackEvent_.yIntercept[track]*10));
    }
  }
  for(int hits=0; hits<theStripTree_->GetEntries(); hits++)
  {
    theStripTree_->GetEntry(hits);
    hNumberOfHits_->Fill(theStripEvent_.numberOfHits);  	      
    for(int i=0; i<theStripEvent_.numberOfHits && i<MAX_HITS; i++)
    {
      hStripProfile_	  ->Fill(theStripEvent_.istrip[i]); 		    
      hAssociatedTriggers_->Fill(theStripEvent_.triggerBco[i]-theStripEvent_.bco[i]); 
      hAdc_		  ->Fill(theStripEvent_.adc[i]);				    
    }									    
    hNumberOfClusters_->Fill(theStripEvent_.numberOfClusters);  	      
    for(int i=0; i<theStripEvent_.numberOfClusters && i<MAX_CLUSTERS; i++)
    {
      hClusterSize_          ->Fill(theStripEvent_.clusterSize[i]);	   
      hClusterProfile_       ->Fill(theStripEvent_.clusters[i]);	   
      hMeasuredProfile_      ->Fill(theStripEvent_.measuredX[i]);		   
      h2DClusterSizeVsAdc_   ->Fill(theStripEvent_.clusterSize[i],theStripEvent_.clusterAdc[i]);	   
      h2DClusterSizeVsCharge_->Fill(theStripEvent_.clusterSize[i],theStripEvent_.clusterCharge[i]);	   
    }
  }

  
  int lastPixelMatch             = 0;
  int adjustmentToTriggerCounter = 0;
  int currentAdjustmentToTrigger = 0;
  int adjustmentToTrigger        = 0;
  int noMatchCounter             = 0;

  int lastStripEvent = 0;
  for(int pixelEvent=0; pixelEvent<theTrackTree_->GetEntries(); pixelEvent++)
  {
    theTrackTree_->GetEntry(pixelEvent);
    if(theTrackEvent_.numberOfTracks != 1) continue;
    int track = 0;
    if(!(theTrackEvent_.chi2[track] < 3 && theTrackEvent_.numberOfHits[track] == 8)) continue; 
    double projectedX;
    double projectedY;
    double projectedZ;
    ROOT::Math::SVector<double,4> tParameters(theTrackEvent_.xSlope[track],theTrackEvent_.xIntercept[track],
    					      theTrackEvent_.ySlope[track],theTrackEvent_.yIntercept[track]);
    stripPlane_.getPredictedGlobal(tParameters, projectedX, projectedY, projectedZ);
    projectedX = projectedX*10;
    projectedY = projectedY*10;
    projectedZ = projectedZ*10;
    //cout << "Pixel Trigger #: " << theTrackEvent_.trigger << " Projected X: " << projectedX << endl;  
     for(int stripEvent=lastStripEvent; stripEvent<theStripTree_->GetEntries(); stripEvent++)
    {
      theStripTree_->GetEntry(stripEvent);
      //for(unsigned int c=0; c<theStripEvent_.numberOfClusters && c<MAX_CLUSTERS; c++)
      //{
      //  cout << "Strip Trigger #: " << theStripEvent_.trigger << " Measured X: " << theStripEvent_.measuredX[c] << endl;  
      //}
      if(theTrackEvent_.trigger < theStripEvent_.trigger) break;
      if(theTrackEvent_.trigger == theStripEvent_.trigger)
      {  
	lastStripEvent = stripEvent + 1;
	const double window = 200;
        double bestDelta = window;
	//cout << "------->>>>> Found Trigger #: " << theTrackEvent_.trigger << " Number of clusters: " << theStripEvent_.numberOfClusters << endl;
        
	int matchedCluster = -1; 
	double deltaX = 0;
	double deltaY = 0;
        double globalX = projectedX/10;
        double globalY = projectedY/10;
        double globalZ = projectedZ/10;
	stripPlane_.fromGlobalToLocal(&globalX,&globalY,&globalZ);
        std::pair<int,int> rc;
        rc = stripPlane_.getPixelCellFromLocal(globalX,globalY);
        double xPixelCenter = stripPlane_.getPixelCenterLocalX((unsigned int)rc.second);
        double xRes = (xPixelCenter - globalX)*10;
        bool found = false;
	if(rc.first != -1 && rc.second != -1)
	{
	  hEfficiencyNorm_->Fill(0);
	  hEfficiencyXNorm_->Fill(xRes);
	  hEfficiencyVsTriggerNorm_->Fill(theTrackEvent_.trigger);
	  for(int h=0; h<theStripEvent_.numberOfHits && h<MAX_HITS; h++)
          {
	    if(theStripEvent_.istrip[h] == rc.second || theStripEvent_.istrip[h]+1 == rc.second || theStripEvent_.istrip[h]-1 == rc.second)
	    {
	      //cout << "#Hits: " << theStripEvent_.numberOfHits << " Projected Strip: " << rc.second  << " Found: " << theStripEvent_.istrip[h] << endl;
	      hEfficiency_->Fill(0);
	      hEfficiencyX_->Fill(xRes);	      
	      hAdcX_->Fill(xRes,theStripEvent_.adc[h]);
	      hAdcXNorm_->Fill(xRes);
	      hEfficiencyVsTrigger_->Fill(theTrackEvent_.trigger);
	      found = true;
	      break;
	    }
	  }
	  if(!found)
	  {
	    cout << "Trigger: " << theTrackEvent_.trigger << " #Hits: " << theStripEvent_.numberOfHits << " Projected Strip: " << rc.second << endl;
	    for(int h=0; h<theStripEvent_.numberOfHits && h<MAX_HITS; h++)
            {
	      cout << "Event Strip: " << theStripEvent_.istrip[h] << endl;
	    }
	  }
	}
	for(int c=0; c<theStripEvent_.numberOfClusters && c<MAX_CLUSTERS; c++)
        {
          //cout << "Trigger #: " << theTrackEvent_.trigger << " Projected X: " << projectedX << " Measured X: " << theStripEvent_.measuredX[c] << endl;  
		//cout << "old: " << -(theTrackEvent_.xSlope[track]*Z+theTrackEvent_.xIntercept[track]*10) << " new: " << projectedX << endl;

          double measuredX = theStripEvent_.measuredLocalX[c]/10;
          double measuredY = globalY;
          double measuredZ = 0;
	  stripPlane_.fromLocalToGlobal(&measuredX,&measuredY,&measuredZ);
	  measuredX = measuredX*10;
	  measuredY = measuredY*10;
	  double delta = measuredX - projectedX;
          if(fabs(delta) < window && fabs(delta) < bestDelta)
          {
	    matchedCluster = c;
	    deltaX = delta;
	    deltaY = theStripEvent_.measuredY[c] - projectedY;
            bestDelta = fabs(delta);
	  }
	}    
	if(matchedCluster == -1) break;//No match   
	    
	hResidualY_ ->Fill(deltaY);
	//cout << "Delta: " << deltaX << " delta from local: " << measuredX - projectedX << " diff: " << deltaX-(measuredX - projectedX) << endl;
	    
	hResidual_		      ->Fill(deltaX);
      	hClusterSizeMatched_	      ->Fill(theStripEvent_.clusterSize[matchedCluster]);	     
      	h2DClusterSizeVsChargeMatched_->Fill(theStripEvent_.clusterSize[matchedCluster],theStripEvent_.clusterCharge[matchedCluster]);	     
	hResidualXVsX_    	      ->Fill(projectedX,deltaX);
  	hResidualXVsY_    	      ->Fill(projectedY,deltaX);
	hResidualNormXVsX_	      ->Fill(projectedX);
  	hResidualNormXVsY_	      ->Fill(projectedY);
  	h2DResidualXVsX_  	      ->Fill(projectedX,deltaX);
  	h2DResidualXVsY_  	      ->Fill(projectedY,deltaX);
        
	if(theStripEvent_.clusterSize[matchedCluster] == 1)// && theStripEvent_.clusterAdc[c] < 1)
	{
	  hResidualSize1_->Fill(deltaX);
	  if(theStripEvent_.clusterAdc[matchedCluster] == 7)
	    hResidualSize1Adc7_->Fill(deltaX);
	  else if(theStripEvent_.clusterAdc[matchedCluster] == 6)
	    hResidualSize1Adc6_->Fill(deltaX);
	  else if(theStripEvent_.clusterAdc[matchedCluster] == 5)
	    hResidualSize1Adc5_->Fill(deltaX);
	  else if(theStripEvent_.clusterAdc[matchedCluster] == 4)
	    hResidualSize1Adc4_->Fill(deltaX);
	  else if(theStripEvent_.clusterAdc[matchedCluster] == 3)
	    hResidualSize1Adc3_->Fill(deltaX);
	  else if(theStripEvent_.clusterAdc[matchedCluster] == 2)
	    hResidualSize1Adc2_->Fill(deltaX);
	  else if(theStripEvent_.clusterAdc[matchedCluster] == 1)
	    hResidualSize1Adc1_->Fill(deltaX);
	  else if(theStripEvent_.clusterAdc[matchedCluster] == 0)
	    hResidualSize1Adc0_->Fill(deltaX);
	  hChargeAsymmetry_	->Fill(-(30-fabs(deltaX))/(fabs(deltaX)/deltaX),-1*fabs(deltaX)/deltaX);
          hChargeAsymmetryNorm_ ->Fill(-(30-fabs(deltaX))/(fabs(deltaX)/deltaX));
          h2DChargeAsymmetry_	->Fill(-(30-fabs(deltaX))/(fabs(deltaX)/deltaX),-1*fabs(deltaX)/deltaX);
        }
	else if(theStripEvent_.clusterSize[matchedCluster] == 2)
	{
	  hResidualSize2_	->Fill(deltaX);
          //Since we are considering only events with 1 cluster this works!
	  double deltaCharge = (thresholdConverter[theStripEvent_.adc[0]] - thresholdConverter[theStripEvent_.adc[1]])/(thresholdConverter[theStripEvent_.adc[0]] + thresholdConverter[theStripEvent_.adc[1]]);
	  if(theStripEvent_.istrip[0] < theStripEvent_.istrip[1])
	  {
	    deltaCharge = - deltaCharge;
	  }
	  hChargeAsymmetry_	->Fill(deltaX,deltaCharge);
          hChargeAsymmetryNorm_ ->Fill(deltaX);
          h2DChargeAsymmetry_	->Fill(deltaX,deltaCharge);
	}
	else if(theStripEvent_.clusterSize[matchedCluster] == 3) hResidualSize3_->Fill(deltaX);
        hProjectedErrorX_ ->Fill(theTrackEvent_.xError[track]*10);
        hProjectedErrorY_ ->Fill(theTrackEvent_.yError[track]*10);
                    
        hCorrelation_->Fill(projectedX,theStripEvent_.measuredX[matchedCluster]);
      }
    }
  }


  hResidualXVsX_->Divide(hResidualNormXVsX_);
  hResidualXVsY_->Divide(hResidualNormXVsY_);
  hChargeAsymmetry_->Divide(hChargeAsymmetryNorm_);
  hEfficiency_->Divide(hEfficiencyNorm_);
  hEfficiencyX_->Divide(hEfficiencyXNorm_);
  hEfficiencyVsTrigger_->Divide(hEfficiencyVsTriggerNorm_);
  hAdcX_->Divide(hAdcXNorm_);
  
  cout << "##### Final Efficiency: " << hEfficiency_->GetBinContent(1) << endl;
  
  STDLINE("Writing Output File", ACRed);
  theFile_->Write();
  //theFile_->Write("",TObject::kOverwrite);
  theFile_->Close();
  STDLINE("Done!", ACGreen);
  
}


//------------------------------------------------------------------------------------
StripTestReader::~StripTestReader ()
{
}

//------------------------------------------------------------------------------------
// This is the method of the StripTestReader class where users are supposed 
// to implement their own private code for analysis. Here you will find 
// an exampl of how to retrieve and manipulate components of the event
// and the geometry.
//
// NOTE: for a detailed description of the Event class, its public access
//       methods and the meaning of invidual containers, please consult
//       ../include/Event.h and comments therein.
//
void StripTestReader::analyzeEvent(unsigned int event)
{
  Event::fittedTracksDef           & fittedTracks             = theEvent_->getFittedTracks    ()     ;
  Event::chi2VectorDef             & chi2                     = theEvent_->getFittedTracksChi2()     ;
  Event::trackCandidatesDef        & trackPoints              = theEvent_->getTrackCandidates ()     ;
  Event::fittedTracksCovarianceDef & fittedTrackCovariance    = theEvent_->getFittedTracksCovariance();

  if( fittedTracks.size() == 0 ) return ;

  theTrackEvent_.trigger = theEvent_->getTrigger();//-1;
  theTrackEvent_.numberOfTracks = fittedTracks.size();
  for(unsigned int tr=0; tr<fittedTracks.size(); ++tr)
  {
    ROOT::Math::SVector<double,4> tParameters = fittedTracks[tr] ;
    theTrackEvent_.numberOfHits[tr] = trackPoints[tr].size();
    theTrackEvent_.chi2[tr]	    = chi2[tr];
    theTrackEvent_.xSlope[tr]	    = tParameters[0];
    theTrackEvent_.ySlope[tr]	    = tParameters[2];
    theTrackEvent_.xIntercept[tr]   = tParameters[1];
    theTrackEvent_.yIntercept[tr]   = tParameters[3];
    //theTrackEvent_.xError[tr]       = stripPlane_.getTrackErrorsOnPlane(tParameters, fittedTrackCovariance[tr]).first;
    //theTrackEvent_.yError[tr]       = stripPlane_.getTrackErrorsOnPlane(tParameters, fittedTrackCovariance[tr]).second;
    theTrackEvent_.xError[tr]       = sqrt(fittedTrackCovariance[tr](1,1));
    theTrackEvent_.yError[tr]       = sqrt(fittedTrackCovariance[tr](3,3));
    theTrackTree_->Fill();
    stringstream ss;
    ss.str("") ;
    ss << "Event: " << event << " Trigger: " << theTrackEvent_.trigger;
    //    ss.setf(std::ios_base::right,std::ios_base::adjustfield) ;
    //    ss << std::setprecision(8) << " ";
    //    ss << std::setw( 4) << tr  
    //        << std::setw(15) << tParameters[0] 
    //        << std::setw(15) << tParameters[2] 
    //        << std::setw(15) << tParameters[1] 
    //        << std::setw(15) << tParameters[3] 
    //        << std::setprecision(3)
    //        << std::setw( 6) << chi2[tr] ; 
  
    //STDLINE(ss.str(),ACGreen) ;
  }
  lastTriggerInTelescope_ = theTrackEvent_.trigger;
}

void StripTestReader::getStripEvents()
{
  const int dataTriggerDelay = 57; 
  std::stringstream filename;
  //filename << "/data/TestBeam/2013_11_November/PxSuiteData/Run" << pxSuiteRunNumber_ << "_FED1_Raw.dat";
  filename << "/home/cmstestbeam/PxSuite/Run" << pxSuiteRunNumber_ << "_FED1_Raw.dat";
  //filename << "/Users/uplegger/Programming/Data/PxSuiteData/Run" << pxSuiteRunNumber_ << "_FED1_Raw.dat";
  //std::ifstream* inputFile = new std::ifstream (filename.str().c_str(), std::ifstream::in);
  uint64_t currentBCO_;

  unsigned int word;		         
  uint64_t lastBCO = 0ULL;
  struct stat statbuf;
  uint32_t  trigger;
  uint32_t  correctedTrigger = -1;
  uint64_t  previousTriggerBco = -1;
  if ( stat(filename.str().c_str(), &statbuf) < 0)
  {
    std::cout << "OOOps!\n";
    return;
  }
  std::cout << "File size: " << statbuf.st_size << std::endl;
  int fd = open(filename.str().c_str(), O_RDONLY);
  unsigned char * recvbuf = (unsigned char *)mmap(NULL, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if ((void *)recvbuf == (void*)(-1))
  {
    std::cout << "Failed to build buffer " << strerror(errno) << std::endl;
    return;
  }
  //  unsigned char recvbuf[200000];
  
  int len = statbuf.st_size -2;
  /*    inputFile->read( (char*)(&line),sizeof(char));
	inputFile->read( (char*)(&line),sizeof(char));
	while( inputFile->good())
	{
	inputFile->read( (char*)(&line),sizeof(char));
	recvbuf[len] = line;
	//dlen = sizeof(daddr);
    
	//    if ( ( len = recvfrom(s,recvbuf,BUFLEN,0,(struct sockaddr *)&daddr,(socklen_t *)&dlen) ) < 0 ) {
	//      fprintf( stderr, "%s : recvfrom() - %s\n", argv[0], strerror(errno) );
	//      exit(1);  										      
	//    }
	len++;
	}
  */
  recvbuf += 2;
   
  int type, dataType;
  bcoIsComplete_ = false;
  
  int dataState = 3;
  int triggerState = 2;
  int whatTriggeredLongBCO = -1;
  int deltaBCOWhenTriggered = 0; 
  string dataName; 
  for ( int i=0; i<len/4; i++ ) 
  //  for ( int i=0; i<10000/4; i++ ) 
  {								      
    word = ntohl(((unsigned int *)recvbuf)[i]);
    type     = word & 0xf;
	dataType = (word>>4) & 0xf;		       

    if     (type == 8 && dataType == 0x1) dataName = "Low BCO";
    else if(type == 8 && dataType == 0x2) dataName = "High BCO";
    else if(type == 8 && dataType == 0xa) dataName = "Low trigger";
    else if(type == 8 && dataType == 0xb) dataName = "High trigger";
    else if(type == 8 && dataType == 0xc) dataName = "Trigger type";
    else if((type & 0x1) == 1 )           dataName = "Strip";
	else dataName = "Unknown type";
/*
    if(type == 8 && dataType == 0x1)
	  cout << "------- END EVENT ------" << endl << endl << "--------- BEGIN EVENT -----------" << endl; 
    cout << hex << "Type: " << type << " dataType: " << dataType << " data name: " << dataName << dec << endl;
*/
    if(dataState == 2 && triggerState == 0 && type == 8 && dataType == 1)
      cout << "***************************ERROR: I must have a data or a trigger after a bco is complete!" << endl;
    if( (type & 0x1) == 1 )//Strip 
    {
      if(!(dataState == 2 || dataState == 3))
    	cout << "ERROR: Bco is incomplete! dataState=" << dataState << endl;
      dataState = 3;
      PxDecodedHit hit;

      hit.chan      = (word>>27) & 0x07;
      hit.chipid    = (word>>24) & 0x07;
      hit.set	    = (word>>12) & 0x1f;
      hit.strip     = (word>>17) & 0x0f;
      hit.bco	    = (word>>4 ) & 0xff;
      hit.adc	    = (word>>1 ) & 0x7;
      hit.istrip    = computeSensorStrip(hit.chipid, hit.set, hit.strip);

      unsigned int lowBCO   = (currentBCO_ & 0xffULL);
      uint64_t     eventBCO = (currentBCO_ & 0xffffffffffffff00ULL) + hit.bco;
      if(lowBCO < hit.bco)
    	eventBCO -= 0x100;
	  if(whatTriggeredLongBCO == -1)
	  { 
	    whatTriggeredLongBCO = 0;
		currentBCO_ -= 20;
      }
      if((int)(eventBCO - currentBCO_) < 0) eventBCO += 0x100;
      //if(currentBCO_-eventBCO > 20) eventBCO += 0x100;
	  //if(currentBCO_-eventBCO < 0) eventBCO -= 0x100;
	  
	  deltaBCOWhenTriggered = eventBCO - currentBCO_;
      //cout << "Data BCO:    " << hex << eventBCO << dec << endl;
/*
      if(memory_.find(eventBCO) == memory_.end())
    	memory_[eventBCO] = new PxEvent(eventBCO);    
      memory_[eventBCO]->setRawHit(word);
      memory_[eventBCO]->setDecodedHit(hit);
*/
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
          if	 (memory_.find(eventBCO  ) != memory_.end()) event = memory_[eventBCO];
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
//      cout << "- Delta: " << deltaBCOWhenTriggered << "----- EVENT DATA BCO: " << hex << eventBCO << " - " << hit.bco << " curr bco: " << currentBCO_ << dec << " strip: " << hit.istrip << endl;  
      //cout << "EVENT DATA Strip data-> Bco: " << hex << hit.bco << dec
      //     << " istrip: " << hit.istrip
      //     << " adc: " << hit.adc
      //     << endl;
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
//        cout << "BCO: " << hex << currentBCO_ << dec << endl;
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
        //cout << "Trigger " << trigger << " corrected: " << correctedTrigger << endl;					    
	    correctedTrigger = trigger;
        //memory_[currentBCO_]->setTriggerNumber(trigger);
        //std::cout <<  "END EVENT------TRIGGER = " << currentTrigger_ << " Bco: " << hex << currentTriggerBCO_-dataTriggerDelay << " ---------------------" << dec<< endl << endl;		     
        unsigned int lowBCO   = (currentBCO_ & 0xffULL);
        uint64_t   eventBCO = (currentBCO_ & 0xffffffffffffff00ULL) + currentTriggerBCO_;

        eventBCO -= dataTriggerDelay;
	deltaBCOWhenTriggered = eventBCO - currentBCO_;
//        cout << "- Delta: " << deltaBCOWhenTriggered <<"--Trigger " << trigger << " lcorrected: " << correctedTrigger  << " Trig BCO: " << hex << eventBCO << " curr bco: " << currentBCO_ << dec << " WhatTriggeredBCO: " << whatTriggeredLongBCO << endl;					    
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
//        cout << "- Delta: " << deltaBCOWhenTriggered <<"--Trigger " << trigger << " lcorrected: " << correctedTrigger  << " Trig BCO: " << hex << eventBCO << " curr bco: " << currentBCO_ << dec << " WhatTriggeredBCO: " << whatTriggeredLongBCO << endl;					    

        PxEvent* event;
        //for(int i=-10; i<10; i++)
        //  if(memory_.find(eventBCO+i) != memory_.end())
        //    cout << "FOUND " << i << endl;
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
//          cout << hex << "NO MATCH -> Delta: " << eventBCO - currentBCO_ << " -- Trigger number " << currentTrigger_ << " Event BCO: " << eventBCO << dec << endl;
          memory_[eventBCO] = new PxEvent(eventBCO);
          event = memory_[eventBCO];
        }
        else
        {
//          cout << hex << "!! MATCH -> Delta: " << eventBCO - currentBCO_ << " -- Trigger number " << currentTrigger_ << " Event BCO: " << eventBCO << dec << endl;
          //cout << "Matched BCO! For trigger Number " << currentTrigger_ << endl;
          if	 (memory_.find(eventBCO  ) != memory_.end()) event = memory_[eventBCO];
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
          /*
            cout << "END OF EVENT ############# TRIG DATA BCO: " << currentTrigger_ << "-" << hex << eventBCO  << " - " << event->getBCONumber() 
            << " | " << currentTriggerBCO_  
            << " = " << event->getDecodedHit(0).bco 
            << " -> delta = " << event->getDecodedHit(0).bco-currentTriggerBCO_ 
            << dec 
            << endl << endl;
          */
        }
        if(event->getTriggerNumber() > lastTriggerInTelescope_) break;    
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
  close(fd);
  cout << "--------------Last Trigger #: " << correctedTrigger << "-------------" << endl;
}

int StripTestReader::computeSensorStrip(int chip,int set,int strip) {
  if (chip < 1 || chip > 5)
    return -1000;
  if (set < 0 || set > 31)
    return -1000;
  if (strip < 0 || strip > 15)
    return -1000;
  static unsigned char set_number[] = { 255, 255, 255, 255, 255, 255, 255, 255,
					255, 255,   0,   1,   4,   5,   3,   2,
					255, 255,  12,  13,   8,   9,  11,  10,
					255, 255,  15,  14,   7,   6, 255, 255  };
  static unsigned char strip_number[] = { 255, 255, 255, 255,
                                          255,   0,   2,   1,
                                          255,   6,   4,   5,
                                          255,   7,   3, 255  };

  return 128*(chip-1)+set_number[set]*8+strip_number[strip];
}

