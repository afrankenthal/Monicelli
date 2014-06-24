#ifndef HistogramWindow_h
#define HistogramWindow_h

#include "Window.h"

class TTree;
class TH2F;


class HistogramWindow : public Window
{
 public: 
  HistogramWindow (TTree* tree=0);	
  ~HistogramWindow(void);

  /*bool checkWindow(double x, double y) const;*/
  bool checkWindow(float col, float row) const;
  void calculateWindow(Plane& plane, TTree* tree=0);
  
 private:
  TH2F* theHWindow_;
}; 

#endif
