/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#include "customTableWidget.h"
#include "customTableWidgetPlugin.h"

#include <QtPlugin>

//=============================================================================
customTableWidgetPlugin::customTableWidgetPlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}
//=============================================================================
void customTableWidgetPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}
//=============================================================================
bool customTableWidgetPlugin::isInitialized() const
{
    return initialized;
}
//=============================================================================
QWidget *customTableWidgetPlugin::createWidget(QWidget *parent)
{
    return new customTableWidget(parent);
}
//=============================================================================
QString customTableWidgetPlugin::name() const
{
    return "customTableWidget";
}
//=============================================================================
QString customTableWidgetPlugin::group() const
{
    return "Input Widgets";
}
//=============================================================================
QIcon customTableWidgetPlugin::icon() const
{
    return QIcon();
}
//=============================================================================
QString customTableWidgetPlugin::toolTip() const
{
    return "This is a custom TableWidget";
}
//=============================================================================
QString customTableWidgetPlugin::whatsThis() const
{
    return "";
}
//=============================================================================
bool customTableWidgetPlugin::isContainer() const
{
    return false;
}
//=============================================================================
QString customTableWidgetPlugin::domXml() const
{
    return "<ui language=\"c++\" displayname=\"MyTableWidget\">\n"
           " <widget class=\"customTableWidget\" name=\"customTableWidget\">\n"
           "  <customwidgets>"
           "   <customwidget>"
           "    <property name=\"geometry\">\n"
           "     <rect>\n"
           "      <x>0</x>\n"
           "      <y>0</y>\n"
           "      <width>200</width>\n"
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
QString customTableWidgetPlugin::includeFile() const
{
    return "customTableWidget.h";
}
//=============================================================================
Q_EXPORT_PLUGIN2(customTableWidgetPlugin, customTableWidgetPlugin)


