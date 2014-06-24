#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lengthyComputation.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    lA_ = new lengthyComputation(this) ;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonA_clicked()
{
    lA_->start() ;
}
