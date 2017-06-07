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
 
#ifndef FITTER_H
#define FITTER_H

#include <iostream>
#include <sstream>

using namespace std;

class TH1;
class TF1;

class fitter
{

public:
    fitter         (void);
    ~fitter        (void);

    typedef std::pair<double*,double*> fitResultDef;

    fitResultDef    calibrationFit               (TH1         * histo,
                                                  double        xmin,
                                                  double        xmax,
                                                  double      * pars=0       );
    void            linearFit                    (TH1         * histo,
                                                  double      * slope,
                                                  double      * q,
                                                  double        tolerance=2.5);
    void            gaussFit                     (TH1         * histo        );
    void            gaussFit                     (TH1         * histo,
                                                  double        mean,
                                                  double        rms,
                                                  double        nRMS         );
    void            chi2Fit                      (TH1         * histo        );
    double          getMean                      (void                       ){return mean_                      ;}
    double          getSigma                     (void                       ){return sigma_                     ;}
    const char *    getCalibrationFitFunctionName(void                       ){return calibrationFitFunctionName_;}
    int             getCalibrationFitFunctionNPar(void                       );
    void            setFitFunctionType           (std::string   type         );
    std::string     getFitFunctionType           (void                       ){return fitFunctionType_           ;}

private:
    double          mean_                      ;
    double          sigma_                     ;
    const char *    calibrationFitFunctionName_;
    TF1        *    calibrationFitFunction_    ;
    std::string     fitFunctionType_           ;

    std::stringstream ss_;

};



#endif // FITTER_H
