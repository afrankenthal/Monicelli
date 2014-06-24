/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#include "customSpinBox.h"
#include "customSpinBoxPlugin.h"

#include <QtPlugin>

//=============================================================================
customSpinBoxPlugin::customSpinBoxPlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}
//=============================================================================
void customSpinBoxPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}
//=============================================================================
bool customSpinBoxPlugin::isInitialized() const
{
    return initialized;
}
//=============================================================================
QWidget *customSpinBoxPlugin::createWidget(QWidget *parent)
{
    return new customSpinBox(parent);
}
//=============================================================================
QString customSpinBoxPlugin::name() const
{
    return "customSpinBox";
}
//=============================================================================
QString customSpinBoxPlugin::group() const
{
    return "Input Widgets";
}
//=============================================================================
QIcon customSpinBoxPlugin::icon() const
{
    return QIcon();
}
//=============================================================================
QString customSpinBoxPlugin::toolTip() const
{
    return "This is a custom SpinBox";
}
//=============================================================================
QString customSpinBoxPlugin::whatsThis() const
{
    return "";
}
//=============================================================================
bool customSpinBoxPlugin::isContainer() const
{
    return false;
}
//=============================================================================
QString customSpinBoxPlugin::domXml() const
{
    return "<ui language=\"c++\" displayname=\"MySpinBox\">\n"
           " <widget class=\"customSpinBox\" name=\"customSpinBox\">\n"
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
QString customSpinBoxPlugin::includeFile() const
{
    return "customSpinBox.h";
}
//=============================================================================
Q_EXPORT_PLUGIN2(customSpinBoxPlugin, customSpinBoxPlugin)


