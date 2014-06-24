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
