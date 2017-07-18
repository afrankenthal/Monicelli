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
 
#ifndef GEOMETRYPARAMETERS_H
#define GEOMETRYPARAMETERS_H

#include <QtWidgets/QWidget>
#include <string>
#include <vector>

class Detector;
class QLineEdit;

namespace Ui {
class GeometryParameters;
}

class GeometryParameters : public QWidget
{
    Q_OBJECT
    
public:
    explicit GeometryParameters      (QWidget  * parent = 0       );
            ~GeometryParameters      (void                        );
    void     showDetectorPars        (Detector * detector         );
    double   getDetectorParTotal     (int        par              );//0=Alpha,1=Beta,2=Gamma,3=X,4=Y,5=Z
    double   getDetectorParBase      (int        par              );//0=Alpha,1=Beta,2=Gamma,3=X,4=Y,5=Z
    double   getDetectorParCorrection(int        par              );//0=Alpha,1=Beta,2=Gamma,3=X,4=Y,5=Z
    void     setDetectorParTotal     (int        par, double value);//0=Alpha,1=Beta,2=Gamma,3=X,4=Y,5=Z
    void     setDetectorParBase      (int        par, double value);//0=Alpha,1=Beta,2=Gamma,3=X,4=Y,5=Z
    void     setDetectorParCorrection(int        par, double value);//0=Alpha,1=Beta,2=Gamma,3=X,4=Y,5=Z
    bool     isEnabled               (void                        );
    void     enable                  (bool       enable           );

private:
    enum{nParameters=6};
    Ui::GeometryParameters* ui         ;
    std::vector<QLineEdit*> total_     ;
    std::vector<QLineEdit*> base_      ;
    std::vector<QLineEdit*> correction_;
    bool                    badValue_  ;


private slots:
    void updateTotal(void        );
    void checkValue (QString text);
};

#endif // GEOMETRYPARAMETERS_H
