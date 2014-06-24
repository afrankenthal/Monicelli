/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef ROC_H
#define ROC_H

#include <string>
#include <map>
#include <string>
#include <vector>
#include <TObject.h>

#include <Math/SMatrix.h>
#include <Math/SVector.h>

#include "MessageTools.h"

class ROC : public TObject
{
 public:
    ROC(unsigned int position=0, int chipID=-1, unsigned int degrees=0);
   ~ROC(void) {;}

    //only 0, 90, 180, 270
    // Y
    // ^
    // |
    // |-------------|  //an example of chip orientations
    // |      max row|
    // |            ^| 90 degrees
    // |max col...< 0|
    // |-------------|
    // |max col...< 0|
    // |            :| 180 degrees
    // |      max row|
    // |-------------|
    // |0 >...max col|
    // |:            | 270 degrees
    // |max row      |
    // |-------------|
    // |max row      |
    // |^            | 0 degrees
    // |0 >...max col|
    // o--------------------->X
    // Z


//   typedef   std::map< int, std::map<int, double> >                                  covMatMapDef                  ;
//   typedef   std::pair<double*, ROC::covMatMapDef>                                   fitResultPairDef              ;

   static double calibrationFitFunction   (double * x, double * par                       );
          double calibrationFitFunctionInv(double * x, double * par                       );
   bool          calibratePixel           (int row, int col, int adc, int& charge         );

   void         setCalibrationFilePath    (std::string path                               ) {calibrationFilePath_= path; }
//   void         setCalibrationFunction    (int row, int col, double *par, double *cov, int nPars) ;
   void         setCalibrationFunction    (int row, int col, double *par, double *cov) ;
   void         setStandardPixPitch       (double       raw_cm  , double col_cm           ) ;
   void         setOneRowPitch            (unsigned int raw     , double row_cm           ) ;
   void         setOneColPitch            (unsigned int col     , double col_cm           ) ;
   void         setOrientation            (unsigned int degrees                           ) ;
   void         setNumberOfRowsCols       (unsigned int maxRows , unsigned int maxCols    ) ;
   void         setNumberOfRows           (unsigned int maxRows                           ) ;
   void         setNumberOfCols           (unsigned int maxCols                           ) ;
   void         setLocalXPosition         (unsigned int xLocalPosition		              ) {xLocalPosition_ = xLocalPosition;}
   void         setLocalYPosition         (unsigned int yLocalPosition    		          ) {yLocalPosition_ = yLocalPosition;}
   void         setFirstRow 	          (unsigned int firstRow	   		              ) {firstRow_  = firstRow;           }
   void         setLastRow  	          (unsigned int lastRow	   		                  ) {lastRow_   = lastRow;            }
   void         setFirstCol 	          (unsigned int firstCol	   		              ) {firstCol_  = firstCol;           }
   void         setLastCol  	          (unsigned int lastCol	   		                  ) {lastCol_   = lastCol;            }

   std::string  getCalibrationFilePath    (void                                           ) {return calibrationFilePath_;     }
   int          getID                     (void                                           ) {return chipID_;                  }
   unsigned int getOrientation            (void                                           ) {return orientation_;             }
   unsigned int getPosition               (void                                           ) {return position_;                }
   double       getPixelCenterLocalX      (unsigned int col                               ) ;
   double       getPixelCenterLocalY      (unsigned int row                               ) ;
   double       getPixelHiEdgeLocalX      (unsigned int col                               ) ;
   double       getPixelHiEdgeLocalY      (unsigned int row                               ) ;
   double       getPixelLowEdgeLocalX     (unsigned int col                               ) ;
   double       getPixelLowEdgeLocalY     (unsigned int row                               ) ;
   double       getPixelPitchLocalX       (unsigned int col                               ) ;
   double       getPixelPitchLocalY       (unsigned int row                               ) ;
   double       getLengthLocalX           (void                                           ) ;
   double       getLengthLocalY           (void                                           ) ;

   double       getCalibrationError       (int row, int col ,int adc                      ) ;
   double*      getCalibrationFunction    (int row, int col                               ) ;
   bool         isPixelCalibrated         (int row, int col                               ) ;

   unsigned int getNumberOfRows           (void                                           ) {return numberOfRows_;       }
   unsigned int getNumberOfCols           (void                                           ) {return numberOfCols_;       }
   unsigned int getPositionLocalX         (void					                          ) {return xLocalPosition_;     }
   unsigned int getPositionLocalY         (void					                          ) {return yLocalPosition_;     }
   unsigned int getFirstRow 	          (void					                          ) {return firstRow_;           }
   unsigned int getLastRow  	          (void					                          ) {return lastRow_;            }
   unsigned int getFirstCol 	          (void					                          ) {return firstCol_;           }
   unsigned int getLastCol  	          (void					                          ) {return lastCol_;            }



 private:

   typedef   std::map< int,std::map<int, ROOT::Math::SMatrix<double,4> > >         pixelCalibrationsCovDef     ;
                      //row picth   , col pitch
   typedef   std::pair<double, double      >                                       rowColPitchPairDef          ;
                 //key: row or col    Pitch
   typedef   std::map<unsigned int , double>                                       nonStandardPitchMapDef      ;
   //                  row         col    pars
   typedef   std::map< int,std::map<int, double*> >                                pixelCalibrationsMapDef     ;

   typedef   std::map< int,std::map<int, std::vector<double> > >                   pixelCalibrationsTmpMapDef  ;

   bool goodRow          (unsigned int row);
   bool goodCol          (unsigned int col);
   void setRowPitchVector(void);
   void setColPitchVector(void);
   std::vector<double>           rowPitches_;
   std::vector<double>           colPitches_;
   std::vector<double>           rowLowEdge_;
   std::vector<double>           colLowEdge_;
   double                        rocLengthX_;
   double                        rocLengthY_;

   unsigned int                  numberOfRows_                        ;//the number of rows for a chip
                                                                       //it will be used to check merged data integrity
   unsigned int                  numberOfCols_                        ;//the number of columns for a chip
                                                                       //it will be used to check merged data integrity

   int                           chipID_                              ;
   unsigned int                  orientation_                         ;
   unsigned int                  position_                            ;
   rowColPitchPairDef            standardPixelPitch_                  ;
   nonStandardPitchMapDef        nonStandardRowPitch_                 ;
   nonStandardPitchMapDef        nonStandardColPitch_                 ;
   std::string                   calibrationFilePath_                 ;
//   pixelCalibrationsMapDef       pixelCalibrationFunction_            ;//! temporary state value
   pixelCalibrationsTmpMapDef    pixelCalibrationFunctionTmp_         ;
   pixelCalibrationsCovDef       pixelCalibrationCovMat_              ;
   double                        par_[4]                              ;

   //Roc properties set by the detector (plaquette) the rocs belong to
   unsigned int                  xLocalPosition_;
   unsigned int                  yLocalPosition_;
   unsigned int                  firstRow_;
   unsigned int                  lastRow_;
   unsigned int                  firstCol_;
   unsigned int                  lastCol_;


   std::stringstream        ss_                                  ;//! temporary state value

   ClassDef(ROC,5)

} ;

#endif // ROC_H
