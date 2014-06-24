/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#include "roctabwidget.h"
#include "ui_roctabwidget.h"

//================================================================================
ROCTabWidget::ROCTabWidget(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::ROCTabWidget)
{
    ui->setupUi(this);

    this->setMovable(false) ;
}

//================================================================================
ROCTabWidget::~ROCTabWidget()
{
    delete ui;
}

//===========================================================================
void ROCTabWidget::remTab(int ROCSerial)
{
  ss_.str(""); ss_ << "ROC " <<  ROCSerial;
  for(int i=0; i<this->count(); ++i)
  {
    if( this->tabText(i).toStdString() == ss_.str() ) this->removeTab(i) ;
  }
}
