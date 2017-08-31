#include "inc/mainwindow.h"
#include "ui_mainwindow.h"
//=========================================================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);

    targets_.push_back("source"     ) ;
    targets_.push_back("destination") ;

    model_       ["source"]             = new DFileSystemModel();
    model_       ["destination"]        = new DFileSystemModel();

    quadrant_    ["source"]             = make_pair(ui_->sourceTV     ,ui_->sourceLE          ) ;
    quadrant_    ["destination"]        = make_pair(ui_->destinationTV,ui_->destinationLE     ) ;

    diffTable_   ["source"]             = make_pair(ui_->sourceTW     ,ui_->sourceDiffLE      ) ;
    diffTable_   ["destination"]        = make_pair(ui_->destinationTW,ui_->destinationDiffLE ) ;

    diffPossible_["source"]             = false ;
    diffPossible_["destination"]        = false ;

    sortFlagsMap_["Name"              ] = QDir::Name      ;
    sortFlagsMap_["Size"              ] = QDir::Size      ;
    sortFlagsMap_["Time"              ] = QDir::Time      ;
    sortFlagsMap_["Decreasing"        ] = QDir::Reversed  ;
    sortFlagsMap_["Increasing"        ] = QDir::Unsorted  ;
    sortFlagsMap_["Directories first" ] = QDir::DirsFirst ;
    sortFlagsMap_["Files first"       ] = QDir::DirsLast  ;

    nameColumn_    = 0    ;
    sizeColumn_    = 1    ;
    timeColumn_    = 2    ;
    dirsColumn_    = 3    ;
    fileColumn_    = 4    ;
    moveColumn_    = 5    ;
    chekColumn_    = 6    ;
    numColumns_    = 7    ;

    scaleFactor_   = 3    ;
    batchProcess_  = NULL ;
    finderProcess_ = NULL ;
    firstPopulate_ = true ;

    char * LOUVREHOME = getenv("LOUVREHOME") ;

    imagesDir_             = QString(LOUVREHOME) + QString("/images/") ;
    lArrow_                = imagesDir_ + QString("leftArrow.png" ) ;
    rArrow_                = imagesDir_ + QString("rightArrow.png") ;
    checkedIcon_           = imagesDir_ + QString("checked.png"   ) ;
    arrows_["source"]      = rArrow_ ;
    arrows_["destination"] = lArrow_ ;

    QPixmap pix;
    QIcon cIcon;
    if( pix.load(checkedIcon_) && !pix.isNull())
    {
        cIcon.addPixmap(pix);
    }
    else
    {
        ss_.str(""); ss_ << "[WARNING] icon " << checkedIcon_.toStdString() << " not found" ;
        STDLINE(ss_.str(),ACRed) ;
    }

    for( auto & t: diffTable_)
    {
     t.second.first->horizontalHeader    (           )->resizeSection        ( nameColumn_, 345 );
     t.second.first->horizontalHeader    (           )->resizeSection        ( sizeColumn_,  85 );
     t.second.first->horizontalHeader    (           )->resizeSection        ( timeColumn_, 130 );
     t.second.first->horizontalHeader    (           )->resizeSection        ( dirsColumn_,  45 );
     t.second.first->horizontalHeader    (           )->resizeSection        ( fileColumn_,  55 );
     t.second.first->horizontalHeader    (           )->resizeSection        ( moveColumn_,  25 );
     t.second.first->horizontalHeader    (           )->resizeSection        ( chekColumn_,  20 );
     t.second.first->horizontalHeader    (           )->setSortIndicatorShown(true              );
     t.second.first->verticalHeader      (           )->setDefaultSectionSize(18                );
     t.second.first->horizontalHeaderItem(moveColumn_)->setIcon              (cIcon             );
    }

    connect(ui_->sourceTW,      SIGNAL(cellClicked      (int,  int          ))  ,
            this,               SLOT  (reactToClickS    (int,  int          ))) ;
    connect(ui_->destinationTW, SIGNAL(cellClicked      (int,  int          ))  ,
            this,               SLOT  (reactToClickD    (int,  int          ))) ;

    connect(ui_->sourceTV,      SIGNAL(activated        (const QModelIndex &))  ,
            this,               SLOT  (targetSFolder    (const QModelIndex &))) ;
    connect(ui_->destinationTV, SIGNAL(activated        (const QModelIndex &))  ,
            this,               SLOT  (targetDFolder    (const QModelIndex &))) ;

    readSettings    (                      ) ;

    populateTreeView(QString("source")     ) ;
    populateTreeView(QString("destination")) ;

    ui_->sourceTW->setColumnCount (7          );
    ui_->mainTW  ->setCurrentIndex(0          );
    statusBar()  ->showMessage    (tr("Ready"));

}
//=========================================================================================
MainWindow::~MainWindow()
{
    writeSettings() ;
    delete ui_;
}
//=========================================================================================
void MainWindow::on_chooseSourcePB_clicked()
{
    populateTreeView(QString("source")) ;
}
//=========================================================================================
void MainWindow::on_chooseDestinationPB_clicked()
{
    populateTreeView(QString("destination")) ;
}
//=========================================================================================
void MainWindow::populateTreeView(QString target)
{
    statusBar()->showMessage(tr("Begin populating view"));

    QString rootPath = "/" ;
//    QDir    sourceDir(rootPath) ;

    model_[target]->setRootPath(rootPath);
    model_[target]->setFilter(QDir::Files         |
                              QDir::Dirs          |
                              QDir::Hidden        |
                              QDir::NoSymLinks    |
                              QDir::NoDotAndDotDot);

    quadrant_[target].first->setAutoExpandDelay(0             ) ;
    quadrant_[target].first->setModel          (model_[target]) ;
    quadrant_[target].first->header            (              )
                           ->resizeSection     (0,295         ) ;
    QString tmpPath = "" ;
    if( firstPopulate_ )
    {
        QStringList list = lastExpandedFolder_[target].split(QString("/")) ;

        for(int i=0; i<list.size()-1; ++i)
        {
            tmpPath += list[i];
            if( i < list.size()-2) tmpPath += QString("/") ;
        }
        quadrant_[target].first->scrollTo       (model_[target]->index(tmpPath), QAbstractItemView::PositionAtCenter) ;
        quadrant_[target].first->setExpanded    (model_[target]->index(tmpPath), true                               ) ;
        quadrant_[target].first->setCurrentIndex(model_[target]->index(tmpPath)                                     ) ;
    }

    statusBar()->showMessage(tr("View populated"));
//    firstPopulate_ = false ;
}
//=========================================================================================
void MainWindow::targetSFolder(const QModelIndex & theTargetFolder)
{
    targetFolder(theTargetFolder,"source") ;
    currentSTarget_ = theTargetFolder ;
}
//=========================================================================================
void MainWindow::targetDFolder(const QModelIndex & theTargetFolder)
{
    targetFolder(theTargetFolder,"destination") ;
    currentDTarget_ = theTargetFolder ;
}
//=========================================================================================
void MainWindow::targetFolder(const QModelIndex & targetFolder,
                                    QString       target)
{
    if(targetFolder.column() > 0 )
    {
        STDLINE("[WARNING] To select a folder, click in the 'Name' column",ACWhite) ;
        STDLINE(targetFolder.row(),ACCyan) ;
        return ;
    }
    differencesAnalyzed_ = false ;
    ui_->analyzeDeepDiffsPB->setEnabled(false);
    ss_.str("") ; ss_ << "Begin populating " << target.toStdString() << " view" ;
    statusBar()->showMessage(tr(ss_.str().c_str()));

    QString fullPath = "/" ;

    findFullPath(targetFolder, fullPath) ;

    quadrant_ [target].second->setText(fullPath);
    diffTable_[target].second->setText(fullPath);
    diffTable_[target].first->clearContents()   ;

    diffPossible_[target] = true ;

    QDir dir(fullPath);
    QDir::SortFlags order = QDir::NoSort ;
    if( ui_->reverseOrderSCB->isChecked())
    {
        order = QDir::Reversed ;
    }
    QDir::SortFlags sf = sortFlagsMap_[ui_->sortSByCB ->currentText()] ;
    dir.setSorting(sf);
    dir.setFilter(QDir::Files         |
                  QDir::Dirs          |
                  QDir::Hidden        |
                  QDir::NoSymLinks    |
                  QDir::NoDotAndDotDot);

    int indx      = 0 ;
    int fileSizeT = 0 ;

    QFileInfoList list = dir.entryInfoList();

    diffTable_[target].first->setRowCount((int)list.size()); ;
    diffTable_[target].first->clearContents() ;

    int numTot   = 0 ;
    int totDirs  = 0 ;
    int totFiles = 0 ;

    for (int i = 0; i < list.size(); ++i)
    {
        int numDirs  = 0 ;
        int numFiles = 0 ;

        QFileInfo fileInfo = list.at(i);

        QString fullSub = fullPath + fileInfo.fileName() ;
        QDir    sub(fullSub) ;
        QFileInfoList subList = sub.entryInfoList();
        for(int i=0; i<subList.size(); ++i  )
        {
          QFileInfo tmp(subList[i]) ;
          if( tmp.isDir() ) numDirs++  ;
          else              numFiles++ ;
        }

        QTableWidgetItem * fileName = new QTableWidgetItem() ;
        QTableWidgetItem * fileSize = new QTableWidgetItem() ;
        QTableWidgetItem * fileTime = new QTableWidgetItem() ;
        QTableWidgetItem * dirsNums = new QTableWidgetItem() ;
        QTableWidgetItem * fileNums = new QTableWidgetItem() ;
        QTableWidgetItem * moveActi = new QTableWidgetItem() ;

        fileName->setTextAlignment(Qt::AlignLeft );
        fileSize->setTextAlignment(Qt::AlignRight);
        fileTime->setTextAlignment(Qt::AlignCenter );

        fileName->setText(fileInfo.fileName()) ;
        if( fileInfo.isDir())
        {
            QFont theFont = fileName->font() ;
            QBrush brush;
            theFont.setBold(true);

            fileName->setFont(theFont) ;
            brush.setColor(QColor::fromRgb(0,125,255));
            fileName->setForeground(brush) ;

            fileSize->setFont(theFont) ;
            fileSize->setForeground(brush) ;
            ss_.str("") ; ss_ << subList.size() - 2 ;
            fileSize->setText(QString(ss_.str().c_str())) ;

            ss_.str("") ; ss_ << numDirs        - 2 ;
            dirsNums->setText(QString(ss_.str().c_str())) ;

            ss_.str("") ; ss_ << numFiles ;
            fileNums->setText(QString(ss_.str().c_str())) ;
            numTot   += numDirs + numFiles      - 2 ;
            totDirs  += numDirs - 2 ;
            totFiles += numFiles ;
        }
        else
        {
            QRegExp rx("^\\.");
            if( fileInfo.fileName().contains(rx) )
            {
                QBrush brush = fileName->background() ;
                brush.setColor(QColor::fromRgb(255,0,0));
                fileName->setForeground(brush) ;
                stringstream ss ; ss << fileInfo.size() ;
                fileSize->setText(QString(ss.str().c_str())) ;
            }
            else
            {
                stringstream ss ; ss << fileInfo.size() ;
                fileSize->setText(QString(ss.str().c_str())) ;
                fileSizeT += atoi(fileSize->text().toStdString().c_str());
            }
        }

        QDateTime time = fileInfo.lastModified() ;
        fileTime->setText(time.toString(Qt::SystemLocaleShortDate)) ;

        QFont timeFont("Courier", 9, QFont::Bold);
        QFont sizeFont("Courier", 9, QFont::Bold);
        diffTable_[target].first->setItem(indx,nameColumn_, fileName);
        diffTable_[target].first->setItem(indx,sizeColumn_, fileSize);
        diffTable_[target].first->setItem(indx,timeColumn_, fileTime);
        diffTable_[target].first->setItem(indx,dirsColumn_, dirsNums);
        diffTable_[target].first->setItem(indx,fileColumn_, fileNums);
        diffTable_[target].first->setItem(indx,moveColumn_, moveActi);
        diffTable_[target].first->item   (indx,sizeColumn_)->setTextAlignment(Qt::AlignRight);
        diffTable_[target].first->item   (indx,dirsColumn_)->setTextAlignment(Qt::AlignRight);
        diffTable_[target].first->item   (indx,fileColumn_)->setTextAlignment(Qt::AlignRight);
//        diffTable_[target].first->item   (indx,sizeColumn_)->setFont(sizeFont);
        diffTable_[target].first->item   (indx,timeColumn_)->setFont(timeFont);

        for(int c=0; c<numColumns_-1; ++c)
         diffTable_[target].first->item(indx,c)->setFlags(Qt::ItemIsEnabled) ;

        indx++ ;
    }

    QTableWidgetItem * nameHeader = diffTable_[target].first->horizontalHeaderItem(nameColumn_) ;
    QTableWidgetItem * sizeHeader = diffTable_[target].first->horizontalHeaderItem(sizeColumn_) ;
    QTableWidgetItem * dirsHeader = diffTable_[target].first->horizontalHeaderItem(dirsColumn_) ;
    QTableWidgetItem * fileHeader = diffTable_[target].first->horizontalHeaderItem(fileColumn_) ;
    ss_.str("") ; ss_ << "Folder/file [Total: " << indx << "]" ;
    nameHeader->setText(QString(ss_.str().c_str()));
    ss_.str("") ; ss_ << "Items/size\n["  << numTot   << "," << fileSizeT/1000000 << "Mb]" ;
    sizeHeader->setText(QString(ss_.str().c_str()));
    ss_.str("") ; ss_ << "Dirs\n["        << totDirs  << "]" ;
    dirsHeader->setText(QString(ss_.str().c_str()));
    ss_.str("") ; ss_ << "Files\n["       << totFiles << "]" ;
    fileHeader->setText(QString(ss_.str().c_str()));

    ss_.str("") ; ss_ << "Done: " << target.toStdString() << " view has been populated" ;
    statusBar()->showMessage(tr(ss_.str().c_str()));

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
//=========================================================================================
void MainWindow::analizeDifferences(bool ignoreDiffs)
{
    if(ui_->sourceDiffLE     ->text() == QString("") ||
       ui_->destinationDiffLE->text() == QString("") )
    {
        QMessageBox msgBox;
        msgBox.setText("Cannot compare against an empty selection!");
        msgBox.exec();
       return ;
    }

    ui_->moveAllToSourcePB     ->setEnabled(true);
    ui_->moveAllToDestinationPB->setEnabled(true);
    ui_->selectAllS            ->setEnabled(true);
    ui_->selectAllD            ->setEnabled(true);
    ui_->analyzeDeepDiffsPB    ->setEnabled(true);

    if(ui_->sourceDiffLE->text() == ui_->destinationDiffLE->text())
    {
        QMessageBox msgBox;
        msgBox.setText("Source and destination are the same!");
        msgBox.exec();
        clearDestinationArea() ;
        return ;
    }

    QStringList listS = ui_->sourceDiffLE     ->text().split(QString("/")) ;
    QStringList listD = ui_->destinationDiffLE->text().split(QString("/")) ;

    QString s = listS[listS.size()-2] ;
    QString d = listD[listD.size()-2] ;
STDLINE(ignoreDiffs,ACRed) ;
    if( (s != d) && !ignoreDiffs )
    {
        QMessageBox msgBox;
        msgBox.setText("Warning, the source and destination folders have a different name.");
        msgBox.setInformativeText("Do you want to proceed for comparison anyway?");
        msgBox.setStandardButtons(QMessageBox::Yes |
                                  QMessageBox::No  |
                                  QMessageBox::Cancel);
        msgBox.setDefaultButton  (QMessageBox::No    );
        if( msgBox.exec() !=      QMessageBox::Yes   )
        {
//            clearDestinationArea() ;
            return ;
        }
    }

    statusBar()->showMessage(tr("Proceeding to comparison"));
    STDLINE("Proceeding to comparison",ACWhite) ;
    vector<string>    fileSNames ;
    vector<int>       fileSSizes ;
    vector<QString>   fileSTimes ;
    vector<string>    fileDNames ;
    vector<int>       fileDSizes ;
    vector<QString>   fileDTimes ;

    for(int row=0; row<ui_->sourceTW->rowCount(); ++row)
    {
        fileSNames.push_back(ui_->sourceTW     ->item(row,nameColumn_)->text().toStdString()) ;
        fileSSizes.push_back(ui_->sourceTW     ->item(row,sizeColumn_)->text().toInt()      ) ;
        fileSTimes.push_back(ui_->sourceTW     ->item(row,timeColumn_)->text()              ) ;
    }

    for(int row=0; row<ui_->destinationTW->rowCount(); ++row)
    {
        fileDNames.push_back(ui_->destinationTW->item(row,nameColumn_)->text().toStdString()) ;
        fileDSizes.push_back(ui_->destinationTW->item(row,sizeColumn_)->text().toInt()      ) ;
        fileDTimes.push_back(ui_->destinationTW->item(row,timeColumn_)->text()              ) ;
    }

    vector<string> unionSet(fileSNames.size()+fileDNames.size());
    vector<string>::iterator it;

    it=set_union(fileSNames.begin(),
                 fileSNames.end()  ,
                 fileDNames.begin(),
                 fileDNames.end()  ,
                 unionSet  .begin());

    unionSet.resize(it-unionSet.begin());

    ui_->sourceTW     ->clearContents() ;
    ui_->destinationTW->clearContents() ;
    ui_->sourceTW     ->setRowCount(unionSet.size());
    ui_->destinationTW->setRowCount(unionSet.size());

    int diffSFolders = 0 ;
    int diffDFolders = 0 ;
    int diffSizes    = 0 ;
    int totalSSizes  = 0 ;
    int totalDSizes  = 0 ;

    vector<string>::iterator itu;
    vector<string>::iterator its;
    vector<string>::iterator itd;
    int row = 0 ;

    for (itu=unionSet.begin(); itu!=unionSet.end(); ++itu)
    {
        QTableWidgetItem * newSFileItem = new QTableWidgetItem() ;
        QTableWidgetItem * newDFileItem = new QTableWidgetItem() ;
        QTableWidgetItem * newSSizeItem = new QTableWidgetItem() ;
        QTableWidgetItem * newDSizeItem = new QTableWidgetItem() ;
        QTableWidgetItem * newSTimeItem = new QTableWidgetItem() ;
        QTableWidgetItem * newDTimeItem = new QTableWidgetItem() ;
        its = ::find(fileSNames.begin(), fileSNames.end(), *itu) ;
        itd = ::find(fileDNames.begin(), fileDNames.end(), *itu) ;
        int indexS = its - fileSNames.begin() ;
        int indexD = itd - fileDNames.begin() ;

        if( its != fileSNames.end() && itd!= fileDNames.end()) // union name is present for both S and D
        {
            stringstream ss; ss << fileSSizes[indexS] ;
            stringstream sd; sd << fileDSizes[indexD] ;
            totalSSizes += fileSSizes[indexS] ;
            totalDSizes += fileDSizes[indexD] ;
            newSFileItem->setText(QString(fileSNames[indexS].c_str()  ));
            newSSizeItem->setText(QString(ss.str().c_str()            ));
            newSTimeItem->setText(QString(fileSTimes[indexS]          ));
            newDFileItem->setText(QString(fileDNames[indexD].c_str()  ));
            newDSizeItem->setText(QString(sd.str().c_str()            ));
            newDTimeItem->setText(QString(fileDTimes[indexD]          ));
            diffTable_["source"     ].first->setItem(row,nameColumn_, newSFileItem);
            diffTable_["source"     ].first->setItem(row,sizeColumn_, newSSizeItem);
            diffTable_["source"     ].first->setItem(row,timeColumn_, newSTimeItem);
            diffTable_["destination"].first->setItem(row,nameColumn_, newDFileItem);
            diffTable_["destination"].first->setItem(row,sizeColumn_, newDSizeItem);
            diffTable_["destination"].first->setItem(row,timeColumn_, newDTimeItem);
            diffTable_["source"     ].first->item   (row,sizeColumn_              )->setTextAlignment(Qt::AlignRight);
            diffTable_["destination"].first->item   (row,sizeColumn_              )->setTextAlignment(Qt::AlignRight);
            if( fileSSizes[indexS] != fileDSizes[indexD])
            {
                diffSizes++ ;
                ui_->sourceTW     ->item(row, sizeColumn_)->setBackgroundColor(QColor::fromRgb(255,0,0)) ;
                ui_->destinationTW->item(row, sizeColumn_)->setBackgroundColor(QColor::fromRgb(247,0,0)) ;
                QCheckBox * cbS = new QCheckBox() ;
                QCheckBox * cbD = new QCheckBox() ;
                cbS->setCheckState(Qt::Checked) ;
                cbD->setCheckState(Qt::Checked) ;
                ui_->sourceTW     ->setCellWidget(row,chekColumn_,cbS);
                ui_->destinationTW->setCellWidget(row,chekColumn_,cbD);
            }
        }

        if( its == fileSNames.end() && itd!= fileDNames.end()) // union name is present only in destination
        {
            stringstream sd; sd << fileDSizes[indexD] ;
            totalDSizes += fileDSizes[indexD] ;
            newSFileItem->setText(QString("----"                      ));
            newSSizeItem->setText(QString(""                          ));
            newSTimeItem->setText(QString(""                          ));
            newDFileItem->setText(QString(fileDNames[indexD].c_str()  ));
            newDSizeItem->setText(QString(sd.str().c_str()            ));
            newDTimeItem->setText(QString(fileDTimes[indexD]          ));
            diffTable_["source"     ].first->setItem(row,nameColumn_, newSFileItem);
            diffTable_["source"     ].first->setItem(row,sizeColumn_, newSSizeItem);
            diffTable_["source"     ].first->setItem(row,timeColumn_, newSTimeItem);
            diffTable_["destination"].first->setItem(row,nameColumn_, newDFileItem);
            diffTable_["destination"].first->setItem(row,sizeColumn_, newDSizeItem);
            diffTable_["destination"].first->setItem(row,timeColumn_, newDTimeItem);
            diffTable_["source"     ].first->item   (row,nameColumn_              )->setBackgroundColor(QColor::fromRgb(255,252,223)) ;
            diffTable_["source"     ].first->item   (row,sizeColumn_              )->setTextAlignment  (Qt::AlignRight              ) ;
            diffTable_["destination"].first->item   (row,nameColumn_              )->setBackgroundColor(QColor::fromRgb(247,223,255)) ;
            diffTable_["destination"].first->item   (row,sizeColumn_              )->setTextAlignment  (Qt::AlignRight              ) ;
            QCheckBox * cb = new QCheckBox() ;
            cb->setCheckState(Qt::Checked) ;
            ui_->destinationTW->setCellWidget(row,chekColumn_,cb);
            diffSFolders++ ;
            QPixmap pix ;
            QIcon   icon;
            if( pix.load(lArrow_) && !pix.isNull())
            {
             icon = QIcon(pix);
            }
            else
            {
                stringstream s ; s << "Could not load " << lArrow_.toStdString()  ;
                STDLINE(s.str(),ACRed) ;
            }
            QTableWidgetItem * iconItem = new QTableWidgetItem;
            iconItem->setIcon(icon);
            ui_->destinationTW->setItem(row,moveColumn_,iconItem);
            noMatchReason_ = "differentSizes" ;
        }

        if( its != fileSNames.end() && itd== fileDNames.end()) // union name is present only in source
        {
            stringstream ss; ss << fileSSizes[indexS] ;
            totalSSizes += fileSSizes[indexS] ;
            newSFileItem->setText(QString(fileSNames[indexS].c_str()  ));
            newSSizeItem->setText(QString(ss.str().c_str()            ));
            newSTimeItem->setText(QString(fileSTimes[indexS]          ));
            newDFileItem->setText(QString("----"                      ));
            newDSizeItem->setText(QString(""                          ));
            newDTimeItem->setText(QString(""                          ));
            diffTable_["source"     ].first->setItem(row,nameColumn_, newSFileItem);
            diffTable_["source"     ].first->setItem(row,sizeColumn_, newSSizeItem);
            diffTable_["source"     ].first->setItem(row,timeColumn_, newSTimeItem);
            diffTable_["destination"].first->setItem(row,nameColumn_, newDFileItem);
            diffTable_["destination"].first->setItem(row,sizeColumn_, newDSizeItem);
            diffTable_["destination"].first->setItem(row,timeColumn_, newDTimeItem);
            diffTable_["source"     ].first->item   (row,nameColumn_              )->setBackgroundColor(QColor::fromRgb(247,223,255)) ;
            diffTable_["source"     ].first->item   (row,sizeColumn_              )->setTextAlignment  (Qt::AlignRight              ) ;
            diffTable_["destination"].first->item   (row,nameColumn_              )->setBackgroundColor(QColor::fromRgb(255,252,223)) ;
            diffTable_["destination"].first->item   (row,sizeColumn_              )->setTextAlignment  (Qt::AlignRight              ) ;
            QCheckBox * cb = new QCheckBox() ;
            cb->setCheckState(Qt::Checked) ;
            ui_->sourceTW->setCellWidget(row,chekColumn_,cb);
            diffDFolders++ ;
            QPixmap pix ;
            QIcon   icon;
            if( pix.load(rArrow_) && !pix.isNull())
            {
             icon = QIcon(pix);
            }
            else
            {
                stringstream s ; s << "Could not load " << lArrow_.toStdString()  ;
                STDLINE(s.str(),ACRed) ;
            }
            QTableWidgetItem * iconItem = new QTableWidgetItem;
            iconItem->setIcon(icon);
            ui_->sourceTW->setItem(row,moveColumn_,iconItem);
            noMatchReason_ = "differentSizes" ;
        }
        QString tmpPathS = ui_->sourceLE     ->text() +
                           ui_->sourceTW     ->item(row,nameColumn_)->text() ;
        QString tmpPathD = ui_->destinationLE->text() +
                           ui_->destinationTW->item(row,nameColumn_)->text() ;
        QFileInfo fInfoS(tmpPathS) ;
        QFileInfo fInfoD(tmpPathD) ;
        if( fInfoS.isDir())
        {
            QBrush brush(QColor(0,120,255)) ;
            QFont theFontS = ui_->sourceLE     ->font() ;
            theFontS.setBold(true);
            ui_->sourceTW     ->item(row,nameColumn_)->setForeground(brush) ;
            ui_->sourceTW     ->item(row,nameColumn_)->setFont(theFontS) ;
        }
        if( fInfoD.isDir())
        {
            QBrush brush(QColor(0,120,255)) ;
            QFont theFontD = ui_->destinationLE->font() ;
            theFontD.setBold(true);
            ui_->destinationTW->item(row,nameColumn_)->setForeground(brush) ;
            ui_->destinationTW->item(row,nameColumn_)->setFont(theFontD) ;
        }
       row++ ;
    }

    stringstream ss ;

    QTableWidgetItem * fileSHeader = diffTable_["source"     ].first->horizontalHeaderItem(fileColumn_) ;
    ss.str("") ; ss << "Files\n[" << totalSSizes / 1000000 << "Mb]" ;
    fileSHeader->setText(QString(ss.str().c_str()));

    QTableWidgetItem * fileDHeader = diffTable_["destination"].first->horizontalHeaderItem(fileColumn_) ;
    ss.str("") ; ss << "Files\n[" << totalDSizes / 1000000 << "Mb]" ;
    fileDHeader->setText(QString(ss.str().c_str()));
    differencesAnalyzed_ = true ;
}
//=========================================================================================
void MainWindow::clearDestinationArea()
{
    ui_->destinationDiffLE->clear();
    ui_->destinationLE->clear();
    ui_->destinationTW->clearContents();
    populateTreeView(QString("destination")) ;
}

//=========================================================================================
void MainWindow::readSettings()
{
    QSettings theSettings("Louvre","Louvre") ;
    lastFolder_        ["source"]      = theSettings.value("Source folder"            ).toString() ;
    lastFolder_        ["destination"] = theSettings.value("Destination folder"       ).toString() ;
    lastExpandedFolder_["source"]      = theSettings.value("Last expanded source"     ).toString() ;
    lastExpandedFolder_["destination"] = theSettings.value("Last expanded destination").toString() ;

    ui_->sourceLE     ->setText(lastExpandedFolder_["source"]     ) ;
    ui_->destinationLE->setText(lastExpandedFolder_["destination"]) ;
}
//=========================================================================================
void MainWindow::writeSettings()
{
    QSettings theSettings("Louvre","Louvre") ;
    theSettings.setValue ("Source folder",             ui_->sourceLE     ->text()) ;
    theSettings.setValue ("Last expanded source",      ui_->sourceLE     ->text()) ;
    theSettings.setValue ("Destination folder",        ui_->destinationLE->text()) ;
    theSettings.setValue ("Last expanded destination", ui_->destinationLE->text()) ;
}
//=========================================================================================
void MainWindow::on_analyzeDiffsPB_clicked()
{
    analizeDifferences(false) ;
}
//=========================================================================================
void MainWindow::reactToClickS(int row, int column)
{
    reactToClick("source", row, column) ;
}
//=========================================================================================
void MainWindow::reactToClickD(int row, int column)
{
    reactToClick("destination", row, column) ;
}
//=========================================================================================
void MainWindow::reactToClick(QString target, int row, int column)
{
    stringstream ss ;
    if( column == nameColumn_ )
    {
        QCoreApplication::processEvents(); // Allow progress to be shown...
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QCoreApplication::processEvents(); // Allow progress to be shown...
        for(vector<QString>::iterator it=targets_.begin(); it!=targets_.end(); ++it)
        {
            STDLINE((*it).toStdString(),ACWhite);
            QTableWidgetItem * iS    = diffTable_[*it].first->item(row,nameColumn_) ;
            if(!iS)
            {
                ss_.str(""); ss_ << "Item not found at row " << row ; STDLINE(ss_.str(),ACWhite) ;
                continue ;
            }
            QString            item  = iS->text() ;
            ss_.str("") ; ss_ << item.toStdString() << " in " << (*it).toStdString() ;
            STDLINE(ss_.str(),ACGreen) ;
            QString            dir   = diffTable_["source"].second->text() ;
            QRegExp fileType("(\\.jpg|\\.JPG|\\.png|\\.gif|\\.CR2|\\.psd)");
            if( item.contains(fileType) && differencesAnalyzed_)
            {
                STDLINE("Proceed with a picture...",ACCyan) ;
                ui_->mainTW->setCurrentIndex(1) ;
                ui_->pictureInfoLeftTA ->setLineWrapMode(QTextEdit::NoWrap) ;
                ui_->pictureInfoRightTA->setLineWrapMode(QTextEdit::NoWrap) ;
                QString fullPath   = dir + item ;
                QPixmap thePicture = QPixmap(fullPath) ;
                ss_.str(""); ss_ << "Treating: " << fullPath.toStdString() << "...";
                STDLINE(ss_.str(),ACWhite);
                int w = thePicture.width ()*ui_->pictureScaleSB->value() ;
                int h = thePicture.height()*ui_->pictureScaleSB->value() ;
                if( w < 100 ) w = 100 ;
                if( h < 100 ) h = 100 ;
                thePicture = thePicture.scaled(w,
                                               h,
                                               Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation) ;
                if( !thePicture.isNull() )
                {
                    QString details = getPictureDetails(fullPath) ;
                    QTextDocument * detailsDoc = new QTextDocument() ;
                    detailsDoc->setMaximumBlockCount(10000);
                    detailsDoc->setHtml(details);
//                    cout << "Block counts: " << detailsDoc->maximumBlockCount() << endl ;
                    if( (*it) == "source")
                    {
                        ui_->leftImageLB ->setPixmap(thePicture);
                        ui_->pictureInfoLeftTA->setDocument(detailsDoc);
                    }
                    else
                    {
                        ui_->rightImageLB->setPixmap(thePicture);
                        ui_->pictureInfoRightTA->setDocument(detailsDoc);
                    }
                }
                else
                {
                    if( (*it) == "source")
                    {
                        STDLINE("WARNING: unkwnown format (S), the picture cannot be interpreted!",ACCyan) ;
                        ui_->pictureInfoLeftTA->setText("WARNING: unkwnown format, the picture cannot be interpreted!") ;
                    }
                    else
                    {
                        STDLINE("WARNING: unkwnown format (D), the picture cannot be interpreted!",ACCyan) ;
                        ui_->pictureInfoRightTA->setText("WARNING: unkwnown format, the picture cannot be interpreted!") ;
                    }
                }
//                 this->displayTargetFolders(row);
            }
            else
            {
                ss_.str(""); ss_ << "Could not display: " << item.toStdString() << " has an unrecognized picture format" ;
                statusBar()->showMessage(QString(ss_.str().c_str()));
                STDLINE(ss_.str(),ACRed) ;
            }
        }
        STDLINE("Next...", ACBlue) ;
    }

    QApplication::restoreOverrideCursor();

    if( column == dirsColumn_ || column == fileColumn_)
    {
        this->displayTargetFolders(row);
    }

    if( column == moveColumn_ )
    {
        QString     sTarget = "destination" ;
        QString     dTarget = "source"      ;
        QMessageBox msgBox(this)            ;
        QString     item                    ;
        if( target == "source" )
        {
            sTarget = "source"      ;
            dTarget = "destination" ;
        }
        item = QString("You selected:<p>")                             +
               diffTable_[target].second->text()                       +
               diffTable_[target].first->item(row,nameColumn_)->text() ;
        msgBox.setText(item)                                           ;
        QString action = ""                                            ;
        QString source, destination                                    ;
        source      = diffTable_[sTarget].second->text()               +
                      diffTable_[sTarget].first->item(row,0)->text()   ;
        destination = diffTable_[dTarget].second->text()               +
                      diffTable_[dTarget].first->item(row,0)->text()   ;
        action  = "Do you want to <b><u>copy</u></b> "                 ;
        action += "(<b><u>yes</u></b>) or <b><u>rename</u></b> "       ;
        action += "(<b><u>no</u></b>) this file?<p>"                   ;
        item = action                      +
               QString("\"")               +
               source                      +
               QString("\"<p>    to<p>\"") +
               destination                 +
               QString("\" ?")             ;
        msgBox.setInformativeText(item);
        msgBox.setStandardButtons(QMessageBox::Yes   |
                                  QMessageBox::No    |
                                  QMessageBox::Cancel) ;
        msgBox.setDefaultButton  (QMessageBox::Cancel) ;
        msgBox.setBaseSize(QSize(1600, 250));

        int answer = msgBox.exec() ;
        if(     answer ==  QMessageBox::Cancel)
        {
            STDLINE("Nothing done",ACBlack) ;
            return ;
        }
        else if(answer ==  QMessageBox::Yes )
        {
            manipulatePath     maniSource(source     )          ;
            manipulatePath     maniDestin(destination)          ;
            QString            sourPath = maniSource.getPath()  ;
            QString            destPath = maniDestin.getPath()  ;
            QTableWidgetItem * iS    = diffTable_["source"     ].first->item(row,nameColumn_) ;
            QTableWidgetItem * iD    = diffTable_["destination"].first->item(row,nameColumn_) ;
            QString            nameS = iS->text() ;
            QString            nameD = iD->text() ;
            if( target == "source")
            {
                source       = sourPath ;
                source      += nameS ;
                destination  = destPath ;
                destination += nameS ;
            }
            else
            {
                source       = sourPath ;
                source      += nameD ;
                destination  = destPath ;
                destination += nameD ;
             }
            cout << __LINE__ << "]\t/bin/cp " << source.toStdString() << " " << destination.toStdString() << endl ;
            this->copyFiles(source,destination);
        }
        else if(answer ==  QMessageBox::No )
        {
            int rowS, rowD ;
            if( target == "source")
            {
                rowS = row     ;
                rowD = row + 1 ;
            }
            else
            {
                rowS = row - 1 ;
                rowD = row     ;
            }
            manipulatePath maniDestin(destination)   ;
            QString destPath = maniDestin.getPath()  ;
            QTableWidgetItem * iS    = diffTable_["source"     ].first->item(rowS,nameColumn_) ;
            QTableWidgetItem * iD    = diffTable_["destination"].first->item(rowD,nameColumn_) ;
            QString            nameS = iS->text() ;
            QString            nameD = iD->text() ;
            if( target == "source")
            {
                source       = destPath ;
                source      += nameD ;
                destination  = destPath ;
                destination += nameS ;
            }
            else
            {
                source       = destPath ;
                source      += nameS ;
                destination  = destPath ;
                destination += nameD ;
             }
            this->renameFiles(source,destination);
        }
        targetFolder(currentSTarget_,"source"     ) ;
        targetFolder(currentDTarget_,"destination") ;
        STDLINE("Refresh...",ACGreen) ;
        analizeDifferences(true) ;
    }
}

//=========================================================================================
void MainWindow::displayTargetFolders(int row)
{
    QString     itemS  = diffTable_["source"     ].first->item(row,nameColumn_)->text() ;
    QString     itemD  = diffTable_["destination"].first->item(row,nameColumn_)->text() ;
    QString     dirS   = diffTable_["source"     ].second->text() ;
    QString     dirD   = diffTable_["destination"].second->text() ;
    QString     pathS  = dirS + itemS ;
    QString     pathD  = dirD + itemD ;
    QModelIndex indexS = model_    ["source"     ]->index(pathS) ;
    QModelIndex indexD = model_    ["destination"]->index(pathD) ;
    targetFolder(indexS,"source"     ) ;
    targetFolder(indexD,"destination") ;
    analizeDifferences(false) ;
}

//=========================================================================================
void MainWindow::on_moveAllToDestinationPB_clicked()
{
    on_moveAllPB_clicked("source") ;
}
//=========================================================================================
void MainWindow::on_moveAllToSourcePB_clicked()
{
    on_moveAllPB_clicked("destination") ;
}
//=========================================================================================
void MainWindow::on_moveAllPB_clicked(QString target)
{
    QMessageBox msgBox;
    msgBox.setText("You chose a bulky file copy.");
    msgBox.setInformativeText("Are you sure to proceed?");
    msgBox.setStandardButtons(QMessageBox::Yes |
                              QMessageBox::No  |
                              QMessageBox::Cancel);
    msgBox.setDefaultButton  (QMessageBox::No    );
    if( msgBox.exec() !=      QMessageBox::Yes   )
    {
        STDLINE("Nothing done",ACBlack) ;
        return ;
    }

    QString dTarget = "source" ;
    if( target == "source" ) dTarget = "destination" ;
    int rows = diffTable_[target].first->rowCount() ;
    for(int r=0; r<rows; ++r)
    {
        QCheckBox *cb = qobject_cast<QCheckBox *>(diffTable_[target].first->cellWidget(r, chekColumn_));
        if( !cb ) continue ;
        if( cb->checkState() == Qt::Checked)
        {
            QString cmd ;
            QString fromPath = diffTable_[ target].second->text()                     +
                               diffTable_[ target].first->item(r,nameColumn_)->text() ;
            QString toPath   = diffTable_[dTarget].second->text()                     +
                               QString(".")                                           ;
            cmd = QString("cp ") +
                  fromPath       +
                  QString(" "  ) +
                  toPath         ;
            QProcess    cpProcess ;
            QStringList arguments ;
            arguments << "-r" << fromPath << toPath ;
            cpProcess.start(QString("/bin/cp"), arguments);

            if (cpProcess.waitForStarted(100000) == false)
            {
                STDLINE("Error starting copy process",ACRed) ;
                STDLINE(cpProcess.errorString().toStdString(),ACRed) ;
                return;
            }

            cpProcess.waitForFinished (100000) ;
            cpProcess.waitForReadyRead(100000);

            QString result = cpProcess.readAllStandardOutput() ;
            QString error  = cpProcess.readAllStandardError () ;

            STDLINE(cmd   .toStdString(),ACBlue ) ;
            STDLINE(result.toStdString(),ACBlack) ;
            STDLINE(error .toStdString(),ACRed  ) ;

        }
    }
    QString     pathS  = ui_->sourceDiffLE     ->text (     ) ;
    QString     pathD  = ui_->destinationDiffLE->text (     ) ;
    QModelIndex indexS = model_["source"      ]->index(pathS) ;
    QModelIndex indexD = model_["destination" ]->index(pathD) ;
    targetFolder(indexS,"source"     ) ;
    targetFolder(indexD,"destination") ;
    analizeDifferences(false) ;
}
//=========================================================================================
void MainWindow::on_selectAllS_clicked()
{
    on_selectAll_clicked(ui_->sourceTW,ui_->selectAllS) ;
}
//=========================================================================================
void MainWindow::on_selectAllD_clicked()
{
    on_selectAll_clicked(ui_->destinationTW,ui_->selectAllD) ;
}
//=========================================================================================
void MainWindow::on_selectAll_clicked(QTableWidget * theTW, QRadioButton * theCB)
{
    int rows = theTW->rowCount() ;
    for(int r=0; r<rows; ++r)
    {
        QCheckBox *cb = qobject_cast<QCheckBox *>(theTW->cellWidget(r, chekColumn_));
        if( !cb ) continue ;
        if( theCB->isChecked() )
            cb->setChecked(true);
        else
            cb->setChecked(false);
    }
}
//=========================================================================================
void MainWindow::on_drillDownPB_clicked(QString target)
{
    QString dTarget = "source" ;
    if( target == "source" ) dTarget = "destination" ;
    int rows = diffTable_[target].first->rowCount() ;
    int totalSubDirs = 0 ;

    QFont timeFont("Courier", 9, QFont::Bold);
    QFont sizeFont("Courier", 9, QFont::Bold);

    map<QString, int> searches ;
    searches[QString("d")] = dirsColumn_ ; // Search for directories
    searches[QString("f")] = fileColumn_ ; // Search for regular files

    map<QString,int> offSet ;
    offSet[QString("d")] = 1 ;
    offSet[QString("f")] = 1 ;

    QString program = "/usr/bin/find";

    for(int r=0; r<rows; ++r)
    {
        QString fullPath = diffTable_[target].second->text()           +
                           diffTable_[target].first->item(r,nameColumn_)->text() ;
        if(fullPath.toStdString() == "----") {continue;}
        QStringList arguments;

        int dirsAndFiles = 0 ;
        QString n;

        for(map<QString,int>::iterator it =searches.begin();
                                       it!=searches.end() ;
                                      ++it)
        {
            arguments.clear() ;
            arguments << fullPath << "-type" << (*it).first << "-print";

            stringstream ss; ss << "Analyzing " << fullPath.toStdString() << "...";
            statusBar()->showMessage(tr(ss.str().c_str()));
            diffTable_[target].first->item(r, nameColumn_)->setBackgroundColor(QColor::fromRgb(255,255,0)) ;
            diffTable_["source"     ].first->scrollToItem(diffTable_["source"     ].first->item(r, nameColumn_),QAbstractItemView::PositionAtCenter) ;
            diffTable_["destination"].first->scrollToItem(diffTable_["destination"].first->item(r, nameColumn_),QAbstractItemView::PositionAtCenter) ;
            QCoreApplication::processEvents(); // Allow progress to be shown...
            if( ui_->abortComparisonCB->isChecked() ) {return ;}

            QProcess findProcess;

            findProcess.start(program, arguments);

            if (findProcess.waitForStarted(10000) == false)
            {
                STDLINE("Error starting process",ACRed) ;
                STDLINE(findProcess.errorString().toStdString(),ACRed) ;
                return;
            }

            findProcess.waitForFinished (10000) ;
            findProcess.waitForReadyRead(10000);

            QStringList fullPathElements = fullPath.split("/") ;
            QString theCurrentPath = fullPathElements[fullPathElements.size()-1] ;
            QString result = findProcess.readAllStandardOutput() ;
            QStringList e  = result.split("\n") ;
            QRegExp rx("\\.DS_Store");
            int indx     = 0 ;
            fileNames_.clear() ;
            for(int i=0; i<e.size(); ++i)
            {
                if( e.at(i).contains(rx) ) {continue;}
                QStringList singleName = e.at(i).split("/") ;
                fileNames_.push_back(singleName[singleName.size()-1]) ;
                indx++ ;
            }
            fileNamesForPath_[theCurrentPath] = fileNames_ ;

            int subDirs    = indx - offSet[(*it).first] ;

            dirsAndFiles  += subDirs ;

            if( r > 0 || r == rows)
            {
             diffTable_[target].first->item(r-1, nameColumn_)->setBackgroundColor(QColor::fromRgb(255,255,255)) ;
             QCoreApplication::processEvents(); // Allow progress to be shown...
            }

            if( subDirs <= 0 ) continue ;

            QTableWidgetItem * newItem  = new QTableWidgetItem() ;
            totalSubDirs += subDirs ;
            newItem->setText(n.setNum(subDirs)) ;
            diffTable_[target].first->setItem(r,(*it).second,newItem) ;
            diffTable_[target].first->item   (r,(*it).second)->setTextAlignment(Qt::AlignRight);
            diffTable_[target].first->item   (r,(*it).second)->setFont(sizeFont);
        }
        QTableWidgetItem * newItem  = new QTableWidgetItem() ;
        newItem->setText(n.setNum(dirsAndFiles)) ;
        diffTable_[target].first->setItem(r,sizeColumn_,newItem) ;
        diffTable_[target].first->item   (r,timeColumn_)->setTextAlignment(Qt::AlignRight);
        diffTable_[target].first->item   (r,timeColumn_)->setFont(timeFont);
        diffTable_[target].first->item   (r,sizeColumn_)->setTextAlignment(Qt::AlignRight);
        diffTable_[target].first->item   (r,sizeColumn_)->setFont(sizeFont);
    }
    diffTable_[target].first->item(rows-1, nameColumn_)->setBackgroundColor(QColor::fromRgb(255,255,255)) ;
    diffTable_[target].first->scrollToTop() ;
    diffTable_["source"     ].first->scrollToTop() ;
    diffTable_["destination"].first->scrollToTop() ;
    fileNamesForTarget_[target] = fileNamesForPath_ ;

}
//=========================================================================================
void MainWindow::checkDeepDifferences()
{
    int rowsS = diffTable_["source"     ].first->rowCount() ;
    int rowsD = diffTable_["destination"].first->rowCount() ;
    if( rowsS != rowsD)
    {
        QMessageBox msgBox;
        msgBox.setText("Warning: the number of folders in the right and left windows is different");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return ;
    }

    stringstream ss ;

    for(int row=0; row<rowsS; ++row)
    {
        QTableWidgetItem * dirsS  = diffTable_["source"     ].first->item(row,dirsColumn_) ;
        QTableWidgetItem * dirsD  = diffTable_["destination"].first->item(row,dirsColumn_) ;
        QTableWidgetItem * fileS  = diffTable_["source"     ].first->item(row,fileColumn_) ;
        QTableWidgetItem * fileD  = diffTable_["destination"].first->item(row,fileColumn_) ;
        QTableWidgetItem * fileNS = diffTable_["source"     ].first->item(row,nameColumn_) ;
        QTableWidgetItem * fileND = diffTable_["destination"].first->item(row,nameColumn_) ;

        if( fileNS->text().toStdString() != fileND->text().toStdString())
        {
            if( fileNS->text().toStdString() == "----")
            {
                ui_->sourceTW     ->item   (row, nameColumn_)->setBackgroundColor(QColor::fromRgb(255,252,223)) ;
                ui_->destinationTW->item   (row, nameColumn_)->setBackgroundColor(QColor::fromRgb(247,223,255)) ;
            }
            if( fileND->text().toStdString() == "----")
            {
                ui_->sourceTW     ->item   (row, nameColumn_)->setBackgroundColor(QColor::fromRgb(247,223,255)) ;
                ui_->destinationTW->item   (row, nameColumn_)->setBackgroundColor(QColor::fromRgb(255,252,223)) ;
            }
        }

//        fileNamesForTargetDef_::iterator fNFTS = fileNamesForTarget_.find("source"     ) ;
//        fileNamesForTargetDef_::iterator fNFTD = fileNamesForTarget_.find("destination") ;
//        fileNamesForPathDef_::iterator   fNFPS ;
//        fileNamesForPathDef_::iterator   fNFPD ;
//        if( fNFTS->second.size() != fNFTD->second.size() )
//        {
//            statusBar()->showMessage(tr("Number of subfolders do not match..."));
//        }
//        else
//        {
//            if( fNFTS->second.find(fileNS->text()) != fNFTS->second.end())
//            {
//                fNFPS = fNFTS->second.find(fileNS->text()) ;
//                fNFPD = fNFTD->second.find(fileND->text()) ;
//                for(int n=0; n<(int)fNFPS->second.size(); ++n)
//                {
//                    if( fNFPS->second[n].toStdString() != fNFPD->second[n].toStdString() )
//                    {
//                        diffTable_["source"     ].first->item(row, nameColumn_)->setBackgroundColor(QColor::fromRgb(255,0,255)) ;
//                        diffTable_["destination"].first->item(row, nameColumn_)->setBackgroundColor(QColor::fromRgb(255,0,255)) ;
//                    }
//                }
//            }
//            else
//            {
//                cout << __LINE__ << "]\tDies: no member " << fileNS->text().toStdString() << " found" << endl ;
//                continue ;
//            }
//        }

        vector<QString> namesD ;

        QTableWidgetItem * wi = NULL ;

        if(  !dirsS && !dirsD) continue ;
        if( (!dirsS &&  dirsD) || (dirsS && !dirsD))
        {
            wi = diffTable_["source"     ].first->item(row, dirsColumn_) ;
            if( wi ) wi->setBackgroundColor(QColor::fromRgb(255,0,0)) ;

            wi = diffTable_["destination"].first->item(row, dirsColumn_) ;
            if( wi ) wi->setBackgroundColor(QColor::fromRgb(255,0,0)) ;
            continue ;
        }
        if(  !fileS && !fileD) continue ;
        if( (!fileS &&  fileD) || (fileS && !fileD))
        {
            wi = diffTable_["source"     ].first->item(row, fileColumn_) ;
            if( wi == NULL) {continue ;}
            wi->setBackgroundColor(QColor::fromRgb(255,0,0)) ;

            wi = diffTable_["destination"].first->item(row, fileColumn_) ;
            if( wi == NULL) {continue ;}
            wi->setBackgroundColor(QColor::fromRgb(255,0,0)) ;
            continue ;
        }
        int numDirsS = dirsS->text().toInt() ;
        int numDirsD = dirsD->text().toInt() ;
        if( numDirsS != numDirsD)
        {
            diffTable_["source"     ].first->item(row, dirsColumn_)->setBackgroundColor(QColor::fromRgb(255,0,0)) ;
            diffTable_["destination"].first->item(row, dirsColumn_)->setBackgroundColor(QColor::fromRgb(255,0,0)) ;
            continue ;
        }
        int numFileS = fileS->text().toInt() ;
        int numFileD = fileD->text().toInt() ;
        if( numFileS != numFileD)
        {
            diffTable_["source"     ].first->item(row, fileColumn_)->setBackgroundColor(QColor::fromRgb(255,0,0)) ;
            diffTable_["destination"].first->item(row, fileColumn_)->setBackgroundColor(QColor::fromRgb(255,0,0)) ;
            continue ;
        }
    }
}
//=========================================================================================
void MainWindow::on_analyzeDeepDiffsPB_clicked()
{
    STDLINE("",ACBlack) ;
    statusBar()->showMessage(tr("Deep search of left folder in progress..."));
    QCoreApplication::processEvents();
    on_drillDownPB_clicked("source"     ) ;
    if( ui_->abortComparisonCB->isChecked() ) {return ;}

    STDLINE("",ACBlack) ;
    statusBar()->showMessage(tr("Deep search of right folder in progress..."));
    QCoreApplication::processEvents();
    on_drillDownPB_clicked("destination") ;
    if( ui_->abortComparisonCB->isChecked() )
    {
        ui_->abortComparisonCB->setChecked(false);
        return ;
    }

    STDLINE("",ACBlack) ;
    statusBar()->showMessage(tr("Analyzing differences..."));
    QCoreApplication::processEvents();
    checkDeepDifferences  (             ) ;

    statusBar()->showMessage(tr("Analysis completed."));
}
//=========================================================================================
void MainWindow::on_connectionCB_clicked(bool checked)
{
    if( checked )
    {
        connect   (ui_->sourceTW     ->verticalScrollBar(), SIGNAL(valueChanged(int)),
                   ui_->destinationTW->verticalScrollBar(), SLOT  (setValue    (int)));
        STDLINE("Synchronize"  ,ACGreen) ;
    }
    else
    {
        disconnect(ui_->sourceTW     ->verticalScrollBar(), SIGNAL(valueChanged(int)),
                   ui_->destinationTW->verticalScrollBar(), SLOT  (setValue    (int)));
        STDLINE("Unsynchronize",ACRed  ) ;
    }
}
//=========================================================================================
QString MainWindow::getPictureDetails(QString fullPath)
{
    QProcess    IMProcess ;
    QStringList arguments ;
    arguments << fullPath  ;
    IMProcess.start(QString("/Users/menasce/Programming/myQtProjects/Louvre/runIdentify"), arguments);

    if (IMProcess.waitForStarted(100000) == false)
    {
        STDLINE("Error starting ImageMagick process",ACRed) ;
        STDLINE(IMProcess.errorString().toStdString(),ACRed) ;
        return QString("") ;
    }

    IMProcess.waitForFinished (100000) ;
    IMProcess.waitForReadyRead(100000);

    QString     result  = IMProcess.readAllStandardOutput() ;
    QString     error   = IMProcess.readAllStandardError () ;
    QString     detail  = "" ;
    QStringList details = result.split("\n") ;

    stringstream ssTemp ;
    ssTemp << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n" ;
    ssTemp << "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n" ;
    ssTemp << "p, li { white-space: pre-wrap; }\n</style></head>\n" ;
    ssTemp << "<body style=\" font-family:'courier'; font-size:13pt; font-weight:400; font-style:normal;\">\n" ;
    ssTemp << "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">" ;

    for(int l=0; l<details.size(); ++l)
    {
        if( details[l].contains("Mime type"     ) ||
            details[l].contains("Geometry"      ) ||
            details[l].contains("Filesize"      ) ||
            details[l].contains("date"          ) ||
            details[l].contains("Quality"       ) ||
            details[l].contains("Gamma"         ) ||
            details[l].contains("Colorspace"    ) ||
            details[l].contains("Tainted"       ) ||
            details[l].contains("Number pixels" ) )
        {
            ssTemp << "<b><font color=\"#fff000000\">"
                   << details[l].toStdString()
                   << "</font></b><br>\n" ;
        }
        else
        {
            ssTemp << details[l].toStdString()
                   << "<br>\n" ;
        }
    }
    ssTemp << "<br /></p></body></html>" ;
//    cout << ssTemp.str() << endl ;
    return QString(ssTemp.str().c_str()) ;
}
//=========================================================================================
void MainWindow::renameFiles(QString source, QString destination)
{
    QString cmd ;
    cmd = QString("/bin/mv ") +
          source         +
          QString(" "  ) +
          destination    ;
    QProcess    cpProcess ;
    QStringList arguments ;
    arguments << source << destination ;
    cpProcess.start(QString("/bin/mv"), arguments);

    if (cpProcess.waitForStarted(100000) == false)
    {
        STDLINE("Error starting copy process",ACRed) ;
        STDLINE(cpProcess.errorString().toStdString(),ACRed) ;
        return;
    }

    cpProcess.waitForFinished (100000) ;
    cpProcess.waitForReadyRead(100000);

    QString result = cpProcess.readAllStandardOutput() ;
    QString error  = cpProcess.readAllStandardError () ;

    STDLINE(cmd   .toStdString(),ACBlue ) ;
    STDLINE(result.toStdString(),ACBlack) ;
    STDLINE(error .toStdString(),ACRed  ) ;
}

//=========================================================================================
void MainWindow::copyFiles(QString source, QString destination)
{
    ui_->progressBar->reset();
    if( !batchProcess_ ) delete batchProcess_ ;
    batchProcess_ = new QProcess() ;
    QStringList arguments ;

    arguments << "-vR" << source << destination ;
    batchProcess_->start(QString("/bin/cp"), arguments);

    connect(batchProcess_, SIGNAL(readyReadStandardOutput()),
            this         , SLOT  (processOutput          ()));

//    connect(batchProcess_, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
//                                                     [=](int, QProcess::ExitStatus)
//            {
//             this->getCounts() ;
//             delete batchProcess_ ;
//             batchProcess_ = new QProcess() ;
//             QStringList arguments ;
//             arguments << "-vR" << source << destination ;
//             connect(batchProcess_, SIGNAL(readyReadStandardOutput()),
//                         this,      SLOT  (processOutput          ()));  // connect process signals with your code
////             connect(batchProcess_, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
////                                                              [=](int, QProcess::ExitStatus)
////                     {
////                      this->processOutput() ;
////                     }
////                    );
//             batchProcess_->start(QString("/bin/cp"), arguments);
//            }
//           );
//    arguments << source << "-print";
//    batchProcess_->start(QString("/usr/bin/find"), arguments);
//    batchProcess_->waitForFinished() ;

//    ss_.str("") ; ss_ << "cp " << source.toStdString() << " " << destination.toStdString() ;
//    STDLINE(ss_.str(),ACRed) ;
////    statusBar()->showMessage(tr("Precomputing the number of files to transfer..."));
//    statusBar()->showMessage(QString(ss_.str().c_str()));
//    targetFolder(indexS,"source"     ) ;
//    targetFolder(indexD,"destination") ;
//    QString     pathS  = ui_->sourceDiffLE     ->text() ;
//    QString     pathD  = ui_->destinationDiffLE->text() ;
//    QModelIndex indexS = model_["source"     ]->index(pathS) ;
//    QModelIndex indexD = model_["destination"]->index(pathD) ;
//    analizeDifferences(false);
 }
//=========================================================================================
void MainWindow::on_startProcessPB_clicked()
{
//    ui_->progressBar->reset();
//    if( !batchProcess_ ) batchProcess_ = new QProcess() ;
//    QStringList arguments ;

//    connect(batchProcess_, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
//                                            [=](int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
//            {
//             STDLINE("",ACRed)
//             this->getCounts() ;
//             delete batchProcess_ ;
//             batchProcess_ = new QProcess() ;
//             QStringList arguments ;
//             arguments << "/Users/menasce/" << "-print" ;
//             connect    (batchProcess_, SIGNAL(readyReadStandardOutput()),
//                         this,          SLOT  (processOutput          ()));  // connect process signals with your code
//             batchProcess_->start(QString("/usr/bin/find"), arguments);
//             connect(batchProcess_, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
//                                                     [=](int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
//                     {
//                      this->processOutput() ;
//                     }
//                    );
//            }
//           );
//    arguments << "/Users/menasce" << "-print";
//    batchProcess_->start(QString("/usr/bin/find"), arguments);
}
//=========================================================================================
void MainWindow::getCounts()
{
    QString     files    = QString(batchProcess_->readAllStandardOutput().data()) ;
    QStringList fileList = files.split(QString("\n")) ;
                counts_  = fileList.size() ;
    STDLINE(files.toStdString(),ACBlue) ;
    ui_->processOutput->appendPlainText(files);  // read normal output
}

//=========================================================================================
void MainWindow::processOutput()
{
    int iteration = ui_->processOutput->document()->lineCount() ;
    double step = ((double)iteration / (double)counts_) * 100;
    ss_.str("") ; ss_ << ": cp " << QString(batchProcess_->readAllStandardOutput()).toStdString() ;
    STDLINE(ss_.str(),ACRed) ;
    ui_->progressBar->setValue(step);
    ui_->processOutput->appendPlainText(QString(ss_.str().c_str()));  // read normal output
    QCoreApplication::processEvents();
}
//=========================================================================================
void MainWindow::on_startProcessPB_2_clicked()
{
    batchProcess_->close() ;
    ui_->processOutput->appendPlainText(QString("Process aborted..."));  // read normal output
}

//=========================================================================================
void MainWindow::on_finderPB_clicked()
{
//   static bool alreadyRunning = false ;
//   if(!alreadyRunning)
//   {
//       alreadyRunning = true ;
//       char * MONICELLIDIR = getenv("MonicelliDir") ;
//       ss_.str("") ; ss_ << MONICELLIDIR << "/finder/finder &" ;
//       system(ss_.str().c_str()) ;
//   }
   STDLINE("",ACWhite) ;
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

   psProcess.waitForFinished (100000) ;
   psProcess.waitForReadyRead(100000);

   QString                 result = psProcess.readAllStandardOutput() ;
   QRegularExpression      regexFind("\n\\s*(\\d+)\\s+(.+)?finder"    ) ;
   QRegularExpressionMatch match ;

   match  = regexFind.match(result);
   if (match.hasMatch())
   {
     STDLINE("finder is already running... ",ACCyan) ;
   }
   else
   {
       char * MONICELLIDIR = getenv("MonicelliDir") ;
       ss_.str("") ; ss_ << MONICELLIDIR << "/finder/finder" ;
       if( finderProcess_ ) delete finderProcess_ ;
       finderProcess_ = new QProcess() ;
       finderProcess_->start(QString(ss_.str().c_str())) ;
   }
}
