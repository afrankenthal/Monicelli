#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <iostream>
#include <sstream>
#include <maintabs.h>
#include <ui_maintabs.h>
#include <QSettings>

using namespace std ;

namespace Ui
{
  class maintabs ;
}

class settingsManager
{
 public:
         settingsManager(QWidget      * mainTabs,
                         Ui::mainTabs * ui      ) ;
        ~settingsManager(void                   ) ;

    void save           (QString configuration  ) ;
    void read           (QString configuration  ) ;

private:
    QWidget      * mainTabs_ ;
    Ui::mainTabs * ui_       ;
    stringstream   ss_       ;
};

#endif // SETTINGSMANAGER_H
