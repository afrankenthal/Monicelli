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
