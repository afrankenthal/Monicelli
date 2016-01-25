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
 
#ifndef ALPHANUM__HPPD
#define ALPHANUM__HPPD

/*
The Alphanum Algorithm is an improved sorting algorithm for strings
containing numbers.  Instead of sorting numbers in ASCII order like a
standard sort, this algorithm sorts numbers in numeric order.

The Alphanum Algorithm is discussed at http://www.DaveKoelle.com

This implementation is Copyright (c) 2008 Dirk Jagdmann <doj@cubic.org>.
It is a cleanroom implementation of the algorithm and not derived by
other's works. In contrast to the versions written by Dave Koelle this
source code is distributed with the libpng/zlib license.

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you
       must not claim that you wrote the original software. If you use
       this software in a product, an acknowledgment in the product
       documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and
       must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
       distribution. */

/* $Header: /gwpool/cvsroot/Monicelli/batch/inc/alphanum.h,v 1.1 2011/02/02 14:53:57 menasce Exp $ */

#include <cassert>
#include <functional>
#include <string>
#include <string.h>
#include <sstream>

#include <iostream>
#include <typeinfo>

using namespace std; 

namespace doj
{
//  namespace 
//  {

  bool alphanum_isdigit(const char c) ;
  int  alphanum_impl   (const char *l, const char *r) ;

  template <typename lT, typename rT> int alphaNumComp(const lT& left, const rT& right) ;

  template <> int alphaNumComp<std::string>(const std::string& l, const std::string& r) ;

  ////////////////////////////////////////////////////////////////////////////

  // now follow a lot of overloaded alphaNumComp() functions to get a
  // direct call to alphanum_impl() upon the various combinations of c
  // and c++ strings.

  /**
     Compare l and r with the same semantics as strcmp(), but with
     the "Alphanum Algorithm" which produces more human-friendly
     results.

     @return negative if l<r, 0 if l==r, positive if l>r.
  */
  int alphaNumComp(            char * l,                     char* r) ;
  int alphaNumComp(const       char * l,  const              char* r) ;
  int alphaNumComp(            char * l,  const              char* r) ;
  int alphaNumComp(const       char * l,                     char* r) ;
  int alphaNumComp(const std::string& l,                     char* r) ;
  int alphaNumComp(            char * l,        const std::string& r) ;
  int alphaNumComp(const std::string& l,        const        char* r) ;
  int alphaNumComp(const        char* l,        const std::string& r) ;

  ////////////////////////////////////////////////////////////////////////////

  /**
     Functor class to compare two objects with the "Alphanum
     Algorithm". If the objects are no std::string, they must
     implement "std::ostream operator<< (std::ostream&, const Ty&)".
  */
  template<class Ty>
  struct alphanum_less : public std::binary_function<Ty, Ty, bool>
  {
    bool operator()(const Ty& left, const Ty& right) const
    {
      return alphaNumComp(left, right) < 0;
    }
  };

// }

}

#endif
