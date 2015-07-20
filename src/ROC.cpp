/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include "ROC.h"
#include <iostream>

using namespace std;

ClassImp(ROC)

//==========================================================================
ROC::ROC(unsigned int position, int chipID, unsigned int degrees) :
        rocLengthX_(0)
        ,rocLengthY_(0)
        ,numberOfRows_(0)
        ,numberOfCols_(0)
        ,chipID_(chipID)
        ,orientation_(0)
        ,position_(position)
        ,standardPixelPitch_(std::pair<double,double>(-1,-1))
        ,calibrationFilePath_("")
{
    this->setOrientation(degrees);
}

//===============================================================================
double ROC::calibrationFitFunction(double *x, double *par)
{
    return par[0]+par[1]*tanh(par[2]*x[0]+par[3]);
}

//===============================================================================
double ROC::calibrationFitFunctionInv(double *x, double *par)
{
    return (atanh((x[0]-par[0])/par[1]) - par[3])/par[2];
}

//==========================================================================
void ROC::setOrientation(unsigned int degrees)
{
    if (degrees==0 || degrees==180)
        orientation_ = degrees;
    else if(degrees==90 || degrees==270)
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
    if ( row < numberOfRows_ )
        return true;
    else{
        ss_.str("");
        ss_ << "WARNING: no pixel found with col number " <<  row;
        STDLINE(ss_.str(), ACRed);
        return false;
    }
}

//==========================================================================
bool ROC::goodCol(unsigned int col)
{
    if ( col < numberOfCols_ )
        return true;
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
    if(numberOfRows_ == 0 || standardPixelPitch_.first == 0) return;
    rowPitches_.clear();
    rowLowEdge_.clear();
    rowPitches_.resize(numberOfRows_, standardPixelPitch_.first);
    rowLowEdge_.resize(numberOfRows_,0);
    for(nonStandardPitchMapDef::iterator it=nonStandardRowPitch_.begin(); it != nonStandardRowPitch_.end(); it++)
    {
        if(it->first < rowPitches_.size())
        {
            rowPitches_[it->first] = it->second;
        }
    }
    rowLowEdge_[0] = 0;
    for(unsigned int r=1; r<rowPitches_.size(); r++)
    {
        rowLowEdge_[r] = rowLowEdge_[r-1] + rowPitches_[r-1];
    }
    rocLengthY_ = rowLowEdge_[rowLowEdge_.size()-1] + rowPitches_[rowPitches_.size()-1];
//    cout << __PRETTY_FUNCTION__ << "Roc lengthY: " << rocLengthX_ << " row 0 pitch: " <<  rowPitches_[rowPitches_.size()-1] << endl;
}

//==========================================================================
void ROC::setColPitchVector(void)
{
    if(numberOfCols_ == 0 || standardPixelPitch_.second == 0) return;
    colPitches_.clear();
    colLowEdge_.clear();
    colPitches_.resize(numberOfCols_, standardPixelPitch_.second);
    colLowEdge_.resize(numberOfCols_,0);
    for(nonStandardPitchMapDef::iterator it=nonStandardColPitch_.begin(); it != nonStandardColPitch_.end(); it++)
    {
        if(it->first < colPitches_.size())
        {
            colPitches_[it->first] = it->second;
        }
    }
    colLowEdge_[0] = 0;
    for(unsigned int c=1; c<colPitches_.size(); c++)
    {
        colLowEdge_[c] = colLowEdge_[c-1] + colPitches_[c-1];
    }
    rocLengthX_ = colLowEdge_[colLowEdge_.size()-1] + colPitches_[colPitches_.size()-1];
//    cout << __PRETTY_FUNCTION__ << "Roc lengthX: " << rocLengthX_ << " col 0 pitch: " <<  colPitches_[0] << " col 51 pitch: " <<  colPitches_[colPitches_.size()-1] << endl;
}

//==========================================================================
void ROC::setNumberOfRowsCols(unsigned int numberOfRows, unsigned int numberOfCols)
{
    numberOfRows_=numberOfRows;
    setRowPitchVector();
    numberOfCols_=numberOfCols;
    setColPitchVector();
}

//==========================================================================
void ROC::setNumberOfRows(unsigned int numberOfRows)
{
    numberOfRows_=numberOfRows;
    setRowPitchVector();
}

//==========================================================================
void ROC::setNumberOfCols(unsigned int numberOfCols)
{
    numberOfCols_=numberOfCols;
    setColPitchVector();
}

//==========================================================================
void ROC::setStandardPixPitch(double row_cm, double col_cm)
{
    standardPixelPitch_=std::make_pair(row_cm,col_cm);
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
double ROC::getPixelCenterLocalX( unsigned int pixelCol )
{
    if ( !goodCol(pixelCol) ) return -1;
    return (this->getPixelLowEdgeLocalX(pixelCol) + this->getPixelPitchLocalX(pixelCol)/2.);
}

//==========================================================================
double ROC::getPixelCenterLocalY(unsigned int pixelRow )
{
    if ( !goodRow(pixelRow) ) return -1;
    return (this->getPixelLowEdgeLocalY(pixelRow) + this->getPixelPitchLocalY(pixelRow)/2.);
}

//==========================================================================
double ROC::getPixelLowEdgeLocalX(unsigned int pixelCol )
{
    if ( !goodCol(pixelCol) ) return -1;

    if( orientation_ == 0)
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
double ROC::getPixelLowEdgeLocalY(unsigned int pixelRow )
{
    if ( !goodRow(pixelRow) ) return -1;

    if( orientation_ == 0)
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
double ROC::getPixelHiEdgeLocalX(unsigned int pixelCol )
{
    if ( !goodCol(pixelCol) ) return -1;
    return (this->getPixelLowEdgeLocalX(pixelCol) + this->getPixelPitchLocalX(pixelCol));
}

//==========================================================================
double ROC::getPixelHiEdgeLocalY(unsigned int pixelRow )
{
    if ( !goodRow(pixelRow) ) return -1;
    return (this->getPixelLowEdgeLocalY(pixelRow) + this->getPixelPitchLocalY(pixelRow));
}

//==========================================================================
double ROC::getPixelPitchLocalX(unsigned int pixelCol )
{
    if ( !goodCol(pixelCol) ) return -1;
    return colPitches_[pixelCol];
}

//==========================================================================
double ROC::getPixelPitchLocalY(unsigned int pixelRow )
{
    if ( !goodRow(pixelRow) ) return -1;
    return rowPitches_[pixelRow];
}

//==========================================================================
double ROC::getLengthLocalX(void  )
{
    return rocLengthX_;
}

//==========================================================================
double ROC::getLengthLocalY(void  )
{
    return rocLengthY_;
}

//=============================================================================
bool ROC::calibratePixel(int row, int col, int adc, int& charge)
{
    double newAdc[1];
    //double maxVCal[1] = {255*421};
    double maxVCal[1] = {255*332};

    newAdc[0] = adc;
    double *par = this->getCalibrationFunction(row, col);
    if(par != 0)
    {
        if(newAdc[0] > ROC::calibrationFitFunction(maxVCal, par))
            newAdc[0] = (int)ROC::calibrationFitFunction(maxVCal, par);
        charge = (int)ROC::calibrationFitFunctionInv(newAdc, par);//, true);
        return true;
    }
    else
    {
        //newAdc[0] = abs(adc-400);
        //charge = -999999;
        charge = adc;
        return false;
    }
}

//=============================================================================
bool ROC::isPixelCalibrated(int row, int col)
{
    if( pixelCalibrationFunctionTmp_.count(row) && pixelCalibrationFunctionTmp_[row].count(col))
        return true;
    else
        return false;
}

//============================================================================
void ROC::setCalibrationFunction(int row, int col, double *par, double *cov)//, int nPars )
{
    if( pixelCalibrationFunctionTmp_.count(row) && pixelCalibrationFunctionTmp_[row].count(col))
        pixelCalibrationFunctionTmp_.clear();

    if(par != NULL)
    {
        for(int i=0; i < 4; i++)
        {
            pixelCalibrationFunctionTmp_[row][col].push_back(par[i]);
            cov = 0;
        }
    }
}

//============================================================================
double* ROC::getCalibrationFunction(int row, int col)
{   
    if( pixelCalibrationFunctionTmp_.count(row) && pixelCalibrationFunctionTmp_[row].count(col))
        for(unsigned int i=0; i < pixelCalibrationFunctionTmp_[row][col].size(); i++)
            par_[i] = pixelCalibrationFunctionTmp_[row][col][i] ;
    else
        return 0;

    return par_;
}

//============================================================================
double ROC::getCalibrationError(int /*row*/, int /*col*/, int /*adc*/)
{
    /*
    double err;
    if( pixelCalibrationFunctionTmp_.count(row) && pixelCalibrationFunctionTmp_[row].count(col) )
    {
        double arg  = ( adc - pixelCalibrationFunctionTmp_[row][col][0] )/pixelCalibrationFunctionTmp_[row][col][1];
        double der0 = -1./( pixelCalibrationFunctionTmp_[row][col][2]*pow(cosh(arg),2) );
        double der1 = -arg/( pow(cosh(arg),2)*pixelCalibrationFunctionTmp_[row][col][2]*pixelCalibrationFunctionTmp_[row][col][1] );
        double der2 = ( pixelCalibrationFunctionTmp_[row][col][3] - tanh(arg) ) / ( pixelCalibrationFunctionTmp_[row][col][2]*pixelCalibrationFunctionTmp_[row][col][2] );
        double der3 = -1./pixelCalibrationFunctionTmp_[row][col][2];

        ss_.str("");
        ss_ << "par0: " << pixelCalibrationFunctionTmp_[row][col][0]
            << " par1: " << pixelCalibrationFunctionTmp_[row][col][1]
            << " par2: " << pixelCalibrationFunctionTmp_[row][col][2]
            << " par3: " << pixelCalibrationFunctionTmp_[row][col][3];
        //STDLINE(ss_.str(), ACCyan);
        ss_.str("");
        ss_ << "der0: " << der0 << " der1: " << der1 << " der2: " << der2 << " der3: " << der3;
        //STDLINE(ss_.str(), ACPurple);
        ss_.str("");
        ss_ << "eer0: " << pixelCalibrationCovMat_[row][col][0][0] << " eer1: " << pixelCalibrationCovMat_[row][col][1][1]
            << " eer2: " << pixelCalibrationCovMat_[row][col][2][2] << " eer3: " << pixelCalibrationCovMat_[row][col][3][3];
        //STDLINE(ss_.str(), ACYellow);

        err = sqrt(der0*der0*pixelCalibrationCovMat_[row][col][0][0]   +
                   der1*der1*pixelCalibrationCovMat_[row][col][1][1]   +
                   der2*der2*pixelCalibrationCovMat_[row][col][2][2]   +
                   der3*der3*pixelCalibrationCovMat_[row][col][3][3]   +
                   2*der0*der1*pixelCalibrationCovMat_[row][col][0][1] +
                   2*der0*der2*pixelCalibrationCovMat_[row][col][0][2] +
                   2*der0*der3*pixelCalibrationCovMat_[row][col][0][3] +
                   2*der1*der2*pixelCalibrationCovMat_[row][col][1][2] +
                   2*der1*der3*pixelCalibrationCovMat_[row][col][1][3] +
                   2*der2*der3*pixelCalibrationCovMat_[row][col][2][3]  );

        ss_.str("");
        ss_ << "err " <<  err;
        //STDLINE(ss_.str(), ACRed);

    }
    else return -1;

    return err;
    */
    return -1;
}
