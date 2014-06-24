/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
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
