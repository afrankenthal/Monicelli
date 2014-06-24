/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

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
         if(PFSize<(int)PF_.size())                          \
          {maxL -= 4; PFs_<<ACRed<<"...";}                   \
         msg_ << ACCyan  << ACBold                           \
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
                   << std::endl ;                            \
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
