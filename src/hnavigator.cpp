/*===============================================================================
 * Monicelli: the FERMILAB MTEST geometry builder and track reconstruction tool
 * 
 * Copyright (C) 2014 
 *
 * Authors:
 *
 * Dario Menasce      (INFN) 
 * Luigi Moroni       (INFN)
 * Jennifer Ngadiuba  (INFN)
 * Stefano Terzo      (INFN)
 * Lorenzo Uplegger   (FNAL)
 * Luigi Vigani       (INFN)
 *
 * INFN: Piazza della Scienza 3, Edificio U2, Milano, Italy 20126
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ================================================================================*/
 
#include "hnavigator.h"
#include "ui_hnavigator.h"

#include "MessageTools.h"

#include <TKey.h>
#include <TIterator.h>

#include <QtWidgets/QFileDialog>
#include <QtCore/QList>
#include <QSettings>
#include <QPoint>
#include <QSize>

//===========================================================================
HNavigator::HNavigator(QWidget * parent) :
    QTabWidget(parent),
    ui(new Ui::HNavigator)
{
  ui->setupUi(this);

  theHManager_        = NULL ;
  theTabWidget_       = NULL ;
  theFileEater_       = NULL ;
  theHTreeBrowser_    = NULL ;

  emptyFileLabel_     = "No files are currently open"     ;
  displayAllLabel_    = "Display all"                     ;
  displayMemoryLabel_ = "Display memory resident objects" ;

  counter_         = 0 ;

  QSettings settings("CMS", "Monicelli");
  QPoint pos  = settings.value("posHnavigator" , QPoint(10 , 10 )).toPoint();
  QSize  size = settings.value("sizeHnavigator", QSize (400, 400)).toSize() ;
  ss_.str(""); ss_ << "Geometry: "
                   << pos.x()
                   << " + "
                   << pos.y()
                   << "  "
                   << size.width()
                   << " x "
                   << size.height() ;
  STDLINE(ss_.str(),"") ;
//  this->move  (pos ) ;
//  this->resize(size) ;

//  this->move  (pos ) ;
//  this->resize(size) ;

  this->addItem(emptyFileLabel_.toStdString()) ;

  this->collectExistingWidgets(parent);

  connect(ui->hNavigatorInputFileCB,
          SIGNAL(currentIndexChanged(QString)),
          this,
          SLOT(  updateTree(         QString))) ;

  timer_ = new QTimer(this) ;

  connect(timer_,
          SIGNAL(timeout                () ),
          this,
          SLOT  (checkNewObjectsInMemory()));

  timer_->start(1000) ;

  for(int i=0; i<10; ++i)
  {
    ss_.str(""); ss_ << i ;
    ui->hCanvasCB->addItem(QString(ss_.str().c_str()));
  }

  ui->canvasSizeCB->addItem(QString("Small" ));
  ui->canvasSizeCB->addItem(QString("Medium"));
  ui->canvasSizeCB->addItem(QString("Large" ));
  ui->canvasSizeCB->addItem(QString("Huge"  ));
  ui->canvasSizeCB->setCurrentIndex(2);

  usePartitions_ = false;
}

//===========================================================================
HNavigator::~HNavigator()
{
    ss_.str("") ;
    ss_ << "Geometry: "
        << this->geometry().x()
        << " + "
        << this->geometry().y()
        << "  "
        << this->geometry().width()
        << " x "
        << this->geometry().height() ;
    STDLINE(ss_.str(),ACCyan) ;
    QSettings settings("CMS", "Monicelli");
    settings.setValue("sizeHnavigator" , QSize (
                                                this->geometry().width(),
                                                this->geometry().height()
                                               )) ;
    settings.setValue("posHnavigator"  , QPoint(
                                                this->pos().x(),
                                                this->pos().y()
                                               )) ;
    delete ui;
    delete timer_ ;
}

//===========================================================================
void HNavigator::checkNewObjectsInMemory(void)
{    
    theHTreeBrowser_->populate(gROOT->GetRootFolder()) ;    
}

//===========================================================================
void HNavigator::updateTree(QString currentFile)
{
  //theHTreeBrowser_->clear();
  if( currentFile != displayAllLabel_ )
  {
    this->fillWidgetTree(currentFile.toStdString());
  }
  else
  {
    for(int item=0; item < ui->hNavigatorInputFileCB->count(); ++item)
    {
      this->fillWidgetTree(ui->hNavigatorInputFileCB->itemText(item).toStdString());
    }
  }
}

//===========================================================================
void HNavigator::collectExistingWidgets(QWidget * parent)
{
    STDLINE("Collecting information about existing open widgets",ACYellow) ;
    if( ! parent )
    {
        STDLINE("No parent widget found (yet)",ACCyan) ;
        return ;
    }

    parent_ = (MainWindow*)parent ;
    //  STDLINE("",ACWhite) ;
    if( !theHTreeBrowser_ )
    {
        //      STDLINE("",ACWhite) ;
        // Create a tree-like folder-navigation tool
        //    this->setGeometry(this->x(),this->y(),this->width(),this->height()+120) ;
        this->show() ;
        this->setCurrentIndex(0);
        QWidget * input  = this->widget(0) ;
        theHTreeBrowser_ = new hTreeBrowser(input, parent_) ;
        theHTreeBrowser_->setGeometry(ui->hNavigatorTreeFrame->geometry()) ;
        theHTreeBrowser_->show() ;
    }
    //  STDLINE("",ACWhite) ;

    // Recover pointers to essential objects
    theHManager_  = parent_->getHManager()  ;
    theTabWidget_ = parent_->getTabWidget() ;

    // Populate the folders navigation tool with the existing open files
    // and their content and point to the current one
    if( !theTabWidget_ ) // No mainTabs widget has been opened yet.
    {
        return ;
    }
    else
    {
        theFileEater_ = theTabWidget_->getFileEater() ;
        if( !theFileEater_ ) return ;
    }
    //  STDLINE("",ACWhite) ;

    this->fillWidget()     ; // Fill the combo-box with the list of open files
    STDLINE("",ACWhite) ;
    this->fillWidgetTree() ; // Populate the tree widget with file structure content
    STDLINE("",ACWhite) ;
}

//===========================================================================
QMdiArea * HNavigator::getMdiArea(void)
{
  return parent_->getMdiArea();
}

//===========================================================================
MainWindow * HNavigator::getMainWindow(void)
{
  return parent_;
}

//===========================================================================
// Fill the combo-box with the list of currently open root files
void HNavigator::fillWidget()
{
  openFiles_ = theFileEater_->getOpenFiles() ;

  for(fileEater::fileMapDef::iterator it=openFiles_.begin(); it!=openFiles_.end(); ++it)
  {
      if(it->first.find(".root")!=string::npos) this->addItem(it->first) ;
  }
}

//===========================================================================
// Fill the tree
void HNavigator::fillWidgetTree()
{
  this->fillWidgetTree(ui->hNavigatorInputFileCB->currentText().toStdString());
}

//===========================================================================
// Fill the tree
void HNavigator::fillWidgetTree(std::string currentFile)
{
  if( currentFile == emptyFileLabel_.toStdString() ) return ;

  TFile * file = NULL ;

  STDLINE(std::string("Exploring ")+currentFile,ACWhite) ;

  fileEater::fileMapDef::iterator filesIt=openFiles_.find(currentFile) ;

  if( filesIt == openFiles_.end() )
  {
    STDLINE(std::string("Could not find ")+currentFile,ACRed) ;
    return ;
  }
  else
  {
    file = filesIt->second ;
    if( file->IsOpen() )
    {
      STDLINE("The file is actually open",ACGreen) ;
    }
    else
    {
      STDLINE("The file is NOT currently open: opening",ACRed) ;
    }
  }

  theHTreeBrowser_->populate((TDirectory *)file) ;
}

//===========================================================================
void HNavigator::addNewFile(const QString &)
{
  STDLINE("Added new file to the list in the tree-navigator",ACCyan) ;

  this->refresh() ;
  this->collectExistingWidgets(parent_);
}

//===========================================================================
void HNavigator::refresh()
{
  STDLINE("Refreshing the content of the file tree-structure navigator",ACCyan) ;

  // Populate the folders navigation tool with the existing open files
  // and their content and point to the current one
  if( !theTabWidget_ )
  {
    return ;
  }
  else
  {
    theFileEater_ = theTabWidget_->getFileEater() ;
  }
}

//===========================================================================
void HNavigator::addItem(std::string item)
{
  bool alreadyIncluded = false ;

  for(int i=0; i<ui->hNavigatorInputFileCB->count(); ++i)
  {
    if( ui->hNavigatorInputFileCB->itemText(i).toStdString() ==  emptyFileLabel_.toStdString()         ) ui->hNavigatorInputFileCB->setItemText(i,displayAllLabel_) ;
    if( ui->hNavigatorInputFileCB->itemText(i).toStdString() == displayAllLabel_.toStdString() && i > 0) ui->hNavigatorInputFileCB->removeItem(i) ;
    if( ui->hNavigatorInputFileCB->itemText(i).toStdString() == item                                   ) alreadyIncluded = true ;
  }

  if( !alreadyIncluded )
  {
    ui->hNavigatorInputFileCB->addItem(QString::fromStdString(item)) ;
  }

  ui->hNavigatorInputFileCB->setCurrentIndex(ui->hNavigatorInputFileCB->count()-1);
}

//===========================================================================
void HNavigator::resizeEvent(QResizeEvent * )
{
  /*   +-O-------------------------------+
       |                                 |
       | A-----------------------------a |
       | |            comb             | |
       | B-----------------------------b |
       |                                 |
       | C-----------------------------c |
       | |                             | |
       | |            fram             | |
       | |                             | |
       | |                             | |
       | D-----------------------------d |
       |                                 |
       | E-----------------------------e |
       | |            grbx             | |
       | F-----------------------------f |
       |                                 |
       +---------------------------------+

       A: topComb
       B: botComb
       C: topFram
       D: botFram
       E: topGrbx
       F: botGrbx

       AB: combRect.height()
       CD: framRect.height()
       EF: grbxRect.height()

       OA: topComb
       BC: topFram - botComb
       DE: topGrbx - botFram (dYGrbx)

   */
  int topFram = 0;
  int topGrbx = 0;
  int botFram = 0;

  QRect thisRect = this->geometry() ;

  QRect combRect = ui->hNavigatorInputFileCB->geometry() ;
  QRect framRect = ui->hNavigatorTreeFrame  ->geometry() ;
  QRect grbxRect = ui->hNavigatorCanvasGB   ->geometry() ;

  topFram = framRect.y()  		;
  topGrbx = grbxRect.y()  		;
  botFram = topFram + framRect.height() ;

  int dw = combRect.height() + grbxRect.height() + 40 ; // 40 is the total amount of vertical space between components

  // Adjust for width stretching first
  ui->hNavigatorInputFileCB->setGeometry(combRect.x(), combRect.y(), thisRect.width()-20, combRect.height()) ;
  ui->hNavigatorTreeFrame  ->setGeometry(framRect.x(), framRect.y(), thisRect.width()-20, framRect.height()) ;

  // Adjust for height stretching the only strechable part (the frame)
  framRect = ui->hNavigatorTreeFrame  ->geometry() ;
  ui->hNavigatorTreeFrame  ->setGeometry(framRect.x(), framRect.y(), framRect.width()   , thisRect.height()-dw) ; // Consider room for components without vertical stretch

  // Recompute current corners for vertical stretch
  combRect = ui->hNavigatorInputFileCB->geometry() ;
  framRect = ui->hNavigatorTreeFrame  ->geometry() ;

  topFram = framRect.y()     		;
  topGrbx = botFram + 8        		;
  botFram = topFram + framRect.height() ;       
	 
  // Adjust for vertical stretching
  if( theHTreeBrowser_ ) // At first call this component is not yet existing
    theHTreeBrowser_       ->setGeometry(framRect.x(), framRect.y(), thisRect.width()-20, framRect.height() ) ;

  ui->hNavigatorCanvasGB   ->setGeometry(grbxRect.x(), topGrbx     , thisRect.width()-20, grbxRect.height() ) ;

  int currentIndex = this->currentIndex() ;
  if( currentIndex == 0 )
  {
    this->setCurrentIndex(1);
    this->setCurrentIndex(0);
  }
  else
  {
    this->setCurrentIndex(0);
    this->setCurrentIndex(currentIndex);
  }

}

//===========================================================================
std::string HNavigator::twoDOption(void)
{
  if( ui->surfCB   ->isChecked() ) return "SURF4";
  if( ui->legoCB   ->isChecked() ) return "LEGO4";
  if( ui->contourCB->isChecked() ) return "CONT" ;
  if( ui->COLZCB   ->isChecked() ) return "COLZ" ;
  return "" ;
}

//===========================================================================
void HNavigator::on_saveComponentsPB_clicked()
{
  // Ask for output file name
  if(!theFileEater_)
  {
      STDLINE("No file is registered as open in input, there is nothing to save",ACRed) ;
      return ;
  }

  inputFileName_ = theFileEater_->getInputFileName();
  QStringList tmpFileName  = QString(inputFileName_.c_str()).split(".root") ;
  QString newName  = tmpFileName.at(0) + "_histograms.root" ;

  if(usePartitions_)
  {
      QRegExp rx ("(.+)?:\\s+(\\d+)\\s+-\\s(.+)?:\\s+(\\d+)");
      QString tempDUTName;

      if(rx.indexIn(QString(theCurrentDUT_.c_str()))!= -1)
      {
         QStringList list = rx.capturedTexts();
         for(int i = 1; i<list.size();i++)
         {
             tempDUTName.append(list.at(i));
             tempDUTName.append("_");
         }
      }

      QString partitionInfo = "_DUT_"                            +
                              tempDUTName                        +
                              "Sector_"                          +
                              QString(theCurrentSector_.c_str()) ;

      newName  = tmpFileName.at(0) + partitionInfo + "_histograms.root";
      QString monicelliOutputFileNewName =tmpFileName.at(0) + partitionInfo + ".root" ;
      QStringList tmpGeoName = QString(geometryFileName_.c_str()).split(".geo") ;
      QString geometryFileNewName = tmpGeoName.at(0) + partitionInfo + ".geo" ;

      ss_.str("");
      ss_<<"cp "<<inputFileName_<<" "<<monicelliOutputFileNewName.toStdString()<<endl;
      system(ss_.str().c_str());

      ss_.str("");
      ss_<<"cp "<<geometryFileName_<<" "<<geometryFileNewName.toStdString()<<endl;
      system(ss_.str().c_str());
  }


  QString fileName = QFileDialog::getSaveFileName(this,
                                                  tr("Save File"),
                                                  newName,
                                                  tr("Root files (*.root)"));
  if(fileName.isEmpty()) return ;

  TFile * tmpFile = new TFile(fileName.toStdString().c_str(),"recreate") ;

  // Get list of selected items to dump into new file
  hTreeBrowser::selectedObjectsDef selectedObjects = theHTreeBrowser_->getSelectedItems()  ;
//  STDLINE(selectedObjects.size(),ACReverse) ;
  // Build the folder structure (if requested, otherwise dump a flat list)
  if( !ui->flattenHierarchyCB->isChecked())
  {
      for(hTreeBrowser::selectedObjectsDef::iterator it=selectedObjects.begin(); it!=selectedObjects.end(); ++it)
      {
//          STDLINE(it->first,ACWhite) ;
          if( !(it->first == "" ))
          {
              tmpFile->cd() ; // Restart always from top directory
              std::string slashTerminatedFullPath = it->first + std::string("/") ;
              this->makeDir(slashTerminatedFullPath) ;
//              STDLINE(slashTerminatedFullPath,ACCyan) ;
          }
      }
  }

  // Save each selected item (if requested, into the appropriate folder)
  for(hTreeBrowser::selectedObjectsDef::iterator it=selectedObjects.begin(); it!=selectedObjects.end(); ++it)
  {
//      STDLINE(it->first,ACCyan) ;
      if(!(it->first == "" ))
      {
          for(hTreeBrowser::tFileVDef::iterator jt=it->second.begin(); jt!=it->second.end(); ++jt)
          {
              tmpFile->cd() ; // Restart always from top directory
              if( !ui->flattenHierarchyCB->isChecked() )
              {
                  tmpFile->cd(it->first.c_str()) ;
              }

              if( (*jt)->IsA() == TFolder::Class() ) continue ;

//              STDLINE((*jt)->GetName(),ACCyan) ;
              (*jt)->Write() ;
          }
      }
  }

  tmpFile->Close() ;
  delete tmpFile ;
  return ;
}

//===========================================================================
void HNavigator::saveAll(void)
{
    STDLINE("Saving all",ACRed) ;
    theHTreeBrowser_->selectAll() ;
    this->on_saveComponentsPB_clicked() ;
}
//===========================================================================
void HNavigator::makeDir(std::string dirName)
{
  // Recursive method: builds a directory structure into the currently open file
  //                   by scanning the elements names in dirName
  boost::cmatch what;
  static const boost::regex exp("(.*?)/(.*)", boost::regex::perl);

  std::string match = "" ;
  std::string rest  = "" ;

  // Split directory name according to the '/' character
  if( boost::regex_match(dirName.c_str(), what, exp, boost::match_extra) )
  {
    match = what[1] ;
    rest  = what[2] ;
    TKey * key = gDirectory->FindKey(match.c_str()) ;
    if( key )
    {
      gDirectory->cd(match.c_str()) ;
    }
    else
    {
      gDirectory->mkdir(match.c_str())->cd() ;
    }
    if( !rest.empty()) this->makeDir(rest) ; // If there are still additional components in the directory name, continue splitting
  }
}

//===========================================================================
void HNavigator::on_hCanvasCB_activated(int index)
{
    theHTreeBrowser_->setCurrentCanvas(index) ;
}

//===========================================================================
void HNavigator::on_canvasSizeCB_activated(QString size)
{
    theHTreeBrowser_->setCanvasSize(size.toStdString()) ;
}

//===========================================================================
bool HNavigator::plotStatBox(void)
{
 return ui->statCB->isChecked();
}

//===========================================================================
bool HNavigator::plotFitBox(void)
{
 return ui->fitCB->isChecked();
}

//===========================================================================
void HNavigator::on_unZoomPB_clicked()
{
   theHTreeBrowser_->unZoom() ;
//  hTreeBrowser::selectedObjectsDef selectedObjects = theHTreeBrowser_->getSelectedItems()  ;
////  ss_.str("") ;
////  ss_ << "Selected " << selectedObjects.size() << " hisotgrams" ;
////  STDLINE(ss_.str(),ACWhite) ;

//  hTreeBrowser::canvasWidgetDef theCanvases = theHTreeBrowser_->getCanvases() ;

//  for(hTreeBrowser::selectedObjectsDef::iterator it=selectedObjects.begin(); it!=selectedObjects.end(); ++it)
//  {
////    ss_.str("") ;
////    ss_ << "  Plot: " << (*it).first;
////    STDLINE(ss_.str(),ACWhite) ;

//    for(hTreeBrowser::tFileVDef::iterator jt=it->second.begin(); jt!=it->second.end(); ++jt)
//    {

//      if( (*jt)->IsA() == TFolder::Class() ) continue ;
//      ((TH1*)(*jt))->GetXaxis()->UnZoom() ;
//      ((TH1*)(*jt))->GetYaxis()->UnZoom() ;
//      theCanvases[theHTreeBrowser_->getCurrentCanvas()]->update() ;
//      ss_.str("") ;
//      ss_ << "      Plot: " << (*jt)->GetTitle() ;
//      STDLINE(ss_.str(),ACWhite) ;
//    }
//  }

//  theCanvases[theHTreeBrowser_->getCurrentCanvas()]->update() ;
}
//===========================================================================
void HNavigator::getPartitionsInfos(bool usePartitions, string theCurrentDUT, string theCurrentSector)
{
    usePartitions_    = usePartitions;
    theCurrentDUT_    = theCurrentDUT;
    theCurrentSector_ = theCurrentSector;

    //ss_.str(""); ss_<<"Use Partitions is: "<<usePartitions_<<" DUT is:  "<<theCurrentDUT_<<" Sector is: "<<theCurrentSector_<<endl; STDLINE(ss_.str(),ACCyan);
}
//===========================================================================
double HNavigator::getLabelsSize(void)
{
    return ui->labelsSizeDS->value() ;
}
//===========================================================================
double HNavigator::getLabelsOffset(void)
{
    return ui->labelsOffsetDS->value() ;
}
//===========================================================================
bool HNavigator::removeNULLRMSPlots(void)
{
    return ui->removeNullMeanPlotsCB->isChecked() ;
}
//===========================================================================
bool HNavigator::plotWithErrorBars(void)
{
    return ui->errorBarsCB->isChecked() ;
}
