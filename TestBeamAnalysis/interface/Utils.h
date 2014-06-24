#ifndef Utils2_h
#define Utils2_h
#include <cmath>
#include <string>

class Utils2
{
 public:
   Utils2(){}
  ~Utils2(){}
  static double cint(double x)
  {
    if ((x-(int)x)>=.5){
      return x>=0?ceil(x):floor(x);
    }
    else
    {
      return x<0?ceil(x):floor(x);
    }
  }
  
  static std::string formFileName(std::string unformattedName)
  {
    std::string formattedName = "Histos_" + unformattedName;
    return formattedName;
  }
};


#endif
