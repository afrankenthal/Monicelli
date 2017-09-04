#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <algorithm>
#include <unistd.h>
#include <sstream>
#include <iomanip>

#include <QMainWindow>
#include <QWidget>
#include <QListView>

#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QTextBrowser>
#include <QComboBox>
#include <QProcess>
#include <QListWidgetItem>
#include <QList>
#include <QSettings>
#include <QEvent>
#include <QTextBlock>
#include <QTextStream>
#include <QScrollBar>
#include <QMenu>
#include <QStringList>
#include <QValidator>
#include <QTimer>
#include <QFileSystemModel>
#include <QPalette>
#include <QColor>
#include <QColorDialog>
#include <QSplitter>
#include <QFileDialog>
#include <QClipboard>
#include <QMessageBox>

#include "filelistmodel.h" 
#include "MessageTools.h"
#include "marker.h"

namespace Ui 
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow                     (      QWidget     * parent=0   );
            ~MainWindow                     (void                           );

private:
    void     initializeStringFinder         (void                           );
    void     initializeCompareFiles         (void                           );
    void     setRootPath                    (const QString     & path       ) {rootPath_ = path;}
    void     dispatchResults                (      QString     & results   ,
                                                   QString     & errors     );
    void     readQSettings                  (      QString       id         );
    bool     eventFilter                    (      QObject     * obj       ,
                                                   QEvent      * event      );
    void     editFile                       (void                           );
    void     startEditorProcess             (      QString       file      ,
                                                   int           lineNumber );
    void     findFullPath                   (const QModelIndex & ,
                                                   QString     &            );
    void     compareFiles                   (      QString       lFile     ,
                                                   QString       rFile      );
    void     compareDirs                    (      QString       lFile     ,
                                                   QString       rFile      );
    void     formatLine                     (      QString     & line      ,
                                                   int           lineNumber );
    void     mergeFiles                     (      QTextEdit   * fTE       ,
                                                   QTextEdit   * tTE        );
    void     saveToFile                     (      QTextEdit   * te        ,
                                                   QLineEdit   * le         );

protected:
    void     closeEvent                     (      QCloseEvent * event      );

public slots:
    void     updateLog                      (      int           number     );
    void     updateListView                 (void                           );
    void     reactToClick                   (const QModelIndex & index      );

private slots:
    void     enableLogicCB                  (      QComboBox   * theCombo,
                                                   QTextEdit   * theTextEdit);
    void     on_findPB_clicked              (void                           );
    void     on_matchPathsTE_textChanged    (void                           );
    void     on_matchPathsCB_clicked        (void                           );
    void     on_searchStringsCB_clicked     (void                           );
    void     on_excludeStringsCB_clicked    (void                           );
    void     on_excludePathsCB_clicked      (void                           );
    void     on_restoreCB_clicked           (void                           );
    void     on_clearPathSelectionPB_clicked(void                           );
    void     on_searchStringsTE_textChanged (void                           );
    void     on_excludePathsTE_textChanged  (void                           );
    void     on_excludeStringsTE_textChanged(void                           );
    void     on_addSettingsPB_clicked       (void                           );
    void     on_settingsLE_editingFinished  (void                           );
    void     on_clearAllSettingsCB_clicked  (void                           );
    void     on_restrictClearCB_clicked     (void                           );
    void     on_saveSettingsPB_clicked      (void                           );
    void     on_loadSettingsPB_clicked      (void                           );
    QString  getSelectedPaths               (void                           );
    QString  selectSettings                 (      QString       id         );
    void     checkEditor                    (void                           );
    void     selectL                        (const QModelIndex & index      );
    void     selectR                        (const QModelIndex & index      );
    void     on_synchronizeCB_clicked       (      bool          checked    );
    void     on_comparePB_clicked           (void                           );
    void     on_editPB_clicked              (void                           );
    void     on_deleteSettingsPB_clicked    (void                           );
    void     on_toRightCB_clicked           (void                           );
    void     on_toLeftCB_clicked            (void                           );
    void     on_lSavePB_clicked             (void                           );
    void     on_rSavePB_clicked             (void                           );
    void     on_louvrePB_clicked            (void                           );
    void     synchLPanelSplitter            (      int           pos       ,
                                                   int                      );
    void     synchRPanelSplitter            (      int           pos       ,
                                                   int                      );
    void     on_saveToClipboardPB_clicked   (void                           );
    void     on_lSelectedLE_textChanged     (const QString     & lFileName  );
    void     on_rSelectedLE_textChanged     (const QString     & rFileName  );
    void     on_compareDirsLegendPB_clicked (void                           );

private:
    typedef map<string , QFileSystemModel *> fileModelDef  ;
    typedef map<QString, QProcess         *> processMapDef ;

    Ui::MainWindow * ui_               ;
    QTextBrowser   * logViewer_        ;
    FileListModel  * model_            ;
    QListView      * allPaths_         ;
    QLineEdit      * lineEdit_         ;
    QString          rootPath_         ;
    QStringList      selectedPaths_    ;
    QString          qsettingPath_     ;
    QString          HOME_             ;
    QString          FINDERHOME_       ;
    QValidator     * settingsValidator_;
    processMapDef    startEditor_      ;
    fileModelDef     fileModel_        ;
    bool             firstEditorCall_  ;
    QTimer         * editorTimer_      ;
    QString          lRootPath_        ;
    QString          rRootPath_        ;
    stringstream     ss_               ;
    QProcess       * finderProcess_    ;
};

#endif // MAINWINDOW_H
