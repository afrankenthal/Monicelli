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
 
// g++ -o splitter splitter.cpp -I$BOOSTINC -L$BOOSTLIB -lboost_regex

#include <iostream>
#include <string>
#include <vector>
#include <boost/regex.hpp>

using namespace std;

typedef std::vector<std::string> stringVDef ;

stringVDef splitPath(std::string s) ;

int main(int argc)
{
   cout << "Please enter a full file path-name with slash-separated characters: " << endl ;

   string s;
   if(argc == 1)
   {
      cout << "Enter text to split (or \"quit\" to exit): ";
      getline(cin, s);
      if(s == "quit") return 0;
   }
   else
      s = "root/folderA/folderB/histogram";

   stringVDef pathV_ = splitPath(s) ;
   
   cout << "Head: " << pathV_[0]               << endl ;
   cout << "Tail: " << pathV_[pathV_.size()-1] << endl ;
   
}

stringVDef splitPath(std::string s) 
{
   stringVDef path ;

   boost::regex re("\\/");
   boost::sregex_token_iterator i(s.begin(), s.end(), re, -1);
   boost::sregex_token_iterator j;

   path.push_back( *i ) ;
 
   while(i != j) {path.push_back( *i++ );}
   
  return path ;
}
