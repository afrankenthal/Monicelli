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
