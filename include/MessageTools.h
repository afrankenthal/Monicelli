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
 
#ifndef MESSAGETOOLS_H
#define MESSAGETOOLS_H

#include <iostream>
#include <sstream>
#include <algorithm>

#include "ANSIColors.h"

#define FATAL(MSG,COLOR)                                     \
        {                                                    \
         std::stringstream msg_ ;                            \
         std::stringstream PFs_ ;                            \
         std::string PF_ = __PRETTY_FUNCTION__ ;             \
         int PFSize = PF_.size() ;                           \
         int maxL = 90 ;                                     \
         if(PFSize > 30) {PFSize = 30 ;}                     \
         for(int i=0; i<PFSize; ++i) {PFs_<<PF_[i];}         \
         if( PFSize < 30 )                                   \
	 {                                                   \
          for(int i=0; i<33-PFSize; ++i) {PFs_<<" ";}        \
	 }                                                   \
         if(PFSize<(int)PF_.size())                          \
          {maxL -= 3; PFs_<<ACRed<<"...";}                   \
         msg_ << ACCyan  << ACBold                           \
              << __LINE__                                    \
              << ACPlain << ACYellow                         \
              << "\t] ["                                     \
              << ACRed << ACBold << ACReverse                \
              << "FATAL:"                                    \
              << ACPlain                                     \
              << ACWhite << ACBold                           \
              << " "                                         \
              << PFs_.str()                                  \
              << ACPlain << ACYellow                         \
              << "]"  ;                                      \
         int blankSize = maxL - msg_.str().size() + 1 ;      \
         if( blankSize < 0 ) blankSize = 2 ;                 \
         std::string blanks = "" ;                           \
         for(int i=0; i<blankSize; ++i)                      \
         {                                                   \
          blanks += " " ;                                    \
         }                                                   \
         std::cout << msg_.str()                             \
                   << blanks                                 \
                   << COLOR << ACBold                        \
                   << MSG                                    \
                   << ACPlain                                \
                   << std::endl ;                            \
        }

#define STDLINE(MSG,COLOR)                                   \
        {                                                    \
         std::stringstream msg_ ;                            \
         std::stringstream PFs_ ;                            \
         std::string PF_ = __PRETTY_FUNCTION__ ;             \
         int PFSize = PF_.size() ;                           \
         int maxL = 90 ;                                     \
         if(PFSize > 30) {PFSize = 30 ;}                     \
         for(int i=0; i<PFSize; ++i) {PFs_<<PF_[i];}         \
         if( PFSize < 30 )                                   \
	 {                                                   \
          for(int i=0; i<33-PFSize; ++i) {PFs_<<" ";}        \
	 }                                                   \
         if(PFSize<(int)PF_.size())                          \
          {maxL -= 4; PFs_<<ACRed<<"...";}                   \
         else                                                \
          {PFs_<<ACPlain<<ACPlain<<ACPlain<<ACPlain;}        \
         msg_ << ACCyan  << ACBold                           \
              << __LINE__                                    \
              << ACPlain << ACYellow                         \
              << "\t] ["                                     \
              << ACPlain                                     \
              << ACWhite << ACBold                           \
              << PFs_.str()                                  \
              << ACPlain << ACYellow                         \
              << "]"  ;                                      \
         int msgS = msg_.str().size() + 1 ;                  \
         if(  msgS < 30 ) msgS = 30 ;                        \
         int blankSize = maxL - msgS ;                       \
         if( blankSize < 0 ) blankSize = 3 ;                 \
         std::string blanks = "" ;                           \
         for(int i=0; i<blankSize; ++i)                      \
         {                                                   \
          blanks += " " ;                                    \
         }                                                   \
         std::cout << msg_.str()                             \
                   << blanks                                 \
                   << COLOR << ACBold                        \
                   << MSG                                    \
                   << ACPlain                                \
                   << std::endl ;                            \
        }

#define STDSNAP(MSG,COLOR)                                   \
        {                                                    \
         std::stringstream msg_ ;                            \
         std::stringstream PFs_ ;                            \
         std::string PF_ = __PRETTY_FUNCTION__ ;             \
         int PFSize = PF_.size() ;                           \
         int maxL = 90 ;                                     \
         if(PFSize > 30) {PFSize = 30 ;}                     \
         for(int i=0; i<PFSize; ++i) {PFs_<<PF_[i];}         \
         if( PFSize < 30 )                                   \
	 {                                                   \
          for(int i=0; i<33-PFSize; ++i) {PFs_<<" ";}        \
	 }                                                   \
         if(PFSize<(int)PF_.size())                          \
          {maxL -= 4; PFs_<<ACRed<<"...";}                   \
         msg_ << ACCR                                        \
              << ACCyan  << ACBold                           \
              << __LINE__                                    \
              << ACPlain << ACYellow                         \
              << "\t] ["                                     \
              << ACPlain                                     \
              << ACWhite << ACBold                           \
              << PFs_.str()                                  \
              << ACPlain << ACYellow                         \
              << "]"  ;                                      \
         int blankSize = maxL - msg_.str().size() + 1 ;      \
         if( blankSize < 0 ) blankSize = 3 ;                 \
         std::string blanks = "" ;                           \
         for(int i=0; i<blankSize; ++i)                      \
         {                                                   \
          blanks += " " ;                                    \
         }                                                   \
         std::cout << msg_.str()                             \
                   << blanks                                 \
                   << COLOR << ACBold                        \
                   << MSG                                    \
                   << ACPlain                                \
                   << std::flush ;                           \
        }

class Utils
{
  public:
      Utils() {;}

      inline static int toInt(std::string inputString)
      {
          std::istringstream iss_ ;
          iss_.str(inputString)	  ;
          int n			  ;
          iss_ >> n		  ;

          return n                ;
      }
      //=============================================================================
      inline static double toDouble(std::string inputString)
      {
          std::istringstream iss_ ;
          iss_.str(inputString)	  ;
          double n	          ;
          iss_ >> n		  ;

          return n                ;
      }
      //=============================================================================
      inline static int toDec(unsigned long long inputBinary)
      {
          std::stringstream ss       ;
          ss.str("")                 ;
          int n = inputBinary        ;
          ss << std::dec << n        ;
          n = Utils::toInt(ss.str()) ;

          return n                   ;
      }

} ;

#endif // MESSAGETOOLS_H
