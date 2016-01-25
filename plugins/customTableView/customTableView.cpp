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
 
#include "customTableView.h"
#include "ui_customTableView.h"
#include "MessageTools.h"

#include <QtGui>
#include <iomanip>

//================================================================================
customTableView::customTableView(QWidget *parent) :
    QTableView(parent),
    ui(new Ui::customTableView)
{
    ui->setupUi(this);
    parent_ = parent ;

    model_  = NULL ;

    this->verticalHeader()->setDefaultSectionSize(20);

    columnHeaderText_.push_back("Detector") ;
    columnHeaderText_.push_back("Alfa"    ) ;
    columnHeaderText_.push_back("Beta"    ) ;
    columnHeaderText_.push_back("Gamma"   ) ;
    columnHeaderText_.push_back("X"       ) ;
    columnHeaderText_.push_back("Y"       ) ;
    columnHeaderText_.push_back("Z"       ) ;
    columnHeaderText_.push_back("Update"  ) ;

    connect( this, SIGNAL(clicked(QModelIndex)),
             this, SLOT  (clicked(QModelIndex))) ;

}

//================================================================================
customTableView::~customTableView()
{
    delete ui;
}

//================================================================================
void customTableView::clicked(QModelIndex )
{
    //  ss_.str("");

    //  if( model_->item(mIndex.row(),mIndex.column())->checkState() == 0 ) ss_ << "row " << mIndex.row() << ", column " << mIndex.column() << " is unchecked" ;
    //  if( model_->item(mIndex.row(),mIndex.column())->checkState() == 2 ) ss_ << "row " << mIndex.row() << ", column " << mIndex.column() << " is checked" ;
    //  STDLINE(ss_.str(),ACPurple) ;
}

//================================================================================
void customTableView::clearTable(void)
{
    if( model_ ) delete model_ ;
    detectors_.clear() ;
}

//================================================================================
void customTableView::addDetector(std::string detector, double zPosition)
{
    detectors_[zPosition] = detector ; // The purpose of the map is to automatically order names by zPosition
}

//================================================================================
void customTableView::fixAll(int state)
{
    if(model_ == 0) std::cout << __PRETTY_FUNCTION__ << "model_ is NULL!" << std::endl;
    for(int r=0; r<model_->rowCount(); ++r )
    {
        for( int c=1; c<model_->columnCount()-1; ++c )
        {
            QStandardItem * item = model_->item(r, c) ;
            if (item->isEnabled())
            {
                if( state == 0 )
                    item->setCheckState(Qt::Unchecked);
                else
                    item->setCheckState(Qt::Checked);
            }
        }
    }
    //  this->dump() ;
}

//================================================================================
void customTableView::fixExtremes(int state)
{
    if(model_ == 0) std::cout << __PRETTY_FUNCTION__ << "model_ is NULL!" << std::endl;
    for(int c=1; c<model_->columnCount()-1; ++c )
    {
        QStandardItem * first = model_->item(0,                    c) ;
        QStandardItem * last  = model_->item(model_->rowCount()-1, c) ;
        if( state == 0 )
        {
            if (first->isEnabled())
                first->setCheckState(Qt::Unchecked);
            if (last->isEnabled())
                last ->setCheckState(Qt::Unchecked);
        }
        else
        {
            if (first->isEnabled())
                first->setCheckState(Qt::Checked);
            if (last->isEnabled())
                last ->setCheckState(Qt::Checked);
        }
    }

    //  this->dump() ;
}

//================================================================================
void customTableView::fixAllX(int state)
{
    if(model_ == 0) std::cout << __PRETTY_FUNCTION__ << "model_ is NULL!" << std::endl;
    for(int r=0; r<model_->rowCount(); r++ )
    {
        QStandardItem * item = model_->item(r, 4) ;
        if (item->isEnabled())
        { if( state == 0 )
                item->setCheckState(Qt::Unchecked);
            else
                item->setCheckState(Qt::Checked);
        }
    }
    //  this->dump() ;
}

//================================================================================
void customTableView::fixAllY(int state)
{
    if(model_ == 0) std::cout << __PRETTY_FUNCTION__ << "model_ is NULL!" << std::endl;
    for(int r=0; r<model_->rowCount(); r++ )
    {
        QStandardItem * item = model_->item(r, 5) ;
        if (item->isEnabled())
        { if( state == 0 )
                item->setCheckState(Qt::Unchecked);
            else
                item->setCheckState(Qt::Checked);
        }
    }
    //  this->dump() ;
}

//================================================================================
void customTableView::fixAllZ(int state)
{
    if(model_ == 0) std::cout << __PRETTY_FUNCTION__ << "model_ is NULL!" << std::endl;
    for(int r=0; r<model_->rowCount(); r++ )
    {
        QStandardItem * item = model_->item(r, 6) ;
        if (item->isEnabled())
        { if( state == 0 )
                item->setCheckState(Qt::Unchecked);
            else
                item->setCheckState(Qt::Checked);
        }
    }
    //  this->dump() ;
}

//================================================================================
void customTableView::fixAllTrans(int state)
{
    if(model_ == 0) std::cout << __PRETTY_FUNCTION__ << "model_ is NULL!" << std::endl;
    for(int r=0; r<model_->rowCount(); ++r )
    {
        for( int c=4; c<=6; c++ )
        {
            QStandardItem * item = model_->item(r, c) ;
            if (item->isEnabled())
            { if( state == 0 )
                    item->setCheckState(Qt::Unchecked);
                else
                    item->setCheckState(Qt::Checked);
            }
        }
    }
    //  this->dump() ;
}

//================================================================================
void customTableView::fixAllAngles(int state)
{
    if(model_ == 0) std::cout << __PRETTY_FUNCTION__ << "model_ is NULL!" << std::endl;
    for(int r=0; r<model_->rowCount(); ++r )
    {
        for( int c=1; c<=3; c++ )
        {
            QStandardItem * item = model_->item(r, c) ;
            if (item->isEnabled())
            {
                if( state == 0 )
                    item->setCheckState(Qt::Unchecked);
                else
                    item->setCheckState(Qt::Checked);
            }
        }
    }
    //  this->dump() ;
}

//================================================================================
void customTableView::fixAllAlpha(int state)
{
    if(model_ == 0) std::cout << __PRETTY_FUNCTION__ << "model_ is NULL!" << std::endl;
    for(int r=0; r<model_->rowCount(); ++r )
    {
        QStandardItem * item = model_->item(r, 1) ;
        if (item->isEnabled())
        {
            if( state == 0 )
                item->setCheckState(Qt::Unchecked);
            else
                item->setCheckState(Qt::Checked);
        }
    }
    //  this->dump() ;
}

//================================================================================
void customTableView::fixAllBeta(int state)
{
    if(model_ == 0) std::cout << __PRETTY_FUNCTION__ << "model_ is NULL!" << std::endl;
    for(int r=0; r<model_->rowCount(); ++r )
    {
        QStandardItem * item = model_->item(r, 2) ;
        if (item->isEnabled())
        {
            if( state == 0 )
                item->setCheckState(Qt::Unchecked);
            else
                item->setCheckState(Qt::Checked);
        }
    }
    //  this->dump() ;
}
//================================================================================
void customTableView::fixAllGamma(int state)
{
    if(model_ == 0) std::cout << __PRETTY_FUNCTION__ << "model_ is NULL!" << std::endl;
    for(int r=0; r<model_->rowCount(); ++r )
    {
        QStandardItem * item = model_->item(r, 3) ;
        if (item->isEnabled())
        {
            if( state == 0 )
                item->setCheckState(Qt::Unchecked);
            else
                item->setCheckState(Qt::Checked);
        }
    }
    //  this->dump() ;
}

//================================================================================
void customTableView::enableAll(int state)
{
    if(model_ == 0) std::cout << __PRETTY_FUNCTION__ << "model_ is NULL!" << std::endl;
    for(int r=0; r<model_->rowCount(); ++r )
    {
        for( int c=1; c<=3; c++ )
        {
            QStandardItem * item = model_->item(r, c) ;
            if (!item->isEnabled())
            {
                if( state == 0 );
                else item->setEnabled(true);
            }
        }
    }
}

//================================================================================
void customTableView::fixXStrip(int state, int row)
{
    if(model_ == 0) std::cout << __PRETTY_FUNCTION__ << "model_ is NULL!" << std::endl;
    QStandardItem * trans = model_->item(row, 5) ;//set checked alt Y
    QStandardItem * angle = model_->item(row, 1) ;//set checked alt alpha
    //std::cout << "Found!!" << endl;

    if ( state == 0 )
    {
        trans->setCheckState(Qt::Unchecked);
        trans->setEnabled(true);
        angle->setCheckState(Qt::Unchecked);
        angle->setEnabled(true);
    }
    else
    {
        trans->setCheckState(Qt::Checked);
        trans->setEnabled(false);//set above enabled=false
        angle->setCheckState(Qt::Checked);
        angle->setEnabled(false);//set above enabled=false
    }
}

//================================================================================
void customTableView::fixYStrip(int state, int row)
{
    if(model_ == 0) std::cout << __PRETTY_FUNCTION__ << "model_ is NULL!" << std::endl;
    QStandardItem * trans = model_->item(row, 4) ;//set checked alt X
    QStandardItem * angle = model_->item(row, 2) ;//set checked alt beta

    if (state == 0)
    {
        trans->setCheckState(Qt::Unchecked);
        trans->setEnabled(true);
        angle->setCheckState(Qt::Unchecked);
        angle->setEnabled(true);
    }
    else
    {
        trans->setCheckState(Qt::Checked);
        trans->setEnabled(false);//set above enabled=false
        angle->setCheckState(Qt::Checked);
        angle->setEnabled(false);//set above enabled=false
    }
}

//================================================================================
void customTableView::dump(void)
{
    STDLINE(" ",ACWhite) ;
    STDLINE("------------------ Dump ------------------",ACYellow) ;
    for(std::vector<std::string>::iterator it =orderedDetectors_.begin();
        it!=orderedDetectors_.end(); ++it)
    {
        STDLINE(*it, ACPurple);
        for(std::vector<std::string>::iterator jt= columnHeaderText_.begin()+1;
            jt!=columnHeaderText_.end(); ++jt)
        {
            ss_.str("") ;
            ss_ << "   " << std::left << std::setw(6) << *jt << ": " << this->isFixed(*it,*jt) ;
            STDLINE(ss_.str(),ACWhite) ;
        }
    }
    STDLINE("",ACWhite) ;
}

//================================================================================
bool customTableView::isFixed(std::string detector, std::string parameter)
{
    if(model_ == 0) std::cout << __PRETTY_FUNCTION__ << "model_ is NULL!" << std::endl;
    for(int r=0; r<model_->rowCount(); ++r )
    {
        std::string d = model_->data(model_->index(r, 0, QModelIndex()))  .toString().toStdString() ;
        if( d != detector ) continue ;

        for(int c=1; c<model_->columnCount(); ++c )
        {
            std::string p = model_->headerData(c, Qt::Horizontal).toString().toStdString() ;
            if( p != parameter ) continue ;

            if( model_->item(r,c)->checkState() == 0 ) return false ;
            if( model_->item(r,c)->checkState() == 2 ) return true  ;
        }
    }

    return false ; // Should never reach this point...
}

//================================================================================
void customTableView::post(void)
{
    //    this->setItemDelegate(new Delegate());

    maxCols_ = (int)columnHeaderText_.size() ;
    maxRows_ = (int)detectors_.size()       ;

    model_ = new QStandardItemModel(maxRows_, maxCols_, parent_);

    for(int c=0; c<maxCols_; ++c)
    {
        model_->setHeaderData(c, Qt::Horizontal, tr(columnHeaderText_[c].c_str()));
    }

    for(std::map<double, std::string>::iterator it=detectors_.begin(); it!=detectors_.end(); ++it)
    {
        orderedDetectors_.push_back((*it).second) ;
    }

    for(int r=0; r<maxRows_; ++r)
    {
        model_->setData(model_->index(r, 0, QModelIndex()),orderedDetectors_[r].c_str());
        for(int c=1; c<maxCols_; ++c)
        {
            QStandardItem * item = new QStandardItem() ;
            item->setCheckable(true);
            model_->setItem(r, c, item);
            if( c == maxCols_ -1 ) item->setCheckState(Qt::Checked);
        }
    }

    this->setModel(model_);
    this->resizeColumnsToContents();
}

//================================================================================
//
//                     D e l e g a t e
//
//================================================================================
//QSize Delegate::sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const
//{
//    //std::stringstream ss_ ;
//    //  STDLINE(">>>>",ACYellow) ;
//    std::cout << __PRETTY_FUNCTION__ << "Calling delegate!" << std::endl;
//    std::cout << __PRETTY_FUNCTION__ << "Position" << option.decorationPosition << "Row: " << index.row() << std::endl;
//    QSize s = QItemDelegate::sizeHint(option, index);
//    std::cout << __PRETTY_FUNCTION__ << "Calling delegate!" << std::endl;
//    if( s.isValid() )
//    {
//        std::cout << __PRETTY_FUNCTION__ << "Calling delegate!" << std::endl;
//        //     ss_.str(""); ss_ << "Size: " << s.height(); STDLINE(ss_.str(),ACRed) ;
//        s.setHeight((int)(0.5*s.height()));
//        std::cout << __PRETTY_FUNCTION__ << "Calling delegate!" << std::endl;
//        //     ss_.str(""); ss_ << "Size: " << s.height(); STDLINE(ss_.str(),ACGreen) ;
//    }
//    return s;
//}
