/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef FITTER_H
#define FITTER_H

#include <sstream>

class TH1;
class TF1;

class fitter
{

public:
    fitter         (void);
    ~fitter        (void);

    typedef std::pair<double*,double*> fitResultDef;

    fitResultDef    calibrationFit(TH1* histo, double xmin, double xmax, double* pars=0               );
    void            linearFit     (TH1* histo, double* slope, double* q, double xmin=0, double xmax=0 );
    void            gaussFit      (TH1* histo                                                         );
    void            gaussFit      (TH1* histo, double mean, double rms, double nRMS                   );

    double          getMean       (void               ){return mean_ ;}
    double          getSigma      (void               ){return sigma_;}
    const char*     getCalibrationFitFunctionName(void){return calibrationFitFunctionName_;}
    int             getCalibrationFitFunctionNPar(void);

private:
    double            mean_ ;
    double            sigma_;
    const char*       calibrationFitFunctionName_;
    TF1*              calibrationFitFunction_;

    std::stringstream ss_;

};



#endif // FITTER_H
