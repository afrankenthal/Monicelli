/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include <iostream>

#include <QApplication>
#include <QBitmap>
#include <QPixmap>
#include <QPainter>
#include <QPlastiqueStyle>
#include <QSplashScreen>
#include <QEventLoop>
#include <QThread>

#include "MessageTools.h"

#include "mainwindow.h"
#include <stdlib.h>

//================================================================================
int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(Monicelli);

    QApplication::setStyle(new QPlastiqueStyle()) ;

    QApplication app(argc, argv);

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
    splash.show();

    app.processEvents();

    window.show();

    splash.raise() ;

    sleep(1) ;

    splash.finish(&window);

    std::string color = std::string(ACYellow)+std::string(ACBold)+std::string(ACReverse) ;
    STDLINE("",color);
    STDLINE("+--------------------------------------------------+",color);
    STDLINE("|                                                  |",color);
    STDLINE("|      W e l c o m e  t o  M o n i c e l l i       |",color);
    STDLINE("|                                                  |",color);
    STDLINE("|        The MTEST pixel-telescope tracks          |",color);
    STDLINE("|            pattern recognition code              |",color);
    STDLINE("|                                                  |",color);
    STDLINE("|           D. Menasce, L. Moroni, S. Terzo, 2011  |",color);
    STDLINE("|                                                  |",color);
    STDLINE("+--------------------------------------------------+",color);
    STDLINE("",color);

    app.exec();

    STDLINE("Done! (A fake assert condition will now be generated to actually bring the client to a halt...).",ACCyan);

    std::cout << std::endl << std::endl ;

    assert(0) ;

    return 0 ;
}
