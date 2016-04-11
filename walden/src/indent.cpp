#include "indent.h"

#include <string>
#include <sstream>

using namespace std;

//==================================================================================
indent::indent(void)
{
    aBlank_ = "  " ;
}

//==================================================================================
string indent::writeBlank(int a)
{
    stringstream ss ;

    for (int i=0; i<a; i++)
    {
        ss << aBlank_;
    }

    return ss.str();
}
