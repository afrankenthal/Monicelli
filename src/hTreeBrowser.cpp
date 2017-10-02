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
 
#include <QtGui/QtGui>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QMdiArea>

#include <TDirectory.h>
#include <TKey.h>
#include <TIterator.h>
#include <TObject.h>
#include <TTree.h>

#include "hTreeBrowser.h"
#include "mainwindow.h"
#include "MessageTools.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>

//=========================================================================
hTreeBrowser::hTreeBrowser(QWidget *parent, MainWindow * mainWindow)
    : QTreeWidget(parent)
{
    parent_        = parent->parentWidget() ;
    theMainWindow_ = mainWindow ;

    theHManager_   = theMainWindow_->getHManager()   ;

    canvasPosX_   = 5;
    canvasPosY_   = 5;
    canvasWitdh_  = 800;
    canvasHeight_ = 600;

    currentCanvas_ = 0 ;

    serviceCanvas_.clear();
    existingWItems_.clear() ;
    cSw_.clear() ;

    this->setGeometry(600,10,900,250);

    QStringList labels;
    labels << tr("Folder Name") << tr("Object Type") << tr("Entries");
    this->setHeaderLabels(labels);
    this->header()->resizeSection(0,200);

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    QStyle *style = parent_->style();

    QPixmap closedFolderPM((std::string(getenv("MonicelliDir")) + std::string("/images/ClosedFolder.png")).c_str());
    QPixmap openFolderPM  ((std::string(getenv("MonicelliDir")) + std::string("/images/OpenFolder.png"  )).c_str());
    QPixmap TH1IconPM     ((std::string(getenv("MonicelliDir")) + std::string("/images/TH1Icon.png"     )).c_str());
    QPixmap TH2IconPM     ((std::string(getenv("MonicelliDir")) + std::string("/images/TH2Icon.png"     )).c_str());
    QPixmap TTreePM       ((std::string(getenv("MonicelliDir")) + std::string("/images/TreeIcon.png"    )).c_str());

    folderIcon_.addPixmap(closedFolderPM, QIcon::Normal, QIcon::Off      );
    folderIcon_.addPixmap(openFolderPM,   QIcon::Normal, QIcon::On       );
    canvasIcon_.addPixmap(style->standardPixmap(QStyle::SP_FileIcon     ));
    TH1Icon_   .addPixmap(TH1IconPM                                      );
    TH2Icon_   .addPixmap(TH2IconPM                                      );
    tTreeIcon_ .addPixmap(TTreePM                                        );
    mainIcon_  .addPixmap(style->standardPixmap(QStyle::SP_DriveHDIcon  ));

    this->setAnimated(true);

    this->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QObject::connect(this, SIGNAL(customContextMenuRequested( const QPoint         &     )),
                     this, SLOT(  showContextMenu           ( const QPoint         &     ))) ;
    QObject::connect(this, SIGNAL(itemClicked               (       QTreeWidgetItem*, int)),
                     this, SLOT(  manipulateFolder          (       QTreeWidgetItem*, int))) ;
    QObject::connect(this, SIGNAL(itemEntered               (       QTreeWidgetItem*, int)),
                     this, SLOT(  showHint                  (       QTreeWidgetItem*, int))) ;
    zones_[ 1] = std::make_pair(1,1) ;
    zones_[ 2] = std::make_pair(2,1) ;
    zones_[ 3] = std::make_pair(3,1) ;
    zones_[ 4] = std::make_pair(2,2) ;
    zones_[ 5] = std::make_pair(3,2) ;
    zones_[ 6] = std::make_pair(3,2) ;
    zones_[ 7] = std::make_pair(3,3) ;
    zones_[ 8] = std::make_pair(3,3) ;
    zones_[ 9] = std::make_pair(3,3) ;
    zones_[10] = std::make_pair(4,3) ;
    zones_[11] = std::make_pair(4,3) ;
    zones_[12] = std::make_pair(4,3) ;
    zones_[13] = std::make_pair(4,4) ;
    zones_[14] = std::make_pair(4,4) ;
    zones_[15] = std::make_pair(4,4) ;
    zones_[16] = std::make_pair(4,4) ;
    zones_[17] = std::make_pair(4,5) ;
    zones_[18] = std::make_pair(4,5) ;
    zones_[19] = std::make_pair(4,5) ;
    zones_[20] = std::make_pair(4,5) ;
    zones_[21] = std::make_pair(5,5) ;
    zones_[22] = std::make_pair(5,5) ;
    zones_[23] = std::make_pair(5,5) ;
    zones_[24] = std::make_pair(5,5) ;
    zones_[25] = std::make_pair(5,5) ;
    zones_[26] = std::make_pair(5,6) ;
    zones_[27] = std::make_pair(5,6) ;
    zones_[28] = std::make_pair(5,6) ;
    zones_[29] = std::make_pair(5,6) ;
    zones_[30] = std::make_pair(5,6) ;
    zones_[31] = std::make_pair(6,6) ;
    zones_[32] = std::make_pair(6,6) ;
    zones_[33] = std::make_pair(6,6) ;
    zones_[34] = std::make_pair(6,6) ;
    zones_[35] = std::make_pair(6,6) ;
    zones_[36] = std::make_pair(6,6) ;
    zones_[37] = std::make_pair(6,7) ;
    zones_[38] = std::make_pair(6,7) ;
    zones_[39] = std::make_pair(6,7) ;
    zones_[40] = std::make_pair(6,7) ;
    zones_[41] = std::make_pair(6,7) ;
    zones_[42] = std::make_pair(6,7) ;
    zones_[43] = std::make_pair(7,7) ;
    zones_[44] = std::make_pair(7,7) ;
    zones_[45] = std::make_pair(7,7) ;
    zones_[46] = std::make_pair(7,7) ;
    zones_[47] = std::make_pair(7,7) ;
    zones_[48] = std::make_pair(7,7) ;
    zones_[49] = std::make_pair(7,7) ;
    zones_[50] = std::make_pair(7,8) ;
    zones_[51] = std::make_pair(7,8) ;
    zones_[52] = std::make_pair(7,8) ;
    zones_[53] = std::make_pair(7,8) ;
    zones_[54] = std::make_pair(7,8) ;
    zones_[55] = std::make_pair(7,8) ;
    zones_[56] = std::make_pair(7,8) ;
    zones_[57] = std::make_pair(8,8) ;
    zones_[58] = std::make_pair(8,8) ;
    zones_[59] = std::make_pair(8,8) ;
    zones_[60] = std::make_pair(8,8) ;
    zones_[61] = std::make_pair(8,8) ;
    zones_[62] = std::make_pair(8,8) ;
    zones_[63] = std::make_pair(8,8) ;
    zones_[64] = std::make_pair(8,8) ;
}

//=========================================================================
hTreeBrowser::~hTreeBrowser(void)
{
//    ss_.str("") ; ss_ << "Geometry: "
//                      << this->geometry().x()
//                      << "+"
//                      << this->geometry().y()
//                      << " "
//                      << this->geometry().width()
//                      << " x "
//                      << this->geometry().height() ;
//    STDLINE(ss_.str(),ACWhite) ;
    QSettings settings("CMS", "hTreeBrowser");
    settings.setValue("windowWidth" , windowSize_.width()) ;
    settings.setValue("windowHeight", windowSize_.height());
    STDLINE("Dtor",ACRed) ;
}

//=========================================================================
void hTreeBrowser::cleanDestroy(void)
{
    STDLINE("Destroying pointer to serviceCanvas_ ",ACRed) ;
    serviceCanvas_.clear() ;
    cSw_.clear() ;
}

//=========================================================================
void hTreeBrowser::populate(TDirectory * currentDirectory)
{
    if( ! currentDirectory )
    {
        ss_.str("");
        ss_<<"Directory "<<currentDirectory->GetName()<<" not found!";
        STDLINE(ss_.str(),ACRed) ;
        return ;
    }
    QTreeWidgetItem * wItem = NULL ;
    if( existingWItems_.find(std::string(currentDirectory->GetName())) == existingWItems_.end())
    {
        wItem = new QTreeWidgetItem(this);
        existingWItems_[std::string(currentDirectory->GetName())] = wItem ;
        wItem->setText(0, currentDirectory->GetName()) ;
        wItem->setIcon(0, mainIcon_                  ) ;
    }
    else
    {
        wItem = existingWItems_[std::string(currentDirectory->GetName())] ;
    }

    this->populate(currentDirectory, wItem) ;
}

//=========================================================================
void hTreeBrowser::populate(TFolder * currentFolder)
{
    if( ! currentFolder )
    {
        ss_.str("");
        ss_<< "Directory "
           << currentFolder->GetName()
           << " not found!";
        STDLINE(ss_.str(),ACRed) ;
        return ;
    }
    QTreeWidgetItem * wItem = NULL ;
    if( existingWItems_.find(std::string(currentFolder->GetName())) == existingWItems_.end())
    {
        wItem = new QTreeWidgetItem(this);
        existingWItems_[std::string(currentFolder->GetName())] = wItem ;
        wItem->setText(0, currentFolder->GetName()) ;
        wItem->setIcon(0, mainIcon_               ) ;
    }
    else
    {
        wItem = existingWItems_[std::string(currentFolder->GetName())] ;
    }

    this->populate(currentFolder,wItem) ;
}

//=========================================================================
void hTreeBrowser::populate(TFolder * currentFolder, QTreeWidgetItem * parentWItem)
{
    if (!parentWItem || !currentFolder )
    {
        ss_.str("");
        ss_<<"Item "<<parentWItem->text(0).toStdString()<<" or folder "<<currentFolder->GetName()<<" not found!";
        STDLINE(ss_.str(),ACRed) ;
        return ;
    }

    QTreeWidgetItem * wItem   = NULL  ;
    TObject         * obj     = NULL  ;
    std::string       objPath = ""    ;

    bool              create  = false ;

    TCollection * rootFoldersColl     = (TCollection *)currentFolder->GetListOfFolders() ;
    TIterator   * rootFoldersIterator = rootFoldersColl->MakeIterator() ;

    while((obj = rootFoldersIterator->Next()))
    {
        std::string folderName = obj->GetName() ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Classes"))        continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Colors"))         continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("MapFiles"))       continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Sockets"))        continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Canvases"))       continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Styles"))         continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Functions"))      continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Tasks"))          continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Geometries"))     continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Browsers"))       continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Specials"))       continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Handlers"))       continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Cleanups"))       continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("StreamerInfo"))   continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("SecContexts"))    continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("PROOF Sessions")) continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("ROOT Memory"))    continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("ROOT Files"))     continue ;

        objPath = std::string(currentFolder->GetName()) + std::string("-") + folderName ;
        if( existingWItems_.find(objPath) == existingWItems_.end())
        {
            wItem = new QTreeWidgetItem(parentWItem,0) ;
            existingWItems_[objPath] = wItem ;
            create = true ;
        }
        else
        {
            wItem  = existingWItems_[objPath] ;
            create = false ;
        }

        this->getObjectType(obj) ;

        if( obj->IsA() == TFolder::Class() )
        {
            if( create )
            {
                wItem->setText(0, tr(obj->GetName())) ;
                wItem->setText(1, tr("Folder")) ;
                wItem->setIcon(0, folderIcon_);
            }
            this->populate((TFolder*)obj,wItem) ;
        }
        else if( this->getObjectType(obj) == "TH1" )
        {
            ss_.str("");
            ss_<<"Folder: "<<obj->GetName()<<" Item: "<<wItem->text(0).toStdString();
            if( !create ) continue ;
            ss_.str(""); ss_ << ((TH1*)obj)->GetEntries() ;
            wItem->setText(0, tr(obj->GetName())) ;
            wItem->setText(1, tr("TH1")) ;
            wItem->setText(2, tr(ss_.str().c_str())) ;
            wItem->setIcon(0, TH1Icon_);
        }
        else if( this->getObjectType(obj) == "TH2" )
        {
            if( !create ) continue ;
            ss_.str(""); ss_ << ((TH1*)obj)->GetEntries() ;
            wItem->setText(0, tr(obj->GetName())) ;
            wItem->setText(1, tr("TH2")) ;
            wItem->setText(2, tr(ss_.str().c_str())) ;
            wItem->setIcon(0, TH2Icon_);
        }
    }
}

//=========================================================================
std::string hTreeBrowser::getObjectType(TObject * obj)
{
    std::string objectType = "Unknown" ;

    TKey * keyH = NULL ;

    TIter bases(obj->IsA()->GetListOfBases() ) ;
    int count = 0 ;
    while((keyH = (TKey*)bases()))
    {
        if( count++ == 0 ) objectType = keyH->GetName() ;
    }
    return objectType ;
}

//=========================================================================
void hTreeBrowser::populate(TDirectory * currentDirectory, QTreeWidgetItem * parentWItem)
{
    QTreeWidgetItem * wItem   = NULL;
    bool              create  = false ;

    TKey * keyH = NULL ;

    if( !parentWItem || !currentDirectory)
    {
        ss_.str("");
        ss_<<"Item "<<parentWItem->text(0).toStdString()<<" or folder "<<currentDirectory->GetName()<<" not found!";
        STDLINE(ss_.str(),ACRed) ;
        return ;
    }

    TIter hList(currentDirectory->GetListOfKeys());

    while((keyH = (TKey*)hList()))
    {
        this->getObjectType(keyH->ReadObj()) ;
        std::string objPath = std::string(currentDirectory->GetName()) + std::string("-") +  std::string(keyH->GetName());
        if( existingWItems_.find(objPath) == existingWItems_.end())
        {
            wItem = new QTreeWidgetItem(parentWItem) ;
            existingWItems_[objPath] = wItem ;
            create = true ;
        }
        else
        {
            wItem  = existingWItems_[objPath] ;
            create = false ;
        }

        if( keyH->IsFolder() )
        {
            if(      std::string(keyH->GetClassName()) == "TTree")
            {
                if( !create ) continue ;
                wItem->setText(0, keyH->GetName()) ;
                wItem->setText(1, "TTree") ;
                wItem->setIcon(0, tTreeIcon_);
            }
            else if (std::string(keyH->GetClassName()) == "TFolder" )
            {
                TFolder * aFolder = (TFolder*)(currentDirectory->GetKey(keyH->GetName())->ReadObj()) ; // Get the folder in memory (this is the trick!)
                if( create )
                {
                    wItem->setText(0, tr(keyH->GetName())) ;
                    wItem->setText(1, tr(keyH->GetClassName())) ;
                    wItem->setIcon(0, folderIcon_);
                }
                this->populate(aFolder,wItem) ;
            }
            else
            {
                currentDirectory->cd(keyH->GetName());
                if( create )
                {
                    wItem->setText(0, tr(gDirectory->GetName())) ;
                    wItem->setText(1, tr(keyH->GetClassName())) ;
                    wItem->setIcon(0, folderIcon_);
                }
                this->populate(gDirectory,wItem) ;
            }
        }
        else
        {
            if( !create ) continue ;
            wItem->setText(0, tr(keyH->GetName())) ;
            wItem->setText(1, tr(keyH->GetClassName())) ;
            if(std::string(keyH->GetClassName()) == "TCanvas" )
            {
                wItem->setIcon(0, canvasIcon_);
            }
            else if( this->getObjectType(keyH->ReadObj()) == "TH1" )
            {
                wItem->setIcon(0, TH1Icon_);
            }
            else if( this->getObjectType(keyH->ReadObj()) == "TH2" )
            {
                wItem->setIcon(0, TH2Icon_);
            }
            else
            {
                wItem->setIcon(0, fileIcon_);
            }
        }
    }
}

//=========================================================================
hTreeBrowser::dirPathDef hTreeBrowser::getFullPath(QTreeWidgetItem * wItem)
{
    std::string fullPath = wItem->text(0).toStdString() ;

    if( wItem->parent() )
    {
        this->getFullPath(wItem->parent());
        dirPath_.push_back(fullPath) ;
        return dirPath_ ;
    }

    dirPath_.clear() ;
    dirPath_.push_back(fullPath) ;
    return dirPath_ ;
}

//=========================================================================
void hTreeBrowser::showContextMenu(const QPoint &)
{
    theHNavigator_ = theMainWindow_->getHNavigator() ;

    // Create suitable canvas (a singleton)
    if( serviceCanvas_.find(currentCanvas_) == serviceCanvas_.end() )
    {
        serviceCanvas_[currentCanvas_] = new canvasWidget() ;
        ss_.str(""); ss_ << "Current canvas: " << currentCanvas_ ;
        serviceCanvas_[currentCanvas_]->setTitle(ss_.str()) ;
        connect( serviceCanvas_[currentCanvas_], SIGNAL(destroyed()    ),
                 this                          , SLOT  (cleanDestroy())) ;
    }
    else
    {
        serviceCanvas_[currentCanvas_]->clear() ;
        serviceCanvas_[currentCanvas_]->show()  ;
    }

    // Add the canvas to the main window
    if( cSw_.find(currentCanvas_) == cSw_.end())
    {
        cSw_[currentCanvas_] = theMainWindow_->getMdiArea()->addSubWindow(serviceCanvas_[currentCanvas_]) ;
        cSw_[currentCanvas_]->setGeometry(canvasPosX_,canvasPosY_,canvasWitdh_,canvasHeight_) ;
        canvasPosX_   += 5;
        canvasPosY_   += 5;
    }

    // Determine name -> full-path association for the selected items
    selectedObjectsDef selectedObjects = this->getSelectedItems() ;

    int numberOfPlots = 0 ;
    for (selectedObjectsDef::iterator it=selectedObjects.begin(); it!=selectedObjects.end(); ++it)
    {
        for(tFileVDef::iterator jt=it->second.begin(); jt!=it->second.end(); ++jt)
        {
            if( (*jt)->IsA() == TFolder::Class()        ) continue ;
            if( (*jt)->IsA() == TDirectory::Class()     ) continue ;
            if( (*jt)->IsA() == TDirectoryFile::Class() ) continue ;
            if( (*jt)->IsA() == TCanvas::Class()        ) continue ;
            if(theHNavigator_->removeNULLRMSPlots() &&
               ( ((TH1*)(*jt))->GetRMS() == 0 )         ) continue ;
            numberOfPlots++ ;
        }
    }

    if( theHNavigator_->plotStatBox() )
    {
//      STDLINE("Enabling statistics box",ACWhite);
      gStyle->SetOptStat(111111) ;
    }
    else
    {
//      STDLINE("Disabling statistics box",ACWhite);
      gStyle->SetOptStat(0) ;
    }

    if( theHNavigator_->plotFitBox() )
    {
//      STDLINE("Enabling fit results box",ACWhite);
      gStyle->SetOptFit(111111) ;
    }
    else
    {
//      STDLINE("Disabling fit results box",ACWhite);
      gStyle->SetOptFit(0) ;
    }

    float xmargin = 0.01 ;
    float ymargin = 0.01 ;
    if( numberOfPlots > 36 )
    {
        xmargin = 0. ;
        ymargin = 0. ;
        STDLINE("WARNING: using zero margins",ACYellow) ;
    }

    serviceCanvas_[currentCanvas_]->divide(
                                           zones_[numberOfPlots].first,
                                           zones_[numberOfPlots].second,
                                           xmargin,
                                           ymargin
                                          ) ;

    std::string options = "" ;

    if( theHNavigator_ ) options += theHNavigator_->twoDOption() ;

    int pad = 1 ;
    for (selectedObjectsDef::iterator it=selectedObjects.begin(); it!=selectedObjects.end(); ++it)
    {
        for(tFileVDef::iterator jt=it->second.begin(); jt!=it->second.end(); ++jt)
        {
            string errorBars = "" ;
            if( (*jt)->IsA() == TDirectory::Class()     ) continue ;
            if( (*jt)->IsA() == TFolder::Class()        ) continue ;
            if( (*jt)->IsA() == TDirectoryFile::Class() ) continue ;
            if( (*jt)->IsA() == TCanvas::Class() )
            {
                (*jt)->Draw() ;
                continue ;
            }
            ((TH1*)(*jt))->GetXaxis()->SetLabelSize  (theHNavigator_->getLabelsSize  ()) ;
            ((TH1*)(*jt))->GetYaxis()->SetLabelSize  (theHNavigator_->getLabelsSize  ()) ;
            ((TH1*)(*jt))->GetXaxis()->SetLabelOffset(theHNavigator_->getLabelsOffset()) ;
            ((TH1*)(*jt))->GetYaxis()->SetLabelOffset(theHNavigator_->getLabelsOffset()) ;
            if(theHNavigator_->removeNULLRMSPlots() && ( ((TH1*)(*jt))->GetRMS() == 0 )) continue ;
            if(theHNavigator_->plotWithErrorBars()) errorBars = "E1" ;
            serviceCanvas_[currentCanvas_]->cd(pad++) ;
            if(this->getObjectType(*jt) == "TH2" )
            {
                (*jt)->Draw(options.c_str()) ;
            }
            else
            {
                (*jt)->Draw(errorBars.c_str()) ;
            }
        }
    }

    serviceCanvas_[currentCanvas_]->update() ;

    cSw_[currentCanvas_]->show() ;
    cSw_[currentCanvas_]->raise() ;
}

//===========================================================================
void hTreeBrowser::resizeEvent(QResizeEvent * ev)
{
    windowSize_ = ev->size() ;
//    ss_.str("") ; ss_ << "Geometry: "
//                      << this->geometry().x()
//                      << "+"
//                      << this->geometry().y()
//                      << " "
//                      << this->geometry().width()
//                      << " x "
//                      << this->geometry().height() ;
//    STDLINE(ss_.str(),ACWhite) ;
}

//===========================================================================
void hTreeBrowser::unZoom()
{
  // Determine name -> full-path association for the selected items
  selectedObjectsDef selectedObjects = this->getSelectedItems() ;

  for (selectedObjectsDef::iterator it=selectedObjects.begin(); it!=selectedObjects.end(); ++it)
  {
      for(tFileVDef::iterator jt=it->second.begin(); jt!=it->second.end(); ++jt)
      {
          if( (*jt)->IsA() == TDirectory::Class()     ) continue ;
          if( (*jt)->IsA() == TFolder::Class()        ) continue ;
          if( (*jt)->IsA() == TDirectoryFile::Class() ) continue ;
          if( (*jt)->IsA() == TCanvas::Class()        ) continue ;

          ((TH1*)(*jt))->GetXaxis()->UnZoom() ;
          ((TH1*)(*jt))->GetYaxis()->UnZoom() ;

//          serviceCanvas_[currentCanvas_]->flush() ;
      }
   }
  serviceCanvas_[currentCanvas_]->flush() ;
  serviceCanvas_[currentCanvas_]->update() ;

}
//===========================================================================
hTreeBrowser::selectedObjectsDef hTreeBrowser::getSelectedItems()
{
    hTreeBrowser::selectedObjectsDef selectedObjects ;

    QList<QTreeWidgetItem *> selectedItems = this->selectedItems() ;

    for (int item = 0; item < selectedItems.size(); ++item)
    {
//        if( selectedItems[item]->parent() )
//        {
//            STDLINE(selectedItems[item]->parent()->text(0).toStdString(),ACBlue) ;
//            STDLINE(selectedItems[item]->text(0).toStdString(),ACWhite) ;
//        }
//        else
//        {
//            STDLINE(selectedItems[item]->text(0).toStdString(),ACCyan) ;
//        }
        this->collectItems(selectedItems[item],selectedObjects) ;
    }
//    STDLINE(selectedObjects.size(),ACCyan) ;
    return selectedObjects ;
}

//===========================================================================
void hTreeBrowser::collectItems(QTreeWidgetItem                  * wItem,
                                hTreeBrowser::selectedObjectsDef & selectedObjects)
{
    hTreeBrowser::dirPathDef dirPath = this->getFullPath(wItem) ;

    if( dirPath.size() < 2 ) return ;

    std::string dir = "";

    if( dirPath.size() > 2 )
    {
        for(hTreeBrowser::dirPathDef::iterator ni=dirPath.begin()+1; ni!=dirPath.end()-1; ++ni)
        {
//            STDLINE(*ni,ACWhite) ;
            dir += *ni ;
            if( ni != dirPath.end()-2) dir += "/" ;
        }
    }

    if( dirPath[0] == "root" ) // Selected object is in memory only
    {
//        STDLINE(dir,ACCyan) ;
        TFolder * targetFolder = (TFolder *)(gROOT->GetRootFolder()->FindObjectAny(dir.c_str())) ;
        TObject * obj = targetFolder->FindObject(wItem->text(0).toStdString().c_str()) ;
        if( !obj ) {/*STDLINE("Not found",ACWhite)*/; return;}
//        ss_.str(""); ss_<<obj->GetName()<< " in " << dir; STDLINE(ss_.str(),ACWhite) ;
        selectedObjects[dir].push_back(obj) ;
    }
    else // Selected object resides on files
    {
//        STDLINE(dirPath[0],ACYellow) ;
        TFile * file = theHManager_->getFileHandle(dirPath[0]) ;
//        STDLINE(file->GetName(),ACGreen) ;

//        STDLINE(dirPath[1],ACBold) ;
        TObject * obj = file->FindObjectAny(dirPath[1].c_str()) ;
        if( obj )
        {
//            STDLINE(obj->GetName(),ACRed) ;

            if( obj->IsA() == TFolder::Class() )
            {
                for(hTreeBrowser::dirPathDef::iterator ni=dirPath.begin()+2; ni!=dirPath.end(); ++ni)
                {
                    if( ni == dirPath.end() - 1 ) // Last is the histogram name
                    {
//                        ss_.str(""); ss_<<*ni<< " in " << dir; STDLINE(ss_.str(),ACWhite) ;
                        selectedObjects[dir].push_back(((TFolder*)obj)->FindObjectAny((*ni).c_str())) ;
                    }
//                    else
//                    {
//                        STDLINE(*ni,ACReverse) ;
//                        obj = ((TFolder*)obj)->FindObjectAny((*ni).c_str()) ;
//                    }
                }
            }
//            else if(obj->IsA() == TDirectoryFile::Class())
//            {
//                file->cd(dir.c_str()) ;

//                STDLINE(wItem->text(0).toStdString(),ACWhite) ;
//                ss_.str(""); ss_<<*ni<< " in " << dirPath[1]; STDLINE(ss_.str(),ACWhite) ;
//                selectedObjects[dirPath[0]].push_back(gDirectory->GetKey(wItem->text(0).toStdString().c_str())->ReadObj());

//                if(selectedObjects[dirPath[0]].back()->IsA() == TDirectoryFile::Class())
//                {
//                    for(int i=0; i<wItem->childCount(); ++i)
//                    {
//                        this->collectItems(wItem->child(i),selectedObjects) ;
//                    }
//                }
//            }
        }
        else
        {
            TFolder * targetFolder = (TFolder *)(gROOT->GetRootFolder()->FindObjectAny(dirPath[1].c_str())) ;
//            if(!targetFolder)
//            {
//                ss_.str(""); ss_ << "targetFolder " << dirPath[1] << " not found";
//                STDLINE(ss_.str(),ACRed) ;
//            }
            TObject * obj = targetFolder->FindObjectAny(wItem->text(0).toStdString().c_str()) ;
            if( obj )
            {
//                STDLINE(obj->GetName(),ACWhite)
                ss_.str(""); ss_<<obj->GetName()<< " in " << dir; STDLINE(ss_.str(),ACWhite) ;
                selectedObjects[dir].push_back(obj) ;
            }
//            else
//            {
//                ss_.str(""); ss_ << "object " << wItem->text(0).toStdString() << " not found";
//                STDLINE(ss_.str(),ACRed) ;
//            }
        }
    }
}

//===========================================================================
void hTreeBrowser::manipulateFolder( QTreeWidgetItem * wItem, int )
{
 /*
   Selection strategy for the current item:
   1) if it is a leaf just add it to the list of already selected items
   2) if it is a folder, deselect any explicitly selected child but add the
      folder to the list of already selected items.
 */
    if( wItem->childCount() == 0 )
    {
        wItem->setSelected(true);
    }
    else
    {
        this->selectAllChildren(wItem, wItem->isSelected()) ;
    }
}


//===========================================================================
void hTreeBrowser::showHint( QTreeWidgetItem * wItem, int )
{
    if( wItem->childCount() > 0 )
    {
//        STDLINE(wItem->text(0).toStdString(),ACYellow) ;
    }
    else
    {
//        STDLINE(wItem->text(0).toStdString(),ACWhite) ;
    }
}

//===========================================================================
void hTreeBrowser::selectAllChildren(QTreeWidgetItem * wItem, bool select)
{
    for(int i=0; i < wItem->childCount(); ++i)
    {
        wItem->child(i)->setSelected(select);
        this->selectAllChildren(wItem->child(i), select);
    }
}

//===========================================================================
void hTreeBrowser::getAllItems(QTreeWidgetItem * )
{
}

//===========================================================================
void hTreeBrowser::setCanvasSize(std::string canvasSize)
{
    if(      canvasSize == std::string("Small" ) )
    {
        canvasWitdh_  = 300;
        canvasHeight_ = 180;
    }
    else if( canvasSize == std::string("Medium") )
    {
        canvasWitdh_  = 400;
        canvasHeight_ = 300;
    }
    else if( canvasSize == std::string("Large") )
    {
        canvasWitdh_  = 800;
        canvasHeight_ = 600;
    }
    else if( canvasSize == std::string("Huge") )
    {
        canvasWitdh_  = 1200;
        canvasHeight_ = 800;
    }

    if( cSw_.find(currentCanvas_) != cSw_.end())
    {
        cSw_[currentCanvas_]->setGeometry(canvasPosX_,canvasPosY_,canvasWitdh_,canvasHeight_) ;
    }

}
