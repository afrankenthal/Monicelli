#include "settingsmanager.h"
#include "MessageTools.h"
#include "maintabs.h"
#include "ui_maintabs.h"

//=====================================================================
settingsManager::settingsManager(Ui::mainTabs * ui)
{
    ui_ = ui ;
}
//=====================================================================
void settingsManager::save(void)
{
    STDLINE(ui_->loadedFileLE->text().toStdString(),ACWhite) ;
}
