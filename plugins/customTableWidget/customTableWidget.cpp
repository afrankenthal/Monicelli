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

#include "customTableWidget.h"
#include "ui_customTableWidget.h"
#include <QTableWidgetItem>
#include <QStandardItemModel>
#include "MessageTools.h"

//================================================================================
customTableWidget::customTableWidget(QWidget *parent) :
    QTableView(parent)
  , ui(new Ui::customTableWidget)
  , theParent_(parent)
{
    ui->setupUi(this);
    connect(ui->tableWidget, SIGNAL(cellChanged(int,int)),
            this,            SLOT  (setNodeValue(int,int))) ;
}

//================================================================================
customTableWidget::~customTableWidget()
{
    delete ui;
}

//================================================================================
void customTableWidget::init(void)
{
    ui->tableWidget->setGeometry(0, 0, geometry().width(), geometry().height()) ;
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(this->verticalHeader()->defaultSectionSize());
}

//================================================================================
void customTableWidget::assignXmlElement(QDomNodeList&  element)
{
    STDLINE("",ACRed);
    elementNode_ = element;
    std::map<QString,int> headerMap;
    int cols = ui->tableWidget->columnCount();
    int rows = elementNode_.size();
    for(int c=0; c<cols; c++)
    {
        headerMap[ui->tableWidget->horizontalHeaderItem(c)->text()] = c;
    }
    ui->tableWidget->setRowCount(rows);
    ui->tableWidget->setColumnCount(cols);

    for(int r=0; r<rows; r++)
    {
        QDomNamedNodeMap attributes = elementNode_.at(r).attributes();
        for(int c=0; c<attributes.size(); c++)
        {
            QString name = attributes.item(c).nodeName();
            if(headerMap.find(attributes.item(c).nodeName()) != headerMap.end())
                ui->tableWidget->setItem(r,headerMap[name],new QTableWidgetItem(elementNode_.at(r).toElement().attribute(name)));
        }
    }

    ui->tableWidget->resizeColumnsToContents();
}

//================================================================================
int customTableWidget::columnCount(void) const
{
    return ui->tableWidget->columnCount();
}

//================================================================================
void customTableWidget::setColumnCount(int col)
{
    ui->tableWidget->setColumnCount(col);
}

//================================================================================
void customTableWidget::setHorizontalHeaderItem(int col, QTableWidgetItem * item)
{
    ui->tableWidget->setHorizontalHeaderItem(col, item);
}

//================================================================================
QTableWidgetItem* customTableWidget::horizontalHeaderItem(int col) const
{
    return ui->tableWidget->horizontalHeaderItem(col);
}

//================================================================================
void customTableWidget::setEnabled(int enable)
{
    ui->tableWidget->setEnabled((bool)enable);
}

//================================================================================
void customTableWidget::setEnabled(bool enable)
{
    ui->tableWidget->setEnabled(enable);
}

//================================================================================
void customTableWidget::setNodeValue(int row, int col)
{
    //    std::stringstream ss;
    //    ss << "r: " << row << " c: " << col << " val: " << ui->tableWidget->item(row,col)->text().toStdString();
    //    STDLINE(ss.str(),ACRed);
    QString name  = ui->tableWidget->horizontalHeaderItem(col)->text();
    QString value = ui->tableWidget->item(row,col)->text();
    elementNode_.at(row).toElement().setAttribute(name,value);
}
