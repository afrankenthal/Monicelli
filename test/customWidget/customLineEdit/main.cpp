#include <QtGui/QApplication>
#include "customLineEdit.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    customLineEdit w;
    w.show();

    return a.exec();
}
