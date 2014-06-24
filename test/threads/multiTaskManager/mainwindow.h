#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <map>

#include <QFutureWatcher>
#include <QMainWindow>
#include <QPushButton>
#include <QStringList>

#include "cProcess.h"
#include "parser.h"
#include "builder.h"
#include "aligner.h"

class parser ;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
            ~MainWindow();


private slots:
    void loadFiles    (void         ) ;
    void start        (int theButton) ;

private:
    Ui::MainWindow * ui;

    void enableButtons(bool enable  ) ;

    typedef std::vector<QPushButton*>            buttonsDef ;
    typedef std::map<int, QFutureWatcher<int>* > watcherDef ;
    typedef std::map<int, cProcess*>             processDef ;

    watcherDef    watcher_      ;
    processDef    theProcesses_ ;

    QStringList   files_        ;

    buttonsDef    buttons_      ;

};

#endif // MAINWINDOW_H
