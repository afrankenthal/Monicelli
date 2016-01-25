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
 
#include <iostream>
#include <sstream>
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
//    canvas_ = new QRootCanvas(ui->theCanvas,"") ;
    canvas_ = new QRootCanvas(this,"") ;
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
//  canvas_ = new QRootCanvas(canvas_,"") ;

  canvas_->GetCanvas()->Clear() ;
  canvas_->GetCanvas()->Divide(nx, ny, 0.001, 0.001) ;
  canvas_->GetCanvas()->Modified() ;
  canvas_->GetCanvas()->Update();
}

//===========================================================================
void canvasWidget::cd( int pos )
{
  canvas_->GetCanvas()->cd(pos) ;
}

//===========================================================================
void canvasWidget::flush( void )
{
  int tot = 0 ;
  TObject * obj = NULL ;
  TIter next(canvas_->GetCanvas()->GetListOfPrimitives()) ;
  while((obj = next()))
  {
    if( obj->InheritsFrom(TVirtualPad::Class()))
    {
      canvas_->GetCanvas()->SetClickSelectedPad((TPad*)(obj)) ;
      canvas_->GetCanvas()->SetClickSelected((obj)) ;
      tot++ ;
      canvas_->GetCanvas()->cd(tot) ;
      gPad->Update();
      canvas_->GetCanvas()->Modified() ;
      canvas_->GetCanvas()->Update();

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
  canvas_->GetCanvas()->Modified() ;
  canvas_->GetCanvas()->Update();
}

//===========================================================================
void canvasWidget::clear( )
{
  canvas_->GetCanvas()->Clear();
  canvas_->GetCanvas()->Modified() ;
  canvas_->GetCanvas()->Update();
}

//===========================================================================
void canvasWidget::resizeEvent ( QResizeEvent * re )
{
  int margin = 0 ;
  QSize newSize(re->size().width()-margin, re->size().height()-margin); // Allow room for margins
  canvas_->resize(newSize) ;
  canvas_->GetCanvas()->Modified() ;
  canvas_->GetCanvas()->Update();
}

//===========================================================================
void canvasWidget::setTitle(std::string title)
{
  STDLINE(title,ACWhite) ;
  canvas_->setWindowTitle(QApplication::translate("canvasWidget", title.c_str(), 0, QApplication::UnicodeUTF8));
}
