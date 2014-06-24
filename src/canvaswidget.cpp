/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include <iostream>
#include "canvaswidget.h"

#include "ui_canvaswidget.h"
#include "MessageTools.h"

#include "TH1D.h"
#include "TList.h"

//===========================================================================
canvasWidget::canvasWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::canvasWidget)
{
    ui->setupUi(this);
}

//===========================================================================
canvasWidget::~canvasWidget()
{
  STDLINE("Dtor",ACRed) ;
//  delete ui;
}

//===========================================================================
void canvasWidget::divide( int nx, int ny )
{
  ui->theCanvas->GetCanvas()->Clear() ;
  ui->theCanvas->GetCanvas()->Divide(nx, ny, 0.001, 0.001) ;
  ui->theCanvas->GetCanvas()->Modified() ;
  ui->theCanvas->GetCanvas()->Update();
}

//===========================================================================
void canvasWidget::cd( int pos )
{
  ui->theCanvas->GetCanvas()->cd(pos) ;
}

//===========================================================================
void canvasWidget::flush( void )
{
  int tot = 0 ;
  TObject * obj = NULL ;
  TIter next(ui->theCanvas->GetCanvas()->GetListOfPrimitives()) ;
  while((obj = next()))
  {
    if( obj->InheritsFrom(TVirtualPad::Class()))
    {
      ui->theCanvas->GetCanvas()->SetClickSelectedPad((TPad*)(obj)) ;
      ui->theCanvas->GetCanvas()->SetClickSelected((obj)) ;
      tot++ ;
      ui->theCanvas->GetCanvas()->cd(tot) ;
      gPad->Update();
      ui->theCanvas->GetCanvas()->Modified() ;
      ui->theCanvas->GetCanvas()->Update();


      ss_.str("") ;
      ss_ << "Working on pad " << tot ;
      STDLINE(ss_.str(),ACWhite) ;
//      sleep(1) ;
    }
  }
}

//===========================================================================
void canvasWidget::update( void )
{
  ui->theCanvas->GetCanvas()->Modified() ;
  ui->theCanvas->GetCanvas()->Update();
}

//===========================================================================
void canvasWidget::clear( )
{
  ui->theCanvas->GetCanvas()->Clear();
  ui->theCanvas->GetCanvas()->Modified() ;
  ui->theCanvas->GetCanvas()->Update();
}

//===========================================================================
void canvasWidget::resizeEvent ( QResizeEvent * re )
{
  int margin = 20 ;
  QSize newSize(re->size().width()-margin, re->size().height()-margin); // Allow room for margins
  ui->theCanvas->resize(newSize) ;
  ui->theCanvas->GetCanvas()->Modified() ;
  ui->theCanvas->GetCanvas()->Update();
}

//===========================================================================
void canvasWidget::setTitle(std::string title)
{
  STDLINE(title,ACWhite) ;
  ui->theCanvas->setWindowTitle(QApplication::translate("canvasWidget", title.c_str(), 0, QApplication::UnicodeUTF8));
}
