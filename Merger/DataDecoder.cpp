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
 
#include "DataDecoder.h"
#include <iostream>

//-----------------------------------------------------------------------
DataDecoder::DataDecoder() : data_(0), ordered_(false)
{
}

//-----------------------------------------------------------------------
DataDecoder::DataDecoder(const std::string& data, bool isTimestamp, int station) : ordered_(false)
{
  convert(data);
  addStation(station);
}

//-----------------------------------------------------------------------
DataDecoder::DataDecoder(unsigned long long data, bool isTimestamp, int station) : data_(data), ordered_(false)
{
}

//-----------------------------------------------------------------------
DataDecoder::~DataDecoder()
{
}

//-----------------------------------------------------------------------
void DataDecoder::setData(const std::string& dataString, int station)
{
  ordered_ = false;
  convert(dataString);
  addStation(station);
//  order();
}

//-----------------------------------------------------------------------
void DataDecoder::setData(unsigned long long data, int station)
{
  ordered_ = false;
  data_ = data;
  addStation(station);
//  order();
}

//-----------------------------------------------------------------------
void DataDecoder::setTimestamp(const std::string& dataString)
{
  convert(dataString);
}

//-----------------------------------------------------------------------
void DataDecoder::setTimestamp(unsigned long long data)
{
  data_ = data;
}

//-----------------------------------------------------------------------
unsigned long long DataDecoder::getTimestamp(void)
{
  return data_ & 0xffffffffffffff;    
}

//-----------------------------------------------------------------------
unsigned long DataDecoder::getPacketNumber(void)
{
  return (data_ >> 55) && 0xff;
}
//-----------------------------------------------------------------------
bool DataDecoder::isData(void)
{
  if(!ordered_){
    order();
  }
  return !(data_ >> 63);
}
//-----------------------------------------------------------------------
unsigned long long DataDecoder::convert(const std::string& dataString)
{
  const unsigned int size = 8;//sizeof(unsigned long long);
  data_ = 0;
  for(unsigned int d=0; d<size; d++){
    data_ += (unsigned char)dataString[size-d-1];
    if(d!=7) data_ = data_ << 8;
  }
  return data_;
}

//-----------------------------------------------------------------------
unsigned long long DataDecoder::order(void)
{
  if(!ordered_){
    const unsigned int size = 8;//sizeof(unsigned long long);
    unsigned long long tmpData = 0;
    for(unsigned int byte=0; byte<size; byte++){
      tmpData += ((data_ >> ((size-byte-1)*8))& 0xff) << (byte*8);
    }
    data_ = tmpData;
    ordered_ = true;
  }
  return data_;
}

//-----------------------------------------------------------------------
unsigned long long DataDecoder::unOrderedData(void)
{
  if(ordered_){
    const unsigned int size = 8;//sizeof(unsigned long long);
    unsigned long long tmpData = 0;
    for(unsigned int byte=0; byte<size; byte++){
      tmpData += ((data_ >> ((size-byte-1)*8))& 0xff) << (byte*8);
    }
    return tmpData;
  }
  return data_;
}

//-----------------------------------------------------------------------
Hit DataDecoder::decodeHit(const std::string& dataString, int station)
{

  ordered_ = false;
  convert(dataString);
  addStation(station);
  return decodeHit();
}

//-----------------------------------------------------------------------
Hit DataDecoder::decodeHit(unsigned long long data, int station)
{
  ordered_ = false;
  data_ = data;
  addStation(station);
  return decodeHit();
}

//-----------------------------------------------------------------------
Hit DataDecoder::decodeHit()
{
  // All hits in event
  // the high byte reserved bits (63:56)
  // the low 7 bytes of hit quad word are saved (55:0)
  // Quad Word: 
  // reserved(63:52) | trigger number(51:32) | plaquette(31:29) | data0/debug1(28) | chipid(27:23) | row(22:16) | col(15:10) | pulseheight(9:0)
  Hit hit;
  if(ordered_){
    hit.data = unOrderedData();
  }
  else{
    hit.data = data_;
    order();
  }
  hit.adc     = data_ & 0x3ff;
  hit.col     = (data_ >> 10) & 0x3f;
  hit.row     = (data_ >> 16) & 0x7f;
  hit.cid     = (data_ >> 23) & 0xf;
//  dataDebug = (data_ >> 28) & 0x1;
  hit.plaq    = (data_ >> 29) & 0x7;
  hit.trig    = (data_ >> 32) & 0xfffff;
  hit.station = (data_ >> 56) & 0xf;
  hit.detId   = (hit.station << 5) + (hit.plaq << 3) + hit.cid;
  return hit;
}

//-----------------------------------------------------------------------
Status DataDecoder::decodeStatus()
{
  // All hits in event
  // the high byte reserved bits (63:56)
  // the low 7 bytes of hit quad word are saved (55:0)
  // Quad Word: 
  // error(62) | rep_err(61) | tok_in_err(60) | tok_out_err(59) | st_err(58) | plaq(57:56) | tokin(55:42) | tokout(41:28) | trig(27:14) | ub(13:0)
  Status status;
  if(ordered_){
    status.data = unOrderedData();
  }
  else{
    status.data = data_;
    order();
  }
  status.error         = (data_ >> 62) & 0x1;
  status.repErr        = (data_ >> 61) & 0x1;
  status.tokenInError  = (data_ >> 60) & 0x1;
  status.tokenOutError = (data_ >> 59) & 0x1;
  status.stError       = (data_ >> 58) & 0x1;
  status.plaquette     = (data_ >> 56) & 0x3;
  status.tokenIn       = (data_ >> 42) & 0x3fff;
  status.tokenOut      = (data_ >> 28) & 0x3fff;
  status.trigger       = (data_ >> 14) & 0x3fff;
  status.ultraBlack    =  data_        & 0x3fff;

  return status;
}

//-----------------------------------------------------------------------
int DataDecoder::decodeTrigger(const std::string& dataString){
  convert(dataString);
  return decodeTrigger();
}

//-----------------------------------------------------------------------
int DataDecoder::decodeTrigger(unsigned long long data){
  data_ = data;
  return decodeTrigger();
}

//-----------------------------------------------------------------------
int DataDecoder::decodeTrigger(){
  if(!ordered_){
    order();
  }
  return (data_ >> 32) & 0xfffff;
}

//-----------------------------------------------------------------------
void DataDecoder::addStation(int station){
  if(station != -1){
    unsigned long long longStation = station;
    order();
    unsigned long long reset = 0x00ffffffffffffff;
    data_ &= reset;
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    data_ += ((longStation & 0xf) << 56);
  }
}
