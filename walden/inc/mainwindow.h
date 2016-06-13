#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QFileInfo>
#include <QMainWindow>
#include <QModelIndex>
#include <QModelIndexList>
#include <QPoint>
#include <QResizeEvent>
#include <QSize>
#include <QSplitter>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QVector>

#include <TCanvas.h>
#include <TFile.h>
#include <TFolder.h>
#include <TH1F.h>
#include <TKey.h>
#include <TSystem.h>

#include <extractor.h>
#include <indent.h>
#include <qrootcanvas.h>

#include <cmath>
#include <sstream>
#include <unistd.h>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
            ~MainWindow();

public slots:
    void        handle_root_events    (void                ) ; // ToROOT6

private slots:
    void        on_BrowsePB_clicked   (void                );
    void        searchReversedPath    (const QModelIndex & );
    void        on_ExpandPB_clicked   (void                );
    void        on_ContractPB_clicked (void                );
    void        listPaths             (void                );
    void        onCustomContextMenu   (const QPoint      & );
    QString     getSaveFileName       (void                );
    void        selectAll             (const QModelIndex & );
    void        drawAll               (void                );
    void        saveAll               (void                );
    void        on_savePB_clicked     (void                );
    void        histoMapDumper        (void                );
    void        resizeEvent           (QResizeEvent      * );
    void        clickedSelection      (void                );
    void        createStatusBar       (void                );
    void        resizeChildrens       (int                 ,
                                       int                 );

private:

    typedef multimap<string, string>::iterator    hMapIterDef_;
    typedef std::pair<hMapIterDef_, hMapIterDef_> pairDef_    ;

    Extractor               * extract_      ;
    Ui::MainWindow          * ui            ;
    stringstream              ssPath_       ;
    QString                   qStringPath_  ;
    QStringList               pathList_     ;
    TFile                   * inputFile_    ;
    TFile                   * outputFile_   ;
    multimap<string,string>   histoMap_     ;
    QRootCanvas             * canvas_       ;
    TCanvas                 * c_            ;
    QModelIndexList           clickedList_  ;
    QList<int>                splitterSizes_;
    bool                      foldersInFile_;

protected:
    QTimer                  * fRootTimer_ ; // ToROOT6

} ;

#endif // MAINWINDOW_H
