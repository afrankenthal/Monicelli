#include "mainwindow.h"
#include "ui_mainwindow.h"
//==================================================================
MainWindow::MainWindow (QWidget *parent   ) :
            QMainWindow(parent            ) ,
            ui_        (new Ui::MainWindow)
{
    ui_ ->setupUi      (this              ) ;

    QList<int> vSizes ;
    vSizes << 336 << 191 << 408 ;
    ui_->splitter->setSizes(vSizes) ;
    ui_->splitter->show() ;

    char* homePath   = getenv ("HOME"      ) ;
    char* finderHome = getenv ("FINDERHOME") ;
    HOME_            = QString(homePath    ) ;
    FINDERHOME_      = QString(finderHome  ) ;
    qsettingPath_    = HOME_ + QString("/.config/finder/");
    finderProcess_   = NULL ;

    if( FINDERHOME_.isEmpty())
    {
        cout << "\nFATAL: FINDERHOME envorment variable is not defined\n\n" << endl ;
        exit(0) ;
    }

    this->initializeStringFinder() ;

    // Things connected to file differences
    this->initializeCompareFiles() ;
}

//==================================================================
MainWindow::~MainWindow()
{
    delete ui_;
}

//==================================================================
void MainWindow::initializeCompareFiles(void)
{
//    lRootPath_ = "/Users/menasce/AnalysisTBF/" ;
//    rRootPath_ = "/Users/menasce/" ;

    QFileSystemModel *lModel = new QFileSystemModel;
    QFileSystemModel *rModel = new QFileSystemModel;
    lModel->setRootPath(lRootPath_);
    rModel->setRootPath(rRootPath_);
    ui_->lDirTV->setModel(lModel);
    ui_->rDirTV->setModel(rModel);
    ui_->lDirTV->header()->resizeSection(0,300);
    ui_->rDirTV->header()->resizeSection(0,300);
    ui_->lDirTV->setExpanded    (lModel->index(lRootPath_), true) ;
    ui_->rDirTV->setExpanded    (rModel->index(rRootPath_), true) ;
    ui_->lDirTV->setCurrentIndex(lModel->index(lRootPath_)      ) ;
    ui_->rDirTV->setCurrentIndex(rModel->index(rRootPath_)      ) ;
    ui_->lPanelTE->ensureCursorVisible() ;
    ui_->rPanelTE->ensureCursorVisible() ;

    connect(ui_->lDirTV, SIGNAL(activated(const QModelIndex &))  ,
            this,        SLOT  (selectL  (const QModelIndex &))) ;
    connect(ui_->rDirTV, SIGNAL(activated(const QModelIndex &))  ,
            this,        SLOT  (selectR  (const QModelIndex &))) ;
}

//==================================================================
void MainWindow::formatLine(QString & line, int lineNumber)
{
    stringstream ss ;
    QString t       ;
    if( lineNumber == 0 )
    {
        ss << setw(5) << setfill(' ') << " " << "]</font> "  ;
    }
    else
    {
        ss << setw(5) << setfill(' ') << lineNumber << "]</font> "  ;

    }
    t    = QString(ss.str().c_str()).replace(QString(" "), QString("&nbsp;"));
    ss.str("") ; ss << "<font color=\"#ff00de\" wheight=\"bold\">" << t.toStdString() << line.toStdString();
    line = QString(ss.str().c_str()) ;
    return ;
}
//==================================================================
void MainWindow::compareFiles(QString lFileName, QString rFileName)
{
    statusBar()->showMessage(tr("View populated"));

    QProcess    * diffProcess = new QProcess();
    QString       command     = "diff"        ;
    QStringList   arguments                   ;

    if( ui_->ignoreCaseCB  ->isChecked() ) arguments << "-i" << "-B" ;
    if( ui_->ignoreBlanksCB->isChecked() ) arguments << "-b" ;
    arguments << "-t" << "-y" << "-W" << "270" << lFileName << rFileName ;
    diffProcess->start(command,arguments) ;
    diffProcess->waitForFinished (3000)   ;
    diffProcess->waitForReadyRead(3000)   ;

    ui_->commandLineTE->setText(QString("diff ")+arguments.join(QString(" ")));

    QByteArray  result = diffProcess->readAllStandardOutput();
    QByteArray  error  = diffProcess->readAllStandardError ();
    quint32     offset = 134                                 ;
    QStringList all    = QString(result.data()).split('\n')  ;
    LCS         lcs ;

    QTextBlockFormat bf = ui_->lPanelTE->textCursor().blockFormat();

    ui_->lPanelTE->clear();
    ui_->rPanelTE->clear();

    int lLine = 0, rLine = 0;

    for(int i=0; i<all.size(); ++i)
    {
        QString t = all.at(i) ;
        if( t.size() == 0 )
        {
            lLine++; rLine++ ;
            if( ui_->diffOnlyCB->isChecked() ) continue ;
            QString emptyL = QString("") ;
            QString emptyR = QString("") ;

            this->formatLine(emptyL, lLine) ;
            this->formatLine(emptyR, rLine) ;

            ui_->lPanelTE->insertHtml(emptyL);
            ui_->rPanelTE->insertHtml(emptyR);
            ui_->lPanelTE->textCursor().insertBlock();
            ui_->rPanelTE->textCursor().insertBlock();
            continue ;
        }
        QString l = t.mid(0     ,         offset    );
        QString c = t.mid(offset,         1         );
        QString r = t.mid(offset+3,t.size()-offset-3);
        l         = l.replace(QString("<"), QString("&lt;")  );
        l         = l.replace(QString(">"), QString("&gt;")  );
        r         = r.replace(QString("<"), QString("&lt;")  );
        r         = r.replace(QString(">"), QString("&gt;")  );
        l         = l.replace(QString(" "), QString("&nbsp;"));
        r         = r.replace(QString(" "), QString("&nbsp;"));

        if(      c == QString("|") )
        {
            lLine++; rLine++ ;

            string ls = l.toStdString();
            string rs = r.toStdString();
            lcs.Correspondence(ls,rs)  ;
            l = QString(ls.c_str())    ;
            r = QString(rs.c_str())    ;
            this->formatLine(l, lLine) ;
            this->formatLine(r, rLine) ;
            bf.setBackground(QBrush(QColor(255,255,0,90)));
        }
        else if( c == QString("<") )
        {
            lLine++ ;
            this->formatLine(l, lLine) ;
            this->formatLine(r, 0    ) ;
            l = QString("<font color=\"yellow\" wheight=\"bold\">") + l + QString("</font>") ;
            r = QString("<font color=\"cyan\" wheight=\"bold\">"  ) + r + QString("</font>") ;
            bf.setBackground(QBrush(QColor(0,180,255,50)));
        }
        else if( c == QString(">") )
        {
            rLine++ ;
            this->formatLine(l, 0    ) ;
            this->formatLine(r, rLine) ;
            l = QString("<font color=\"cyan\" wheight=\"bold\">"  ) + l + QString("</font>") ;
            r = QString("<font color=\"yellow\" wheight=\"bold\">") + r + QString("</font>") ;
            bf.setBackground(QBrush(QColor(255,0,50,50)));
        }
        else
        {
            lLine++; rLine++ ;
            this->formatLine(l, lLine) ;
            this->formatLine(r, rLine) ;
            if( ui_->diffOnlyCB->isChecked() ) continue ;
            l = QString("<font wheight=\"bold\">")                  + l + QString("</font>") ;
            r = QString("<font wheight=\"bold\">")                  + r + QString("</font>") ;
            bf.setBackground(QBrush(QColor(0,0,0)));
        }

        ui_->lPanelTE->textCursor().setBlockFormat(bf);
        ui_->rPanelTE->textCursor().setBlockFormat(bf);
        ui_->lPanelTE->textCursor().insertHtml(l);
        ui_->rPanelTE->textCursor().insertHtml(r);
        ui_->lPanelTE->textCursor().insertBlock();
        ui_->rPanelTE->textCursor().insertBlock();
    }

    this->on_synchronizeCB_clicked(true) ;

    ui_->lPanelTE->moveCursor(QTextCursor::Start) ;
    ui_->rPanelTE->moveCursor(QTextCursor::Start) ;
}

//=========================================================================================
void MainWindow::selectR(const QModelIndex & index)
{
    QString fullPath = "" ;
    findFullPath(index, fullPath);
    ui_->rSelectedLE->setText(fullPath) ;
    if( ui_->lSelectedLE->text().isEmpty())
    {
        ui_->comparePB->setEnabled(false);
        ui_->editPB   ->setEnabled(false);
    }
    else
    {
        ui_->comparePB->setEnabled(true);
        ui_->editPB   ->setEnabled(true);
    }
}
//=========================================================================================
void MainWindow::selectL(const QModelIndex & index)
{
    QString fullPath = "" ;
    findFullPath(index, fullPath);
    cout << __LINE__ << "] full path: " << fullPath.toStdString() << endl ;
    ui_->lSelectedLE->setText(fullPath) ;
    if( ui_->rSelectedLE->text().isEmpty())
    {
        ui_->comparePB->setEnabled(false);
        ui_->editPB   ->setEnabled(false);
    }
    else
    {
        ui_->comparePB->setEnabled(true);
        ui_->editPB   ->setEnabled(true);
    }
}
//=========================================================================================
void MainWindow::findFullPath(const QModelIndex & folder, QString & fullPath)
{
    QModelIndex parentFolder = folder.parent() ;

    if( parentFolder.isValid())
    {
        fullPath += folder.data().toString() + QString("/") ;
        findFullPath(parentFolder, fullPath) ;
    }
    else
    {
        QString tmpPath = "" ;
        QStringList list = fullPath.split(QString("/")) ;
        for(int i=list.size()-1; i>=0; --i)
        {
            tmpPath += list[i] ;
            if( i>0 ) tmpPath += QString("/") ;
        }
        fullPath = tmpPath ;
        return;
    }
}
//==================================================================
void MainWindow::closeEvent(QCloseEvent */*event*/)
{

    QString   selectedPaths = this->getSelectedPaths()        ;
    QString   lastSettings  = ui_ ->settingsCB->currentText() ;
    QString   settingsList  = ""                              ;

    if( ! lastSettings.contains(QRegularExpression("\\w+")) )
    {
        ui_->settingsCB->insertItem(0,QString("default"),QVariant(QString("default")));
    }

    for(int i=0; i<ui_->settingsCB->count(); ++i)
    {
        settingsList += ui_->settingsCB->itemData(i).toString() + QString(" ") ;
    }

    QSettings allSettings(qsettingPath_+QString("allSettings.txt"), QSettings::NativeFormat);
    allSettings.setValue("settingsList",settingsList);
    allSettings.setValue("lastSettings",lastSettings);

    QSettings chosenSettings(qsettingPath_+lastSettings+QString(".txt"), QSettings::NativeFormat);

    QString fullLPath, fullRPath ;
    findFullPath(ui_->lDirTV->selectionModel()->currentIndex(),fullLPath);
    findFullPath(ui_->rDirTV->selectionModel()->currentIndex(),fullRPath);

    chosenSettings.setValue("rootPath"              , rootPath_                                   ) ;
    chosenSettings.setValue("selectedPaths"         , selectedPaths                               ) ;
    chosenSettings.setValue("matchPaths"            , ui_->matchPathsTE            ->toPlainText()) ;
    chosenSettings.setValue("searchStrings"         , ui_->searchStringsTE         ->toPlainText()) ;
    chosenSettings.setValue("excludePaths"          , ui_->excludePathsTE          ->toPlainText()) ;
    chosenSettings.setValue("suffixes"              , ui_->suffixesTE              ->toPlainText()) ;
    chosenSettings.setValue("searchStringsLogic"    , ui_->searchStringsLogicCB    ->currentText()) ;
    chosenSettings.setValue("caseSensitiveString"   , ui_->caseSensitiveStringCB   ->isChecked()  ) ;
    chosenSettings.setValue("excludeStringsLogic"   , ui_->excludeStringsLogicCB   ->currentText()) ;
    chosenSettings.setValue("excludeStringsTE"      , ui_->excludeStringsTE        ->toPlainText()) ;
    chosenSettings.setValue("caseSensitiveEString"  , ui_->caseSensitiveEStringCB  ->isChecked()  ) ;
    chosenSettings.setValue("matchPathsLogic"       , ui_->matchPathsLogicCB       ->currentText()) ;
    chosenSettings.setValue("caseSensitiveMatch"    , ui_->caseSensitiveMatchCB    ->isChecked()  ) ;
    chosenSettings.setValue("excludePathsLogic"     , ui_->excludePathsLogicCB     ->currentText()) ;
    chosenSettings.setValue("caseSensitiveExclusion", ui_->caseSensitiveExclusionCB->isChecked()  ) ;
    chosenSettings.setValue("lFilePath"             , fullLPath                                   ) ;
    chosenSettings.setValue("rFilePath"             , fullRPath                                   ) ;
    chosenSettings.setValue("lFile"                 , ui_->lSelectedLE->text()                    ) ;
    chosenSettings.setValue("rFile"                 , ui_->rSelectedLE->text()                    ) ;

}
//==================================================================
void MainWindow::initializeStringFinder(void)
{
    allPaths_          = ui_->pathSelectionLV   ;
    logViewer_         = ui_->pathInfoTB        ;
    model_             = new FileListModel(this);
    firstEditorCall_   = true                   ;
    editorTimer_       = new QTimer(this)       ;

    settingsValidator_ = new QRegExpValidator(QRegExp("[^\\s*|\\t*]*"), this);
    ui_->settingsLE->setValidator(settingsValidator_);

    this->readQSettings(QString(""));

    ui_       ->rootPathLE      ->insert        (rootPath_                          );
    ui_       ->matchedFilesTE  ->viewport      (                                   )->installEventFilter(this);
    ui_       ->matchedResultsTE->viewport      (                                   )->installEventFilter(this);
    ui_       ->lPanelTE        ->viewport      (                                   )->installEventFilter(this);
    ui_       ->rPanelTE        ->viewport      (                                   )->installEventFilter(this);
    this                        ->setRootPath   (ui_->rootPathLE->text()            );
    this                        ->updateListView(                                   );
    logViewer_                  ->setSizePolicy (QSizePolicy(QSizePolicy::Preferred,
                                                             QSizePolicy::Preferred));

    connect(ui_->rootPathLE, &QLineEdit    ::editingFinished,
            this           , &MainWindow   ::updateListView  );
    connect(ui_->rootPathLE, &QLineEdit    ::editingFinished,
            logViewer_     , &QTextEdit    ::clear           );
    connect(model_         , &FileListModel::numberPopulated,
            this           , &MainWindow   ::updateLog       );
    connect(allPaths_      , &QListView    ::doubleClicked  ,
            this           , &MainWindow   ::reactToClick    );
    connect(editorTimer_   , SIGNAL        (timeout    ())  ,
            this           , SLOT          (checkEditor())   );
}
//==================================================================
void MainWindow::checkEditor(void)
{
    char buf[4096];
    FILE *cmd_pipe = popen("pgrep -f '/Applications/nedit-5.5-Darwin-i386/nedit -server'", "r");

    fgets(buf, 4096, cmd_pipe);
    pid_t pid = strtoul(buf, NULL, 10);

    if( pid == 0 ) {firstEditorCall_ = true ;}

    pclose( cmd_pipe );
}

//==================================================================
QString MainWindow::selectSettings(QString id)
{
    ui_->settingsCB->clear() ;

    QSettings   allSettings(qsettingPath_+QString("allSettings.txt"), QSettings::NativeFormat);
    QFile       sFile      (allSettings.fileName()                                           );

    QString     settingsList = allSettings .value("settingsList" , ""                     ).toString();
    QString     lastSettings = allSettings .value("lastSettings" , "INFN"                 ).toString();
    QStringList theSettings  = settingsList.split(QRegExp("\\s+"), QString::SkipEmptyParts) ;

    if( !sFile.exists())
    {
        ui_->settingsCB->insertItem    (1,  QString("default"),QVariant(QString("default"))) ;
        ui_->settingsCB->setCurrentText(    QString("default")                             ) ;
    }

    if( id.contains(QRegularExpression("\\w+")))
    {
        lastSettings = id ;
    }
    if( theSettings.size() == 0 )
    {
        theSettings << "default" ;
        lastSettings = "default" ;
    }
    for(int i=0; i<theSettings.size(); ++i)
    {
        bool alreadyThere = false ;
        for(int j=0; j<ui_->settingsCB->count(); ++j)
        {
            if( ui_->settingsCB->itemData(j).toString() == theSettings.at(i))
            {
                alreadyThere = true ;
            }
        }
        if (!alreadyThere )
        {
            ui_->settingsCB->insertItem(i,theSettings.at(i),QVariant(theSettings.at(i))) ;
        }
    }

    ui_->settingsCB->setCurrentText(lastSettings);

    return qsettingPath_+ lastSettings + QString(".txt");
}
    //==================================================================
void MainWindow::readQSettings(QString id)
{
    char currentDirectory[4096];

    QString selectedSettingsFile = this->selectSettings(id) ;

    QSettings chosenSettings(selectedSettingsFile, QSettings::NativeFormat);

    QString   currentDir     = QString             (getcwd(currentDirectory, sizeof(currentDirectory)))           ;
              rootPath_      = chosenSettings.value("rootPath"             , currentDir               ).toString();
    QString   selectedPaths  = chosenSettings.value("selectedPaths"        , ""                       ).toString();
              selectedPaths_ = selectedPaths.split ("\n"                   , QString::SkipEmptyParts  )           ;

    if( HOME_.size() > 0                     ) {currentDir = HOME_      ;}
    if( !rootPath_.contains(QRegExp("\\w+")) ) {rootPath_  = currentDir ;}

    ui_->matchPathsTE                     ->clear() ;
    ui_->searchStringsTE                  ->clear() ;
    ui_->excludePathsTE                   ->clear() ;

    if( ui_->pathSelectionLV->selectionModel() != NULL )
    {
        ui_->pathSelectionLV->selectionModel()->clear() ;
        this->updateLog(0) ;
    }

    ui_->matchPathsTE            ->setText       (chosenSettings.value("matchPaths"            , ""     ).toString()) ;
    ui_->searchStringsTE         ->setText       (chosenSettings.value("searchStrings"         , "::"   ).toString()) ;
    ui_->excludePathsTE          ->setText       (chosenSettings.value("excludePaths"          , ""     ).toString()) ;
    ui_->suffixesTE              ->setText       (chosenSettings.value("suffixes"              , "cpp h").toString()) ;
    ui_->searchStringsLogicCB    ->setCurrentText(chosenSettings.value("searchStringsLogic"    , "or"   ).toString()) ;
    ui_->caseSensitiveStringCB   ->setChecked    (chosenSettings.value("caseSensitiveString"   , 0      ).toInt   ()) ;
    ui_->excludeStringsLogicCB   ->setCurrentText(chosenSettings.value("excludeStringsLogic"   , "or"   ).toString()) ;
    ui_->excludeStringsTE        ->setText       (chosenSettings.value("excludeStringsTE"      , "or"   ).toString()) ;
    ui_->caseSensitiveEStringCB  ->setChecked    (chosenSettings.value("caseSensitiveEString"  , 0      ).toInt   ()) ;
    ui_->matchPathsLogicCB       ->setCurrentText(chosenSettings.value("matchPathsLogic"       , "or"   ).toString()) ;
    ui_->caseSensitiveMatchCB    ->setChecked    (chosenSettings.value("caseSensitiveMatch"    , 0      ).toInt   ()) ;
    ui_->excludePathsLogicCB     ->setCurrentText(chosenSettings.value("excludePathsLogic"     , "or"   ).toString()) ;
    ui_->caseSensitiveExclusionCB->setChecked    (chosenSettings.value("caseSensitiveExclusion", 0      ).toInt   ()) ;
    ui_->lSelectedLE             ->setText       (chosenSettings.value("lFile"                 , ""     ).toString()) ;
    ui_->rSelectedLE             ->setText       (chosenSettings.value("rFile"                 , ""     ).toString()) ;

    lRootPath_ = chosenSettings.value("lFilePath", "/Users/menasce").toString() ;
    rRootPath_ = chosenSettings.value("rFilePath", "/Users/menasce").toString() ;

    if( ui_->lSelectedLE->text().isEmpty() || ui_->rSelectedLE->text().isEmpty() )
    {
        ui_->comparePB->setEnabled(false);
        ui_->editPB   ->setEnabled(false);
    }
    else
    {
        ui_->comparePB->setEnabled(true);
        ui_->editPB   ->setEnabled(true);
    }

    this->on_searchStringsTE_textChanged() ;
}
//===========================================================================
void MainWindow::startEditorProcess(QString file, int lineNumber)
{
    processMapDef::iterator pIt ;
    QStringList  arguments      ;
    stringstream ss             ;

    QString flag         = QString("second") ;
    if( firstEditorCall_ )
    {
        firstEditorCall_ = false ;
        flag             = QString("first" ) ;
    }
    QString       script = FINDERHOME_ + QString("/startNedit.pl");
    ss        << lineNumber                                ;
    arguments << flag << file << QString(ss.str().c_str()) ;
cout << __LINE__ << "] " << script.toStdString() << " " << arguments.join(QString(" ")).toStdString() << endl ;
    QString pID = file + QString("-") + QString(ss.str().c_str())        ;
    pIt         = startEditor_.find(pID)                                 ;
    if( pIt    == startEditor_.end() ) startEditor_[pID] = new QProcess();
    startEditor_[pID]->start(script,arguments);
    if( !editorTimer_->isActive())
    {
        editorTimer_->start(1000) ;
    }
    startEditor_[pID]->waitForStarted  (3000);
    startEditor_[pID]->waitForReadyRead(3000);

    QByteArray result = startEditor_[pID]->readAllStandardOutput();
    QByteArray error  = startEditor_[pID]->readAllStandardError ();

}

//===========================================================================
void MainWindow::editFile(void)
{
    QTextCursor textCursor = ui_->matchedResultsTE->textCursor() ;
    if(textCursor.isNull()) return ;
    int         tc         = textCursor.blockNumber() ;
    QTextBlock  textBlock  = ui_->matchedResultsTE->document()->findBlockByNumber(tc) ;
    if( textBlock.text() == "") return ;
    QStringList parts      = textBlock.text().split(QRegExp("^\\d+\\]"), QString::SkipEmptyParts);
    QString     candidate  = parts.at(0);
                candidate  = candidate.replace(QString(" "),QString("")) ;
    int         lineNumber = 0 ;
    QFile candidateFile(candidate) ;
    if( candidateFile.exists() )
    {
        lineNumber = 1 ;
    }
    else
    {
        QStringList ln  = textBlock.text().split(QRegExp("\\]"), QString::SkipEmptyParts);
        lineNumber      = ln.at(0).toInt() ;
        for(int pBlockN = tc; pBlockN > 0; --pBlockN)
        {
            textBlock      = ui_->matchedResultsTE->document()->findBlockByNumber(pBlockN-1) ;
            parts          = textBlock.text().split(QRegExp("^\\d+\\]"), QString::SkipEmptyParts);
            candidate      = parts.at(0);
            candidate      = candidate.replace(QString(" "),QString("")) ;
            QFile candidateFile(candidate) ;
            if( candidateFile.exists() ){break ;}
        }
    }
    this->startEditorProcess(candidate,lineNumber);
}

//===========================================================================
void MainWindow::mergeFiles(QTextEdit * fTE, QTextEdit * tTE)
{
    QTextCursor fCursor = fTE->textCursor();
    QTextCursor tCursor = tTE->textCursor();
    if(!fCursor.hasSelection())
    {
        cout << __LINE__ << "] No selection available" << endl ;
        return ;
    }

    int start = fCursor.selectionStart();
    int end   = fCursor.selectionEnd  ();

    fCursor.setPosition(start                       );
    int firstLine = fCursor.blockNumber()            ;
    fCursor.setPosition(end, QTextCursor::KeepAnchor);
    int lastLine  = fCursor.blockNumber()            ;

    QString                 fText = fTE->toPlainText()          ;
    QStringList             allF  = QString(fText).split('\n')  ;
    QString                 fline                               ;

    for (int n = 0; n<allF.size(); ++n)
    {
        if( n >= firstLine && n < lastLine )
        {
          fline = allF[n] ;
          fline = fline.replace(QString("<")                     , QString("&lt;"  ));
          fline = fline.replace(QString(">")                     , QString("&gt;"  ));
          fline = fline.replace(QRegularExpression("^\\s+\\d*] "), QString(""      ));
          fline = fline.replace(QString(" ")                     , QString("&nbsp;"));
          this->formatLine(fline, n+1)                                               ;
          QTextBlock fTextBlock = fTE->document()->findBlockByLineNumber(n)          ; // Restart always from top
          QTextBlock tTextBlock = tTE->document()->findBlockByLineNumber(n)          ; // Restart always from top

          fCursor.setPosition(fTextBlock.position())   ;
          fCursor.clearSelection()                     ;
          fCursor.select(QTextCursor::LineUnderCursor) ;
          fCursor.deleteChar()                         ;
          fCursor.insertHtml(fline)                    ;
          tCursor.setPosition(tTextBlock.position())   ;
          tCursor.clearSelection()                     ;
          tCursor.select(QTextCursor::LineUnderCursor) ;
          tCursor.deleteChar()                         ;
          tCursor.insertHtml(fline)                    ;
          QTextBlockFormat ff = fCursor.blockFormat()  ;
          QTextBlockFormat tf = tCursor.blockFormat()  ;
          ff.setBackground(QBrush(QColor(0,255,0,80))) ;
          tf.setBackground(QBrush(QColor(0,255,0,80))) ;
          fCursor.setBlockFormat(ff)                   ;
          tCursor.setBlockFormat(tf)                   ;
        }
    }
}

//===========================================================================
bool MainWindow::eventFilter(QObject *obj, QEvent * event)
{
  if      (event->type() == QEvent::Resize && obj == this)
  {

  }
  else if (obj           == ui_->matchedResultsTE->viewport() &&
            event->type() == QMouseEvent::MouseButtonDblClick  )
  {
      this->editFile();
  }
  else if ((obj           == ui_->lPanelTE->viewport()  ||
            obj           == ui_->rPanelTE->viewport()) &&
            event->type() == QMouseEvent::MouseButtonDblClick  )
  {

  }
  else if ( obj           == ui_->matchedFilesTE->viewport()   &&
            event->type() == QMouseEvent::MouseButtonDblClick  )
  {
      QTextCursor textCursor = ui_->matchedFilesTE->textCursor() ;
      int         tc         = textCursor.blockNumber() ;
      QTextBlock  textBlock  = ui_->matchedFilesTE->document()->findBlockByNumber(tc) ;

      QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

      QTextCharFormat fmt;
      fmt.setBackground(Qt::yellow);

      QTextCursor cursor = ui_->matchedFilesTE->cursorForPosition(QPoint(mouseEvent->x(),mouseEvent->y()));

      QList<QTextEdit::ExtraSelection> extraSelections;

      QTextEdit::ExtraSelection selection;

      selection.format.setBackground(QColor(Qt::gray).lighter(80)         );
      selection.format.setProperty  (QTextFormat::FullWidthSelection, true);
      selection.cursor = cursor;
      selection.cursor.clearSelection();
      extraSelections.append(selection);

      ui_->matchedFilesTE->setExtraSelections(extraSelections);

      QStringList sourceElements ;
      QStringList destinElements ;
                  sourceElements = ui_->matchedFilesTE  ->toPlainText().split('\n', QString::SkipEmptyParts);
                  destinElements = ui_->matchedResultsTE->toPlainText().split('\n', QString::SkipEmptyParts);
      QString     allText        = ui_->matchedFilesTE  ->toPlainText() ;

    // Find double-clicked line number (newLine) first
      int line = 1 ;
      for(int i=0; i<ui_->matchedFilesTE->toPlainText().size(); ++i)
      {
          if( allText.at(i).toLatin1() == '\n' ) {line++ ;}
          if( i > cursor.position()            ) {break  ;}
      }

      int newLine = 1 ;
      for(int i=0; i<destinElements.size(); ++i)
      {
          if( sourceElements.at(line-1) == destinElements.at(i)) {break;}
          newLine++ ;
      }

      textBlock  = ui_->matchedResultsTE->document()->findBlockByLineNumber(0); // Restart always from top
      textCursor = ui_->matchedResultsTE->textCursor ();
      textCursor.setPosition (textBlock.position());

      textBlock  = ui_->matchedResultsTE->document()->findBlockByLineNumber(newLine-1);
      textCursor = ui_->matchedResultsTE->textCursor ();
      textCursor.setPosition (textBlock.position());
      selection.cursor.clearSelection();
      selection.cursor = textCursor;
      extraSelections.append(selection);
      ui_->matchedResultsTE->setExtraSelections(extraSelections);
//      cout << __LINE__ << "] " << endl << endl ;
//      cout << __LINE__ << "] lineNumber       : " << newLine-1                       << endl ;
//      cout << __LINE__ << "] blockNumber      : " << textCursor.blockNumber()        << endl ;
//      cout << __LINE__ << "] position         : " << textCursor.position()           << endl ;
//      cout << __LINE__ << "] position in block: " << textCursor.positionInBlock()    << endl ;
//      cout << __LINE__ << "] selectionStart   : " << textCursor.selectionStart()     << endl ;
//      cout << __LINE__ << "] selectionEnd     : " << textCursor.selectionEnd()       << endl ;
//      cout << __LINE__ << "] height           : " << ui_->matchedResultsTE->height() << endl ;
      int nLinesBack = ui_->matchedResultsTE->height() / 17 - 2 ;
//      cout << __LINE__ << "] back             : " << nLinesBack                      << endl ;

      if(newLine>2) for(int i=0; i<nLinesBack; ++i ) textCursor.movePosition(QTextCursor::Down) ;
      ui_->matchedResultsTE->setTextCursor(textCursor);
//      cout << __LINE__ << "] " << endl ;
//      cout << __LINE__ << "] blockNumber      : " << textCursor.blockNumber()     << endl ;
//      cout << __LINE__ << "] position         : " << textCursor.position()        << endl ;
//      cout << __LINE__ << "] position in block: " << textCursor.positionInBlock() << endl ;
//      cout << __LINE__ << "] selectionStart   : " << textCursor.selectionStart()  << endl ;
//      cout << __LINE__ << "] selectionEnd     : " << textCursor.selectionEnd()    << endl ;
  }
  return QWidget::eventFilter(obj, event);
}

//===========================================================================
void MainWindow::updateLog(int number)
{
    logViewer_->clear() ;
    logViewer_->append(tr("Searching into %1 sub-directories.").arg(number-2));

    QStringList elements = model_->getList() ;
    for(int i=0; i<elements.size(); ++i)
    {
     for(int j=0; j<selectedPaths_.size(); ++j)
     {
         if( elements.at(i) == selectedPaths_.at(j) )
         {
             allPaths_->selectionModel()->select( model_->index(i), QItemSelectionModel::Select );
         }
     }
    }
    allPaths_->setFocus();
}
//===========================================================================
void MainWindow::updateListView(void)
{
 rootPath_ = ui_->rootPathLE->text() ;
 model_   ->setDirPath        (rootPath_) ;
 allPaths_->setModel          (model_   ) ;
 ui_      ->rootPathLE->clear (         ) ;
 ui_      ->rootPathLE->insert(rootPath_) ;
}

//===========================================================================
void MainWindow::reactToClick(const QModelIndex &index)
{
 if( index.data().toString() == ".." ||
     index.data().toString() == "."    )
 {
  QStringList list        ;
  QString     newPath = "";
  list = rootPath_.split('/', QString::SkipEmptyParts) ;
  for(int i=0; i<list.size()-1; ++i)
  {
   newPath += QString("/") + list.at(i) ;
  }
  newPath  += QString("/") ;
  rootPath_ = newPath ;
 }
 else
 {
  rootPath_ += index.data().toString() + QString("/");
 }

 ui_ ->rootPathLE->setText(rootPath_);

 this->updateListView() ;
}
//===========================================================================
QString MainWindow::getSelectedPaths()
{
    QString         selectedPaths = ""                    ;
    QModelIndexList selectedItems = ui_->pathSelectionLV->selectionModel()->selectedIndexes() ;

    foreach(QModelIndex index, selectedItems)
    {
        selectedPaths += index.data().toString()+ QString("\n");
    }
    return selectedPaths ;
}

//===========================================================================
void MainWindow::on_findPB_clicked()
{
    QString       selectedPaths  = this->getSelectedPaths() ;

    QString       script         = FINDERHOME_ + QString("/trova.pl")                      ;
    QString       root           = ui_->rootPathLE      ->text()                          ;
    QString       matchPaths     = ui_->matchPathsTE    ->toPlainText()                   ;
    QString       excludePaths   = ui_->excludePathsTE  ->toPlainText()                   ;
    QString       searchStrings  = ui_->searchStringsTE ->toPlainText()                   ;
    QString       excludeStrings = ui_->excludeStringsTE->toPlainText()                   ;
    QString       suffixesAll    = ui_->suffixesTE      ->toPlainText()                   ;
    QStringList   selectedP      = selectedPaths .split('\n', QString::SkipEmptyParts)    ;
    QStringList   matchP         = matchPaths    .split('\n', QString::SkipEmptyParts)    ;
    QStringList   excludedP      = excludePaths  .split('\n', QString::SkipEmptyParts)    ;
    QStringList   searchS        = searchStrings .split('\n', QString::SkipEmptyParts)    ;
    QStringList   excludedS      = excludeStrings.split('\n', QString::SkipEmptyParts)    ;
    QStringList   suffixes       = suffixesAll   .split('\n', QString::SkipEmptyParts)    ;
    QStringList   arguments                                                               ;
    QString       parts    = "\""                                                         ;
    QProcess    * process  = new QProcess(this)                                           ;

    QApplication::setOverrideCursor(Qt::WaitCursor) ;

    arguments << QString("--html") ;
    arguments << QString("--ps") ;

    if( selectedP.size() == 0 )
    {
        parts = root ;
    }
    else
    {
        for(int i=0; i< selectedP.size(); ++i)
        {
            parts += root + selectedP.at(i) ;
            if( i != selectedP.size()-1) parts += QString(" " ) ;
        }
        parts += "\"" ;
    }

    arguments << parts ;

    if( matchP.size() > 0 )
    {
        parts = "\"" ;
        arguments << QString("--mp") ;
        for(int i=0; i< matchP.size(); ++i)
        {
            parts += matchP.at(i) ;
            if( i != matchP.size()-1) parts += QString(" " ) ;
        }
        parts += "\"" ;
        arguments << parts ;
    }

    if( suffixes.size() > 0 )
    {
        parts = "\"" ;
        arguments << QString("--su") ;
        for(int i=0; i< suffixes.size(); ++i)
        {
            parts += suffixes.at(i) ;
            if( i != suffixes.size()-1) parts += QString(" " ) ;
        }
        parts += "\"" ;
        arguments << parts ;
    }

    arguments << "--ml" << ui_->matchPathsLogicCB           ->currentText() ;
    arguments << "--pl" << ui_->excludePathsLogicCB         ->currentText() ;
    arguments << "--sl" << ui_->searchStringsLogicCB        ->currentText() ;
    arguments << "--mh" << ui_->maxTotalFilesFoundLE        ->text()        ;
    arguments << "--mc" << ui_->maxTotalFilesConsideredLE   ->text()        ;
    arguments << "--mt" << ui_->maxTotalFileMatchesFoundLE  ->text()        ;
    arguments << "--mz" << ui_->maxTotalStringMatchesFoundLE->text()        ;
    arguments << "--el" << ui_->excludeStringsLogicCB       ->currentText() ;

    if( ui_->caseSensitiveMatchCB    ->isChecked()) {arguments << "--cm" << "true"  ;}
    else                                            {arguments << "--cm" << "false" ;}
    if( ui_->caseSensitiveExclusionCB->isChecked()) {arguments << "--em" << "true"  ;}
    else                                            {arguments << "--em" << "false" ;}
    if( ui_->caseSensitiveStringCB   ->isChecked()) {arguments << "--cs" << "true"  ;}
    else                                            {arguments << "--cs" << "false" ;}
    if( ui_->caseSensitiveEStringCB  ->isChecked()) {arguments << "--es" << "true"  ;}
    else                                            {arguments << "--es" << "false" ;}

    if( excludedP.size() > 0 )
    {
        parts = "\"" ;
        arguments << QString("--ep") ;
        for(int i=0; i< excludedP.size(); ++i)
        {
            parts += excludedP.at(i) ;
            if( i != excludedP.size()-1) parts += QString(" " ) ;
        }
        parts += "\"" ;
        arguments << parts ;
    }

    if( searchS.size() > 0 )
    {
        parts = "\"" ;
        arguments << QString("--ss") ;
        for(int i=0; i< searchS.size(); ++i)
        {
            parts += searchS.at(i) ;
            if( i != searchS.size()-1) parts += QString(" " ) ;
        }
        parts += "\"" ;
        arguments << parts ;
    }

    if( excludedS.size() > 0 )
    {
        parts = "\"" ;
        arguments << QString("--xs") ;
        for(int i=0; i< excludedS.size(); ++i)
        {
            parts += excludedS.at(i) ;
            if( i != excludedS.size()-1) parts += QString(" " ) ;
        }
        parts += "\"" ;
        arguments << parts ;
    }

    process->start(script,arguments) ;
    stringstream ss ;
    ss   << script.toStdString()
         << " "
         << arguments.join(" ").toStdString() ;
    QString cmd = QString(ss.str().c_str()) ;
    cmd.replace(QRegExp("--html"),"") ;
    ui_->commandLineTE->setText(cmd) ;

    cout << __LINE__ << "] " << ss.str() << endl ;

    process->waitForFinished() ;

    QByteArray result = process->readAllStandardOutput();
    QByteArray error  = process->readAllStandardError ();

    QString results = QString(result.data()) ;
    QString errors  = QString(error .data()) ;

    this->dispatchResults(results, errors) ;
}
//===========================================================================
void MainWindow::dispatchResults(QString & results, QString & errors)
{
    ui_->errorsTE->clear() ;
    ui_->errorsTE->setText(errors) ;

    QStringList             linesR = results.split("\n", QString::SkipEmptyParts) ;
    QRegularExpression      regexFile("\\[FileMatch\\]\\s+(.*)"              ) ;
    QRegularExpression      regexText("\\[stringMatch\\]\\s+(.*)"            ) ;
    QRegularExpression      regexTFF ("\\[TotalFilesFound\\]\\s+(.*)"        ) ;
    QRegularExpression      regexTFC ("\\[TotalFilesConsidered\\]\\s+(.*)"   ) ;
    QRegularExpression      regexTFMF("\\[TotalFileMatchesFound\\]\\s+(.*)"  ) ;
    QRegularExpression      regexTSMF("\\[TotalStringMatchesFound\\]\\s+(.*)") ;
    QRegularExpression      regexPerl("\\[perl\\]\\s+(.*)"                   ) ;
    QRegularExpression      regexFind("\\[finder\\]\\s+(.*)"                 ) ;
    QRegularExpressionMatch match ;

    ui_->matchedFilesTE  ->clear();
    ui_->matchedResultsTE->clear();

    for( int l=0; l<linesR.size(); ++l)
    {
//        cout << __LINE__ << "] " << linesR.at(l).toStdString() << endl ;
        match  = regexFind.match(linesR.at(l)); // Intercept this programs's messages
        if (match.hasMatch())
        {
          cout << __LINE__ << "] "
               << match.captured(1).toStdString() << " "
               << endl ;
        }

        match  = regexPerl.match(linesR.at(l)); // Intercept perl messages
        if (match.hasMatch())
        {
          cout << __LINE__ << "] "
               << match.captured(1).toStdString() << " "
               << endl ;
        }

        QString     msg ;
        QTextCursor fiCursor = ui_->matchedFilesTE  ->textCursor ();
        QTextCursor reCursor = ui_->matchedResultsTE->textCursor ();
                    match    = regexFile.match(linesR.at(l));
        if (match.hasMatch())
        {
            msg = match.captured(1) ;
            fiCursor.insertHtml(msg) ;
            fiCursor.insertBlock() ;
            reCursor.insertHtml(QString("<font color=\"yellow\" wheight=\"bold\">_________________________________________________________________________________________</font>")) ;
            reCursor.insertBlock() ;
            reCursor.insertHtml(QString("<pre>") + msg) ;
            reCursor.insertBlock() ;
        }
        match = regexText.match(linesR.at(l));
        if (match.hasMatch())
        {
            msg = match.captured(1) ;
            reCursor.insertHtml(QString("<pre>") + msg) ;
            reCursor.insertBlock()    ;
        }
        match = regexTFF .match(linesR.at(l));
        if (match.hasMatch()) {ui_->totalFilesFoundLE        ->setText(match.captured(1));}
        match = regexTFC .match(linesR.at(l));
        if (match.hasMatch()) {ui_->totalFilesConsideredLE   ->setText(match.captured(1));}
        match = regexTFMF.match(linesR.at(l));
        if (match.hasMatch()) {ui_->totalFileMatchesFoundLE  ->setText(match.captured(1));}
        match = regexTSMF.match(linesR.at(l));
        if (match.hasMatch()) {ui_->totalStringMatchesFoundLE->setText(match.captured(1));}

    }
    QApplication::restoreOverrideCursor() ;
}

//===========================================================================
void MainWindow::on_matchPathsCB_clicked()
{
    ui_->matchPathsTE->clear() ;
}
//===========================================================================
void MainWindow::on_searchStringsCB_clicked()
{
    ui_->searchStringsTE->clear() ;
}
//===========================================================================
void MainWindow::on_excludeStringsCB_clicked()
{
    ui_->excludeStringsTE->clear() ;
}
//===========================================================================
void MainWindow::on_excludePathsCB_clicked()
{
    ui_->excludePathsTE->clear() ;
}
//===========================================================================
void MainWindow::on_restoreCB_clicked()
{
    this->readQSettings(QString("")) ;
}
//===========================================================================
void MainWindow::on_clearPathSelectionPB_clicked()
{
    ui_->pathSelectionLV->selectionModel()->clear() ;
}
//===========================================================================
void MainWindow::on_searchStringsTE_textChanged()
{
    this->enableLogicCB(ui_->searchStringsLogicCB,ui_->searchStringsTE);
}
//===========================================================================
void MainWindow::on_matchPathsTE_textChanged()
{
    this->enableLogicCB(ui_->matchPathsLogicCB,ui_->matchPathsTE);
}
//===========================================================================
void MainWindow::on_excludePathsTE_textChanged()
{
    this->enableLogicCB(ui_->excludePathsLogicCB,ui_->excludePathsTE);
}
//===========================================================================
void MainWindow::on_excludeStringsTE_textChanged()
{
    this->enableLogicCB(ui_->excludeStringsLogicCB,ui_->excludeStringsTE);
}
//===========================================================================
void MainWindow::enableLogicCB(QComboBox * theCombo, QTextEdit * theTextEdit)
{
    QString content = theTextEdit->toPlainText() ;
    QStringList all = content.split("\n", QString::SkipEmptyParts) ;
    if( all.size() > 1)
    {
        theCombo->setEnabled    (true );
    }
    else
    {
        theCombo->setEnabled    (false);
        theCombo->setCurrentText("or" );
    }
}
//===========================================================================
void MainWindow::on_addSettingsPB_clicked()
{
    if( ui_->settingsLE->text() == "" ) return ;
    int lastOne = ui_->settingsCB->count() ;
    for(int i=0; i<lastOne; ++i)
    {
//        cout << __LINE__ << "] [finder] item: " << i << " " << ui_->settingsCB->itemData(i).toString().toStdString() << endl ;
        if( ui_->settingsCB->itemData(i).toString() == ui_->settingsLE->text() )
        {
            ui_->settingsLE->clear() ;
            return ;
        }
    }
    ui_->settingsCB->insertItem(lastOne,ui_->settingsLE->text(),QVariant(ui_->settingsLE->text())) ;
    ui_->settingsCB->setCurrentIndex(lastOne);
    ui_->settingsLE->clear() ;
}
//===========================================================================
void MainWindow::on_settingsLE_editingFinished()
{
    this->on_addSettingsPB_clicked() ;
}
//===========================================================================
void MainWindow::on_clearAllSettingsCB_clicked()
{
    this->on_matchPathsCB_clicked    () ;
    this->on_searchStringsCB_clicked () ;
    this->on_excludeStringsCB_clicked() ;
    this->on_excludePathsCB_clicked  () ;

    ui_->pathSelectionLV->selectionModel()->clear() ;
}
//===========================================================================
void MainWindow::on_restrictClearCB_clicked()
{
    ui_->suffixesTE->clear() ;
}
//===========================================================================
void MainWindow::on_saveSettingsPB_clicked()
{
    this->closeEvent(NULL) ;
}
//===========================================================================
void MainWindow::on_loadSettingsPB_clicked()
{
    this->readQSettings(ui_->settingsCB->currentText());
}

//===========================================================================
void MainWindow::on_synchronizeCB_clicked(bool checked)
{
    if( checked )
    {
        connect   (ui_->lPanelTE->verticalScrollBar()  , SIGNAL(valueChanged(int)),
                   ui_->rPanelTE->verticalScrollBar()  , SLOT  (setValue    (int)));
        connect   (ui_->lPanelTE->horizontalScrollBar(), SIGNAL(valueChanged(int)),
                   ui_->rPanelTE->horizontalScrollBar(), SLOT  (setValue    (int)));
        connect   (ui_->rPanelTE->verticalScrollBar()  , SIGNAL(valueChanged(int)),
                   ui_->lPanelTE->verticalScrollBar()  , SLOT  (setValue    (int)));
        connect   (ui_->rPanelTE->horizontalScrollBar(), SIGNAL(valueChanged(int)),
                   ui_->lPanelTE->horizontalScrollBar(), SLOT  (setValue    (int)));
    }
    else
    {
        disconnect(ui_->lPanelTE->verticalScrollBar()  , SIGNAL(valueChanged(int)),
                   ui_->rPanelTE->verticalScrollBar()  , SLOT  (setValue    (int)));
        disconnect(ui_->lPanelTE->horizontalScrollBar(), SIGNAL(valueChanged(int)),
                   ui_->rPanelTE->horizontalScrollBar(), SLOT  (setValue    (int)));
        disconnect(ui_->rPanelTE->verticalScrollBar()  , SIGNAL(valueChanged(int)),
                   ui_->lPanelTE->verticalScrollBar()  , SLOT  (setValue    (int)));
        disconnect(ui_->rPanelTE->horizontalScrollBar(), SIGNAL(valueChanged(int)),
                   ui_->lPanelTE->horizontalScrollBar(), SLOT  (setValue    (int)));
    }
}

//===========================================================================
void MainWindow::on_comparePB_clicked()
{
    this->compareFiles(ui_->lSelectedLE->text(), ui_->rSelectedLE->text());
}

//===========================================================================
void MainWindow::on_editPB_clicked()
{
    this->startEditorProcess(ui_->lSelectedLE->text(),1);
    sleep(1) ;
    this->startEditorProcess(ui_->rSelectedLE->text(),1);
}

//===========================================================================
void MainWindow::on_deleteSettingsPB_clicked()
{
    ui_->settingsCB->removeItem(ui_->settingsCB->currentIndex()) ;
}

//===========================================================================
void MainWindow::on_toRightCB_clicked()
{
    this->mergeFiles(ui_->lPanelTE, ui_->rPanelTE) ;
}

//===========================================================================
void MainWindow::on_toLeftCB_clicked()
{
    this->mergeFiles(ui_->rPanelTE, ui_->lPanelTE) ;
}

//===========================================================================
void MainWindow::on_lSavePB_clicked()
{
    this->saveToFile(ui_->lPanelTE,ui_->lSelectedLE) ;
}

//===========================================================================
void MainWindow::on_rSavePB_clicked()
{
    this->saveToFile(ui_->rPanelTE,ui_->rSelectedLE) ;
}
//===========================================================================
void MainWindow::saveToFile(QTextEdit * te, QLineEdit * le)
{
    QString text = te->toPlainText() ;
    text.replace(QRegularExpression("\n\\s+\\d+] "),QString("\n"))  ;
    text.replace(QRegularExpression("\\s+1] "     ),QString(""  ))  ;
    QString fileName = QFileDialog::getSaveFileName(this            ,
                                                    tr("Save File") ,
                                                    le->text(),
                                                    tr("ASCII (*)"));
    if(fileName.isEmpty()) return ;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        cout << __LINE__ << "] Could not open output file!... " << endl ;
        return;
    }

    QTextStream out(&file);
    text = text.replace(QRegularExpression("\\s+\n"),QString("\n")) ;
    out << text ;
    file.close() ;
    statusBar()->showMessage(tr("File saved"));
}

//===========================================================================
void MainWindow::on_louvrePB_clicked()
{
    QProcess    psProcess ;
    QStringList arguments ;
    arguments << " " << "-a" ;
    psProcess.start(QString("/bin/ps"));
    STDLINE(psProcess.program().toStdString() + arguments.join(QString("")).toStdString(),ACYellow) ;

    if (psProcess.waitForStarted(100000) == false)
    {
        STDLINE("Error starting ps process",ACRed) ;
        STDLINE(psProcess.errorString().toStdString(),ACRed) ;
        return;
    }

    psProcess.waitForFinished (100000);
    psProcess.waitForReadyRead(100000);

    QString                 result = psProcess.readAllStandardOutput() ;
    QRegularExpression      regexFind("\n\\s*(\\d+)(.+)?Louvre\n"    ) ;
    QRegularExpressionMatch match ;

    cout << __LINE__ << "] " << result.toStdString() << endl ;

    match  = regexFind.match(result);
    if (match.hasMatch())
    {
        STDLINE("Louvre is already running... ",ACCyan) ;
    }
    else
    {
        char * LOUVRE = getenv("LOUVRE") ;
        ss_.str("") ; ss_ << LOUVRE  ;
        if( finderProcess_ ) delete finderProcess_ ;
        finderProcess_ = new QProcess() ;
        finderProcess_->start(QString(ss_.str().c_str())) ;
    }
}
