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
 
#ifndef CommonVariables_h
#define CommonVariables_h

#define NPLANES 10
#define ROWPITCH .100 
#define COLPITCH .150 
#define ROWEDGE  .200 
#define COLEDGE  .300 



#define XMINBOUND -8.100
#define XMAXBOUND  8.100


#define XCHIP0MIN   4.05
#define XCHIP0MAX  12.15
#define XCHIP1MIN  -4.05
#define XCHIP1MAX   4.05
#define XCHIP2MIN -12.15
#define XCHIP2MAX  -4.05
#define XCHIP3MIN -12.15
#define XCHIP3MAX  -4.05
#define XCHIP4MIN  -4.05
#define XCHIP4MAX   4.05
#define XCHIP5MIN   4.05
#define XCHIP5MAX  12.15


enum planeIDs_t         {PLANE0, PLANE1, PLANE2, PLANE3, PLANE4, PLANE5, PLANE6, PLANE7, DUT1, DUT0};
enum stationIDs_t       {STATION0, STATION1, STATION2};
enum orientationIDs_t   {XPLANE, YPLANE, DUT};


#endif
