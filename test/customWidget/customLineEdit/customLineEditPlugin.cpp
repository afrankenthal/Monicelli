#include "customLineEdit.h"
#include "customLineEditPlugin.h"

#include <QtPlugin>

//=============================================================================
customLineEditPlugin::customLineEditPlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}
//=============================================================================
void customLineEditPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}
//=============================================================================
bool customLineEditPlugin::isInitialized() const
{
    return initialized;
}
//=============================================================================
QWidget *customLineEditPlugin::createWidget(QWidget *parent)
{
    return new customLineEdit(parent);
}
//=============================================================================
QString customLineEditPlugin::name() const
{
    return "customLineEdit";
}
//=============================================================================
QString customLineEditPlugin::group() const
{
    return "Input Widgets";
}
//=============================================================================
QIcon customLineEditPlugin::icon() const
{
    return QIcon();
}
//=============================================================================
QString customLineEditPlugin::toolTip() const
{
    return "This is a custom lineedit";
}
//=============================================================================
QString customLineEditPlugin::whatsThis() const
{
    return "";
}
//=============================================================================
bool customLineEditPlugin::isContainer() const
{
    return false;
}
//=============================================================================
QString customLineEditPlugin::domXml() const
{
    return "<ui language=\"c++\" displayname=\"MyComboBox\">\n"
           " <widget class=\"customLineEdit\" name=\"customLineEdit\">\n"
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
           "     <string>The current time</string>\n"
           "    </property>\n"

           "    <property name=\"whatsThis\" >\n"
           "     <string>The analog clock widget displays the current time.</string>\n"
           "    </property>\n"
           "   </customwidget>"
           "  </customwidgets>"

           " </widget>\n"
           "</ui>\n";
}
//=============================================================================
QString customLineEditPlugin::includeFile() const
{
    return "customLineEdit.h";
}
//=============================================================================
Q_EXPORT_PLUGIN2(customLineEditPlugin, customLineEditPlugin)

