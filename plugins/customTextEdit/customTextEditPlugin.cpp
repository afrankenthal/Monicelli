
/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#include "customTextEdit.h"
#include "customTextEditPlugin.h"

#include <QtPlugin>

//=============================================================================
customTextEditPlugin::customTextEditPlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}
//=============================================================================
void customTextEditPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}
//=============================================================================
bool customTextEditPlugin::isInitialized() const
{
    return initialized;
}
//=============================================================================
QWidget *customTextEditPlugin::createWidget(QWidget *parent)
{
    return new customTextEdit(parent);
}
//=============================================================================
QString customTextEditPlugin::name() const
{
    return "customTextEdit";
}
//=============================================================================
QString customTextEditPlugin::group() const
{
    return "Input Widgets";
}
//=============================================================================
QIcon customTextEditPlugin::icon() const
{
    return QIcon();
}
//=============================================================================
QString customTextEditPlugin::toolTip() const
{
    return "This is a custom textedit";
}
//=============================================================================
QString customTextEditPlugin::whatsThis() const
{
    return "";
}
//=============================================================================
bool customTextEditPlugin::isContainer() const
{
    return false;
}
//=============================================================================
QString customTextEditPlugin::domXml() const
{
    return "<ui language=\"c++\" displayname=\"MyComboBox\">\n"
           " <widget class=\"customTextEdit\" name=\"customTextEdit\">\n"
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
QString customTextEditPlugin::includeFile() const
{
    return "customTextEdit.h";
}
//=============================================================================
Q_EXPORT_PLUGIN2(customTextEditPlugin, customTextEditPlugin)

