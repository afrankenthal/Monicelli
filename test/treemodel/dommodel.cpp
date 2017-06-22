/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QFont>
#include <QtGui>
#include <QtXml>


#include <iostream>
#include "domitem.h"
#include "dommodel.h"

using namespace std ;
//===========================================================================
DomModel::DomModel(QDomDocument document, QObject *parent)
    : QAbstractItemModel(parent), domDocument(document)
{
    rootItem = new DomItem(domDocument, 0);
}

//===========================================================================
DomModel::~DomModel()
{
    delete rootItem;
}

//===========================================================================
int DomModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 3;
}

//===========================================================================
QVariant DomModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole    && 
        role != Qt::ForegroundRole &&
        role != Qt::FontRole         )
        return QVariant();

    if (     index.isValid() && role == Qt::ForegroundRole )
    {
        if ( index.column() == 1 )
        {
            return QVariant( QColor( Qt::red ) );
        }
        return QVariant( QColor( Qt::black ) );
    }
    else if (index.isValid() && role == Qt::FontRole && index.column() == 1 )
    {
        QFont font;
        font.setBold(true);
        return font;
    }
    
    DomItem *item = static_cast<DomItem*>(index.internalPointer());

    QDomNode         node           = item->node()     ;
    QStringList      attributes                        ;
    QDomNamedNodeMap attributeMap   = node.attributes();
    QString          attributeValue                    ;
    
    switch (index.column()) 
    {
        case 0:
            {
             QString n = node.nodeName();
             if( n == "station" )
             {
              n = QString("Station Id: ") + 
                  this->getAttribute("id", attributeMap);
             }
             if( n == "detector" )
             {
              n = QString("Detector Id: ") + 
                  this->getAttribute("id", attributeMap);
             }
             return QVariant(n);
            }
        case 1:
            for (int i = 0; i < attributeMap.count(); ++i) 
            {
                QDomNode attribute = attributeMap.item(i);
                if(attribute.nodeName()  == "used" &&
                   attribute.nodeValue() == "yes"    )
                {
                   attributes << "Used";
                }            
            }
            return attributes.join(" ");
        case 2:
            return node.nodeValue().split("\n").join(" ");
        default:
            return QVariant();
    }
}

//===========================================================================
QString DomModel::getAttribute(QString attribute, 
                               QDomNamedNodeMap &attributeMap) const
{
    QString found = QString("") ;
    for (int i = 0; i < attributeMap.count(); ++i) 
    {
     QDomNode attributeItem = attributeMap.item(i);
     if(attributeItem.nodeName() == attribute)
     {
      found = attributeItem.nodeValue();
     }
    }
    return found ;
}
//===========================================================================
Qt::ItemFlags DomModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//===========================================================================
QVariant DomModel::headerData(int section, Qt::Orientation orientation,
                              int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) 
    {
        switch (section) 
        {
            case 0:
                return tr("Node Name"      );
            case 1:
                return tr("Node Attributes");
            case 2:
                return tr("Node Value"     );
            default:
                return QVariant();
        }
    }

    return QVariant();
}

//===========================================================================
QModelIndex DomModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    DomItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());
    DomItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

//===========================================================================
QModelIndex DomModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    DomItem *childItem = static_cast<DomItem*>(child.internalPointer());
    DomItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

//===========================================================================
int DomModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    DomItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());

    return parentItem->node().childNodes().count();
}
