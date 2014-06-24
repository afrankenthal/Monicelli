
/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#include "customCheckBox.h"
#include "customCheckBoxPlugin.h"

#include <QtPlugin>

//=============================================================================
customCheckBoxPlugin::customCheckBoxPlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}
//=============================================================================
void customCheckBoxPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}
//=============================================================================
bool customCheckBoxPlugin::isInitialized() const
{
    return initialized;
}
//=============================================================================
QWidget *customCheckBoxPlugin::createWidget(QWidget *parent)
{
    return new customCheckBox(parent);
}
//=============================================================================
QString customCheckBoxPlugin::name() const
{
    return "customCheckBox";
}
//=============================================================================
QString customCheckBoxPlugin::group() const
{
    return "Input Widgets";
}
//=============================================================================
QIcon customCheckBoxPlugin::icon() const
{
    return QIcon();
}
//=============================================================================
QString customCheckBoxPlugin::toolTip() const
{
    return "This is a custom CheckBox";
}
//=============================================================================
QString customCheckBoxPlugin::whatsThis() const
{
    return "";
}
//=============================================================================
bool customCheckBoxPlugin::isContainer() const
{
    return false;
}
//=============================================================================
QString customCheckBoxPlugin::domXml() const
{
    return "<ui language=\"c++\" displayname=\"MyComboBox\">\n"
           " <widget class=\"customCheckBox\" name=\"customCheckBox\">\n"
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
QString customCheckBoxPlugin::includeFile() const
{
    return "customCheckBox.h";
}
//=============================================================================
Q_EXPORT_PLUGIN2(customCheckBoxPlugin, customCheckBoxPlugin)

