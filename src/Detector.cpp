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
 
#include "Detector.h"
#include <TMath.h>

#define PI TMath::Pi()

ClassImp(Detector)

//===============================================================================
Detector::Detector (std::string ID, bool isDUT, bool isStrip) :
    ID_                      (ID)
  , isDUT_                   (isDUT)
  , name_                    ("")
  , isStrip_                 (isStrip)
  , xPosition_     	         (0)
  , xPositionCorrection_     (0)
  , xPositionError_     	 (0)
  , yPosition_     	         (0)
  , yPositionCorrection_     (0)
  , yPositionError_     	 (0)
  , zPosition_     	         (0)
  , zPositionCorrection_     (0)
  , zPositionError_     	 (0)
  , xNumberOfROCs_  	     (0)
  , yNumberOfROCs_  	     (0)
  , numberOfROCs_    	     (0)
  , numberOfRows_            (0)
  , numberOfCols_            (0)
  , xBackFlipped_  	         (false)
  , yBackFlipped_  	         (false)
  , xRotation_     	         (0)
  , xRotationCorrection_     (0)
  , xRotationCorrectionError_(0)
  , yRotation_               (0)
  , yRotationCorrection_     (0)
  , yRotationCorrectionError_(0)
  , zRotation_               (0)
  , zRotationCorrection_     (0)
  , zRotationCorrectionError_(0)
{

}

//===============================================================================
Detector::~Detector(void)
{
    //    STDLINE("Detector::~Detector",ACRed);
    for(ROCsMapDef::iterator it=ROCsChipIDMap_.begin(); it!=ROCsChipIDMap_.end(); it++)
    {
        delete it->second;
    }
    ROCsChipIDMap_  .clear();
    ROCsPositionMap_.clear();
    //    STDLINE("Detector::~Detector",ACGreen);
}

//===============================================================================
void Detector::setupVariables(void)
{
    numberOfCols_ = 0;
    numberOfRows_ = 0;
    for(ROCsMapDef::iterator it=ROCsChipIDMap_.begin(); it!=ROCsChipIDMap_.end(); it++)
    {
        unsigned int ROCPosition = it->second->getPosition();
        unsigned int yPosition   = ROCPosition/xNumberOfROCs_;//division between ints is what i want!

        it->second->setLocalYPosition(yPosition);
        if( yPosition%2 != 0 ) it->second->setLocalXPosition((yPosition+1) * xNumberOfROCs_ - (ROCPosition+1));
        else                   it->second->setLocalXPosition(ROCPosition - yPosition * xNumberOfROCs_)        ;
        if(it->second->getPositionLocalY() == 0)
            numberOfCols_ += it->second->getNumberOfCols();
        if(it->second->getPositionLocalX() == 0)
            numberOfRows_ += it->second->getNumberOfRows();
        it->second->getNumberOfCols();
    }

    ROCsPositionMap_[0]->setFirstCol(0);
    ROCsPositionMap_[0]->setLastCol(ROCsPositionMap_[0]->getNumberOfCols()-1);
    for(unsigned int posX=1; posX < xNumberOfROCs_; posX++)
    {
        ROCsPositionMap_[posX]->setFirstCol(ROCsPositionMap_[(posX-1)]->getLastCol() +1);
        ROCsPositionMap_[posX]->setLastCol (ROCsPositionMap_[ posX   ]->getFirstCol()+
                                            ROCsPositionMap_[ posX   ]->getNumberOfCols()-1);
    }

    ROCsPositionMap_[0]->setFirstRow(0);
    ROCsPositionMap_[0]->setLastRow(ROCsPositionMap_[0]->getNumberOfRows()-1);
    for(unsigned int posY=xNumberOfROCs_; posY<yNumberOfROCs_*xNumberOfROCs_; posY+=xNumberOfROCs_)
    {
        ROCsPositionMap_[posY]->setFirstRow(ROCsPositionMap_[(posY-xNumberOfROCs_)]->getLastRow() +1);
        ROCsPositionMap_[posY]->setLastRow (ROCsPositionMap_[ posY                ]->getFirstRow()+
                                            ROCsPositionMap_[ posY                ]->getNumberOfRows()-1);
    }

    for(ROCsMapDef::iterator it=ROCsChipIDMap_.begin(); it!=ROCsChipIDMap_.end(); it++)
    {
        it->second->setFirstCol(ROCsPositionMap_[it->second->getPositionLocalX()]->getFirstCol());
        it->second->setLastCol (ROCsPositionMap_[it->second->getPositionLocalX()]->getLastCol());
        it->second->setFirstRow(ROCsPositionMap_[it->second->getPositionLocalY()*xNumberOfROCs_]->getFirstRow());
        it->second->setLastRow (ROCsPositionMap_[it->second->getPositionLocalY()*xNumberOfROCs_]->getLastRow());
    }
}

//===============================================================================
Detector::matrix33Def Detector::rotationMatrix(double alpha, double beta, double gamma)
{
    Detector::matrix33Def R;

    R(0,0) = cos(beta )*cos(gamma);
    R(0,1) =-cos(alpha)*sin(gamma)+sin(alpha)*sin(beta )*cos(gamma);
    R(0,2) = sin(alpha)*sin(gamma)+cos(alpha)*sin(beta )*cos(gamma);
    R(1,0) = cos(beta)*sin(gamma);
    R(1,1) = cos(alpha)*cos(gamma)+sin(alpha)*sin(beta)*sin(gamma);
    R(1,2) =-sin(alpha)*cos(gamma)+cos(alpha)*sin(beta)*sin(gamma);
    R(2,0) =-sin(beta);
    R(2,1) = sin(alpha)*cos(beta);
    R(2,2) = cos(alpha)*cos(beta);

    return R;
}

//===============================================================================
ROC* Detector::addROC(unsigned int chipPosition, int chipID, unsigned int rotationDegrees)
{
    if( chipPosition > numberOfROCs_ )
    {
        std::stringstream ss;
        ss.str("");
        ss << chipID;
        STDLINE("ERROR: there is no ROC position: " + ss.str() + " in current detector!!",ACRed);
        STDLINE("ERROR: Maybe you have to use setComposition first",ACRed);
    }
    if( ROCsChipIDMap_.find( chipID ) == ROCsChipIDMap_.end() && ROCsPositionMap_.find( chipID ) == ROCsPositionMap_.end() )
    {
        ROCsChipIDMap_      [ chipID ]       = new ROC(chipPosition, chipID, rotationDegrees );
        ROCsPositionMap_    [ chipPosition ] = ROCsChipIDMap_[ chipID ]                       ;
    }
    else
    {
        std::stringstream ss;
        ss.str("");
        ss << "WARNING: ROC: " << chipID << " in position " << chipPosition << "was already set!!";
        STDLINE(ss.str(),ACRed);
    }
    return ROCsChipIDMap_[ chipID ];
}

//===============================================================================
void  Detector::setNumberOfROCs(unsigned int xNumberOfROCs, unsigned int yNumberOfROCs)
{
    xNumberOfROCs_ = xNumberOfROCs;
    yNumberOfROCs_ = yNumberOfROCs;
    this->updateNumberOfROCs();
}

//===============================================================================
unsigned int Detector::getNumberOfCols(bool global)
{
    if( this->isRotated(global) )
        return numberOfCols_;
    else
        return this->getNumberOfRows();
}

//===============================================================================
unsigned int Detector::getNumberOfRows(bool global)
{
    if( this->isRotated(global) )
        return numberOfRows_;
    else
        return this->getNumberOfCols();
}
//===============================================================================
bool Detector::isRotated(bool global)
{
    if( !global || (zRotation_ <   45 && zRotation_ >=  0)
                || (zRotation_ <= 360 && zRotation_ > 315)
                || (zRotation_ >  135 && zRotation_ < 225) ) return true ;
    return false ;
}

//===============================================================================
ROC* Detector::getROC(int chipID)
{
    if( ROCsChipIDMap_.find( chipID ) != ROCsChipIDMap_.end() )
        return ROCsChipIDMap_[chipID] ;
    else
    {
        STDLINE("WARNING: No ROC found",ACRed);
        return 0;
    }
}

//===============================================================================
ROC* Detector::getROCByPosition(unsigned int chipPosition)
{
    if( ROCsPositionMap_.find( chipPosition ) != ROCsPositionMap_.end() )
        return ROCsPositionMap_[chipPosition] ;
    else
    {
        STDLINE("WARNING: No ROC found",ACRed);
        return 0;
    }
}

//===============================================================================
int Detector::getPixelColFromLocalX(double xLocal)
{
    if(xLocal < 0 || xLocal >= this->getDetectorLengthX())
        return -1;

    double         l = 0;
    ROC*         roc = 0;
    unsigned int col = 0;

    for(unsigned int posX=0; posX<xNumberOfROCs_; posX++)
    {
        roc = getROCByPosition(posX);
        if( xLocal < l+roc->getLengthLocalX())
            break;
        else
        {
            l += roc->getLengthLocalX();
            if(posX+1 == xNumberOfROCs_)
                return -1;
            col += roc->getNumberOfCols();
        }

    }
    for(unsigned int rocCol=0; rocCol<roc->getNumberOfCols(); rocCol++)
    {
        l += roc->getPixelPitchLocalX(rocCol);
        if(xLocal < l)
            return col+rocCol;
    }
    return -1;
}

//==============================================================================
int Detector::getPixelRowFromLocalY(double yLocal)
{
    if(yLocal < 0 || yLocal >= this->getDetectorLengthY())
        return -1;

    double l=0  ;
    for(unsigned int row=0; row<this->getNumberOfRows(); row++)
    {
        l+=this->getPixelPitchLocalY(row);
        if(yLocal < l) return row;
    }
    return -1;
}

//===============================================================================
Detector::rowColPair Detector::getPixelCellFromLocal(double x, double y)
{
    Detector::rowColPair rowCol;
    rowCol.first  = this->getPixelRowFromLocalY(y);
    rowCol.second = this->getPixelColFromLocalX(x);
    return rowCol;
}

//===============================================================================
Detector::rowColPair Detector::getPixelCellFromGlobal(double x, double y, double z)
{
    fromGlobalToLocal(&x,&y,&z);
    Detector::rowColPair rowCol;
    rowCol.first  = this->getPixelRowFromLocalY(y);
    rowCol.second = this->getPixelColFromLocalX(x);
    return rowCol;
}

//===============================================================================
double Detector::getPixelCenterLocalX(unsigned int col)
{
    return this->getPixelLowEdgeLocalX(col) + this->getPixelPitchLocalX(col)/2.;
}

//===============================================================================
double Detector::getPixelCenterLocalY(unsigned int row)
{
    return this->getPixelLowEdgeLocalY(row) + this->getPixelPitchLocalY(row)/2.;
}

//===============================================================================
double Detector::getPixelLowEdgeLocalX(unsigned int col)
{
    double lowEdge=0;
    for (unsigned int pos=0; pos < xNumberOfROCs_; pos++)
    {
        ROC *roc = getROCByPosition(pos);

        if(col > roc->getLastCol())
            lowEdge += roc->getLengthLocalX();
        else
        {
            unsigned int rocRow = 0;
            roc = this->convertPixelToROC(&rocRow,&col);
            lowEdge += roc->getPixelLowEdgeLocalX(col)                       ;
            return lowEdge;
        }
    }
    STDLINE("WARNING: pixel column is out of range, returning detector's length X", ACRed);
    return lowEdge;
}

//===============================================================================
double Detector::getPixelLowEdgeLocalY(unsigned int row)
{
    double lowEdge=0;
    for (unsigned int pos=0; pos < numberOfROCs_ ; pos+=xNumberOfROCs_)
    {
        ROC *roc = getROCByPosition(pos);
        if(row > roc->getLastRow())
            lowEdge += roc->getLengthLocalY();
        else
        {
            unsigned int rocCol = 0;
            roc = this->convertPixelToROC(&row,&rocCol);
            lowEdge += roc->getPixelLowEdgeLocalY(row)                         ;
            return lowEdge;
        }
    }
    STDLINE("WARNING: pixel row is out of range, returning detector's length Y", ACRed);
    return lowEdge;
}

//===============================================================================
double Detector::getPixelPitchLocalX(unsigned int col)
{
    unsigned int rocRow = 0;
    ROC* roc = this->convertPixelToROC(&rocRow,&col);
    return roc->getPixelPitchLocalX(col);
}

//===============================================================================
double Detector::getPixelPitchLocalY(unsigned int row)
{
    unsigned int rocCol = 0;
    ROC* roc = this->convertPixelToROC(&row,&rocCol);
    return roc->getPixelPitchLocalY(row);
}

//===============================================================================
double Detector::getDetectorLengthX(bool global)
{
    if( this->isRotated(global) )
    {
        double length=0;
        for (unsigned int pos=0; pos < xNumberOfROCs_; pos++)
            length += getROCByPosition(pos)->getLengthLocalX();
        return length;
    }
    else
        return this->getDetectorLengthY(false);
}

//===============================================================================
double Detector::getDetectorLengthY(bool global)
{
    if( this->isRotated(global) )
    {
        double length=0;
        for (unsigned int pos=0; pos < numberOfROCs_ ; pos+=xNumberOfROCs_)
            length  += getROCByPosition(pos)->getLengthLocalY();
        return length;
    }
    else
        return this->getDetectorLengthX(false);

}

//===============================================================================
unsigned int Detector::getROCPositionLocalX(int chipID)
{
    return getROC(chipID)->getPositionLocalX();
}

//===============================================================================
unsigned int Detector::getROCPositionLocalY(int chipID)
{
    return getROC(chipID)->getPositionLocalY();
}

//=================================================================
//From row and col of the ROC they belong to the row and col of the detector
void Detector::convertPixelFromROC(ROC* roc, unsigned int *row, unsigned int *col)
{
    if(roc->getOrientation() == 0)
    {
        *row += roc->getFirstRow();
        *col += roc->getFirstCol();
    }
    else if(roc->getOrientation() == 180)
    {
        *row = roc->getLastRow() - *row;
        *col = roc->getLastCol() - *col;
    }
    else
    {
        STDLINE("ERROR: Only 0 and 180 degrees rotations are supported for a ROC!", ACRed);
        exit(EXIT_FAILURE);
    }
}

//=================================================================
//From row and col of the detector to row and col of the ROC they belong to
ROC* Detector::convertPixelToROC(unsigned int *row, unsigned int *col)
{
    ROC *roc = this->findROC(*row, *col);
    if(roc->getOrientation() == 0)
    {
        *row -= roc->getFirstRow();
        *col -= roc->getFirstCol();
    }
    else if(roc->getOrientation() == 180)
    {
        *row = roc->getLastRow() - *row;
        *col = roc->getLastCol() - *col;
    }
    else
    {
        STDLINE("ERROR: Only 0 and 180 degrees rotations are supported for a ROC!", ACRed);
        exit(EXIT_FAILURE);
    }
    return roc;
}

//=================================================================
ROC* Detector::findROC(unsigned int row, unsigned int col)
{
    unsigned int posX = getROCPositionLocalXFromCol(col);
    unsigned int posY = getROCPositionLocalYFromRow(row);
    if(posX != xNumberOfROCs_ &&  posY != yNumberOfROCs_)
    {
        if(posY%2 == 0)
            return getROCByPosition(posX+posY*xNumberOfROCs_);
        else
            return getROCByPosition(xNumberOfROCs_-1-posX+posY*xNumberOfROCs_);
    }
    STDLINE("WARNING: pixel column or pixel row is out of detector's range, return NULL", ACRed);
    return 0;
}

//=================================================================
unsigned int Detector::getROCPositionLocalXFromCol(unsigned int col)
{
    unsigned int posX = 0;
    for (posX=0; posX < xNumberOfROCs_; posX++)
        if ( col <= getROCByPosition(posX)->getLastCol() )
            break;
    return posX;
}

//=================================================================
unsigned int Detector::getROCPositionLocalYFromRow(unsigned int row)
{
    unsigned int posY = 0;
    for (posY=0; posY < numberOfROCs_; posY+=xNumberOfROCs_)
        if ( row <= getROCByPosition(posY)->getLastRow() )
            break;
    return posY/xNumberOfROCs_;
}

//=================================================================
void Detector::flipPixel(unsigned int *row, unsigned int *col)
{
    unsigned int lastRow  = this->getLastRow();
    unsigned int lastCol  = this->getLastCol();

    // reflection through horizonatal center of symmetry
    if( this->isXBackFlipped())
        *col = lastCol - *col;
    // reflection through vertical center of symmetry
    if( this->isYBackFlipped())
        *row = lastRow - *row ;

    // rotation around z axis
    int tmp;
    // vertical clockwise
    if(     zRotation_ > 225  && zRotation_ < 315 )
    {
        tmp  = *col           ;
        *col = *row           ;
        *row = lastCol - tmp  ;
    }
    // vertical anti-clockwise
    else if(zRotation_ > 45   && zRotation_ < 135 )
    {
        tmp  = *col           ;
        *col = lastRow - *row ;
        *row = tmp            ;
    }
    // horizontal anti-clockwise
    else if(zRotation_ >= 135 && zRotation_ <= 225)
    {
        *row = lastRow - *row ;
        *col = lastCol - *col ;
    }
    // no rotation, no flip
}

//=================================================================
void Detector::flipBackPixel(unsigned int *row, unsigned int *col )
{
    unsigned int lastRow  = this->getLastRow();
    unsigned int lastCol  = this->getLastCol();
    // rotation around z axis
    int tmp;
    // vertical clockwise
    if(zRotation_ > 225 && zRotation_ < 315)
    {
        tmp  = *row          ;
        *row = *col          ;
        *col = lastCol - tmp ;
    }
    // vertical anti-clockwise
    else if(zRotation_ > 45 && zRotation_ < 135)
    {
        tmp  = *row          ;
        *row = lastRow - *col;
        *col = tmp           ;
    }
    // horizontal anti-clockwise
    else if(zRotation_ >= 135 && zRotation_ <= 225)
    {
        *col = lastCol - *col;
        *row = lastRow - *row;
    }

    // reflection through horizonatal center of symmetry
    if( this->isXBackFlipped() )
        *col = lastCol - *col;
    // reflection through vertical center of symmetry
    if( this->isYBackFlipped() )
        *row = lastRow - *row ;
}

//=================================================================
void Detector::flipPositionLocal(double *x, double *y, double *xErr, double *yErr)
{
    double yLength  = this->getDetectorLengthY();
    double xLength  = this->getDetectorLengthX();

    //ss_.str("");
    //ss_ << "yLength: " << yLength << " xLength: " << xLength;
    //STDLINE(ss_.str(),ACPurple);


    // reflection through vertical center of symmetry
    if( this->isXBackFlipped() )
        *x = xLength - *x ;
    // reflection through horizontal center of symmetry
    if( this->isYBackFlipped() )
        *y = yLength - *y ;
    // rotation around z axis (only horizontal or vertical)
    double tmp;
    // vertical clockwise
    if(zRotation_ > 225 && zRotation_ < 315)
    {
        tmp = *x            ;
        *x  = *y            ;
        *y  = xLength - tmp ;
        if(xErr != 0 && yErr != 0)
        {
            tmp   = *xErr   ;
            *xErr = *yErr   ;
            *yErr = tmp     ;
        }
    }
    // vertical anti-clockwise
    else if(zRotation_ > 45 && zRotation_ < 135)
    {
        tmp = *x            ;
        *x  = yLength - *y  ;
        *y  = tmp           ;
        if(xErr != 0 && yErr != 0)
        {
            tmp   = *xErr   ;
            *xErr = *yErr   ;
            *yErr = tmp     ;
        }
    }
    // horizontal anti-clockwise
    else if(zRotation_ >= 135 && zRotation_ <= 225)
    {
        *y = yLength - *y ;
        *x = xLength - *x ;
    }
}

//=================================================================
void Detector::flipBackPositionLocal(double *x, double *y, double *xErr, double *yErr)
{
    double yLength  = this->getDetectorLengthY();
    double xLength  = this->getDetectorLengthX();

    // rotation around z axis (only orizontal or vertical)
    double tmp;
    // vertical clockwise
    if(zRotation_ > 225 && zRotation_ < 315)
    {
        tmp = *y            ;
        *y  = *x            ;
        *x  = xLength - tmp ;
        if(xErr != 0 && yErr != 0)
        {
            tmp   = *yErr   ;
            *yErr = *xErr   ;
            *xErr = tmp     ;
        }
    }
    // vertical anti-clockwise
    else if(zRotation_ > 45 && zRotation_ < 135)
    {
        tmp = *y            ;
        *y  = yLength - *x  ;
        *x  = tmp           ;
        if(xErr != 0 && yErr != 0)
        {
            tmp   = *yErr   ;
            *yErr = *xErr   ;
            *xErr = tmp     ;
        }
    }
    // horizontal anti-clockwise
    else if(zRotation_ >= 135 && zRotation_ <= 225)
    {
        *x = xLength - *x   ;
        *y = yLength - *y   ;
    }

    // reflection through vertical center of symmetry
    if( this->isXBackFlipped() )
        *x = xLength - *x ;
    // reflection through horizontal center of symmetry
    if( this->isYBackFlipped() )
        *y = yLength - *y ;
}

//=================================================================
void Detector::flipDistance(double* deltaX, double* deltaY)
{
    //reflection through vertical center of symmetry
    if( this->isXBackFlipped() )
        *deltaX = -*deltaX ;
    //reflection through orizontal center of symmetry
    if( this->isYBackFlipped() )
        *deltaY = -*deltaY ;

    //rotation around z axis (only orizontal or vertical)
    double tmp;
    //vertical clockwise
    if(zRotation_ > 225 && zRotation_ < 315)
    {
        tmp      = *deltaX;
        *deltaX  = *deltaY;
        *deltaY  = tmp;
    }
    //vertical anti-clockwise
    else if(zRotation_ > 45 && zRotation_ < 135)
    {
        tmp      =  *deltaX;
        *deltaX  = -*deltaY;
        *deltaY  = tmp;
    }
    //orizontal anti-clockwise
    else if(zRotation_ >= 135 && zRotation_ <= 225)
    {
        *deltaY = -*deltaY;
        *deltaX = -*deltaX;
    }
}

//=================================================================
void Detector::flipBackDistance(double* deltaX, double* deltaY)
{
    //rotation around z axis (only orizontal or vertical)
    double tmp;
    //vertical clockwise
    if(zRotation_ > 225 && zRotation_ < 315)
    {
        tmp     = *deltaY;
        *deltaY = *deltaX;
        *deltaX = -tmp ;
    }
    //vertical anti-clockwise
    else if(zRotation_ > 45 && zRotation_ < 135)
    {
        tmp     = *deltaY ;
        *deltaY = -*deltaX;
        *deltaX = tmp     ;
    }
    //orizontal anti-clockwise
    else if(zRotation_ >= 135 && zRotation_ <= 225)
    {
        *deltaX = -*deltaX;
        *deltaY = -*deltaY;
    }

    //reflection through orizonatal center of symmetry
    if( this->isXBackFlipped() )
        *deltaX = -*deltaX;
    //reflection through vertical center of symmetry
    if( this->isYBackFlipped() )
        *deltaY = -*deltaY;
}

//=================================================================
bool Detector::switchXYFromLocaToGlobal(void)
{
    //vertical clockwise
    if( (zRotation_ > 225 && zRotation_ < 315) || (zRotation_ > 45 && zRotation_ < 135) ) return true;
    return false;
}
//=================================================================
double Detector::getXRotation(bool global)
{
    if(global) return xRotation_;
    else
    {
        //vertical clockwise
        if     (zRotation_ > 225  && zRotation_ < 315 ) return -yRotation_;
        //vertical anti-clockwise
        else if(zRotation_ > 45   && zRotation_ < 135 ) return  yRotation_;
        //orizontal anti-clockwise
        else if(zRotation_ >= 135 && zRotation_ <= 225) return -xRotation_;
        //orizontal clockwise
        else                                            return  xRotation_;
    }
}

//=================================================================
double Detector::getYRotation(bool global)
{
    if (global) return yRotation_;
    else
    {
        //vertical clockwise
        if     (zRotation_ > 225  && zRotation_ < 315 ) return -xRotation_;
        //vertical anti-clockwise
        else if(zRotation_ > 45   && zRotation_ < 135 ) return  xRotation_;
        //orizontal anti-clockwise
        else if(zRotation_ >= 135 && zRotation_ <= 225) return -yRotation_;
        //orizontal clockwise
        else                                            return  yRotation_;
    }
}

//=======================================================================================
double Detector::fromLocalToGlobal(double* x, double* y, double* z, double* xErr, double* yErr, double* zErr)
{

    this->flipPositionLocal  (x,y,xErr,yErr);
    this->translateXY        (x,y);
    this->translateCorrection(x,y);
    Detector::matrix33Def R = this->getRotationMatrix();

    double sigxy = R(0,0)*R(1,0)*pow(*xErr,2)+R(0,1)*R(1,1)*pow(*yErr,2);

    //this->XRotation(y,z,yErr,zErr);
    //this->YRotation(x,z,xErr,zErr);
    //this->ZRotation(x,y,xErr,yErr);
    this->XYZRotation(x,y,z,xErr,yErr,zErr);
    *z += this->getZPositionTotal();
    return sigxy;
}

//=======================================================================================
void Detector::fromLocalToGlobal(double* x, double* y, double* z)
{

    this->flipPositionLocal(x,y);
    this->translateXY(x,y);
    this->translateCorrection(x,y);

    //this->XRotation(y,z);
    //this->YRotation(x,z);
    //this->ZRotation(x,y);
    this->XYZRotation(x,y,z);
    *z += this->getZPositionTotal();
}

//=======================================================================================
void Detector::fromLocalToGlobalKalman(double* x, double* y, double* z, matrix33Def fRinv)
{
    this->flipPositionLocal  (x,y);
    this->translateXY        (x,y);
    this->translateCorrection(x,y);
    this->XYZRotationKalman  (x,y,z,fRinv) ;
    *z += this->getZPositionTotal();
}

//=======================================================================================
void Detector::fromLocalToGlobalNoRotation(double* x, double* y,double* xErr, double* yErr)
{
    this->flipPositionLocal  (x,y,xErr,yErr);
    this->translateXY        (x,y          );
    this->translateCorrection(x,y          );
}

//=======================================================================================
void Detector::fromGlobalToLocal(double* x, double* y, double* z, double* xErr, double *yErr, double* zErr)
{
    *z -= this->getZPositionTotal();

    //this->ZRotation(x,y,xErr,yErr,true);
    //this->YRotation(x,z,xErr,zErr,true);
    //this->XRotation(y,z,yErr,zErr,true);
    this->XYZRotation(x,y,z,xErr,yErr,zErr,true);

    this->translateXY(x,y,true);
    this->translateCorrection(x,y,true);
    this->flipBackPositionLocal(x,y,xErr,yErr);
    *z    = 0;
    *zErr = 0;
}

//=======================================================================================
void Detector::fromGlobalToLocal(double* x, double* y, double* z)
{
    *z -= this->getZPositionTotal();

    //this->ZRotation(x,y,xErr,yErr,true);
    //this->YRotation(x,z,xErr,zErr,true);
    //this->XRotation(y,z,yErr,zErr,true);
    this->XYZRotation(x,y,z,true);

    this->translateXY(x,y,true);
    this->translateCorrection(x,y,true);
    this->flipBackPositionLocal(x,y);
    *z    = 0;
}

//=======================================================================================
void Detector::XYZRotation(double* x, double* y, double* z, double* xErr, double* yErr, double* zErr, bool backward)
{
    XYZRotation(x,y,z,backward);
    XYZRotation(xErr,yErr,zErr,backward);
}

//=======================================================================================
void Detector::XYZRotation(double* x, double* y, double* z, bool backward)
{
    Detector::matrix33Def R = this->getRotationMatrix();

    double  xv[3]      = {*x,*y,*z};
    double* xPrimev[3] = {x,y,z};

    for(int i=0; i<3; i++)
    {
        *(xPrimev[i]) = 0;
        for(int j=0; j<3; j++)
            if(backward)
                *(xPrimev[i]) += R(j,i)*xv[j];
            else
                *(xPrimev[i]) += R(i,j)*xv[j];
    }
}

//=======================================================================================
void Detector::XYZRotationKalman(double* x, double* y, double* z, Detector::matrix33Def fRinv, bool backward)
{
    double  xv[3]      = {*x,*y,*z};
    double* xPrimev[3] = {x,y,z};

    for(int i=0; i<3; i++)
    {
        *(xPrimev[i]) = 0;
        for(int j=0; j<3; j++)
            if(backward)
                *(xPrimev[i]) += fRinv(j,i)*xv[j];
            else
                *(xPrimev[i]) += fRinv(i,j)*xv[j];
    }
    *z += this->getZPositionTotal();
}

//=======================================================================================
void Detector::XRotation(double* y, double* z, double* yErr, double* zErr, bool backward)
{
    XRotation(y,z,backward);
    XRotation(yErr,zErr,backward);
    //*yErr = fabs(*yErr);
    //*zErr = fabs(*zErr);
    /*
        double yTmp    = *y    ;
        double zTmp    = *z    ;
        double yErrTmp = *yErr ;
        double zErrTmp = *zErr ;
        double alpha = (xRotation_ + xRotationCorrection_)*PI/180. ;

        if(backward)
            alpha *= -1;

        *y    = (yTmp)*cos(alpha) - (zTmp)*sin(alpha);
        *z    = (yTmp)*sin(alpha) + (zTmp)*cos(alpha);

        *yErr  = sqrt( pow(yErrTmp*cos(alpha),2) + pow(zErrTmp*sin(alpha),2) );
        *zErr  = sqrt( pow(yErrTmp*sin(alpha),2) + pow(zErrTmp*cos(alpha),2) );
    */
}

//=======================================================================================
void Detector::XRotation(double* y, double* z, bool backward)
{
    double yTmp  = *y;
    double zTmp  = *z;
    double alpha = (xRotation_ + xRotationCorrection_)*PI/180. ;

    if(backward)
        alpha *= -1;

    *y = (yTmp)*cos(alpha) - (zTmp)*sin(alpha);
    *z = (yTmp)*sin(alpha) + (zTmp)*cos(alpha);
}

//=======================================================================================
void Detector::YRotation(double *x, double *z, double *xErr, double *zErr, bool backward)
{
    YRotation(x,z,backward);
    YRotation(xErr,zErr,backward);
    //*xErr = fabs(*xErr);
    //*zErr = fabs(*zErr);
    /*
        double xTmp    = *x;
        double zTmp    = *z;
        double xErrTmp = *xErr;
        double zErrTmp = *zErr;
        double beta  = (yRotation_+ yRotationCorrection_)*PI/180. ;

        if(backward)
            beta *= -1;

        *x    =  (xTmp)*cos(beta) + (zTmp)*sin(beta);
        *z    = -(xTmp)*sin(beta) + (zTmp)*cos(beta);

        *xErr  = sqrt( pow(xErrTmp*cos(beta),2) + pow(zErrTmp*sin(beta),2) );
        *zErr  = sqrt( pow(xErrTmp*sin(beta),2) + pow(zErrTmp*cos(beta),2) );
    */
}

//=======================================================================================
void Detector::YRotation(double *x, double *z, bool backward)
{
    double xTmp  = *x;
    double zTmp  = *z;
    double beta  = (yRotation_+ yRotationCorrection_)*PI/180. ;

    if(backward)
        beta *= -1;

    *x =  (xTmp)*cos(beta) + (zTmp)*sin(beta);
    *z = -(xTmp)*sin(beta) + (zTmp)*cos(beta);
}

//=======================================================================================
void Detector::ZRotation(double* x, double* y, double* xErr, double* yErr, bool backward)
{
    ZRotation(x,y,backward);
    ZRotation(xErr,yErr,backward);
    //*xErr = fabs(*xErr);
    //*yErr = fabs(*yErr);
    /*
        double xTmp    = *x;
        double yTmp    = *y;
        double xErrTmp = *xErr;
        double yErrTmp = *yErr;
        double gamma = (zRotationCorrection_)*PI/180.;

        if(backward)
            gamma *= -1;

        *x    = (xTmp)*cos(gamma) - (yTmp)*sin(gamma);
        *y    = (xTmp)*sin(gamma) + (yTmp)*cos(gamma);

        *xErr = sqrt( pow(xErrTmp*cos(gamma),2) + pow(yErrTmp*sin(gamma),2) );
        *yErr = sqrt( pow(xErrTmp*sin(gamma),2) + pow(yErrTmp*cos(gamma),2) );
    */
}

//=======================================================================================
void Detector::ZRotation(double* x, double* y, bool backward)
{
    double xTmp  = *x;
    double yTmp  = *y;
    double gamma = (zRotationCorrection_)*PI/180.;

    if(backward)
        gamma *= -1;

    *x = (xTmp)*cos(gamma) - (yTmp)*sin(gamma);
    *y = (xTmp)*sin(gamma) + (yTmp)*cos(gamma);
}

//=======================================================================================
void Detector::translateXY(double* x, double* y, bool backward)
{
    if(backward)
    {
        *x += xPosition_ ;
        *y += yPosition_ ;
    }
    else
    {
        *x -= xPosition_ ;
        *y -= yPosition_ ;
    }
}

//=======================================================================================
void Detector::translateCorrection(double* x, double* y, bool backward)
{
    if(backward)
    {
        *x += xPositionCorrection_ ;
        *y += yPositionCorrection_ ;
    }
    else
    {
        *x -= xPositionCorrection_ ;
        *y -= yPositionCorrection_ ;
    }
}

//================================================================
Detector::matrix33Def Detector::getRotationMatrix()
{
    double alpha,beta,gamma;
    alpha = (xRotation_ + xRotationCorrection_)*PI/180. ;
    beta  = (yRotation_ + yRotationCorrection_)*PI/180. ;
    gamma = (zRotationCorrection_)*PI/180.              ;

    return Detector::rotationMatrix(alpha, beta, gamma);
}

//================================================================
double Detector::getAlignmentPredictedGlobal(ROOT::Math::SVector<double,4> & trackPars,
                                             matrix33Def                   & RInv     ,
                                             double                          z        ,
                                             double                        & predX    ,
                                             double                        & predY    )
{
    double numX,numY,den;
    numX  = (trackPars[0]*z+trackPars[1])           *(RInv[1][1]-trackPars[2]*RInv[2][1])-
            (trackPars[2]*z+trackPars[3])           *(RInv[0][1]-trackPars[0]*RInv[2][1]);
    numY  = (trackPars[2]*z+trackPars[3])           *(RInv[0][0]-trackPars[0]*RInv[2][0])-
            (trackPars[0]*z+trackPars[1])           *(RInv[1][0]-trackPars[2]*RInv[2][0]);
    den   = (RInv[0][0]    -trackPars[0]*RInv[2][0])*(RInv[1][1]-trackPars[2]*RInv[2][1])-
            (RInv[1][0]    -trackPars[2]*RInv[2][0])*(RInv[0][1]-trackPars[0]*RInv[2][1]);
    predX = numX / den;
    predY = numY / den;
    return den;
}

//================================================================
void Detector::getPredictedGlobal(ROOT::Math::SVector<double,4>& trackPars, double& predX, double& predY, double& predZ)
{
    double x0 = 0;
    double y0 = 0;
    double z0 = 0;
    fromLocalToGlobal(&x0,&y0,&z0);

    Detector::matrix33Def R = this->getRotationMatrix();
    predZ = (R(0,2)*(x0-trackPars[1])+R(1,2)*(y0-trackPars[3])+R(2,2)*z0)/(R(0,2)*trackPars[0]+R(1,2)*trackPars[2]+R(2,2));

    predX = trackPars[0]*predZ + trackPars[1];
    predY = trackPars[2]*predZ + trackPars[3];
    /*
    STDLINE("------------------------------------------------------",ACWhite);
    std::stringstream ss;
    ss.str("");
    ss << "X: " << predX << " Y: " << predY
            << " local2Global: " << z0
            << " predZ: " << predZ
            << " zPos: " << this->getZPosition();
    STDLINE(ss.str(),ACGreen);

    //x0 = 0;
    //y0 = 0;
    //z0 = 0;
    //fromLocalToGlobalNoRotation(&x0,&y0,&z0);
    //z0 = predZ;

    double N_X0 = (trackPars[0]*z0 + trackPars[1]) * (R(1,1) - trackPars[2]*R(2,1)) - (trackPars[2]*z0 + trackPars[3]) * (R(0,1) - trackPars[0]*R(2,1));
    double N_Y0 = (trackPars[2]*z0 + trackPars[3]) * (R(0,0) - trackPars[0]*R(2,0)) - (trackPars[0]*z0 + trackPars[1]) * (R(1,0) - trackPars[2]*R(2,0));
    double D_0 = (R(0,0) - trackPars[0]*R(2,0)) * (R(1,1) - trackPars[2]*R(2,1)) - (R(1,0) - trackPars[2]*R(2,0)) * (R(0,1) - trackPars[0]*R(2,1));
    double predXX = N_X0 / D_0;
    double predYY = N_Y0 / D_0;

    ss.str("");
    ss << "X: " << predXX << " Y: " << predYY << " local2Global: " << z0;
    STDLINE(ss.str(),ACRed);

    z0 = predZ;

    N_X0 = (trackPars[0]*z0 + trackPars[1]) * (R(1,1) - trackPars[2]*R(2,1)) - (trackPars[2]*z0 + trackPars[3]) * (R(0,1) - trackPars[0]*R(2,1));
    N_Y0 = (trackPars[2]*z0 + trackPars[3]) * (R(0,0) - trackPars[0]*R(2,0)) - (trackPars[0]*z0 + trackPars[1]) * (R(1,0) - trackPars[2]*R(2,0));
    D_0 = (R(0,0) - trackPars[0]*R(2,0)) * (R(1,1) - trackPars[2]*R(2,1)) - (R(1,0) - trackPars[2]*R(2,0)) * (R(0,1) - trackPars[0]*R(2,1));
    predXX = N_X0 / D_0;
    predYY = N_Y0 / D_0;

    ss.str("");
    ss << "X: " << predXX << " Y: " << predYY << " predicted z : " << z0;
    STDLINE(ss.str(),ACRed);

    z0 = this->getZPosition();

    N_X0 = (trackPars[0]*z0 + trackPars[1]) * (R(1,1) - trackPars[2]*R(2,1)) - (trackPars[2]*z0 + trackPars[3]) * (R(0,1) - trackPars[0]*R(2,1));
    N_Y0 = (trackPars[2]*z0 + trackPars[3]) * (R(0,0) - trackPars[0]*R(2,0)) - (trackPars[0]*z0 + trackPars[1]) * (R(1,0) - trackPars[2]*R(2,0));
    D_0 = (R(0,0) - trackPars[0]*R(2,0)) * (R(1,1) - trackPars[2]*R(2,1)) - (R(1,0) - trackPars[2]*R(2,0)) * (R(0,1) - trackPars[0]*R(2,1));
    predXX = N_X0 / D_0;
    predYY = N_Y0 / D_0;

    ss.str("");
    ss << "X: " << predXX << " Y: " << predYY << " get z positi: " << z0;
    STDLINE(ss.str(),ACRed);

    //this->translateXY(&predXX,&predYY,true);
    //this->translateCorrection(&predXX,&predYY,true);

    //ss.str("");
    //ss << "X: " << predXX << " Y: " << predYY << " Z0: " << z0;
    //STDLINE(ss.str(),ACPurple);
    STDLINE("------------------------------------------------------",ACWhite);
*/
}

//================================================================
void Detector::getPredictedLocal(ROOT::Math::SVector<double,4>& trackPars, double& predX, double& predY)
{
    double predZ = 0;
    getPredictedGlobal(trackPars,predX,predY,predZ);
    fromGlobalToLocal(&predX,&predY,&predZ);
}

//======================================================================================================
Detector::xyPair Detector::getTrackErrorsOnPlane(ROOT::Math::SVector<double,4>& trackPars, matrix44Def& AtVAInv)
{
    Detector::matrix33Def RInv = this->getRotationMatrix();
    return propagateTrackErrors(trackPars, AtVAInv, RInv, this->getZPositionTotal());
}

//======================================================================================================
Detector::xyPair Detector::propagateTrackErrors(ROOT::Math::SVector<double,4>& trackPars,
                                                matrix44Def& AtVAInv,
                                                matrix33Def& RInv,
                                                double z)
{
    double predX,predY;
    double predSigmaXX,predSigmaYY;

    double den = getAlignmentPredictedGlobal(trackPars,RInv,z,predX,predY);

    double dfNX_dslx  = z*(RInv[1][1]-trackPars[2]*RInv[2][1])-(trackPars[2]*z+trackPars[3])*(-RInv[2][1]);
    double dfNX_dqx   = (RInv[1][1]-trackPars[2]*RInv[2][1]);
    double dfNX_dsly  = (trackPars[0]*z+trackPars[1])*(-RInv[2][1])-(z)*(RInv[0][1]-trackPars[0]*RInv[2][1]);
    double dfNX_dqy   =-(RInv[0][1]-trackPars[0]*RInv[2][1]);

    double dfNY_dslx  = (trackPars[2]*z+trackPars[3])*(-RInv[2][0])-(z+trackPars[1])*(RInv[1][0]-trackPars[2]*RInv[2][0]);
    double dfNY_dqx   =-(RInv[1][0]-trackPars[2]*RInv[2][0]);
    double dfNY_dsly  = (z)*(RInv[0][0]-trackPars[0]*RInv[2][0])-(trackPars[0]*z+trackPars[1])*(-RInv[2][0]);
    double dfNY_dqy   = (RInv[0][0]-trackPars[0]*RInv[2][0]);

    double dfD_dslx   = (-RInv[2][0])*(RInv[1][1]-trackPars[2]*RInv[2][1])-(RInv[1][0]-trackPars[2]*RInv[2][0])*(-RInv[2][1]);
    //double dfD_dqx    = 0;
    double dfD_dsly   = (RInv[0][0]-trackPars[0]*RInv[2][0])*(-RInv[2][1])-(-RInv[2][0])*(RInv[0][1]-trackPars[0]*RInv[2][1]);
    //double dfD_dqy    = 0;
    double dpredx_dslx= dfNX_dslx/den-predX*dfD_dslx/den;
    double dpredx_dqx = dfNX_dqx/den;
    double dpredx_dsly= dfNX_dsly/den-predX*dfD_dsly/den;
    double dpredx_dqy = dfNX_dqy/den;
    double dpredy_dslx= dfNY_dslx/den-predY*dfD_dslx/den;
    double dpredy_dqx = dfNY_dqx/den;
    double dpredy_dsly= dfNY_dsly/den-predY*dfD_dsly/den;
    double dpredy_dqy = dfNY_dqy/den;

    predSigmaXX  =
            dpredx_dslx*dpredx_dslx    * AtVAInv(0,0)
            + dpredx_dqx*dpredx_dqx    * AtVAInv(1,1)
            + dpredx_dsly*dpredx_dsly  * AtVAInv(2,2)
            + dpredx_dqy*dpredx_dqy    * AtVAInv(3,3)
            + 2*dpredx_dslx*dpredx_dqx * AtVAInv(0,1)
            + 2*dpredx_dslx*dpredx_dsly* AtVAInv(0,2)
            + 2*dpredx_dslx*dpredx_dqy * AtVAInv(0,3)
            + 2*dpredx_dqx*dpredx_dsly * AtVAInv(1,2)
            + 2*dpredx_dqx*dpredx_dqy  * AtVAInv(1,3)
            + 2*dpredx_dsly*dpredx_dqy * AtVAInv(2,3);
    predSigmaYY  =
            dpredy_dslx*dpredy_dslx    * AtVAInv(0,0)
            + dpredy_dqx*dpredy_dqx    * AtVAInv(1,1)
            + dpredy_dsly*dpredy_dsly  * AtVAInv(2,2)
            + dpredy_dqy*dpredy_dqy    * AtVAInv(3,3)
            + 2*dpredy_dslx*dpredy_dqx * AtVAInv(0,1)
            + 2*dpredy_dslx*dpredy_dsly* AtVAInv(0,2)
            + 2*dpredy_dslx*dpredy_dqy * AtVAInv(0,3)
            + 2*dpredy_dqx*dpredy_dsly * AtVAInv(1,2)
            + 2*dpredy_dqx*dpredy_dqy  * AtVAInv(1,3)
            + 2*dpredy_dsly*dpredy_dqy * AtVAInv(2,3);

    //predSigmaXX and predSigmaYY are the predicted squared errors at z. To get the error you need to sqrt(predSigmaXX)
    return std::make_pair(predSigmaXX,predSigmaYY);
}

//=======================================================================================
void Detector::test(double* x, double* y, double* z, double* xErr, double* yErr, double* zErr)
{
    std::stringstream ss;
    /*
    double xc1    = 0;
    double yc1    = 0;
    double zc1    = 0;
    double xErrc1 = 100;
    double yErrc1 = 100;
    double zErrc1 = 0;
    double xc2    = 100;
    double yc2    = 100;
    double zc2    = 0;
    double xErrc2 = 100;
    double yErrc2 = 100;
    double zErrc2 = 0;
    fromDetectorToGlobal(&xc1,&yc1,&zc1,&xErrc1,&yErrc1,&zErrc1);
    ss.str("");
    ss << "************************   " << ID_ << "        **************************************";
    STDLINE(ss.str(),ACYellow);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After fromLocalToGlobal1->"
            << " x: " << xc1 << " y: " << yc1 << " z:" << zc1
            << " xErr: " << xErrc1 << " yErr: " << yErrc1 << " zErr:" << zErrc1;
    STDLINE(ss.str(),ACGreen);

    fromDetectorToGlobal(&xc2,&yc2,&zc2,&xErrc2,&yErrc2,&zErrc2);
    ss.str("");
    ss << "************************   " << ID_ << "        **************************************";
    STDLINE(ss.str(),ACYellow);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After fromLocalToGlobal2->"
            << " x: " << xc2 << " y: " << yc2 << " z:" << zc2
            << " xErr: " << xErrc2 << " yErr: " << yErrc2 << " zErr:" << zErrc2;
    STDLINE(ss.str(),ACRed);
    ss.str("");
    ss << "************************   " << ID_ << "        **************************************";
    STDLINE(ss.str(),ACYellow);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After DeltaGlobal       ->"
            << " x: " << xc1-xc2 << " y: " << yc1-yc2 << " z:" << zc1-zc2
            << " xErr: " << xErrc1-xErrc2 << " yErr: " << yErrc1-yErrc2 << " zErr:" << zErrc1-zErrc2;
    STDLINE(ss.str(),ACRed);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After DeltaGlobal square->"
            << " x: " << pow(xc1-xc2,2) << " y: " << pow(yc1-yc2,2) << " z:" << pow(zc1-zc2,2)
            << " Tot: " << sqrt(pow(xc1-xc2,2)+pow(yc1-yc2,2)+pow(zc1-zc2,2)) << " = " << sqrt(2*pow(100,2));
    STDLINE(ss.str(),ACRed);

    fromGlobalToDetector(&xc1,&yc1,&zc1,&xErrc1,&yErrc1,&zErrc1);
    ss.str("");
    ss << "************************   " << ID_ << "        **************************************";
    STDLINE(ss.str(),ACYellow);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After fromGlobalToLocal1->"
            << " x: " << xc1 << " y: " << yc1 << " z:" << zc1
            << " xErr: " << xErrc1 << " yErr: " << yErrc1 << " zErr:" << zErrc1;
    STDLINE(ss.str(),ACGreen);

*/
    double xc    = *x;
    double yc    = *y;
    double zc    = *z;
    double xErrc = *xErr;
    double yErrc = *yErr;
    double zErrc = *zErr;


//    Detector::matrix33Def R = this->getRotationMatrix();


    ss.str("");
    ss << "************************   " << ID_ << "        **************************************";
    STDLINE(ss.str(),ACYellow);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "Original Values->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    STDLINE(ss.str(),ACGreen);

    this->flipPositionLocal(x,y,xErr,yErr);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After flipping ->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    //    STDLINE(ss.str(),ACRed);

    this->translateXY(x,y);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After translate->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    //   STDLINE(ss.str(),ACRed);

    this->translateCorrection(x,y);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After tran corr->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    //   STDLINE(ss.str(),ACGreen);

    xc = *x;
    yc = *y;
    zc = *z;
    xErrc = *xErr;
    yErrc = *yErr;
    zErrc = *zErr;
    this->XRotation(y,z,yErr,zErr);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After XRotation->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    //    STDLINE(ss.str(),ACRed);

    this->YRotation(x,z,xErr,zErr);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After YRotation->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    STDLINE(ss.str(),ACRed);

    this->ZRotation(x,y,xErr,yErr);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After ZRotation->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    //    STDLINE(ss.str(),ACRed);

    this->XYZRotation(&xc,&yc,&zc,&xErrc,&yErrc,&zErrc);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After XYZRot   ->"
                   << " x: " << xc << " y: " << yc << " z:" << zc
                   << " xErr: " << xErrc << " yErr: " << yErrc << " zErr:" << zErrc;
    //    STDLINE(ss.str(),ACRed);

    *z += this->getZPositionTotal();

    ss.str(""); ss << __PRETTY_FUNCTION__ << "Final          ->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    STDLINE(ss.str(),ACPurple);

    //////////////////////////////////////////////////////////////////////////////////////
    ss.str(""); ss << __PRETTY_FUNCTION__ << "Original Values->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    //    STDLINE(ss.str(),ACPurple);
    *z -= this->getZPositionTotal();

    this->ZRotation(x,y,xErr,yErr,true);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After ZRotation->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    //    STDLINE(ss.str(),ACRed);

    this->YRotation(x,z,xErr,zErr,true);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After YRotation->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    //   STDLINE(ss.str(),ACRed);

    this->XRotation(y,z,yErr,zErr,true);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After XRotation->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    STDLINE(ss.str(),ACRed);


    this->XYZRotation(&xc,&yc,&zc,&xErrc,&yErrc,&zErrc,true);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After XYZRot   ->"
                   << " x: " << xc << " y: " << yc << " z:" << zc
                   << " xErr: " << xErrc << " yErr: " << yErrc << " zErr:" << zErrc;
    //    STDLINE(ss.str(),ACRed);

    this->translateXY(x,y,true);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After translate->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    //    STDLINE(ss.str(),ACRed);

    this->translateCorrection(x,y,true);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After tran corr->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    //    STDLINE(ss.str(),ACRed);

    *z = 0;
    this->flipBackPositionLocal(x,y,xErr,yErr);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "Final AfterFlip->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    STDLINE(ss.str(),ACGreen);

    this->fromLocalToGlobal(x,y,z,xErr,yErr,zErr);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After DetToGlob->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    STDLINE(ss.str(),ACPurple);

    this->fromGlobalToLocal(x,y,z,xErr,yErr,zErr);
    ss.str(""); ss << __PRETTY_FUNCTION__ << "After GlobToDet->"
                   << " x: " << *x << " y: " << *y << " z:" << *z
                   << " xErr: " << *xErr << " yErr: " << *yErr << " zErr:" << *zErr;
    STDLINE(ss.str(),ACGreen);

}

//=======================================================================================
void Detector::dump(void)
{
    char buffer[128];
    sprintf (buffer, "Alpha -> Total = %10.2f (%10.2f + %10.2f) deg", getXRotation() + getXRotationCorrection(), getXRotation(), getXRotationCorrection());STDLINE(buffer,ACCyan);
    sprintf (buffer, "Beta  -> Total = %10.2f (%10.2f + %10.2f) deg", getYRotation() + getYRotationCorrection(), getYRotation(), getYRotationCorrection());STDLINE(buffer,ACCyan);
    sprintf (buffer, "Gamma -> Total = %10.2f (%10.2f + %10.2f) deg", getZRotation() + getZRotationCorrection(), getZRotation(), getZRotationCorrection());STDLINE(buffer,ACCyan);
    sprintf (buffer, "X     -> Total = %10.2f (%10.2f + %10.2f) um", 10*(getXPositionTotal()), 10*getXPosition(), 10*getXPositionCorrection());STDLINE(buffer,ACCyan);
    sprintf (buffer, "Y     -> Total = %10.2f (%10.2f + %10.2f) um", 10*(getYPositionTotal()), 10*getYPosition(), 10*getYPositionCorrection());STDLINE(buffer,ACCyan);
    sprintf (buffer, "Z     -> Total = %10.2f (%10.2f + %10.2f) um", 10*(getZPositionTotal()), 10*getZPosition(), 10*getZPositionCorrection());STDLINE(buffer,ACCyan);
}
