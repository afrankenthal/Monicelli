/*===============================================================================
 * Monicelli: the FERMILAB MTEST geometry builder and track reconstruction tool
 * 
 * Copyright (C) 2014 
 *
 * Authors:
 *
 * Dario Menasce      (INFN) 
 * Luigi Moroni       (INFN)
 * Jennifer Ngadiuba  (INFN)
 * Stefano Terzo      (INFN)
 * Lorenzo Uplegger   (FNAL)
 * Luigi Vigani       (INFN)
 *
 * INFN: Piazza della Scienza 3, Edificio U2, Milano, Italy 20126
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ================================================================================*/
 
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
//Q_EXPORT_PLUGIN2(customTextEditPlugin, customTextEditPlugin)

