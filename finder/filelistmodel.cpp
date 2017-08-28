/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
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
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
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

#include "filelistmodel.h"

#include <QApplication>
#include <QBrush>
#include <QDir>
#include <QPalette>

//===========================================================================
FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

//===========================================================================
int FileListModel::rowCount(const QModelIndex & /* parent */) const
{
    return fileCount_;
}

//===========================================================================
QVariant FileListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= fileList_.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) 
    {
        return fileList_.at(index.row());          
    } 
    else if (role == Qt::BackgroundRole) 
    {
        int batch = (index.row() / 100) % 2;
        if (batch == 0)
            return qApp->palette().base();
        else
            return qApp->palette().alternateBase();
    }
    return QVariant();
}

//===========================================================================
bool FileListModel::canFetchMore(const QModelIndex & /* index */) const
{
    if (fileCount_ < fileList_.size())
        return true;
    else
        return false;
}

//===========================================================================
void FileListModel::fetchMore(const QModelIndex & /*index*/  )
{
    int remainder    = fileList_.size() - fileCount_;
    int itemsToFetch = qMin(10000, remainder);      


    beginInsertRows(QModelIndex(), fileCount_, fileCount_+itemsToFetch-1);

    fileCount_ += itemsToFetch;

    endInsertRows();

    emit numberPopulated(itemsToFetch);
}

//===========================================================================
void FileListModel::setDirPath(const QString &path)
{
    rootPath_ = path ;
    QDir dir(path);
    
    beginResetModel();
  
    fileList_.clear() ;
    QStringList tmpList = dir.entryList();
    
    for(int i=0; i<tmpList.size(); ++i)
    {
      QDir subdir(rootPath_ + QString("/") + tmpList.at(i));
      if( subdir.exists())      fileList_ << tmpList.at(i) ;
    }

    cout << __LINE__ << "] " << fileList_.join(QString(" ")).toStdString() << endl ;
    fileCount_ = 0;
    endResetModel();
}
