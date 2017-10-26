#include "settingsmanager.h"
#include "MessageTools.h"
#include "maintabs.h"
#include "ui_maintabs.h"
#include <iomanip>
#include <ctime>
#include <QRegularExpression>

//=====================================================================
settingsManager::settingsManager(QWidget      * mainTabs,
                                 Ui::mainTabs * ui      )
{
    mainTabs_ = mainTabs ;
    ui_       = ui ;
}
//=====================================================================
void settingsManager::read(QString configuration)
{
    QSettings settings("CMS", configuration);

    QList<QLineEdit      *> lineEdits = mainTabs_ ->findChildren<QLineEdit      *>();
    QList<QCheckBox      *> checkBoxs = mainTabs_ ->findChildren<QCheckBox      *>();
    QList<QComboBox      *> comboBoxs = mainTabs_ ->findChildren<QComboBox      *>();
    QList<QSpinBox       *> spinBoxes = mainTabs_ ->findChildren<QSpinBox       *>();
    QList<QDoubleSpinBox *> spinDBoxs = mainTabs_ ->findChildren<QDoubleSpinBox *>();

    bool restore = false ;
    bool dump    = true ;
    if( settings.value("Initialized") == "true") restore = true ;

    if(dump) {ss_.str(""); ss_ << "Restoring settings from " << configuration.toStdString() ; STDLINE(ss_.str(),ACGreen) ;}

    if(dump) {ss_.str(""); ss_ << "Number of lineEdits: " << lineEdits.size() ; STDLINE(ss_.str(),ACCyan) ;{
            for (int i = 0; i < lineEdits.size(); ++i)
            {
                if( lineEdits.at(i)->objectName().toStdString() == "qt_spinbox_lineedit") continue ; // A probable Qt bug...
                if( restore ) lineEdits.at(i)->setText       (settings.value(lineEdits.at(i)->objectName()).toString()) ;
                if( dump    )
                {
                    ss_.str("") ; ss_ << "lineEdits: "
                                      << setw(30)
                                      << lineEdits.at(i)->objectName().toStdString()
                                      << "\t--> "
                                      << settings.value(lineEdits.at(i)->objectName()).toString().toStdString()
                                      << " |";
                    STDLINE(ss_.str(),ACWhite) ;
                }
            }
            if(dump) {ss_.str(""); ss_ << "Number of checkBoxes: " << checkBoxs.size() ; STDLINE(ss_.str(),ACCyan) ;}
            for (int i = 0; i < checkBoxs.size(); ++i)
            {
                if( restore ) checkBoxs.at(i)->setChecked    (settings.value(checkBoxs.at(i)->objectName()).toBool()  ) ;
                if( dump    )
                {
                    ss_.str("") ; ss_ << "checkBox: "
                                      << setw(30)
                                      << checkBoxs.at(i)->objectName().toStdString()
                                      << "\t--> "
                                      << settings.value(checkBoxs.at(i)->objectName()).toString().toStdString()
                                      << " |" ;
                    STDLINE(ss_.str(),ACWhite) ;
                }
            }
            if(dump) {ss_.str(""); ss_ << "Number of comboBoxes: " << comboBoxs.size() ; STDLINE(ss_.str(),ACCyan) ;}
            for (int i = 0; i < comboBoxs.size(); ++i)
            {
                if( restore ) comboBoxs.at(i)->setCurrentText(settings.value(comboBoxs.at(i)->objectName()).toString()) ;
                if( dump    )
                {
                    ss_.str("") ; ss_ << "comboBox: "
                                      << setw(30)
                                      << comboBoxs.at(i)->objectName().toStdString()
                                      << "\t--> "
                                      << settings.value(comboBoxs.at(i)->objectName()).toString().toStdString()
                                      << " |" ;
                    STDLINE(ss_.str(),ACWhite) ;
                }
            }
            if(dump) ss_.str(""); ss_ << "Number of spinBoxes: " << spinBoxes.size() ; STDLINE(ss_.str(),ACCyan) ;}
        for (int i = 0; i < spinBoxes.size(); ++i)
        {
            if( restore ) spinBoxes.at(i)->setValue      (settings.value(spinBoxes.at(i)->objectName()).toInt()   ) ;
            if( dump    )
            {
                ss_.str("") ; ss_ << "spinBoxes: "
                                  << setw(30)
                                  << spinBoxes.at(i)->objectName().toStdString()
                                  << "\t--> "
                                  << settings.value(spinBoxes.at(i)->objectName()).toString().toStdString()
                                  << " |" ;
                STDLINE(ss_.str(),ACWhite) ;
            }
        }
        if(dump) ss_.str(""); ss_ << "Number of spinDBoxes: " << spinDBoxs.size() ; STDLINE(ss_.str(),ACCyan) ;}
    for (int i = 0; i < spinDBoxs.size(); ++i)
    {
        if( restore ) spinDBoxs.at(i)->setValue      (settings.value(spinDBoxs.at(i)->objectName()).toInt()   ) ;
        if( dump    )
        {
            ss_.str("") ; ss_ << "spinBox: "
                              << setw(30)
                              << spinDBoxs.at(i)->objectName().toStdString()
                              << "\t--> "
                              << settings.value(spinDBoxs.at(i)->objectName()).toString().toStdString()
                              << " |" ;
            STDLINE(ss_.str(),ACWhite) ;
        }
    }

    if( ui_->loadedFileLE->text().toStdString() != "No file selected")
    {
        ui_->parseFilePB   ->setEnabled(true) ;
        ui_->maxRawEventsCB->setEnabled(true) ;
    }

    ui_->settingsLE->setText(configuration) ;
}
//=====================================================================
void settingsManager::save(QString configuration)
{
    QSettings settings("CMS", configuration);

    QList<QLineEdit      *> lineEdits = mainTabs_ ->findChildren<QLineEdit      *>();
    QList<QCheckBox      *> checkBoxs = mainTabs_ ->findChildren<QCheckBox      *>();
    QList<QComboBox      *> comboBoxs = mainTabs_ ->findChildren<QComboBox      *>();
    QList<QSpinBox       *> spinBoxes = mainTabs_ ->findChildren<QSpinBox       *>();
    QList<QDoubleSpinBox *> spinDBoxs = mainTabs_ ->findChildren<QDoubleSpinBox *>();

    settings.setValue("Initialized","true") ;

//    ss_.str(""); ss_ << "Number of lineEdits: " << lineEdits.size() ; STDLINE(ss_.str(),ACCyan) ;
    for (int i = 0; i < lineEdits.size(); ++i)
    {
        if( lineEdits.at(i)->objectName().toStdString() == "qt_spinbox_lineedit") continue ; // A probable Qt bug...
        settings.setValue(lineEdits.at(i)->objectName(), lineEdits.at(i)->text()       ) ;
//        ss_.str("") ; ss_ << "lineEdits: "
//                          << setw(30)
//                          << lineEdits.at(i)->objectName().toStdString()
//                          << "\t--> "
//                          << lineEdits.at(i)->text().toStdString() ;
//        STDLINE(ss_.str(),ACWhite) ;
    }
//    ss_.str(""); ss_ << "Number of checkBoxs: " << checkBoxs.size() ; STDLINE(ss_.str(),ACCyan) ;
    for (int i = 0; i < checkBoxs.size(); ++i)
    {
        settings.setValue(checkBoxs.at(i)->objectName(), checkBoxs.at(i)->isChecked()  ) ;
//        ss_.str("") ; ss_ << "checkBoxs: "
//                          << setw(30)
//                          << checkBoxs.at(i)->objectName().toStdString()
//                          << "\t--> "
//                          << checkBoxs.at(i)->isChecked() ;
//        STDLINE(ss_.str(),ACWhite) ;
    }
//    ss_.str(""); ss_ << "Number of comboBoxs: " << comboBoxs.size() ; STDLINE(ss_.str(),ACCyan) ;
    for (int i = 0; i < comboBoxs.size(); ++i)
    {
        settings.setValue(comboBoxs.at(i)->objectName(), comboBoxs.at(i)->currentText()) ;
//        ss_.str("") ; ss_ << "checkBoxs: "
//                          << setw(30)
//                          << comboBoxs.at(i)->objectName().toStdString()
//                          << "\t--> "
//                          << comboBoxs.at(i)->currentText().toStdString() ;
//        STDLINE(ss_.str(),ACWhite) ;
    }
//    ss_.str(""); ss_ << "Number of spinBoxes: " << spinBoxes.size() ; STDLINE(ss_.str(),ACCyan) ;
    for (int i = 0; i < spinBoxes.size(); ++i)
    {
        settings.setValue(spinBoxes.at(i)->objectName(), spinBoxes.at(i)->value()      ) ;
//        ss_.str("") ; ss_ << "spinBoxes: "
//                          << setw(30)
//                          << spinBoxes.at(i)->objectName().toStdString()
//                          << "\t--> "
//                          << spinBoxes.at(i)->value() ;
//        STDLINE(ss_.str(),ACWhite) ;
    }
//    ss_.str(""); ss_ << "Number of spinDBoxs: " << spinDBoxs.size() ; STDLINE(ss_.str(),ACCyan) ;
    for (int i = 0; i < spinDBoxs.size(); ++i)
    {
        settings.setValue(spinDBoxs.at(i)->objectName(), spinDBoxs.at(i)->value()      ) ;
//        ss_.str("") ; ss_ << "spinDBoxs: "
//                          << setw(30)
//                          << spinDBoxs.at(i)->objectName().toStdString()
//                          << "\t--> "
//                          << spinDBoxs.at(i)->value() ;
//        STDLINE(ss_.str(),ACWhite) ;
    }

    string HOME = getenv("HOME") ;
    QFile qsettingsFile(QString(HOME.c_str())+QString("/.config/CMS/MonicelliDefaults.conf")) ;
    if( qsettingsFile.exists() )
    {
        QString fileName = qsettingsFile.fileName() ;
        fileName.replace(QRegularExpression("\\.conf$"),QString("")) ;
        time_t t = time(0);   // get time now
        struct tm * now = localtime( & t );
        ss_.str("");
        ss_ << "cp "
            << fileName.toStdString()
            << ".conf"
            << " "
            << fileName.toStdString()
            << "_"
            << now->tm_mday
            << '-'
            << (now->tm_mon + 1)
            << '-'
            <<  (now->tm_year + 1900)
            << "_"
            <<  now->tm_hour
            << ":"
            <<  now->tm_min
            << ":"
            <<  now->tm_sec
            << ".conf";
        STDLINE("A copy of the current GUI values has been saved in $HOME/.config/CMS",ACYellow) ;
        system(ss_.str().c_str()) ;
    }
}
