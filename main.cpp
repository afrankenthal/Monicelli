/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

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
 
#include <iostream>

#include <QApplication>
#include <QBitmap>
#include <QPixmap>
#include <QPainter>
#include <QPlastiqueStyle>
#include <QSplashScreen>
#include <QEventLoop>
#include <QThread>

#include <TApplication.h>

#include "MessageTools.h"

#include "mainwindow.h"
#include <stdlib.h>

//================================================================================
int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(Monicelli);

    system("clear") ;

    TApplication rootapp("Simple Qt ROOT Application", &argc, argv);

    QApplication app(argc, argv);
    QApplication::setStyle(new QPlastiqueStyle()) ;

    std::stringstream ss_ ;

    char* envVariables;
    envVariables = getenv("MonicelliDir");
    bool missingEnvVariable = false;
    if(envVariables == NULL)
    {
        FATAL("The 'MonicelliDir' environment variable is not defined",ACYellow);
        missingEnvVariable = true;
    }
    envVariables = getenv("Monicelli_RawData_Dir");
    if(envVariables == NULL)
    {
        FATAL("The 'Monicelli_RawData_Dir' environment variable is not defined",ACYellow) ;
        missingEnvVariable = true;
    }
    envVariables = getenv("Monicelli_DataSample_Dir");
    if(envVariables == NULL)
    {
        FATAL("The 'Monicelli_DataSample_Dir' environment variable is not defined",ACYellow) ;
        missingEnvVariable = true;
    }
    envVariables = getenv("Monicelli_CalSample_Dir");
    if(envVariables == NULL)
    {
        FATAL("The 'Monicelli_CalSample_Dir' environment variable is not defined",ACYellow) ;
        missingEnvVariable = true;
    }
    envVariables = getenv("MonicelliOutputDir");
    if(envVariables == NULL)
    {
        FATAL("The 'MonicelliOutputDir' environment variable is not defined",ACYellow) ;
        missingEnvVariable = true;
    }
    else
    {
        QFile tmpFile(envVariables) ;
        if( !tmpFile.exists() )
        {
            ss_.str("") ; ss_ << "The " << envVariables << " directory (MonicelliOutputDir) does not exist" ;
            FATAL(ss_.str(),ACRed) ;
            FATAL("Please create one...",ACCyan) ;
            missingEnvVariable = true;
        }
    }
    envVariables = getenv("Monicelli_XML_Dir");
    if(envVariables == NULL)
    {
        FATAL("The 'Monicelli_XML_Dir' environment variable is not defined",ACYellow) ;
        missingEnvVariable = true;
    }
    if(missingEnvVariable)
    {
        app.closeAllWindows() ;
        app.quit() ;
        exit(0);
    }

    MainWindow window;

    std::string splashIcon = std::string(getenv("MonicelliDir")) + "/images/Monicelli.png" ;

    QPixmap pixmap(splashIcon.c_str());

    QSplashScreen splash(pixmap);

    splash.setMask(pixmap.mask());

    app.processEvents();

    splash.show();

    app.processEvents();

//    sleep(5) ;

    window.show();

    splash.raise() ;

    splash.finish(&window);

    std::string color = std::string(ACYellow)+std::string(ACBold)+std::string(ACReverse) ;
    STDLINE("",color);
    STDLINE("+--------------------------------------------------+",color);
    STDLINE("|                                                  |",color);
    STDLINE("|      W e l c o m e  t o  M o n i c e l l i       |",color);
    STDLINE("|                                                  |",color);
    STDLINE("|        The MTEST pixel-telescope tracks          |",color);
    STDLINE("|            pattern recognition code              |",color);
    STDLINE("|    at Fermi National Accelerator Laboratory      |",color);
    STDLINE("|                                                  |",color);
    STDLINE("|        D. Menasce, L. Moroni, S. Terzo           |",color);
    STDLINE("|      J. Ngadiuba, L. Uplegger, L. Vigani         |",color);
    STDLINE("|                                                  |",color);
    STDLINE("+--------------------------------------------------+",color);
    STDLINE("",color);

    app.exec();

    STDLINE("",ACWhite);
    STDLINE("Done! Hope you enjoyed...",ACCyan);

    std::cout << std::endl << std::endl ;

    return 0 ;
}
