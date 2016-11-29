#include "extractor.h"
#include <helpers.h>
#include <indent.h>

#include <TFile.h>
#include <TH1.h>
#include <TKey.h>

#include <QString>
#include <QFileDialog>

#include <iostream>
#include <string>
#include <cmath>

using namespace std;

//==================================================================================
Extractor::Extractor()
{
    level_ = 0;
}

//==================================================================================
QString Extractor::getFileName()
{
    return QFileDialog::getOpenFileName(NULL, "Open File",
                                        "/data/TestBeam/2016_06_June",
                                        "ROOTFile (*.root)"
                                        );
}

//==================================================================================
void Extractor::searchFile(TDirectory *dir, string folder, string title)

{
    TDirectory *dirsav = gDirectory;
    TIter next(dir->GetListOfKeys());
    string thisDir = gDirectory->GetName() ;
    TKey *key;
    while ((key = (TKey*)next()))
    {
        string thisPlot = key->GetName() ;

        plotName_<<thisPlot<<endl;

        if(key->IsFolder())
        {
            level_++;
            tabs_    = blank_.writeBlank(level_);
            thisPlot = tabs_ + thisPlot ;
            ss_ << thisPlot <<'\t'<<key->ReadObj()->ClassName()<< endl ;
        }
        else
        {
            thisPlot = tabs_ + blank_.getABlank() + thisPlot ;
            ss_ << thisPlot <<'\t'<<key->ReadObj()->ClassName()<< endl ;
        }

        thisDir = thisPlot = "" ;

        if (key->IsFolder())
        {
            dir->cd(key->GetName());
            TDirectory *subdir = gDirectory;
            searchFile(subdir,folder,title);
            dirsav->cd();
            continue;
        }
    }

    level_-- ;
}

//==================================================================================
void Extractor::printMap(void)
{
    cout << ss_.str() << endl ;
}

//==================================================================================
QByteArray Extractor::getByteArray(void)
{
    QByteArray qArray;
    for(unsigned int i=0; i<ss_.str().size(); ++i)
    {
        if(foldersInFile_)qArray.append(ss_.str()[i]) ;
        else qArray.append(plotName_.str()[i]);
    }
    return qArray ;
}
