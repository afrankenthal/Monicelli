/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#include "MessageTools.h"
#include "detectortabwidget.h"
#include "ui_detectortabwidget.h"

//================================================================================
detectorTabWidget::detectorTabWidget(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::detectorTabWidget)
{
    ui->setupUi(this);

    this->setMovable(true) ;

    QTabBar * theTabBar = this->tabBar () ;

    connect(theTabBar, SIGNAL(tabMoved        (int,int)),
            this,      SLOT  (reassignPosition(int,int))) ;
}

//================================================================================
detectorTabWidget::~detectorTabWidget()
{
    delete ui;
}

//===========================================================================
void detectorTabWidget::remTab(int detectorSerial)
{
  ss_.str(""); ss_ << "Detector " <<  detectorSerial;
  for(int i=0; i<this->count(); ++i)
  {
    if( this->tabText(i).toStdString() == ss_.str() ) this->removeTab(i) ;
  }
}

//===========================================================================
void detectorTabWidget::setNode(int        detectorSerial,
                                QDomNode & detectorNode  )
{
  ss_.str(""); ss_ << "Detector " << detectorSerial ;
  std::string label = ss_.str() ;

  detectorMap_[label] = detectorNode;
}

//===========================================================================
void detectorTabWidget::reassignPosition(int from, int to)
{
   int direction = from - to ;

   std::string tabLabelFrom = this->tabText(from).toStdString() ;
   std::string tabLabelTo   = this->tabText(to  ).toStdString() ;

   QDomNode parentNode = detectorMap_[tabLabelFrom].parentNode() ;

   if( direction > 0 ) // Insert after
   {
     parentNode.insertAfter(detectorMap_[tabLabelFrom],detectorMap_[tabLabelTo]) ;
   }
   else                // Insert before
   {
     parentNode.insertBefore(detectorMap_[tabLabelFrom],detectorMap_[tabLabelTo]) ;
   }
 }
