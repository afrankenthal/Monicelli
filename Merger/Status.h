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
 
#ifndef _Status_h_
#define _Status_h_

#include <iostream>
#include <iomanip>
  // error(62) | rep_err(61) | tok_in_err(60) | tok_out_err(59) | st_err(58) | plaq(57:56) | tokin(55:42) | tokout(41:28) | trig(27:14) | ub(13:0)

class Status{
 public:
  unsigned long long data;
  int error;
  int repErr;
  int tokenInError;
  int tokenOutError;
  int stError;
  int plaquette;
  int tokenIn;
  int tokenOut;
  int trigger;
  int ultraBlack;
  friend std::ostream& operator<<(std::ostream& os, const Status& status)
  {
//    os << "[Status::operator<<()]\t" 
    os 
       << "Data: "           << std::hex << (status.data >> 32) << (status.data & 0xffffffff) << std::dec
       << " error : "	     << status.error
       << " repErr: "        << status.repErr
       << " tokenInError: "  << status.tokenInError
       << " tokenOutError: " << status.tokenOutError
       << " stError: "	     << status.stError
       << " plaquette: "     << status.plaquette
       << " tokenIn: "	     << status.tokenIn
       << " tokenOut: "      << status.tokenOut
       << " trigger: "       << status.trigger
       << " ultraBlack: "    << status.ultraBlack;
    return os;
  } 
};


#endif
