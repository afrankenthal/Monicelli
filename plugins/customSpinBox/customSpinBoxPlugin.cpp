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


