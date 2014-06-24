/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include <QtGui>
#include <QTreeWidgetItem>
#include <QMdiArea>

#include <TDirectory.h>
#include <TKey.h>
#include <TIterator.h>
#include <TObject.h>
#include <TTree.h>

#include "hTreeBrowser.h"
#include "mainwindow.h"

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
    cSw_.clear() ;

    QStringList labels;
    labels << tr("Folder Name") << tr("Object Type") << tr("Entries");
    this->setHeaderLabels(labels);
    this->header()->setResizeMode(0, QHeaderView::Interactive);
    this->header()->setResizeMode(1, QHeaderView::Interactive);
    this->header()->setResizeMode(2, QHeaderView::Interactive);

    this->header()->setDefaultSectionSize(200);

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
}

//=========================================================================
hTreeBrowser::~hTreeBrowser(void)
{
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
/*
            // dario
            ss_.str(""); ss_ << currentFolder->GetName() << " || " << "objPath: " << objPath << " parentWItem: " << parentWItem; STDLINE(ss_.str(),ACWhite)
            bool alreadyThere = false ;
            for(existingWItemsDef::iterator pi=existingWItems_.begin(); pi!=existingWItems_.end(); ++pi)
            {
                if( (*pi).second == parentWItem) {alreadyThere = true ; break;}
                ss_.str(""); ss_ << "  " << (*pi).first << " --> " << (*pi).second << " alreadyThere: " << alreadyThere; STDLINE(ss_.str(),ACCyan)
            }
            // dario

            if( alreadyThere )
            {
              wItem = parentWItem ;
            }
            else
            {
              wItem = new QTreeWidgetItem(parentWItem) ;
              existingWItems_[objPath] = wItem ;
              create = true ;
              ss_.str(""); ss_  << "Created " << ACRed << ACBold << parentWItem ; STDLINE(ss_.str(),ACCyan)
            }
            create = true ;
*/
            wItem = new QTreeWidgetItem(parentWItem) ;
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

    int nx, ny ;
    if( numberOfPlots ==  1 ) {nx=1;ny=1;}
    if( numberOfPlots ==  2 ) {nx=2;ny=1;}
    if( numberOfPlots ==  3 ) {nx=3;ny=1;}
    if( numberOfPlots ==  4 ) {nx=2;ny=2;}
    if( numberOfPlots ==  5 ) {nx=3;ny=2;}
    if( numberOfPlots ==  6 ) {nx=3;ny=2;}
    if( numberOfPlots ==  7 ) {nx=3;ny=3;}
    if( numberOfPlots ==  8 ) {nx=3;ny=3;}
    if( numberOfPlots ==  9 ) {nx=3;ny=3;}
    if( numberOfPlots == 10 ) {nx=4;ny=3;}
    if( numberOfPlots == 11 ) {nx=4;ny=3;}
    if( numberOfPlots == 12 ) {nx=4;ny=3;}
    if( numberOfPlots == 13 ) {nx=4;ny=4;}
    if( numberOfPlots == 14 ) {nx=4;ny=4;}
    if( numberOfPlots == 15 ) {nx=4;ny=4;}
    if( numberOfPlots == 16 ) {nx=4;ny=4;}

    serviceCanvas_[currentCanvas_]->divide(nx, ny) ;

    std::string options = "" ;

    if( theHNavigator_ ) options += theHNavigator_->twoDOption() ;

    int pad = 1 ;
    for (selectedObjectsDef::iterator it=selectedObjects.begin(); it!=selectedObjects.end(); ++it)
    {
        for(tFileVDef::iterator jt=it->second.begin(); jt!=it->second.end(); ++jt)
        {
            if( (*jt)->IsA() == TDirectory::Class()     ) continue ;
            if( (*jt)->IsA() == TFolder::Class()        ) continue ;
            if( (*jt)->IsA() == TDirectoryFile::Class() ) continue ;
            if( (*jt)->IsA() == TCanvas::Class() )
            {
                (*jt)->Draw() ;
                continue ;
            }
            serviceCanvas_[currentCanvas_]->cd(pad++) ;
            if(this->getObjectType(*jt) == "TH2" )
            {
                (*jt)->Draw(options.c_str()) ;
            }
            else
            {
                (*jt)->Draw() ;
            }
        }
    }

    serviceCanvas_[currentCanvas_]->update() ;

    cSw_[currentCanvas_]->show() ;
    cSw_[currentCanvas_]->raise() ;
}

//===========================================================================
void hTreeBrowser::resizeEvent(QResizeEvent * )
{
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
        this->collectItems(selectedItems[item],selectedObjects) ;
    }

    return selectedObjects ;
}

//===========================================================================
void hTreeBrowser::collectItems(QTreeWidgetItem                  * wItem,
                                hTreeBrowser::selectedObjectsDef & selectedObjects)
{
    hTreeBrowser::dirPathDef dirPath = this->getFullPath(wItem) ;

    std::string dir  = "";

    for(hTreeBrowser::dirPathDef::iterator ni=dirPath.begin()+1; ni!=dirPath.end()-1; ++ni)
    {
        dir += *ni ;
        if( ni != dirPath.end()-2) dir += "/" ;
    }

    if( dirPath[0] == "root" ) // Selected object is in memory only
    {
        TFolder * targetFolder = (TFolder *)(gROOT->GetRootFolder()->FindObjectAny(dir.c_str())) ;
        TObject * obj = targetFolder->FindObject(wItem->text(0).toStdString().c_str()) ;
        selectedObjects[dir].push_back(obj) ;
    }
    else // Selected object resides on files
    {
        TFile * file  = theHManager_->getFileHandle(dirPath[0]) ;

        TObject * obj = file->FindObjectAny(dirPath[1].c_str()) ;

        if( obj->IsA() == TFolder::Class() )
        {
            for(hTreeBrowser::dirPathDef::iterator ni=dirPath.begin()+2; ni!=dirPath.end(); ++ni)
            {
                if( ni == dirPath.end() - 1 ) // Last is the histogram name
                {
                    selectedObjects[dir].push_back(((TFolder*)obj)->FindObjectAny((*ni).c_str())) ;
                }
                else
                {
                    obj = ((TFolder*)obj)->FindObjectAny((*ni).c_str()) ;
                }
            }
        }
        else if(obj->IsA() == TDirectoryFile::Class())
        {
            file->cd(dir.c_str()) ;

            selectedObjects[dir].push_back(gDirectory->GetKey(wItem->text(0).toStdString().c_str())->ReadObj());

            if(selectedObjects[dir].back()->IsA() == TDirectoryFile::Class())
            {
                for(int i=0; i<wItem->childCount(); ++i)
                {
                    this->collectItems(wItem->child(i),selectedObjects) ;
                }
            }
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
