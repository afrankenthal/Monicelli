/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#include "xmlcreationdialog.h"
#include "ui_xmlcreationdialog.h"
#include "MessageTools.h"

//===========================================================================
xmlCreationDialog::xmlCreationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xmlCreationDialog)
{
  ui->setupUi(this);

  this->setModal(true) ;

  state_ = false ;
}

//===========================================================================
xmlCreationDialog::~xmlCreationDialog()
{
  delete ui;
}

//===========================================================================
int xmlCreationDialog::getStations()
{
  return ui->stationsSB->value() ;
}

//===========================================================================
int xmlCreationDialog::getDetectors()
{
  return ui->detectorsSB->value() ;
}

//===========================================================================
int xmlCreationDialog::getROCs()
{
  return ui->ROCsSB->value() ;
}

//===========================================================================
void xmlCreationDialog::on_okPB_clicked()
{
 state_ = true ;
 this->accept() ;
}

//===========================================================================
bool xmlCreationDialog::state(void)
{
  return state_ ;
}

//===========================================================================
void xmlCreationDialog::on_cancelPB_clicked()
{
    this->reject();
}
