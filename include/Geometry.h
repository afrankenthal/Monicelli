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
 
#ifndef _Geometry_h_
#define _Geometry_h_

#include <list>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <TObject.h>

#include "Detector.h"
#include "KalmanPlaneInfo.h"

#define CONVF 1000

#define DEFAULT_TELESCOPE_FIT_FUNCTION "linear"
#define DEFAULT_DUT_FIT_FUNCTION       "parabola"

class Geometry : public TObject
{
 public:

  Geometry(void);
 ~Geometry(void);

  typedef std::map< std::string , Detector* >   detectorsMapDef;
  typedef std::map< int , int >                 dataTypeMapDef ;
  typedef detectorsMapDef::iterator             iterator       ;

  const iterator              begin                   ( 			      ) {return detectorsMap_.begin()	    ;}
  const iterator              end                     ( 			      ) {return detectorsMap_.end()	    ;}

  Detector                  * addDetector             (std::string  plaqID,
                                                       bool	    isDUT   = false,
                                                       bool	    isStrip = false   );

  void                        clear                   (void			      ) {detectorsMap_.clear()  	    ;}
  bool                        empty                   (void			      ) {return detectorsMap_.empty()	    ;}

  Detector                  * getDetector             (std::string  plaqID	      );
  Detector                  * getDetector             (int	    station,
                                                       int	    plaq	      );
  int                         getDetectorStation      (std::string  plaqID	      );
  int                         getDetectorModule       (std::string  plaqID	      );
  int                         getSize                 (void			      ) {return detectorsMap_.size()	    ;}
  const KalmanPlaneInfo &     getKalmanPlaneInfo      (void			      ) {return theKalmanPlaneInfo_	    ;}

  std::vector<Detector*>      getDUTs                 (void			      );
  detectorsMapDef             getDetectors            (void			      ) {return detectorsMap_		    ;}
  std::string                 getDetectorID           (int	    Station, 
                                                       int	    Plaq	      );
  unsigned int                getDetectorsNumber      (bool	    excludeDUT = false);
  unsigned int                getDetectorsNumberByType(std::string  detectorType      );
  std::string                 getGeometryFileName     (void			      ) {return geometryFileName_	    ;}
  double                      getMaxDetectorsLength   (void			      );
  unsigned int                getMaxRowsNum           (void			      );
  unsigned int                getMaxColsNum           (void			      );
  bool                        calibrationDone         (void			      ) {return calibrationDone_	    ;}

  void                        setDUTnumbers           (unsigned int dutNumbers        ) {dutNumbers_	       = dutNumbers ;}
  void                        setGeometryFileName     (std::string  fileName	      );
  void                        setCalibrationDone      (bool	    done	      ) {calibrationDone_      = done	    ;}
  void                        setDataType             (int	    station,
                                                       int	    d		      ) {dataTypeMap_[station] = d	    ;}
  void                        dump                    (void			              );
  void                        orderPlanes             (void			              );
  void                        calculatePlaneMCS       (void			              );
  void                        calculatePlaneMCSKalman (std::string plaqID         ,
                                                       Detector::matrix33Def fRinv);

 private:

  bool compare_zPosition (std::string first, std::string second);

  detectorsMapDef   detectorsMap_            ;
  dataTypeMapDef    dataTypeMap_             ;
  std::vector<int>  runNumbers_              ;
  unsigned int      dutNumbers_              ;
  std::string       geometryFileName_        ;
  bool              calibrationDone_         ;
  KalmanPlaneInfo   theKalmanPlaneInfo_      ;//! temporary state value
  std::stringstream ss_                      ;//! temporary state value

  ClassDef(Geometry,2)

} ;

#endif // GEOMETRY_H
