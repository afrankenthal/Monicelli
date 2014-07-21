/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef DETECTOR_H
#define DETECTOR_H

#include <map>
#include <math.h>
#include <string>
#include <Math/SMatrix.h>
#include <Math/SVector.h>
#include <TObject.h>

#include "ROC.h"
#include "MessageTools.h"

class Detector: public TObject
{
public:
    Detector(std::string ID = "", bool isDUT = false, bool isStrip = false);
    ~Detector(void);

    ROC* addROC(unsigned int ROCPosition, int ROCID , unsigned int rotationDegrees = 0);

    // Y
    // ^
    // |
    // -------------------------------------------       //THE example of ROCs positions
    // |             |             |             |       //in a plaquette
    // |      6      |      7      |      8      |
    // |             |             |             |
    // |-------------|-------------|-------------|
    // |             |             |             |
    // |      5      |      4      |      3      |
    // |             |             |             |
    // |-------------|------------ |-------------|
    // |             |             |             |
    // |      0      |      1      |      2      |
    // |             |             |             |
    // o-------------------------------------------->X
    // Z

    typedef        ROOT::Math::SMatrix<double,3,3> matrix33Def;
    typedef        ROOT::Math::SMatrix<double,4,4> matrix44Def;
    typedef        std::pair<double, double>       xyPair     ;
    typedef        std::pair<int,int       >       rowColPair ;

    void           setupVariables                (void);
    ROC*           convertPixelToROC             (unsigned int* detector_row, unsigned int* detector_col);
    void           convertPixelFromROC           (ROC* roc, unsigned int* roc_row, unsigned int* roc_col);
    void           fromGlobalToLocal             (double* x, double* y, double* z,    double* xErr, double* yErr, double* zErr);
    void           fromGlobalToLocal             (double* x, double* y, double* z);
    double         fromLocalToGlobal             (double* x, double* y, double* z,    double* xErr, double* yErr, double* zErr);
    void           fromLocalToGlobal             (double* x, double* y, double* z);
    void           fromLocalToGlobalNoRotation   (double* x, double* y, double* xErr, double* yErr);
    void           flipPositionLocal             (double* x, double* y, double* xErr=0, double* yErr=0  );
    void           flipBackPositionLocal         (double* x, double* y, double* xErr=0, double* yErr=0  );
    void           flipPixel                     (unsigned int* row, unsigned int* col                  );
    void           flipBackPixel                 (unsigned int* row, unsigned int* col                  );
    void           flipDistance                  (double* deltaX, double* deltaY                        );
    void           flipBackDistance              (double* deltaX, double* deltaY                        );

    bool           switchXYFromLocaToGlobal      (void);

    matrix33Def    getRotationMatrix             (                                                      );

    unsigned int   getNumberOfCols               (bool global = false                     )                                  ;
    unsigned int   getNumberOfRows               (bool global = false                     )                                  ;
    unsigned int   getLastCol                    (void                                    ) {return (this->getNumberOfCols()-1);}
    unsigned int   getLastRow                    (void                                    ) {return (this->getNumberOfRows()-1);}
    ROC          * getROC                        (int chipID                              )                                  ;
    ROC          * getROCByPosition              (unsigned int chipPosition)                                                 ;
    ROC          * findROC                       (unsigned int row, unsigned int col      ) ;
    unsigned int   getNumberOfROCs               ( void                                   ) {return numberOfROCs_            ;}
    unsigned int   getROCPositionLocalX          (int chipID                              ) ;
    unsigned int   getROCPositionLocalY          (int chipID                              ) ;
    unsigned int   getROCPositionLocalXFromCol   (unsigned int col                        ) ;
    unsigned int   getROCPositionLocalYFromRow   (unsigned int row                        ) ;
    unsigned int   getNumberOfROCsLocalX         (void                                    ) {return xNumberOfROCs_           ;}
    unsigned int   getNumberOfROCsLocalY         (void                                    ) {return yNumberOfROCs_           ;}
    //Locally cols are always in X and rows always in Y
    double         getPixelCenterLocalX          (unsigned int col                        ) ;
    double         getPixelCenterLocalY          (unsigned int row                        ) ;
    double         getPixelLowEdgeLocalX         (unsigned int col                        ) ;
    double         getPixelLowEdgeLocalY         (unsigned int row                        ) ;
    double         getPixelPitchLocalX           (unsigned int col                        ) ;
    double         getPixelPitchLocalY           (unsigned int row                        ) ;
    int            getPixelColFromLocalX         (double xLocal                           ) ;
    int            getPixelRowFromLocalY         (double yLocal                           ) ;
    rowColPair     getPixelCellFromLocal         (double x, double y                      ) ;
    rowColPair     getPixelCellFromGlobal        (double x, double y, double z            ) ;
    double         getAlignmentPredictedGlobal   (ROOT::Math::SVector<double,4>& trackPars, matrix33Def& RInv, double z, double& predX, double& predY);
    void           getPredictedGlobal            (ROOT::Math::SVector<double,4>& trackPars, double& predX, double& predY, double& predZ) ;
    void           getPredictedLocal             (ROOT::Math::SVector<double,4>& trackPars, double& predX, double& predY) ;
    xyPair         getTrackErrorsOnPlane         (ROOT::Math::SVector<double,4>& trackPars, matrix44Def& AtVAInv) ;
    xyPair         propagateTrackErrors          (ROOT::Math::SVector<double,4>& trackPars, matrix44Def& AtVAInv, Detector::matrix33Def& RInv, double z);
    std::string    getID                         (void                                    ) {return ID_;}
    std::string    getName                       (void                                    ) {return name_                    ;}
    double         getDetectorLengthX            (bool global = false                     ) ;
    double         getDetectorLengthY            (bool global = false                     ) ;

    double         getXPositionTotal             (void                                    ) {return xPosition_+xPositionCorrection_;}
    double         getYPositionTotal             (void                                    ) {return yPosition_+yPositionCorrection_;}
    double         getZPositionTotal             (void                                    ) {return zPosition_+zPositionCorrection_;}
    double         getXPosition                  (void                                    ) {return xPosition_               ;}
    double         getXPositionCorrection        (void                                    ) {return xPositionCorrection_     ;}
    double         getXPositionError             (void                                    ) {return xPositionError_          ;}
    double         getYPosition                  (void                                    ) {return yPosition_               ;}
    double         getYPositionCorrection        (void                                    ) {return yPositionCorrection_     ;}
    double         getYPositionError             (void                                    ) {return yPositionError_          ;}
    double         getZPosition                  (void                                    ) {return zPosition_               ;}
    double         getZPositionCorrection        (void                                    ) {return zPositionCorrection_     ;}
    double         getZPositionError             (void                                    ) {return zPositionError_          ;}
    double         getXRotation                  (bool global_coordinate = true           ) ;
    double         getXRotationCorrection        (void                                    ) {return xRotationCorrection_     ;}
    double         getXRotationCorrectionError   (void                                    ) {return xRotationCorrectionError_;}
    double         getYRotation                  (bool global_coordinate = true           ) ;
    double         getYRotationCorrection        (void                                    ) {return yRotationCorrection_     ;}
    double         getYRotationCorrectionError   (void                                    ) {return yRotationCorrectionError_;}
    double         getZRotation                  (void                                    ) {return zRotation_               ;}
    double         getZRotationCorrection        (void                                    ) {return zRotationCorrection_     ;}
    double         getZRotationCorrectionError   (void                                    ) {return zRotationCorrectionError_;}

    bool           isDUT                         (void                                    ) {return isDUT_                 ;}
    bool           isStrip                       (void                                    ) {return isStrip_               ;}

    bool           isXBackFlipped                (void                                    ) {return xBackFlipped_          ;}
    bool           isYBackFlipped                (void                                    ) {return yBackFlipped_          ;}

    static matrix33Def rotationMatrix            (double alpha, double beta, double gamma );

    void           setDUT                        (bool isDUT = true                       ) {isDUT_   = isDUT              ;}
    void           setName                       (std::string name                        ) {name_    = name               ;}
    void           setIsStrip                    (bool isStrip                            ) {isStrip_ = isStrip            ;}

    void           setXBackFlipped               ( bool isIt                              ) {xBackFlipped_ = isIt          ;}
    void           setYBackFlipped               ( bool isIt                              ) {yBackFlipped_ = isIt          ;}
    void           setPosition                   (double x, double y, double z)             {xPosition_ = x; yPosition_ = y; zPosition_ = z;}
    void           setNumberOfROCs               (unsigned int xNumberOfROCs, unsigned int yNumberOfROCs               );
    void           setXNumberOfROCs              (unsigned int xNumberOfROCs)               {xNumberOfROCs_=xNumberOfROCs; this->updateNumberOfROCs() ;}
    void           setYNumberOfROCs              (unsigned int yNumberOfROCs)               {yNumberOfROCs_=yNumberOfROCs; this->updateNumberOfROCs() ;}

    void           setXPosition                  (double x)                                 {xPosition_                = x         ;}
    void           setXPositionCorrection        (double xCorr)                             {xPositionCorrection_      = xCorr     ;}
    void           setXPositionError             (double xErr)                              {xPositionError_           = xErr      ;}
    void           setYPosition                  (double y)                                 {yPosition_                = y         ;}
    void           setYPositionCorrection        (double yCorr)                             {yPositionCorrection_      = yCorr     ;}
    void           setYPositionError             (double yErr)                              {yPositionError_           = yErr      ;}
    void           setZPosition                  (double z)                                 {zPosition_                = z         ;}
    void           setZPositionCorrection        (double zCorr)                             {zPositionCorrection_      = zCorr     ;}
    void           setZPositionError             (double zErr)                              {zPositionError_           = zErr      ;}
    void           setXRotation                  (double xRot)                              {xRotation_                = xRot      ;}
    void           setXRotationCorrection        (double xRotCor)                           {xRotationCorrection_      = xRotCor   ;}
    void           setXRotationCorrectionError   (double xRotCorErr)                        {xRotationCorrectionError_ = xRotCorErr;}
    void           setYRotation                  (double y)                                 {yRotation_                = y         ;}
    void           setYRotationCorrection        (double yRotCor)                           {yRotationCorrection_      = yRotCor   ;}
    void           setYRotationCorrectionError   (double yRotCorErr)                        {yRotationCorrectionError_ = yRotCorErr;}
    void           setZRotation                  (double z)                                 {zRotation_                = z         ;}
    void           setZRotationCorrection        (double zRotCor)                           {zRotationCorrection_      = zRotCor   ;}
    void           setZRotationCorrectionError   (double zRotCorErr)                        {zRotationCorrectionError_ = zRotCorErr;}

    void           dump                          (void);
    typedef std::map<int, ROC*>::iterator   iterator;
    iterator       begin(void) {return ROCsChipIDMap_.begin();}
    iterator       end  (void) {return ROCsChipIDMap_.end();  }
    void test(double* x, double* y, double* z,double* xErr, double* yErr, double* zErr);


private:
    void           XYZRotation                (double* x, double* y, double* z,    double* xErr, double* yErr, double* zErr, bool backward = false);
    void           XYZRotation                (double* x, double* y, double* z,    bool backward = false);
    void           XRotation                  (double* y, double* z, double* yErr, double* zErr, bool backward = false);
    void           YRotation                  (double* x, double* z, double* xErr, double* zErr, bool backward = false);
    void           ZRotation                  (double* x, double* y, double* xErr, double* yErr, bool backward = false);
    void           XRotation                  (double* y, double* z, bool backward = false);
    void           YRotation                  (double* x, double* z, bool backward = false);
    void           ZRotation                  (double* x, double* y, bool backward = false);
    void           translateXY                (double* x, double* y, bool backward = false);
    void           translateCorrection        (double* x, double* y, bool backward = false);

    void           updateNumberOfROCs         (void){numberOfROCs_=xNumberOfROCs_*yNumberOfROCs_ ;}

    typedef std::map<int, ROC*> ROCsMapDef                         ;
    ROCsMapDef             ROCsChipIDMap_                          ;
    ROCsMapDef             ROCsPositionMap_                        ;

    std::string            ID_                                     ;
    bool                   isDUT_                                  ;
    std::string            name_                                   ;
    bool                   isStrip_                                ;

    double                 xPosition_                              ;
    double                 xPositionCorrection_                    ;
    double                 xPositionError_                         ;
    double                 yPosition_                              ;
    double                 yPositionCorrection_                    ;
    double                 yPositionError_                         ;
    double                 zPosition_                              ;
    double                 zPositionCorrection_                    ;
    double                 zPositionError_                         ;


    unsigned int           xNumberOfROCs_                          ;
    unsigned int           yNumberOfROCs_                          ;
    unsigned int           numberOfROCs_                           ;
    unsigned int           numberOfRows_                           ;
    unsigned int           numberOfCols_                           ;


    bool                   xBackFlipped_                           ;
    bool                   yBackFlipped_                           ;

    double                 xRotation_                              ;// -90 < r < 90
    double                 xRotationCorrection_                    ;
    double                 xRotationCorrectionError_               ;
    double                 yRotation_                              ;// -90 < r < 90
    double                 yRotationCorrection_                    ;
    double                 yRotationCorrectionError_               ;
    double                 zRotation_                              ;//   0 < r < 360 always anti-clockwise
    double                 zRotationCorrection_                    ;
    double                 zRotationCorrectionError_               ;

    //std::stringstream      ss_                                     ;//! temporary state value

    ClassDef(Detector,1)

} ;

#endif // DETECTOR_H
