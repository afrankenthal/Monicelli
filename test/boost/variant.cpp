// g++ -o variant variant.cpp -I./ -I$BOOSTINC -L$BOOSTLIB -lboost_regex
#include "variant.h"
int main()
{
  std::vector<VectorOfIntStringOrFloat> mixedBag ;

  std::vector<int> vi ;
  vi.push_back(123)   ;
  vi.push_back(41)    ;
  
  std::vector<std::string> vs ;
  vs.push_back("a")    ;
  vs.push_back("b")    ;
  
  mixedBag.push_back(vi) ;
  mixedBag.push_back(vs) ;

  dumpV  dumpMixedBag ;
  clearV clearMixedBag;
  
  std::for_each(mixedBag.begin(), mixedBag.end(), boost::apply_visitor(dumpMixedBag));
  std::for_each(mixedBag.begin(), mixedBag.end(), boost::apply_visitor(clearMixedBag));
  std::for_each(mixedBag.begin(), mixedBag.end(), boost::apply_visitor(dumpMixedBag));
}

