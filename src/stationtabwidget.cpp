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
 
#include "MessageTools.h"
#include "stationtabwidget.h"
#include "ui_stationtabwidget.h"
#include "xmlParser.h"
#include <QTabBar>
#include <QString>
#include <QTextStream>

//===========================================================================
stationTabWidget::stationTabWidget(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::stationTabWidget)
{
    ui->setupUi(this);

    this->setMovable(true) ;

    QTabBar * theTabBar = this->tabBar () ;

    connect(theTabBar, SIGNAL(tabMoved        (int,int)),
            this,      SLOT  (reassignPosition(int,int))) ;
}

//===========================================================================
stationTabWidget::~stationTabWidget()
{
    delete ui;
}

//===========================================================================
void stationTabWidget::remTab(int stationSerial)
{
  ss_.str(""); ss_ << "Station " <<  stationSerial;
  for(int i=0; i<this->count(); ++i)
  {
    if( this->tabText(i).toStdString() == ss_.str() ) this->removeTab(i) ;
  }
}

//===========================================================================
void stationTabWidget::setNode(int        stationSerial,
                               QDomNode & stationNode  )
{
  ss_.str(""); ss_ << "Station " << stationSerial ;
  std::string label = ss_.str() ;

  stationMap_[label] = stationNode;
}

//===========================================================================
void stationTabWidget::reassignPosition(int from, int to)
{
   int direction = from - to ;

   std::string tabLabelFrom = this->tabText(from).toStdString() ;
   std::string tabLabelTo   = this->tabText(to  ).toStdString() ;

   QDomNode parentNode = stationMap_[tabLabelFrom].parentNode() ;
//   xmlUtils::dumpFragment(parentNode,"children","headers") ;

   if( direction > 0 ) // Insert after
   {
     parentNode.insertAfter(stationMap_[tabLabelFrom],stationMap_[tabLabelTo]) ;
   }
   else                // Insert before
   {
     parentNode.insertBefore(stationMap_[tabLabelFrom],stationMap_[tabLabelTo]) ;
   }

//   xmlUtils::dumpFragment(parentNode,"children","headers") ;
 }

