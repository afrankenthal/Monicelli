#include "Plane.h"

double Plane::xColChip0(double yp)  
{
  colVal = (51 - abs(yp) - (25*COLPITCH + 2*COLEDGE))/COLPITCH
  return colVal;
}

double Plane::xColChip1(double yp)
{

  colVal = (26* COLPITCH - yp)/COLPITCH;
  return colVal;
}

double Plane::xColChip2(double yp)
{
  colVal = (yp - (25*COLPITCH + 2*COLEDGE))/COLPITCH;
  return colVal;
}

double Plane::xColChip3(double yp)
{
  colVal = 
  return colVal;
}

double Plane::xColChip4(double yp)
{
  colVal = (26* COLPITCH +  abs(yp))/COLPITCH;
  return colVal;
}

double Plane::xColChip5(double yp)
{
  colVal = ( abs(yp) - (24*COLPITCH + 2*COLEDGE))/COLPITCH;
  return colVal;
}





double Plane::yColChip0(double yp)
{
  colVal = 
  return colVal;
}

double Plane::yColChip1(double yp)
{
  colVal = 
  return colVal;
}

double Plane::yColChip2(double yp)
{
  colVal = 
  return colVal;
}

double Plane::yColChip3(double yp)
{
  colVal = 
  return colVal;
}

double Plane::yColChip4(double yp)
{
  colVal = 
  return colVal;
}

double Plane::yColChip5(double yp)
{
  colVal = 
  return colVal;
}


