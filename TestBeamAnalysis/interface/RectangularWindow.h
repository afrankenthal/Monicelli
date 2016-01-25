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
 
#ifndef RectangularWindow_h
#define RectangularWindow_h

#include "Window.h"

class TH1F;
class TTree;

class RectangularWindow : public Window
{
 public: 
  RectangularWindow (TTree* tree = 0);	
  ~RectangularWindow(void);

  bool checkWindow(float col, float row) const;
  void calculateWindow(Plane& plane, TTree* tree=0);


  void  setSize(double lowLeftX, double lowLeftY, double upRightX, double upRightY);
  void  setSize(const TH1F* hXBound, const TH1F* hYBound);
  void  setSize(const TH1F& hXBound, const TH1F& hYBound);
  
  const double& getLLX();
  const double& getLLY();
  const double& getURX();
  const double& getURY();
  const double  setMaskWidth(double width);
  const double  setOffset   (double offset);

 private:
  double lowLeftX_;
  double lowLeftY_;
  double upRightX_;
  double upRightY_;
}; 

#endif
