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
 
#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "iostream"
#include "sstream"
#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include <QWidget>
#include <QDomDocument>
#include <QFile>
#include <QDomNode>
#include <QTextStream>

class QWidget     ;
class QDomElement ;
class QFile       ;

class xmlStation  ;
class xmlDetector ;
class xmlROC      ;

class xmlParser
{
public:
  xmlParser(QFile & file) ;
  xmlParser(int     numberOfStations_,
            int     numberOfDetectorsPerStation_,
            int     numberOfROCsPerDetector_) ;
  ~xmlParser(void) ;

  typedef std::pair<int, int        > pairIntDef      ;
  typedef std::vector<pairIntDef    > idSerialListDef ;
  typedef std::map<int, int         > ROCsDef         ;
  typedef std::map<int, xmlStation *> xmlStationsDef  ; // First index: Id number
  typedef std::map<int, int>          tabPositionDef  ;

  void              addStation            (int           stationId,
                                           int           stationSerial    ) ;
  void              buildTelescope        (void                           ) ;
  void              dumpHeaders           (void                           ) ;
  void              dumpStations          (void                           ) ;
  std::string       getDate               (void                           ) {return date_        ;}
  std::string       getDesctiption        (void                           ) {return description_ ;}
  int               getDetectorId         (int           stationSerial,
                                           int           detectorSerial   ) ;
  int               getDetectorSerial     (int           stationId,
                                           int           detectorId       ) ;
  int               getDetectorTabPosition(int           stationId,
                                           int           detectorId       ) ;
  QDomNode        & getNode               (void                           ) {return rootNode_    ;}
  int               getNumberOfDetectors  (int           stationSerial    ) ;
  unsigned int      getNumberOfStations   (void                           ) ;
  int               getNumberOfROCs       (int           stationSerial,
                                           int           detectorSerial   ) ;
  std::string       getRun                (void                           ) {return run_;}
  int               getStationId          (int           stationSerial    ) ;
  idSerialListDef   getStationList        (void                           ) ;
  int               getStationSerial      (int           stationId        ) ;
  int               getStationTabPosition (int           stationId        ) {return stationTabPosition_ [stationId ];}
  xmlDetector     * getXmlDetectorBySerial(int           stationSerial,
                                           int           detectorSerial   ) ;
  xmlDetector     * getXmlDetectorById    (int           stationId,
                                           int           detectorId       ) ;
  xmlStation      * getXmlStationBySerial (int           stationSerial    ) ;
  xmlStation      * getXmlStationById     (int           stationId        ) ;
  xmlStationsDef    getXmlStations        (void                           ) {return xmlStations_;}
  xmlROC          * getROCById            (int           stationId,
                                           int           detectorId,
                                           int           rocId            ) ;
  xmlROC          * getROCBySerial        (int           stationSerial,
                                           int           detectorSerial,
                                           int           rocSerial        ) ;
  void              remStation            (int           stationId        ) ;
  void              save                  (std::string   fileName         ) ;

  xmlStationsDef      xmlStations_         ; // Indexed by station Id !!!!!

private:

  QDomDocument      * document_            ;
  QDomNode            rootNode_            ;

  std::stringstream   ss_                  ;

  std::string         description_         ;
  std::string         run_                 ;
  std::string         date_                ;

  tabPositionDef      stationTabPosition_  ;
};

//=========================================================================================

class xmlUtils
{
public:
  static void        addDetector           (QDomElement & father,
                                            QDomElement & thisNode,
                                            int           detectorId,
                                            int           detectorSerial,
                                            int           numberOfROCsPerDetector    );
  static void        addDetectors          (QDomElement & father,
                                            int           numberOfDetectors,
                                            int           numberOfROCsPerDetector    );
  static void        addROCs               (QDomElement & rocs,
                                            int           numberOfROCsPerDetector    );
  static void        addROC                (QDomElement & ROCsNode,
                                            QDomElement & ROCNode,
                                            int           ROCId,
                                            int           ROCSerial                  );
  static void        addStation            (QDomElement & father,
                                            int           stationId,
                                            int           stationSerial,
                                            int           numberOfDetectorsPerStation,
                                            int           numberOfROCsPerDetector    );
  static void        addStations           (QDomElement & father,
                                            int           numberOfStations,
                                            int           numberOfDetectorsPerStation,
                                            int           numberOfROCsPerDetector    );
  static void        appendTags            (QDomElement & father,
                                            std::string   name,
                                            std::string   value,
                                            std::string   attName="",
                                            std::string   attValue=""                );
  static void        appendTags            (QDomElement & father,
                                            std::string   name,
                                            int           value,
                                            std::string   attName="",
                                            std::string   attValue=""                );
  static void        appendTags            (QDomElement & father,
                                            std::string   name,
                                            double        value,
                                            std::string   attName="",
                                            std::string   attValue=""                );
  static void        dumpFragment          (QDomNode    & thisNode,
                                            std::string   what,
                                            std::string   size                       );
private:

};

//=========================================================================================

class xmlStation : public QWidget
{
  Q_OBJECT
public:
   xmlStation  (QDomNode & stationNode) ;
  ~xmlStation  (void                  ){;}

  typedef std::map<int, xmlDetector *> xmlDetectorsDef ;

  void                         addDetector           (int           detectorId,
                                                      int           detectorSerial) ;
  bool                         isUsed                (void                        ) {return used_                           ;}
  std::vector<int>             getAllStationsId      (void                        ) ;
  std::string                  getDescription        (void                        ) {return description_                    ;}
  int                          getDetectorTabPosition(int           detectorId    ) {return detectorTabPosition_[detectorId];}
  xmlParser::idSerialListDef   getDetectorList       (void                        ) ;
  QDomNode                   & getNode               (void                        ) {return stationNode_                    ;}
  int                          getStationId          (void                        ) {return id_                             ;}
  int                          getStationSerial      (void                        ) {return serial_                         ;}
  xmlDetector                * getXmlDetectorBySerial(int           detectorSerial) ;
  xmlDetector                * getXmlDetectorById    (int           detectorId    ) ;
  xmlROC                     * getROCById            (int           detector,
                                                      int           roc           ) ;
  xmlDetectorsDef              getXmlDetectors       (void                        ) {return detectors_                      ;}

  void                         remStation            (void                        ) ;
  void                         setId                 (int           id            ) ;
  void                         setUsed               (bool          used          ) ;

  bool                         isEnabled             (void                        ) ;
  void                         dumpDetectors         (void                        ) ;

private:

  QDomNode                  stationNode_         ;

  std::stringstream         ss_                  ;
  int                       id_                  ;
  int                       serial_              ;
  bool                      used_                ;
  std::string               description_         ;

  xmlDetectorsDef           detectors_           ; // Indexed by station Id

  xmlParser::tabPositionDef detectorTabPosition_ ;

private slots:

  void                   setValue        (int           value     ) ;

  void                   setAttributeText(std::string   key,
                                          QString     & textValue ) ;
signals:
};

//=========================================================================================

class xmlDetector : public QWidget
{
  Q_OBJECT
public:
  explicit xmlDetector(QDomNode & detectorNode) ;
          ~xmlDetector(void                   ){;}

  typedef std::map<int,         xmlROC *   > xmlROCDef      ;
  typedef std::map<std::string, QString    > keyValueDef    ;

  void                         addROC           (int  ROCId,
                                                 int  ROCSerial     ) ;
  std::string                  getDescription   (void               ) {return description_  ;}
  QDomNode                   & getNode          (void               ) {return detectorNode_ ;}
  QDomNode                   & getROCNode       (void               ) {
                                                                       ROCNode_ = detectorNode_.toElement().elementsByTagName("ROCs").at(0) ;
                                                                       return ROCNode_                                                      ;
                                                                      }

  std::vector<int>             getAllDetectorsId(void               ) ;
  int                          getDetectorId    (void               ) {return id_           ;}
  int                          getDetectorSerial(void               ) {return serial_       ;}
  xmlParser::idSerialListDef   getROCList       (void               ) ;
  xmlROCDef                    getXmlROCs       (void               ) {return ROCs_         ;}
  int                          getStationId     (void               ) {return stationId_    ;}
  xmlROC                     * getXmlROCById    (int  roc           ) ;
  xmlROC                     * getXmlROCBySerial(int  rocSerial     ) ;
  void                         remDetector      (void               ) ;
  bool                         isEnabled        (void               ) ;
  bool                         isUsed           (void               ) {return used_         ;}
  bool                         isDUT            (void               ) {return isDUT_        ;}
  void                         setUsed          (bool used          ) ;
  void                         dumpROCs         (void               ) ;

  keyValueDef          keyValue_                         ;

private:

  QDomNode             detectorNode_                     ;
  QDomNode             ROCNode_                          ;

  std::stringstream    ss_                               ;
  xmlROCDef            ROCs_                             ;
  xmlParser::ROCsDef   ROCId_                            ;
  int                  id_                               ;
  int                  serial_                           ;
  int                  stationId_                        ;
  bool                 used_                             ;
  bool                 isDUT_                            ;
  std::string          description_                      ;

private slots:
};

//=========================================================================================

class xmlROC : public QWidget
{
  Q_OBJECT

public:
  explicit   xmlROC                   (QDomNode &    ROCNode   ) ;
            ~xmlROC                   (void                    ) {;}

  typedef std::map<std::string, QString > keyValueDef    ;
  typedef std::map<int,         double  > nonStandardPitchDef ;

  bool          isUsed                (void                    ) {return used_       ;}
  std::string   getDescription        (void                    ) {return description_;}
  int           getDetectorId         (void                    ) {return detectorId_ ;}
  QDomNode    & getNode               (void                    ) {return ROCNode_    ;}
  int           getROCId              (void                    ) {return id_         ;}
  int           getROCSerial          (void                    ) {return serial_     ;}
  int           getStationId          (void                    ) {return stationId_  ;}
  void          remROC                (void                    ) ;
  void          setUsed               (bool      used          ) ;
  void          addNonStandardPitch   (xmlROC  * ROC,
                                       QString   type   ,
                                       int       rowCol ,
                                       double    pitch         ) ;
  void          removeNonStandardPitch(QString   type,
                                       int       rowCol        ) ;

  keyValueDef         keyValue_             ;

  nonStandardPitchDef nonStandardRowPitch_  ;
  nonStandardPitchDef nonStandardColPitch_  ;


private slots:
  void       setTabbedText   (std::string   key,
                              std::string   tabValue,
                              std::string   attName,
                              QString     & textValue) ;
  void       setAttributeText(std::string   key,
                              QString     & textValue) ;

private:

  void update(void) ;

  QDomNode           ROCNode_    ;
  std::stringstream  ss_         ;
  int                serial_     ;
  int                id_         ;
  bool               used_       ;
  int                detectorId_ ;
  int                stationId_  ;
  std::string          description_                      ;

};

#endif // XMLPARSER_H
