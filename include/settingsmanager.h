#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <iostream>
#include <maintabs.h>
#include <ui_maintabs.h>

using namespace std ;

class settingsManager
{
 public:
         settingsManager(Ui::mainTabs * ui) ;
        ~settingsManager(void) ;

    void save           (void) ;
    void read           (void) ;

 private:
    Ui::mainTabs * ui_ ;
};

#endif // SETTINGSMANAGER_H
