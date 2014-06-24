/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef MDICHILD_H
#define MDICHILD_H

#include <QTextEdit>

class MdiChild : public QTextEdit
{
    Q_OBJECT

public:
    MdiChild();

    bool    loadFile               (const QString  & fileName)   ;
    QString currentFile            ()                             { return curFile; } 
    void    newFile                ()                            ;            
    bool    save                   ()                            ;            
    bool    saveAs                 ()                            ;            
    bool    saveFile               (const QString  & fileName)   ;
    QString userFriendlyCurrentFile()                            ;

protected:
    void    closeEvent             (QCloseEvent    * event)      ;

private slots:
    void    documentWasModified    ()                            ;

private:
    bool    maybeSave              ()                            ;
    void    setCurrentFile         (const QString & fileName)    ;
    QString strippedName           (const QString & fullFileName);

    QString curFile   ;
    bool    isUntitled;
};

#endif
