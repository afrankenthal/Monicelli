#include <iostream>
#include <cmath>
#include "DataClass.h"
#include "Plane.h"
#include "RectangularWindow.h"
#include "HistogramWindow.h"


using namespace std;

/*-------------------------------------------------------------------------------------------------------------------*/

Plane::Plane()
     : zPosition_  (0)                    ,planeID_    (0)    
     , xProjection_(0)                    ,yProjection_(0)    
     , alphaAngle_  (0)	                  ,betaAngle_ (0)
     , orientation_(XPLANE)  	     
     , planeWindow_((true)?(Window*)(new HistogramWindow):(Window*)(new RectangularWindow))
{

}

/*-------------------------------------------------------------------------------------------------------------------*/
Plane::Plane(const Plane& plane)
     : zPosition_  (plane.zPosition_)	 ,planeID_    (plane.planeID_)			    
     , xProjection_(plane.xProjection_)  ,yProjection_(plane.yProjection_)		   			      
     , alphaAngle_ (plane.alphaAngle_)	 ,betaAngle_  (plane.betaAngle_)
     , orientation_(plane.orientation_)							   			      
     , planeWindow_((true)?(Window*)(new HistogramWindow):(Window*)(new RectangularWindow))			       
{
  *planeWindow_ = *(plane.planeWindow_);
}

/*-------------------------------------------------------------------------------------------------------------------*/
Plane::Plane(unsigned int planeID, int zPosition, enum orientationIDs_t orientation, double alphaAngle, double betaAngle)
     : zPosition_(zPosition)             ,planeID_(planeID)
     , xProjection_(0)			 ,yProjection_(0)
     , alphaAngle_(alphaAngle)           ,betaAngle_(betaAngle)
     , orientation_(orientation) 	 	  
     , planeWindow_((true)?(Window*)(new HistogramWindow):(Window*)(new RectangularWindow))
{
}

/*-------------------------------------------------------------------------------------------------------------------*/
Plane::~Plane()
{
  if(planeWindow_ != 0)
  {
    delete planeWindow_;
    planeWindow_ = 0;
  }
}

/*-------------------------------------------------------------------------------------------------------------------*/
Plane& Plane::operator=(const Plane& plane)
{
  zPosition_    = plane.zPosition_;
  planeID_      = plane.planeID_;
  xProjection_  = plane.xProjection_;
  yProjection_  = plane.yProjection_;
  orientation_  = plane.orientation_;
  alphaAngle_   = plane.alphaAngle_;
  betaAngle_    = plane.betaAngle_;				   
  *planeWindow_ = *(plane.planeWindow_);
  return *this;
}

/*-------------------------------------------------------------------------------------------------------------------*/
void Plane::calcProjections(const DataClass& theData)
{
  calcXProjection(theData);
  calcYProjection(theData);
}

/*--------------------------------------------Calculate X Projection-------------------------------------------------*/
const double& Plane::calcXProjection(const DataClass& theData)
{
  xProjection_ = theData.getXslope()*zPosition_ + theData.getXproj0();
  return xProjection_;
}

/*--------------------------------------------Calculate Y Projecton--------------------------------------------------*/
const double& Plane::calcYProjection(const DataClass& theData)
{
  yProjection_ = theData.getYslope()*zPosition_ + theData.getYproj0();
  return yProjection_;
}

/*---------------------------------------------Calculate Pixel Position----------------------------------------------*/
int Plane::calcPixPos(double& rowVal, double& colVal, double xp, double yp)
{


   switch(orientation_)
   {

    case XPLANE:
    {

        rowVal = ( 79 - (abs(xp) - ROWEDGE)/ ROWPITCH );      // row is is calculated same way everywhere on plane
        colVal = 0;

        if( yp < XCHIP1MAX && yp > XCHIP1MIN && xp > 0 )               // Chip 1
	  colVal = xColChip1(yp);

        if( yp < XCHIP4MAX && yp > XCHIP4MIN && xp < 0)                // Chip 4
	  colVal = xColChip4(yp);

	if( yp < XCHIP0MAX && yp > XCHIP0MIN && xp > 0)                // Chip 0
	  colVal = xColChip0(yp);

	if( yp < XCHIP2MAX && yp > XCHIP2MIN && xp > 0)                // Chip 2
	  colVal = xColChip2(yp);

	if( yp < XCHIP5MAX && yp > XCHIP5MIN && xp < 0)                // Chip 5
	  colVal = xColChip5(yp);

	if( yp < XCHIP3MAX && yp > XCHIP3MIN && xp < 0)                // Chip 3
	  colVal = xColChip3(yp);

        //if( colVal == 0 ) cout << "Error Calculating Column Value" << endl;  // There unhandled cases 
    }
    break;
    


    case YPLANE:
    {
      // calculate row value here
     
    }
    break;
    


    case DUT:
    {
      // calculat row value here
    }
    break;
    
    default:
    cout << "\nError: Invalid Plane Type\n";
   }

   return 0;
}

/*-------------------------------------------------------------------------------------------------------------------*/
// Accessors 
const Window& Plane::getWindow( void ) const
{
  return *planeWindow_;
}

/*-------------------------------------------------------------------------------------------------------------------*/
const unsigned int& Plane::getPlaneID(void) const
{
  return planeID_;
}

/*-------------------------------------------------------------------------------------------------------------------*/
const int& Plane::getZPosition(void) const
{
  return zPosition_;
}

/*-------------------------------------------------------------------------------------------------------------------*/
const double& Plane::getXProjection(void) const
{
  return xProjection_;
}

/*-------------------------------------------------------------------------------------------------------------------*/
const double& Plane::getYProjection(void) const
{
  return yProjection_;
}
/*-------------------------------------------------------------------------------------------------------------------*/
enum orientationIDs_t Plane::getOrientation(void)
{
  return orientation_;
}
/*-------------------------------------------------------------------------------------------------------------------*/
double Plane::getAlphaAngle(void)
{
  return alphaAngle_;
}
/*-------------------------------------------------------------------------------------------------------------------*/

double Plane::getBetaAngle(void)
{
  return betaAngle_;
}

/*--------------------------------------------------Modifiers--------------------------------------------------------*/
void Plane::setupWindow(TTree* tree)
{
  planeWindow_->calculateWindow(*this,tree);
}

/*-------------------------------------------------------------------------------------------------------------------*/
void Plane::setOrientation(enum orientationIDs_t orientation)
{
  orientation_ = orientation;
}








/*----------------------------------------------Chip functions-------------------------------------------------------*/
double Plane::xColChip0(double yp)						 // Chip 0
{
  double colVal = 0.0;
  if ( (planeID_ == 2) | (planeID_ == 4) )
  {
    //cout << "\nCalled: Chip0";
    colVal = 51 - (abs(yp) - (25*COLPITCH + 2*COLEDGE))/COLPITCH;
  }
  else if ( (planeID_ == 3) | (planeID_ == 5) )
  {
    colVal = ( abs(yp) - (24*COLPITCH + 2*COLEDGE))/COLPITCH;
    //cout << "\nCalled: Chip5";  
  }
  return colVal;
}

double Plane::xColChip1(double yp)						 // Chip 1
{
  double colVal = 0.0;  
  if ( (planeID_ == 2) | (planeID_ == 4) )
  {
    //cout << "\nCalled: Chip1";
    colVal = (24* COLPITCH + COLEDGE - yp)/COLPITCH;
  }
  else if ( (planeID_ == 3) | (planeID_ == 5) )
  {
    colVal = (24* COLPITCH + COLEDGE +  abs(yp))/COLPITCH;
    //cout << "\nCalled: Chip4";
  }
  return colVal;
}

double Plane::xColChip2(double yp)						 // Chip 2
{
  double colVal = 0.0;
  if ( (planeID_ == 2) | (planeID_ == 4) )
  {
     //cout << "\nCalled: Chip2";
     colVal = (yp - (25*COLPITCH + 2*COLEDGE))/COLPITCH;
  }
  else if ( (planeID_ == 3) | (planeID_ == 5) )
  {
     colVal = ( abs(yp) - (25*COLPITCH + 2*COLEDGE))/COLPITCH;
     //cout << "\nCalled: Chip3";
  }
  return colVal;
}

double Plane::xColChip3(double yp )  						// Chip 3
{
  double colVal = 0.0;  
  if ( (planeID_ == 2) | (planeID_ == 4) )
  {
    colVal = ( abs(yp) - (25*COLPITCH + 2*COLEDGE))/COLPITCH;
    //cout << "\nCalled: Chip3";
  }
  else if ( (planeID_ == 3) | (planeID_ == 5) )
  {
     //cout << "\nCalled: Chip2";
     colVal = (yp - (25*COLPITCH + 2*COLEDGE))/COLPITCH;
  }

  return colVal;
}

double Plane::xColChip4(double yp )  						// Chip 4
{
  double colVal = 0.0;
  if ( (planeID_ == 2) | (planeID_ == 4) )					  
  {										  
    colVal = (24* COLPITCH + COLEDGE +  abs(yp))/COLPITCH;			  
    //cout << "\nCalled: Chip4";							  
  }										  
  else if ( (planeID_ == 3) | (planeID_ == 5) ) 				  
  {										  
    //cout << "\nCalled: Chip1";							  
    colVal = (24* COLPITCH + COLEDGE - yp)/COLPITCH;				  
  }
  return colVal;
}

double Plane::xColChip5(double yp )  						// Chip 5
{
  double colVal = 0.0;
  if ( (planeID_ == 2) | (planeID_ == 4) )
  {
    colVal = ( abs(yp) - (24*COLPITCH + 2*COLEDGE))/COLPITCH;
    //cout << "\nCalled: Chip5";
  }
  else if ( (planeID_ == 3) | (planeID_ == 5) )
  {
    //cout << "\nCalled: Chip0";
    colVal = 51 - (abs(yp) - (25*COLPITCH + 2*COLEDGE))/COLPITCH;
  }
  return colVal;
}





double Plane::yColChip0(double yp)
{
//  0.0 = 
  return 0.0;
}

double Plane::yColChip1(double yp)
{
//  0.0 = 
  return 0.0;
}

double Plane::yColChip2(double yp)
{
// 0.0 = 
  return 0.0;
}

double Plane::yColChip3(double yp)
{
//  0.0 = 
  return 0.0;
}

double Plane::yColChip4(double yp)
{
//  0.0 = 
  return 0.0;
}

double Plane::yColChip5(double yp)
{
//  0.0 = 
  return 0.0;
}
/*----------------------------------------------Chip functions-------------------------------------------------------*/









