#ifndef Window_h
#define Window_h

class TTree;
class Plane;

class Window
{
 public: 
  Window (TTree* tree=0);
  virtual ~Window(void);

  virtual bool checkWindow    (float col, float row) const = 0;
  virtual void calculateWindow(Plane& plane, TTree* tree=0) = 0;
  
 protected:
  TTree* theTree_;

}; 

#endif
