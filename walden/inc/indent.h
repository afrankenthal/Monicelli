#ifndef INDENT_H
#define INDENT_H

#include <string>

using namespace std;

class indent
{
public:
           indent     (void) ;
          ~indent     (void){;}
    string writeBlank (int ) ;
    string getABlank  (void) {return aBlank_ ;}
private:
    string aBlank_ ;
};

#endif // INDENT_H
