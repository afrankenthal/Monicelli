#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

class Util
{
    public:

    typedef std::vector<std::string> stringVDef ;

    static stringVDef  split       (std::string aString,
                                    std::string regexp ) ;
    static std::string getClassName(std::string aString) ;

  private:
};

#endif // UTIL_H
