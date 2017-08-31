#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <unistd.h>

#include <QCheckBox>
#include <QDir>
#include <QDateTime>
#include <QFileSystemModel>
#include <QFileInfo>
#include <QFileInfoList>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHeaderView>
#include <QIcon>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPixmap>
#include <QPoint>
#include <QProcess>
#include <QRadioButton>
#include <QRegExp>
#include <QScrollBar>
#include <QSettings>
#include <QStringList>
#include <QTableWidgetItem>
#include <QTextDocument>
#include <QTimer>
#include <QTreeView>

#include "dfilesystemmodel.h"
#include "inc/MessageTools.h"
#include "inc/manipulatepath.h"

using namespace std;
//=========================================================================================
namespace     Ui
{
 class MainWindow;
}
//=========================================================================================
class MainWindow : public QMainWindow
{
    Q_OBJECT

 public:
    explicit MainWindow                       (      QWidget    * parent = 0) ;
            ~MainWindow                       (void                         ) ;

 private slots:
    QString  getPictureDetails                (      QString                ) ;
    void     on_analyzeDeepDiffsPB_clicked    (void                         ) ;
    void     on_analyzeDiffsPB_clicked        (void                         ) ;
    void     on_chooseDestinationPB_clicked   (void                         ) ;
    void     on_chooseSourcePB_clicked        (void                         ) ;
    void     on_connectionCB_clicked          (      bool                   ) ;
    void     on_moveAllToDestinationPB_clicked(void                         ) ;
    void     on_moveAllToSourcePB_clicked     (void                         ) ;
    void     on_selectAllD_clicked            (void                         ) ;
    void     on_selectAllS_clicked            (void                         ) ;
    void     on_startProcessPB_clicked        (void                         ) ;
    void     on_startProcessPB_2_clicked      (void                         ) ;
    void     processOutput                    (void                         ) ;
    void     reactToClick                     (      QString,
                                               int   row,
                                               int   column                 ) ;
    void     reactToClickD                    (int   row,
                                               int   column                 ) ;
    void     reactToClickS                    (int   row,
                                               int   column                 ) ;
    void     readSettings                     (void                         ) ;
    void     targetDFolder                    (const QModelIndex &          ) ;
    void     targetSFolder                    (const QModelIndex &          ) ;
    void     targetFolder                     (const QModelIndex &,
                                                     QString                ) ;

    void on_finderPB_clicked();

private:    // typedef members
    typedef  map<QString, pair<QTreeView    *, QLineEdit *> > quadrantDef_           ;
    typedef  map<QString, pair<QTableWidget *, QLineEdit *> > diffTableDef_          ;
    typedef  map<QString, DFileSystemModel  * >               systemModDef_          ;
    typedef  map<QString, vector<QString                  > > fileNamesForPathDef_   ;
    typedef  map<QString, fileNamesForPathDef_              > fileNamesForTargetDef_ ;

private:     // method members
    void     analizeDifferences               (bool                         ) ;
    void     checkDeepDifferences             (void                         ) ;
    void     clearDestinationArea             (void                         ) ;
    void     copyFiles                        (      QString,
                                                     QString                ) ;
    void     renameFiles                      (      QString,
                                                     QString                ) ;
    void     findFullPath                     (const QModelIndex & ,
                                                     QString     &          ) ;
    void     getCounts                        (void                         ) ;
    void     on_drillDownPB_clicked           (      QString                ) ;
    void     on_moveAllPB_clicked             (      QString                ) ;
    void     on_selectAll_clicked             (      QTableWidget * ,
                                                     QRadioButton *         ) ;
    void     populateTreeView                 (      QString                ) ;
    void     writeSettings                    (void                         ) ;
    void     displayTargetFolders             (int                          ) ;

 private:    // variable members
    Ui::MainWindow                * ui_                                    ;
    QLineEdit                     * theLineEdit_                           ;
    QModelIndex                     currentSTarget_                        ;
    QModelIndex                     currentDTarget_                        ;
    map<QString, QString>           arrows_                                ;
    systemModDef_                   model_                                 ;
    quadrantDef_                    quadrant_                              ;
    diffTableDef_                   diffTable_                             ;
    map<QString, QString>           lastFolder_                            ;
    map<QString, QString>           lastExpandedFolder_                    ;
    map<QString, QDir::SortFlags>   sortFlagsMap_                          ;
    map<QString, bool>              diffPossible_                          ;
    vector<QString>                 targets_                               ;
    vector<QString>                 fileNames_                             ;
    fileNamesForPathDef_            fileNamesForPath_                      ;
    fileNamesForTargetDef_          fileNamesForTarget_                    ;
    double                          scaleFactor_                           ;
    QString                         imagesDir_                             ;
    QString                         lArrow_                                ;
    QString                         rArrow_                                ;
    QString                         checkedIcon_                           ;
    int                             nameColumn_                            ;
    int                             sizeColumn_                            ;
    int                             timeColumn_                            ;
    int                             dirsColumn_                            ;
    int                             fileColumn_                            ;
    int                             moveColumn_                            ;
    int                             chekColumn_                            ;
    int                             numColumns_                            ;
    QProcess                      * batchProcess_                          ;
    int                             counts_                                ;
    stringstream                    ss_                                    ;
    bool                            firstPopulate_                         ;
    string                          noMatchReason_                         ;
    bool                            differencesAnalyzed_                   ;
};

#endif // MAINWINDOW_H
