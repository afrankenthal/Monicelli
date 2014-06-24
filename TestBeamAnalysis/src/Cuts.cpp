#include "Cuts.h"
#include "DataClass.h"
#include <iostream>

using namespace std;


/*-----------------------------------------------------------------------------------*/
Cuts::Cuts(DataClass& data) : theData_(data), oldEventId_(-1), chi2Cut_(3)
{

}

/*-----------------------------------------------------------------------------------*/
//Cuts(const Cuts& cuts) : theData_(cuts.theData_), oldEventId_(cuts.oldEventId_), chi2Cut_(3)
//{
//}	

/*-----------------------------------------------------------------------------------*/
//Cuts& operator=(const Cuts& cuts)
//{
//  theData_    = cuts.theData_;
//  oldEventId_ = cuts.oldEventId_;
//}	

/*-----------------------------------------------------------------------------------*/
Cuts::~Cuts(void)
{
}

/*-----------------------------------------------------------------------------------*/
void Cuts::reset(void){
  oldEventId_ = -1;
}

/*-----------------------------------------------------------------------------------*/
bool Cuts::newEvent(void)
{
  //	cout << __PRETTY_FUNCTION__
  //	     << (oldEventId_ != theData_.getEvtId()) << ":" 
  //	     << oldEventId_ << ":"
  //		 << theData_.getEvtId()
  //		 << endl;
  if(oldEventId_ != theData_.getEvtId())
    {
      oldEventId_ = theData_.getEvtId();
      return true;
    }
  return false;
}

/*-----------------------------------------------------------------------------------*/
bool Cuts::ntrack(int tracks)
{
  return (theData_.getNtrack() <= tracks);
}

/*-----------------------------------------------------------------------------------*/
bool Cuts::chi2Excl(int plane)
{
  return theData_.getHasHit(plane) && theData_.getNdofExcl(plane) != 0;
}

/*-----------------------------------------------------------------------------------*/
bool Cuts::hasHits(int plane)
{
  return theData_.getHasHit(plane);
}

/*-----------------------------------------------------------------------------------*/
bool Cuts::allTrackProj()
{
  return true;
}

/*-----------------------------------------------------------------------------------*/
 bool Cuts::hitTrackProj(int plane)
 {
   return theData_.getHasHit(plane);
 }

/*-----------------------------------------------------------------------------------*/
bool Cuts::xResid(int plane)
{
  return hasHits(plane) && getChi2Ndof(plane) < chi2Cut_;
}

/*-----------------------------------------------------------------------------------*/
bool Cuts::yResid(int plane)
{
  return hasHits(plane) && getChi2Ndof(plane) < chi2Cut_;
}

/*-----------------------------------------------------------------------------------*/
bool Cuts::efficiency()
{
  return 
  //(theData_.getNtrack() <= 1) && 
  (theData_.getChi2()/theData_.getNdof() <= 2);
}

/*-----------------------------------------------------------------------------------*/
double Cuts::getChi2Ndof(int plane)
{
  if(theData_.getNdofExcl(plane))
    return theData_.getChi2Excl(plane)/theData_.getNdofExcl(plane);
  //cout << __PRETTY_FUNCTION__ << "Ndof == 0!!!!!" << endl;	
  return -1;
}

/*-----------------------------------------------------------------------------------*/
bool Cuts::stationHits(int station)
{
  if( station == 1)
  {
    if( theData_.getHasHit(0) == 1 && theData_.getHasHit(1) == 1 && theData_.getHasHit(2) == 1  )
      return true;
    else 
      return false;  	    
  }

  if (station == 2)
  { 
    if( theData_.getHasHit(4) == 1 && theData_.getHasHit(5) == 1 && theData_.getHasHit(6) == 1  )
      return true;
    else 
      return false;
  }
  cout << "\nInvalid Station\n";
  return -1;
}
