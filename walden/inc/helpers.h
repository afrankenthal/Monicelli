#ifndef HELPERS_H
#define HELPERS_H

#include <iostream>
#include <sstream>
#include <algorithm>

#include "ANSIColors.h"

using namespace std;

#define STDLINE(MSG,COLOR)                                   \
        {                                                    \
         stringstream msg_ ;                                 \
         stringstream PFs_ ;                                 \
         string PF_ = __PRETTY_FUNCTION__ ;                  \
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
                   << endl ;                            \
        }

#define D(s1,s2) cout << __LINE__            << "]\t[" \
                      << __PRETTY_FUNCTION__ << "] "   \
                      << s1                  << " "    \
                      << s2                  << endl ;

#define D3(s1,s2,s3) cout << __LINE__            << "]\t[" \
                      << __PRETTY_FUNCTION__ << "] "       \
                      << s1                  << " "        \
                      << s2                  << " "        \
                      << s3                  <<endl;




#endif // HELPERS_H
