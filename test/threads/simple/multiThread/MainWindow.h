#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <sstream>

#include <QMainWindow>
#include "processA.h"
#include "QTimer"

#include "threaderClass.h"

namespace Ui {
    class MainWindow;
}

class processA ;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    std::stringstream ss_ ;


    threader * theThreader_ ;

    QTimer   * theTimer_    ;

    processA * theProcessA_ ;

    int maxEvents_ ;

private slots:
    void on_JenniferPB_clicked();
    void on_abortPB_clicked();
    void on_startPB_clicked();

    void advanceProgressBar(void) ;

};

#endif // MAINWINDOW_H
