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
 
#include <iostream>

#include "Event.h"

ClassImp(Event) 

//===================================================
Event::Event()
{
  myStruct_.x_ = -1 ;
  myStruct_.y_ = -1 ;
}

//===================================================
void Event::setEventNumber(int event)
{
 eventNumber_ = event ;
}

//===================================================
int Event::getEventNumber()
{
 return eventNumber_ ;
}

//===================================================
void Event::addTrack(double p)
{
 p_.push_back(p) ;
}

//===================================================
void Event::addVertex(int v)
{
 vertices_.push_back(v) ;
}

//===================================================
int Event::getNTracks(void)
{
 return (int)p_.size() ;
}

//===================================================
std::vector<int> Event::getVertices()
{
 return vertices_ ;
}

//===================================================
void Event::cleanData(void)
{
 p_.clear() ;
}

//===================================================
void Event::dumpTracks()
{
 std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tFound " << p_.size() << " tracks for event " << eventNumber_ << std::endl ;

 std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\t  x: " << myStruct_.x_ << std::endl ;
 
 for(std::vector<double>::iterator it=p_.begin(); it!=p_.end(); ++it)
 {
   std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\t  " << *it << std::endl ;
 }

 std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tFound " << vertices_.size() << " vertices for event " << eventNumber_ << std::endl ;
 for(std::vector<int>::iterator it=vertices_.begin(); it!=vertices_.end(); ++it)
 {
   std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\t  " << *it << std::endl ;
 }
}
