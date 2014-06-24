
/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#include "customTableView.h"
#include "customTableViewPlugin.h"

#include <QtPlugin>

//=============================================================================
customTableViewPlugin::customTableViewPlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}
//=============================================================================
void customTableViewPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}
//=============================================================================
bool customTableViewPlugin::isInitialized() const
{
    return initialized;
}
//=============================================================================
QWidget *customTableViewPlugin::createWidget(QWidget *parent)
{
    return new customTableView(parent);
}
//=============================================================================
QString customTableViewPlugin::name() const
{
    return "customTableView";
}
//=============================================================================
QString customTableViewPlugin::group() const
{
    return "Input Widgets";
}
//=============================================================================
QIcon customTableViewPlugin::icon() const
{
    return QIcon();
}
//=============================================================================
QString customTableViewPlugin::toolTip() const
{
    return "This is a custom textedit";
}
//=============================================================================
QString customTableViewPlugin::whatsThis() const
{
    return "";
}
//=============================================================================
bool customTableViewPlugin::isContainer() const
{
    return false;
}
//=============================================================================
QString customTableViewPlugin::domXml() const
{
    return "<ui language=\"c++\" displayname=\"MyTableView\">\n"
           " <widget class=\"customTableView\" name=\"customTableView\">\n"
           "  <customwidgets>"
           "   <customwidget>"
           "    <property name=\"geometry\">\n"
           "     <rect>\n"
           "      <x>0</x>\n"
           "      <y>0</y>\n"
           "      <width>500</width>\n"
           "      <height>100</height>\n"
           "     </rect>\n"
           "    </property>\n"

           "    <property name=\"toolTip\" >\n"
           "     <string></string>\n"
           "    </property>\n"

           "    <property name=\"whatsThis\" >\n"
           "     <string></string>\n"
           "    </property>\n"
           "   </customwidget>"
           "  </customwidgets>"

           " </widget>\n"
           "</ui>\n";
}
//=============================================================================
QString customTableViewPlugin::includeFile() const
{
    return "customTableView.h";
}
//=============================================================================
Q_EXPORT_PLUGIN2(customTableViewPlugin, customTableViewPlugin)

