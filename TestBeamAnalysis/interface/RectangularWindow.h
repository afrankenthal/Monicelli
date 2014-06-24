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
