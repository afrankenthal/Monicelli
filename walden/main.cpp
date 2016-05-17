#include "mainwindow.h"
#include <helpers.h>
#include <QApplication>
#include <QPlastiqueStyle>
#include <TApplication.h>

int main(int argc, char *argv[])
{
    TApplication rootapp("Extractor", &argc, argv);

    QApplication a(argc, argv);
    QApplication::setStyle(new QPlastiqueStyle()) ;

    MainWindow w;
    w.setWindowTitle("walden - ROOT File Navigator");
    w.show();

    std::string color = std::string(ACYellow)+std::string(ACBold)+std::string(ACReverse) ;

    STDLINE("+--------------------------------------------------+",color);
    STDLINE("|                                                  |",color);
    STDLINE("|                 Welcome to Walden                |",color);
    STDLINE("|                                                  |",color);
    STDLINE("|                ROOT File Navigator               |",color);
    STDLINE("|                                                  |",color);
    STDLINE("|                     D. Zuolo                     |",color);
    STDLINE("|                                                  |",color);
    STDLINE("+--------------------------------------------------+",color);

    a.exec() ;

    return 0;
}

TApplication::~TApplication()
{
    STDLINE("Destructor",ACRed);
}
