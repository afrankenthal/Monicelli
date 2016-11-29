#include "mainwindow.h"
#include <helpers.h>
#include "ui_mainwindow.h"
#include <treemodel.h>

//==================================================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    ui = new Ui::MainWindow;

    ui->setupUi(this);

    fRootTimer_ = new QTimer();                         // ToROOT6
    QObject::connect(                                   // ToROOT6
                     fRootTimer_,                       // ToROOT6
                     SIGNAL(timeout()),                 // ToROOT6
                     this,                              // ToROOT6
                     SLOT(handle_root_events())         // ToROOT6
                    );                                  // ToROOT6
    fRootTimer_->start( 20 );                           // ToROOT6

    canvas_ = new QRootCanvas(ui->plotWG,"") ;
    c_ = canvas_->GetCanvas();

    splitterSizes_.append(ui->directoryTV->geometry().width());
    splitterSizes_.append(ui->plotWG->geometry().width());
    ui->splitter->setSizes(splitterSizes_);

    ui->directoryTV->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->directoryTV->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->directoryTV->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->plotWG->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->maxNumOfHistSB->setValue(12);
    ui->maxNumOfHistSB->setRange(1,144);
    ui->openFileLE->setReadOnly(true);

    connect(ui->splitter, SIGNAL(splitterMoved  (int, int)),
            this,         SLOT  (resizeChildrens(int, int))) ;

    foldersInFile_=false;
}

//==================================================================================
MainWindow::~MainWindow()
{
    STDLINE("Destructor",ACRed);
    delete ui;
}

//==================================================================================
void MainWindow::resizeChildrens(int, int)
{
    canvas_->resize(ui->splitter->sizes().at(1),ui->plotWG->geometry().height());
    canvas_->update();
}
//==================================================================================
void MainWindow::on_BrowsePB_clicked()
{
    ui->ExpandPB->setEnabled(true);
    ui->ContractPB->setEnabled(true);

    extract_ = new Extractor() ;

    QString fileName   = extract_->getFileName();
    ui->openFileLE->setText(fileName);
    QFileInfo fileInfo = (fileName);
    QString folderName = fileInfo.filePath();

    inputFile_ = new TFile(fileName.toStdString().c_str());

    if( !inputFile_->IsOpen() ) D("Cannot find file", fileName.toStdString()) ;

    TIter nextItem(inputFile_->GetListOfKeys());
    while(TKey * itemKey = (TKey*)nextItem())
    {
        if(itemKey->IsFolder())
        {
            foldersInFile_ = true;
            break;
        }
    }

    extract_->getFoldersInFile(foldersInFile_);

    extract_->searchFile(inputFile_,folderName.toStdString(),"");
    //extract_->printMap();

    QByteArray qArray = extract_->getByteArray() ;

    TreeModel * model = new TreeModel(qArray);

    ui->directoryTV->setModel(model);
    ui->directoryTV->setColumnWidth(0,300) ;

    connect(ui->directoryTV,
            SIGNAL(customContextMenuRequested(const QPoint &)),
            this,
            SLOT(onCustomContextMenu(const QPoint &)));
}

//==================================================================================
void MainWindow::searchReversedPath (const QModelIndex & index)
{
    if(index.parent().data().toString().toStdString().size()==0)
    {
        ssPath_ << index.data().toString().toStdString()<<endl;
        qStringPath_.append(ssPath_.str().c_str());
    }

    else
    {
        ssPath_ << index.data().toString().toStdString()<<"/";
        searchReversedPath(index.parent());
    }
}

//==================================================================================
void MainWindow::on_ExpandPB_clicked(void)
{
    ui->directoryTV->expandAll();
}

//==================================================================================
void MainWindow::on_ContractPB_clicked(void)
{
    ui->directoryTV->collapseAll();
}

//==================================================================================
void MainWindow::listPaths (void)
{
    QString t;

    pathList_=qStringPath_.split("/");

    for(int i=pathList_.size()-1; i>=1; --i)
    {
        t.append(pathList_.at(i)) ;
        if(i!=0) t.append('/');
    }

    t.remove("\n");

    pairDef_ iterpair = histoMap_.equal_range(t.toStdString());
    std::pair<string,string> temp;

    if(distance(iterpair.first,iterpair.second)==0)
    {
        histoMap_.insert(make_pair(t.toStdString(),pathList_.at(0).toStdString()));
    }

    else
    {
        for (hMapIterDef_ it = iterpair.first; it != iterpair.second; ++it)
        {
            if (it->second != pathList_.at(0).toStdString())
            {
                temp= make_pair(t.toStdString(),pathList_.at(0).toStdString());
                histoMap_.insert(temp);
                break;

            }
        }
    }

    qStringPath_.clear();
    ssPath_.str("");
}

//==================================================================================
void MainWindow::onCustomContextMenu(const QPoint &)
{
    histoMap_.clear();

    clickedList_ = ui->directoryTV->selectionModel()->selectedIndexes();

    for (int i = 0; i < clickedList_.size(); ++i)
    {
        selectAll(clickedList_.at(i));
    }

    drawAll();

    QTimer::singleShot(500, this, SLOT(clickedSelection()));
}

//==================================================================================
QString MainWindow::getSaveFileName(void)
{
    return QFileDialog::getSaveFileName(NULL, "Save File",
                                        "/user/gr1/e831/dzuolo",
                                        "ROOTFile (*.root)"
                                        );
}

//==================================================================================
void MainWindow::selectAll (const QModelIndex & index)
{
    int i = 0 - index.row();

    if(!index.child(index.row()+i,index.column()).isValid())
    {
        if(!ui->directoryTV->selectionModel()->isSelected(index) )
            ui->directoryTV->selectionModel()->select(index,QItemSelectionModel::Select);
        searchReversedPath(index);
        listPaths();
    }

    else
    {
        while(index.child(index.row()+i,index.column()).isValid())
        {
            QModelIndex thisNode = index.child(index.row()+i,index.column()) ;
              if(!ui->directoryTV->selectionModel()->isSelected(thisNode) )
                  ui->directoryTV->selectionModel()->select(thisNode,QItemSelectionModel::Select);
            selectAll(thisNode) ;
            i++;
        }
    }
}

//==================================================================================
void MainWindow::histoMapDumper(void)
{
    D("==================== histoMap_ dump ==================","");
    for (hMapIterDef_ it=histoMap_.begin(); it!=histoMap_.end(); ++it)
    {
        D3("Histogram :",it->first,it->second) ;
    }
}

//==================================================================================
void MainWindow::drawAll (void)
{
    string  folder           ;
    QString file             ;
    int     histoNumber =  1 ;
    int     maxPadNumber = ui->maxNumOfHistSB->value();

    c_->Clear();

    if(histoMap_.size()>(unsigned int)maxPadNumber)
    {
        createStatusBar();
//        if(maxPadNumber>12) c_->DivideSquare(maxPadNumber,0,0);
//        else c_->DivideSquare(maxPadNumber);
        if(maxPadNumber>12) c_->Divide(maxPadNumber/2,maxPadNumber/2,0,0);
        else c_->Divide(maxPadNumber/2,maxPadNumber/2);
        c_->Modified();
        c_->Update();
    }

    else
    {
//        if(maxPadNumber>12) c_->DivideSquare(histoMap_.size(),0,0);
//        else c_->DivideSquare(histoMap_.size());
        if(maxPadNumber>12) c_->Divide(histoMap_.size()/2,histoMap_.size()/2,0,0);
        else c_->Divide(histoMap_.size()/2,histoMap_.size()/2);
        c_->Modified();
        c_->Update();

    }

    for (hMapIterDef_ it=histoMap_.begin(); it!=histoMap_.end(); ++it)
    {
        if(histoNumber>maxPadNumber) return;
        folder = it->first;
        if(foldersInFile_) file.append(folder.c_str());
        file.append((it->second).c_str());
        file.remove('\n');
        TObject * h1 = inputFile_->Get(file.toStdString().c_str());
        if( !h1 )
        {
            D3("Could not access histogram:",folder,file.toStdString()) ;
            return ;
        }
        c_->cd(histoNumber);
        if(string(h1->ClassName()) == "TH2F") h1->Draw("COLZ");
        else h1->Draw();
        histoNumber++;
        file.clear();
    }

    c_->Modified();
    c_->Update();
}

//==================================================================================
void MainWindow::saveAll (void)
{
    QString outputFileName;
    outputFileName =getSaveFileName();
    if(!outputFileName.contains(".root")) outputFileName.append(".root");

    outputFile_ = new TFile(outputFileName.toStdString().c_str(),"RECREATE");

    if(outputFile_->IsZombie())
    {
        D("No output file selected!",endl);
        return;
    }

    string  folder;
    QString file;

    for (hMapIterDef_ it=histoMap_.begin(); it!=histoMap_.end(); ++it)
    {
        folder = it->first;
        if(foldersInFile_)file.append(folder.c_str());
        file.append((it->second).c_str());
        file.remove('\n');

        TH1F * h1 = (TH1F*)inputFile_->Get(file.toStdString().c_str());

        if(!h1)
        {
            D("Cannot access to histogram: ",file.toStdString());
            return;
        }

        if(ui->recreateTreeCB->isChecked())
        {
            outputFile_->mkdir(folder.c_str());
        }


        if(ui->recreateTreeCB->isChecked())
        {
            outputFile_->cd(folder.c_str());
            if(h1->Write()!=0) D3(" File: ",file.toStdString()," saved!");
        }
        else
        {            
            file.replace(QString("/"),QString("_"));
            if(h1->Write(file.toStdString().c_str())!=0) D3(" File: ",file.toStdString()," saved!");
        }

        file.clear();
    }
    outputFile_->Close();
}
//==================================================================================
void MainWindow::on_savePB_clicked(void)
{
    histoMap_.clear();

    clickedList_ = ui->directoryTV->selectionModel()->selectedIndexes();

    for (int i = 0; i < clickedList_.size(); ++i)
    {
        selectAll(clickedList_.at(i));
    }

    saveAll();

    QTimer::singleShot(500, this, SLOT(clickedSelection()));
}
//==================================================================================
void MainWindow::resizeEvent(QResizeEvent *event)
{
   QSize actualSize = event->size();
   QSize oldSize    = event->oldSize();

   if((std::abs(actualSize.width()-oldSize.width())>=30)||((std::abs(actualSize.height()-oldSize.height()))>=30))
   {
   static int splitterHeightDifference = std::abs(actualSize.height() - ui->splitter->geometry().height());
   static int splitterWidthDifference  = std::abs(actualSize.width()  - ui->splitter->geometry().width() );
   ui->splitter->resize(actualSize.width()-splitterWidthDifference,actualSize.height()-splitterHeightDifference);
   canvas_     ->resize(ui->plotWG->geometry().width(),ui->plotWG->geometry().height());
   canvas_     ->update();
   }
}
//==================================================================================
void MainWindow::clickedSelection(void)
{
   ui->directoryTV->selectionModel()->clearSelection();

   for (int i = 0; i < clickedList_.size(); ++i)
   {
       ui->directoryTV->selectionModel()->select(clickedList_.at(i),QItemSelectionModel::Select);
   }
}
//==================================================================================
void MainWindow::createStatusBar(void)
{
    statusBar()->showMessage(tr("Too Many histograms to display!"),2000);
}
//===========================================================================
void MainWindow::handle_root_events()         // ToROOT6
{                                             // ToROOT6
   //call the inner loop of ROOT              // ToROOT6
   gSystem->ProcessEvents();                  // ToROOT6
}
