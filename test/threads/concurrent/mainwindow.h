#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "lengthyComputation.h"

class lengthyComputation ;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    lengthyComputation * lA_ ;

private slots:
    void on_pushButtonA_clicked();
};

#endif // MAINWINDOW_H
