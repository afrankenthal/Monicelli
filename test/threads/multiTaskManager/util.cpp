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
 
#include <util.h>
#include <iostream>
#include <boost/regex.hpp>

//=======================================================================
Util::stringVDef Util::split(std::string s, std::string regexp)
{
    Util::stringVDef path ;

    boost::regex re(regexp);
    boost::sregex_token_iterator i(s.begin(), s.end(), re, -1);
    boost::sregex_token_iterator j;

    while(i != j) {path.push_back( *i++ ) ;}

    return path ;
}

//=======================================================================
std::string Util::getClassName(std::string aString)
{
    Util::stringVDef splitted = Util::split(aString, std::string("::")) ;
    boost::cmatch what;
    static const boost::regex exp("(.*)?\\s(\\w+)?$", boost::regex::perl);

    if( boost::regex_match(splitted[0].c_str(), what, exp, boost::match_extra) )
    {
        return what[2] ;
    }

    return std::string("Undefined") ;
}
