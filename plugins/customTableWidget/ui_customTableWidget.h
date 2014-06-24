/********************************************************************************
** Form generated from reading UI file 'customTableWidget.ui'
**
** Created: Fri May 16 10:43:16 2014
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CUSTOMTABLEWIDGET_H
#define UI_CUSTOMTABLEWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QTableWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_customTableWidget
{
public:
    QTableWidget *tableWidget;

    void setupUi(QWidget *customTableWidget)
    {
        if (customTableWidget->objectName().isEmpty())
            customTableWidget->setObjectName(QString::fromUtf8("customTableWidget"));
        customTableWidget->resize(200, 100);
        tableWidget = new QTableWidget(customTableWidget);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(0, 0, 200, 100));

        retranslateUi(customTableWidget);

        QMetaObject::connectSlotsByName(customTableWidget);
    } // setupUi

    void retranslateUi(QWidget *customTableWidget)
    {
        customTableWidget->setWindowTitle(QApplication::translate("customTableWidget", "customSpinBox", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class customTableWidget: public Ui_customTableWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUSTOMTABLEWIDGET_H
