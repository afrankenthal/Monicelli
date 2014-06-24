#ifndef Plane_h
#define Plane_h
#include "CommonVariables.h"

class Window;
class DataClass;

class Plane
{
  public:
  Plane(void);
  Plane(const Plane& plane);
  Plane(unsigned int planeID, int zPosition, enum orientationIDs_t orientation, double alphaAngle, double betaAngle);
  ~Plane();
  Plane& operator =(const Plane& plane);
  const unsigned int&      getPlaneID	  (void) const;
  const int&               getZPosition   (void) const;
  const double&            getXProjection (void) const;
  const double&            getYProjection (void) const;
  const double&            getDeltaXMean  (void) const;
  const double&            getDeltaYMean  (void) const;
  const Window&            getWindow	  (void) const; 	    
  void                     setupWindow    (TTree* tree);
  const double&            calcXProjection(const DataClass& theData);
  const double&            calcYProjection(const DataClass& theData);
  void                     calcProjections(const DataClass& theData);
  void                     setOrientation (enum orientationIDs_t orientation);
  enum orientationIDs_t    getOrientation (void);
  double                   getAlphaAngle  (void);
  double                   getBetaAngle   (void);
  int                      calcPixPos     (double& Xpix, double& Ypix, double xp, double yp);
  
  private:
  int                   zPosition_;
  unsigned int          planeID_;
  double                xProjection_;
  double                yProjection_;
  double                alphaAngle_;
  double                betaAngle_;
  enum orientationIDs_t orientation_;
  int 			flag_;

  Window*      planeWindow_;    // This is a handel to the window created by calling class 



  // functions for chips 
  double xColChip0(double yp);
  double xColChip1(double yp);
  double xColChip2(double yp);
  double xColChip3(double yp);
  double xColChip4(double yp);
  double xColChip5(double yp);

  double yColChip0(double yp);
  double yColChip1(double yp);
  double yColChip2(double yp);
  double yColChip3(double yp);
  double yColChip4(double yp);
  double yColChip5(double yp);

};

#endif

