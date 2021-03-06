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


#include "ROC.h"

#include <iostream>


// @@@ Hard coded parameters @@@
#define ELECTRONS_NUMBER 329 // [e- / VCal]
#define ADCBITS 10           // Bits of the analog to digital converter
// ============================


using namespace std;

ClassImp(ROC)

//==========================================================================
ROC::ROC(unsigned int position,
         int          chipID,
         unsigned int degrees) :
    rocLengthX_         (0                              )
  ,rocLengthY_         (0                              )
  ,numberOfRows_       (0                              )
  ,numberOfCols_       (0                              )
  ,chipID_             (chipID                         )
  ,orientation_        (0                              )
  ,position_           (position                       )
  ,standardPixelPitch_ (std::pair<double,double>(-1,-1))
  ,calibrationFilePath_(""                             )
{
    this->setOrientation(degrees);
}

//===============================================================================
double ROC::linearFitFunctionROOT(double *x, double *par)
{
    return x[0]*par[0] + par[1];
}

//===============================================================================
double ROC::parabolicFitFunctionROOT(double *x, double *par)
{
    return x[0]*x[0]*par[0] + x[0]*par[1] + par[2];
}

//===============================================================================
double ROC::tanhFitFunctionROOT(double *x, double *par)
{
    return par[0] + par[1]*tanh(par[2]*x[0] + par[3]);
}

//===============================================================================
double ROC::noneFitFunctionROOT(double *x, double */*par*/)
{
    return x[0];
}
//===============================================================================
double ROC::calibrationFitFunction(double *x, double *par, bool isDut)
{
    if      (isDut && calibrationFunctionType_ == "linear"   ) return x[0]*par[0] + par[1];
    else if (isDut && calibrationFunctionType_ == "parabolic") return x[0]*x[0]*par[0] + x[0]*par[1] + par[2];
    else if (isDut && calibrationFunctionType_ == "none"     ) return x[0];
    else                                                       return par[0] + par[1]*tanh(par[2]*x[0] + par[3]);
}

//===============================================================================
double ROC::calibrationFitFunctionInv(double *x, double *par, bool isDut)
{
    if      (isDut && calibrationFunctionType_ == "linear"   ) return (x[0] - par[1]) / par[0];
    else if (isDut && calibrationFunctionType_ == "parabolic")
    {
        if ((par[1]*par[1] - 4.*par[0]*(par[2] - x[0])) >= 0)
            return ((-par[1] + sqrt(par[1]*par[1] - 4.*par[0]*(par[2] - x[0]))) / (2.*par[0]));
        else
            return 0.;
    }
    else if (isDut && calibrationFunctionType_ == "none"     ) return x[0];
    else                                                       return (atanh((x[0] - par[0]) / par[1]) - par[3]) / par[2];
}

//==========================================================================
void ROC::setOrientation(unsigned int degrees)
{
    if (degrees == 0 || degrees == 180) orientation_ = degrees;
    else if(degrees == 90 || degrees == 270)
    {
        STDLINE("ERROR: Only 0 and 180 degrees rotations are supported for a ROC!", ACRed);
        exit(EXIT_FAILURE);
    }
    else
    {
        STDLINE("ERROR: Only 0 and 180 degrees rotations are supported for a ROC!", ACRed);
        exit(EXIT_FAILURE);
    }
}

//==========================================================================
bool ROC::goodRow(unsigned int row)
{
    if (row < numberOfRows_) return true;
    else
    {
        ss_.str("");
        ss_ << "WARNING: no pixel found with col number " <<  row;
        STDLINE(ss_.str(), ACRed);
        return false;
    }
}

//==========================================================================
bool ROC::goodCol(unsigned int col)
{
    if (col < numberOfCols_) return true;
    else
    {
        ss_.str("");
        ss_ << "WARNING: no pixel found with col number " <<  col;
        STDLINE(ss_.str(), ACRed);
        return false;
    }
}

//==========================================================================
void ROC::setRowPitchVector(void)
{
    if (numberOfRows_ == 0 || standardPixelPitch_.first == 0) return;
    rowPitches_.clear();
    rowLowEdge_.clear();
    rowPitches_.resize(numberOfRows_, standardPixelPitch_.first);
    rowLowEdge_.resize(numberOfRows_,0);
    for (nonStandardPitchMapDef::iterator it = nonStandardRowPitch_.begin(); it != nonStandardRowPitch_.end(); it++)
    {
        if(it->first < rowPitches_.size())
        {
            rowPitches_[it->first] = it->second;
        }
    }
    rowLowEdge_[0] = 0;
    for (unsigned int r=1; r<rowPitches_.size(); r++)
    {
        rowLowEdge_[r] = rowLowEdge_[r-1] + rowPitches_[r-1];
    }
    rocLengthY_ = rowLowEdge_[rowLowEdge_.size()-1] + rowPitches_[rowPitches_.size()-1];
}

//==========================================================================
void ROC::setCalibrationFunctionType(string type)
{
    calibrationFunctionType_ = type ;
}
//==========================================================================
void ROC::setColPitchVector(void)
{
    if (numberOfCols_ == 0 || standardPixelPitch_.second == 0) return;
    colPitches_.clear();
    colLowEdge_.clear();
    colPitches_.resize(numberOfCols_, standardPixelPitch_.second);
    colLowEdge_.resize(numberOfCols_,0);
    for (nonStandardPitchMapDef::iterator it = nonStandardColPitch_.begin(); it != nonStandardColPitch_.end(); it++)
    {
        if(it->first < colPitches_.size())
        {
            colPitches_[it->first] = it->second;
        }
    }
    colLowEdge_[0] = 0;
    for (unsigned int c=1; c<colPitches_.size(); c++)
    {
        colLowEdge_[c] = colLowEdge_[c-1] + colPitches_[c-1];
    }
    rocLengthX_ = colLowEdge_[colLowEdge_.size()-1] + colPitches_[colPitches_.size()-1];
}

//==========================================================================
void ROC::setNumberOfRowsCols(unsigned int numberOfRows, unsigned int numberOfCols)
{
    numberOfRows_ = numberOfRows;
    setRowPitchVector();
    numberOfCols_ = numberOfCols;
    setColPitchVector();
}

//==========================================================================
void ROC::setNumberOfRows(unsigned int numberOfRows)
{
    numberOfRows_ = numberOfRows;
    setRowPitchVector();
}

//==========================================================================
void ROC::setNumberOfCols(unsigned int numberOfCols)
{
    numberOfCols_ = numberOfCols;
    setColPitchVector();
}

//==========================================================================
void ROC::setStandardPixPitch(double row_cm, double col_cm)
{
    standardPixelPitch_ = std::make_pair(row_cm,col_cm);
    setRowPitchVector();
    setColPitchVector();
}

//==========================================================================
void ROC::setOneRowPitch(unsigned int row, double row_cm)
{
    nonStandardRowPitch_[row] = row_cm;
    setRowPitchVector();
}

//==========================================================================
void ROC::setOneColPitch(unsigned int col, double col_cm)
{
    nonStandardColPitch_[col] = col_cm;
    setColPitchVector();
}

//==========================================================================
double ROC::getPixelCenterLocalX( unsigned int pixelCol)
{
    if (!goodCol(pixelCol)) return -1;
    return (this->getPixelLowEdgeLocalX(pixelCol) + this->getPixelPitchLocalX(pixelCol)/2.);
}

//==========================================================================
double ROC::getPixelCenterLocalY(unsigned int pixelRow)
{
    if (!goodRow(pixelRow)) return -1;
    return (this->getPixelLowEdgeLocalY(pixelRow) + this->getPixelPitchLocalY(pixelRow)/2.);
}

//==========================================================================
double ROC::getPixelLowEdgeLocalX(unsigned int pixelCol)
{
    if (!goodCol(pixelCol)) return -1;

    if (orientation_ == 0)
        return colLowEdge_[pixelCol];
    else if(orientation_ == 180)
        return rocLengthX_ - colLowEdge_[pixelCol] - colPitches_[pixelCol];
    else
    {
        ss_.str("");
        ss_ << "WARNING: orientation not supported " <<  orientation_;
        STDLINE(ss_.str(), ACRed);
        return -1;
    }
}

//==========================================================================
double ROC::getPixelLowEdgeLocalY(unsigned int pixelRow)
{
    if (!goodRow(pixelRow)) return -1;

    if (orientation_ == 0)
        return rowLowEdge_[pixelRow];
    else if(orientation_ == 180)
        return rocLengthY_ - rowLowEdge_[pixelRow] - rowPitches_[pixelRow];
    else
    {
        ss_.str("");
        ss_ << "WARNING: orientation not supported " <<  orientation_;
        STDLINE(ss_.str(), ACRed);
        return -1;
    }
}

//==========================================================================
double ROC::getPixelHiEdgeLocalX(unsigned int pixelCol)
{
    if (!goodCol(pixelCol)) return -1;
    return (this->getPixelLowEdgeLocalX(pixelCol) + this->getPixelPitchLocalX(pixelCol));
}

//==========================================================================
double ROC::getPixelHiEdgeLocalY(unsigned int pixelRow)
{
    if (!goodRow(pixelRow)) return -1;
    return (this->getPixelLowEdgeLocalY(pixelRow) + this->getPixelPitchLocalY(pixelRow));
}

//==========================================================================
double ROC::getPixelPitchLocalX(unsigned int pixelCol)
{
    if (!goodCol(pixelCol)) return -1;
    return colPitches_[pixelCol];
}

//==========================================================================
double ROC::getPixelPitchLocalY(unsigned int pixelRow)
{
    if (!goodRow(pixelRow)) return -1;
    return rowPitches_[pixelRow];
}

//==========================================================================
double ROC::getLengthLocalX(void)
{
    return rocLengthX_;
}

//==========================================================================
double ROC::getLengthLocalY (void)
{
    return rocLengthY_;
}

//=============================================================================
bool ROC::calibratePixel(int row, int col, int adc, int& charge, bool isDut, bool verbose)
{
    double newAdc[1];
    double maxVCal[1] = {pow(2,ADCBITS) * ELECTRONS_NUMBER};

    //>> wsi Revert COL HERE TO GET THE RIGHT CALIBRATION
    if (this->getNumberOfCols() == 312)
    {
        if (col%4 == 3)
            col = (col+1)/6-1;
        else if (col%4 == 0)
            col = col/6;
        else if (col%4 == 2)
            col = (col+2)/6-2;
        else if (col%4 == 1)
            col = (col-1)/6+1;
    }
    //<< 19/12/17

    
    //>> wsi SENSOR -> ROC (50x50)
    if (this->getNumberOfCols() == 156 && this->getNumberOfRows() == 160)
    {


      if (col<9 || row>155) return false;
      if ((col-9)%18==8 || (col-9)%18==9)
      {
//          std::cout << "ANDRE: before ROC: [" << col << ", " << row << "]" << std::endl;
        if      ((col-9)%18==8) col = 3+(col-9)/18*6+2;
        else if ((col-9)%18==9) col = 3+(col-9)/18*6+3;

        if      (row%12 == 4) row = row/12*6+1;
        else if (row%12 == 5) row = row/12*6+2;
        else if (row%12 == 6) row = row/12*6+3;
        else if (row%12 == 7) row = row/12*6+4;
        else {
//            std::cout << "ANDRE: after ROC: [" << col << ", " << row << "]" << std::endl;
            return false;}
//        std::cout << "ANDRE: after ROC: [" << col << ", " << row << "]" << std::endl;
      }
      else if ((col-9)%18==7 || (col-9)%18==10)
      {
//          std::cout << "ANDRE: before ROC: [" << col << ", " << row << "]" << std::endl;
        if      ((col-9)%18==7) col = 3+(col-9)/18*6+1;
        else if ((col-9)%18==10) col = 3+(col-9)/18*6+4;

        if      (row%12 == 4) row = row/12*6+0;
        else if (row%12 == 5) row = row/12*6+2;
        else if (row%12 == 6) row = row/12*6+3;
        else if (row%12 == 7) row = row/12*6+5;
        else {
//            std::cout << "ANDRE: after ROC: [" << col << ", " << row << "]" << std::endl;
            return false;}
//        std::cout << "ANDRE: after ROC: [" << col << ", " << row << "]" << std::endl;
      }

      else {return false;}

      /* seems not be working... make some changes below
      if (col%6 == 4 || col%6 == 1)
      {
        // COL
        if (col%6 == 4) col = (col+2)/3-2;
        if (col%6 == 1) col = (col+8)/3-2;

        // ROW
        if      (row%6 == 4) row = row/2-2;
        else if (row%6 == 5) row = (row+3)/2-2;
        else if (row%6 == 0) row = (row+4)/2-2;
        else if (row%6 == 1) row = (row+7)/2-2;
        else  {return false;}

      }

      else if (col%6 == 5 || col%6 == 0)
      {
        // COL
        if (col%6 == 5) col = (col+4)/3-2;
        if (col%6 == 0) col = (col+6)/3-2;

        // ROW
        if      (row%6 == 4) row = (row+2)/2-2;
        else if (row%6 == 5) row = (row+3)/2-2;
        else if (row%6 == 0) row = (row+4)/2-2;
        else if (row%6 == 1) row = (row+5)/2-2;
        else  {return false;}

      }

      else {return false;}

      // No more valid short pitch pixels beyond column 50
      if (col >= 50) {return false;}
      */
      

////      col -= 6; /*row -=4;*/
////      /*if (col%6 == 4 || col%6 == 1)
////      {
////        // COL
////        if (col%6 == 4) col = (col+2)/3;
////        if (col%6 == 1) col = (col+8)/3;
////
////        // ROW
////        if      (row%6 == 4) row = row/2-2;
////        else if (row%6 == 5) row = (row+3)/2-2;
////        else if (row%6 == 0) row = (row+4)/2-2;
////        else if (row%6 == 1) row = (row+7)/2-2;
////        else  {return false;}
////
////      }
////
////      else*/ if (col%6 == 5 || col%6 == 0)
////      {
////        // COL
////        if (col%6 == 5) col = (col+4)/3;
////        if (col%6 == 0) col = (col+6)/3;
////
////        // ROW
////        /*if      (row%6 == 4) row = (row+2)/2-2;
////        else*/ if (row%6 == 5) row = (row+3)/2-2;
////        else if (row%6 == 0) row = (row+4)/2-2;
//////        else if (row%6 == 1) row = (row+5)/2-2;
////        else  {return false;}
////
////      }
////
////      else {return false;}
////
////      // No more valid short pitch pixels within column 4
////      if (col < 4) {return false;}


    }
    //<< 14/1/18

    newAdc[0] = adc;
    double *par = this->getCalibrationFunction(row, col);

    if (par != 0)
    {
        if (newAdc[0] >      ROC::calibrationFitFunction(maxVCal, par, isDut))
            newAdc[0] = (int)ROC::calibrationFitFunction(maxVCal, par, isDut);

        charge = (int)ROC::calibrationFitFunctionInv(newAdc , par, isDut);
        if (verbose) {
            std::cout<<"\nWSI newAdc: "<<newAdc[0]<<" par[0]"<<par[0]<<" par[1]: "<<par[1]<<" => charge: "<<charge<<" Col#:"<<this->getNumberOfCols()<<std::endl;
        }
        //cout<<__LINE__<<__PRETTY_FUNCTION__<<" isDut: "<<isDut<<" charge: "<<charge<<endl;
        return true;
    }
    else
    {
        charge = adc;
        //cout<<__LINE__<<__PRETTY_FUNCTION__<<" isDut: "<<isDut<<" charge: "<<charge<<endl;
        return false;

    }
}

//=============================================================================
bool ROC::isPixelCalibrated(int row, int col)
{
    //>> wsi 21/12/17
    if (this->getNumberOfCols()==312)
    {
        //std::cout<<"{'pos': ("<<col<<","<<row<<"), 'isCalibrated':"<<(pixelCalibrationFunctionTmp_.count(row) && pixelCalibrationFunctionTmp_[row].count(col))<<"}"<<std::endl;
        if (col%4 == 3)
            col = (col+1)/6-1;
        else if (col%4 == 0)
            col = col/6;
        else if (col%4 == 2)
            col = (col+2)/6-2;
        else if (col%4 == 1)
            col = (col-1)/6+1;
    }
    //<< wsi 21/12/17

    //>> wsi SENSOR -> ROC (50x50)
    if (this->getNumberOfCols() == 156 && this->getNumberOfRows() == 160)
    {
      if (col<9 || row>155) return false;
      if ((col-9)%18==8 || (col-9)%18==9)
      {
        if      ((col-9)%18==8) col = 3+(col-9)/18*6+2;
        else if ((col-9)%18==9) col = 3+(col-9)/18*6+3;

        if      (row%12 == 4) row = row/12*6+1;
        else if (row%12 == 5) row = row/12*6+2;
        else if (row%12 == 6) row = row/12*6+3;
        else if (row%12 == 7) row = row/12*6+4;
        else {return false;}
      }
      else if ((col-9)%18==7 || (col-9)%18==10)
      {
        if      ((col-9)%18==7) col = 3+(col-9)/18*6+1;
        else if ((col-9)%18==10) col = 3+(col-9)/18*6+4;

        if      (row%12 == 4) row = row/12*6+0;
        else if (row%12 == 5) row = row/12*6+2;
        else if (row%12 == 6) row = row/12*6+3;
        else if (row%12 == 7) row = row/12*6+5;
        else {return false;}
      }

      else {return false;}

      /*  see above.
      if (col%6 == 4 || col%6 == 1)
      {
        // COL
        if (col%6 == 4) col = (col+2)/3-2;
        if (col%6 == 1) col = (col+8)/3-2;

        // ROW
        if      (row%6 == 4) row = row/2-2;
        else if (row%6 == 5) row = (row+3)/2-2;
        else if (row%6 == 0) row = (row+4)/2-2;
        else if (row%6 == 1) row = (row+7)/2-2;
        else  {return false;}

      }

      else if (col%6 == 5 || col%6 == 0)
      {
        // COL
        if (col%6 == 5) col = (col+4)/3-2;
        if (col%6 == 0) col = (col+6)/3-2;

        // ROW
        if      (row%6 == 4) row = (row+2)/2-2;
        else if (row%6 == 5) row = (row+3)/2-2;
        else if (row%6 == 0) row = (row+4)/2-2;
        else if (row%6 == 1) row = (row+5)/2-2;
        else  {return false;}

      }

      else {return false;}

      // No more valid short pitch pixels beyond column 50
      if (col >= 50) {return false;}
      */


////      col -= 6; /*row -=4;*/
////      /*if (col%6 == 4 || col%6 == 1)
////      {
////        // COL
////        if (col%6 == 4) col = (col+2)/3;
////        if (col%6 == 1) col = (col+8)/3;
////
////        // ROW
////        if      (row%6 == 4) row = row/2-2;
////        else if (row%6 == 5) row = (row+3)/2-2;
////        else if (row%6 == 0) row = (row+4)/2-2;
////        else if (row%6 == 1) row = (row+7)/2-2;
////        else  {return false;}
////
////      }
////
////      else*/ if (col%6 == 5 || col%6 == 0)
////      {
////        // COL
////        if (col%6 == 5) col = (col+4)/3;
////        if (col%6 == 0) col = (col+6)/3;
////
////        // ROW
////        /*if      (row%6 == 4) row = (row+2)/2-2;
////        else*/ if (row%6 == 5) row = (row+3)/2-2;
////        else if (row%6 == 0) row = (row+4)/2-2;
//////        else if (row%6 == 1) row = (row+5)/2-2;
////        else  {return false;}
////
////      }
////
////      else {return false;}
////
////      // No more valid short pitch pixels within column 4
////      if (col < 4) {return false;}



    }
    //<< 14/1/18


    if (pixelCalibrationFunctionTmp_.count(row) && pixelCalibrationFunctionTmp_[row].count(col))
        return true;
    else
        return false;
}

//============================================================================
void ROC::setCalibrationFunction(int row, int col, double *par, double */*cov*/)
{
    if (pixelCalibrationFunctionTmp_.count(row) && pixelCalibrationFunctionTmp_[row].count(col))
        pixelCalibrationFunctionTmp_.clear();

    if(par != NULL)
    {
        for(int i=0; i < 4; i++)
        {
            pixelCalibrationFunctionTmp_[row][col].push_back(par[i]);
        }
    }
}

//============================================================================
double* ROC::getCalibrationFunction(int row, int col)
{   
    if (pixelCalibrationFunctionTmp_.count(row) && pixelCalibrationFunctionTmp_[row].count(col))
        for(unsigned int i = 0; i < pixelCalibrationFunctionTmp_[row][col].size(); i++)
            par_[i] = pixelCalibrationFunctionTmp_[row][col][i] ;
    else
        return 0;

    return par_;
}

//============================================================================
double ROC::getCalibrationError(int /*row*/, int /*col*/, int /*adc*/)
{
    return -1;
}
