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
 
#ifndef _ANSICOLORS_H
#define _ANSICOLORS_H

/*! \file ANSIColors.h
 *  \brief Header file containing macros to highlight output lines with color.
 *
 *  Check also <A HREF="http://www.bluesock.org/~willg/dev/ansi.html">http://www.bluesock.org/~willg/dev/ansi.html</A>
 *
 *  Below is a usage example:
 *  <p><b>
 *  \code
 *   #include <ANSIColors.h>"
 *   ...
 *   cout << ACRed << ACBold << "This text will be highlighted in red" << ACPlain << endl ;
 *  \endcode
 *   </b>
 */

#define ACBlack       "\e[0;30m"
#define ACBlue        "\e[0;34m"
#define ACGreen       "\e[0;32m"
#define ACCyan        "\e[0;36m"
#define ACRed         "\e[0;31m"
#define ACPurple      "\e[0;35m"
#define ACBrown       "\e[0;33m"
#define ACGray        "\e[0;37m"
#define ACDarkGray    "\e[1;30m"
#define ACLightBlue   "\e[1;34m"
#define ACLightGreen  "\e[1;32m"
#define ACLightCyan   "\e[1;36m"
#define ACLightRed    "\e[1;31m"
#define ACLightPurple "\e[1;35m"
#define ACYellow      "\e[1;33m"
#define ACWhite       "\e[1;37m"

#define ACPlain       "\e[0m"
#define ACBold        "\e[1m"
#define ACUnderline   "\e[4m"
#define ACBlink       "\e[5m"
#define ACReverse     "\e[7m"

#define ACClear       "\e[2J"
#define ACClearL      "\e[2K"

#define ACSave        "\e[s"
#define ACRecall      "\e[u"

#endif // _ANSICOLORS_H
