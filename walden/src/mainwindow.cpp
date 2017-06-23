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

    gStyle -> SetPalette(1); //To ROOT 5

    langaus_ = new TF1("langaus",Utilities::langaus,0,60000,4);
    langaus_->SetParNames("Width","MPV","Area","GSigma");
    langaus_->SetLineColor(kBlue);
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
                                        "/raid2/data1/user/gr1/e831/dzuolo/",
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
        if(maxPadNumber>12) c_->DivideSquare(maxPadNumber,0,0); //TO ROOT 6
        else c_->DivideSquare(maxPadNumber);
//        if(maxPadNumber>12) c_->Divide(maxPadNumber/2,maxPadNumber/2,0,0);
//        else c_->Divide(maxPadNumber/2,maxPadNumber/2);
        c_->Modified();
        c_->Update();
    }

    else
    {
        if(maxPadNumber>12) c_->DivideSquare(histoMap_.size(),0,0); //TO ROOT 6
        else c_->DivideSquare(histoMap_.size());
//        if(maxPadNumber>12) c_->Divide(histoMap_.size()/2,histoMap_.size()/2,0,0);
//        else c_->Divide(histoMap_.size()/2,histoMap_.size()/2);
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
//===========================================================================
void MainWindow::on_saveAsImagePB_clicked()
{

    TCanvas *c = new TCanvas("c","c",700,700);
    TObject *obj;
    TIter nextPlot(c_->GetPad(1)->GetListOfPrimitives());

    if(ui->imageCB->currentText() == "Landau")
    {
        TH1F *plot;
        while ((obj=nextPlot()))
        {
            if (obj->InheritsFrom("TH1"))
            {
                plot = (TH1F*)obj;
            }
        }
        c->SetCanvasSize(700,900);
        c->Modified();
        c->Update();
        c->cd();
        plot->GetFunction("langaus")->SetLineColor(2);
        plot->Draw("e1");
    }

    if(ui->imageCB->currentText() == "cellEfficiency")
    {
        TH2F *plot;
        while ((obj=nextPlot()))
        {
            if (obj->InheritsFrom("TH1"))
            {
            plot = (TH2F*)obj;
            }
        }
        c->SetCanvasSize(900,600);
        c->Modified();
        c->Update();
        plot->GetXaxis()->SetTitleSize(0.04);
        plot->GetYaxis()->SetTitleSize(0.04);
        plot->Draw("COLZ");
    }

    if(ui->imageCB->currentText() == "other")
    {
        while ((obj=nextPlot()))
        {
            if (obj->InheritsFrom("TH1")||obj->InheritsFrom("TH2"))
            {
                c->cd();
                obj->Draw();
            }
        }
    }

    c->SaveAs(ui->imageNameLE->text().toStdString().c_str());

}
//===========================================================================
void MainWindow::on_fitPlotPB_clicked()
{
    TObject *obj;
    TIter nextPlot(c_->GetPad(1)->GetListOfPrimitives());

    gROOT->SetStyle("Plain");

    gStyle->SetOptStat(0);
    gStyle->SetStatY(0.9);
    gStyle->SetStatX(0.9);
    gStyle->SetStatW(0.2);
    gStyle->SetStatH(0.1);
    gStyle->SetOptTitle(0);
    gStyle->SetOptFit(1);



    if(ui->fitFunctionCB->currentText() == "Langaus")
    {
        TH1F *plot;
        while ((obj=nextPlot()))
        {
            if (obj->InheritsFrom("TH1"))
            {
            plot = (TH1F*)obj;
            }
        }

        c_->GetPad(1)->cd();
        plot->Draw("e1");
        langausFit(plot);

        TGaxis *myX = (TGaxis*)plot->GetXaxis();
        myX->SetMaxDigits(3);
        plot->GetXaxis()->SetRangeUser(0,20000);
        plot->SetYTitle("entries (#)");
        plot->GetYaxis()->SetTitleOffset(1.2);
        plot->GetYaxis()->SetTitleSize(0.04);

        c_->GetPad(1)->Modified();
        c_->GetPad(1)->Update();
    }

    if(ui->fitFunctionCB->currentText() == "Gaus")
    {
        TH1F *plot;
        while ((obj=nextPlot()))
        {
            if (obj->InheritsFrom("TH1"))
            {
            plot = (TH1F*)obj;
            }
        }

        plot->SetYTitle("entries (#)");
        plot->GetYaxis()->SetTitleOffset(1.2);
        plot->GetYaxis()->SetTitleSize(0.04);

        c_->GetPad(1)->cd();
        plot->Draw("e1");
        plot->Fit("gaus","QL","",ui->xMinLE->text().toInt(),ui->xMaxLE->text().toInt());
        c_->GetPad(1)->Modified();
        c_->GetPad(1)->Update();
    }


}
//===========================================================================
void MainWindow::langausFit(TH1* histo)
{

        double gausPar      [3];
        double landauPar    [3];
        double fitRange     [2];
        double startValues  [4];
        double parsLowLimit [4];
        double parsHighLimit[4];

        TF1* landau = new TF1("myLandau","landau",0,60000);
        TF1* gaus   = new TF1("myGaus"  ,"gaus"  ,0,60000);

        fitRange[0]=0.4*(histo->GetMean());
        fitRange[1]=1.8*(histo->GetMean());

        if(!ui->xMinLE->text().isEmpty()) fitRange[0]= ui->xMinLE->text().toInt();
        if(!ui->xMaxLE->text().isEmpty()) fitRange[1]= ui->xMaxLE->text().toInt();

        gaus->SetRange(fitRange[0],fitRange[1]);
        histo->Fit(gaus,"0QR");
        for(int p=0; p<3; p++)
            gausPar[p] = gaus->GetParameter(p);

        landau->SetRange(fitRange[0],fitRange[1]);
        histo->Fit(landau,"0QR");
        for(int p=0; p<3; p++)
            landauPar[p] = landau->GetParameter(p);



        startValues[0]=landauPar[2];
        startValues[1]=landauPar[1];
        startValues[2]=histo->Integral("width");
        startValues[3]=gausPar[2]  ;

        parsLowLimit [0] = startValues[0] - 0.68*startValues[0];
        parsHighLimit[0] = startValues[0] + 0.68*startValues[0];
        parsLowLimit [1] = startValues[1] - 0.68*startValues[1];
        parsHighLimit[1] = startValues[1] + 0.68*startValues[1];
        parsLowLimit [2] = startValues[2] - 0.68*startValues[2];
        parsHighLimit[2] = startValues[2] + 0.68*startValues[2];
        parsLowLimit [3] = startValues[3] - 0.68*startValues[3];
        parsHighLimit[3] = startValues[3] + 0.68*startValues[3];

        langaus_->SetRange(fitRange[0],fitRange[1]);
        langaus_->SetParameters(startValues);

        for (int p=0; p<4; p++) {
            langaus_->SetParLimits(p, parsLowLimit[p], parsHighLimit[p]);
        }

        histo->Fit(langaus_,"QRB");
        langaus_->SetLineColor(2);
        langaus_->Draw("same");

}
