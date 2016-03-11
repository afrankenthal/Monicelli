#include "fileEater.h"
#include "clusterizer.h"
#include "trackFitter.h"
#include "trackFinder.h"
#include "HManager.h"
#include "Geometry.h"
#include "aligner.h"
#include "threader.h"

#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>

#include <QCoreApplication>
#include <QDomDocument>
#include <QFile>
#include <QString>
#include <QDomNode>


class XmlDefaults;
class XmlFile;


class XmlParser
{
public:
  XmlParser (void);
  ~XmlParser(void);
  
  void parseDocument(std::string fileName);
  
  XmlDefaults*          getDefaults(void) {return theDefaults_;}
  std::vector<XmlFile*> getFileList(void) {return theFileList_;}
  
private:
  QDomDocument* document_;
  QDomNode      rootNode_;
  
  XmlDefaults*          theDefaults_;
  std::vector<XmlFile*> theFileList_;
  stringstream          ss_;
};


class XmlDefaults
{
public:
  XmlDefaults (QDomNode& node);
  ~XmlDefaults(void) {;}

  QDomNode&   getNode(void) {return thisNode_;}

  std::string filesPath_;
  std::string geometriesPath_;
  std::string trackFindingAlgorithm_;
  std::string trackFittingAlgorithm_;
  int         numberOfEvents_;     
  double      chi2Cut_;	       
  int         trackPoints_;     
  int         maxPlanePoints_;     
  int         xTolerance_;         
  int         yTolerance_;         
  bool        findDut_;         
  bool        useEtaFunction_;         
  bool        doFineAlignment_;
  
private:
  QDomNode thisNode_;
};


class XmlFile
{
public:
  XmlFile (QDomNode& node);
  ~XmlFile(void) {;}
  QDomNode&   getNode(void) {return thisNode_;}

  std::string fileName_;
  std::string geometryName_;
  
private:
  QDomNode thisNode_;
};


XmlParser::XmlParser(void) : document_(0)
{
}


XmlParser::~XmlParser()
{
  if (document_) delete document_;
}


void XmlParser::parseDocument(std::string xmlFileName)
{
  if (document_) delete document_;
  
  document_ = new QDomDocument("ConfigurationFile");
  QFile xmlFile(xmlFileName.c_str());

  if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) // Is this OK ???
    {
      STDLINE(std::string("Could not open ") + xmlFile.fileName().toStdString(),ACRed);
      return;
    }
  
  QString errMsg = "";
  int line;
  int col;

  if (!document_->setContent( &xmlFile, true , &errMsg, &line, &col))
    {
      STDLINE(std::string("Could not access ") + xmlFile.fileName().toStdString(),ACRed);
      ss_ << "Error: " << errMsg.toStdString() << " line: " << line << " col: " << col;
      STDLINE(ss_.str(),ACGreen);
      xmlFile.close();
      return;
    }
  
  STDLINE(std::string("Parsing ") + xmlFile.fileName().toStdString(),ACGreen);
  
  rootNode_ = document_->elementsByTagName("MonicelliExpressConfiguration").at(0);
  
  QDomNode defaults = document_->elementsByTagName("Defaults").at(0);
  theDefaults_ = new XmlDefaults(defaults);

  QDomNodeList fileList = document_->elementsByTagName("File");

  for (int f = 0; f < fileList.size(); f++)
    {
      QDomNode fileNode = fileList.at(f);

      if (!fileNode.isComment()) theFileList_.push_back(new XmlFile(fileNode));
    }
  
  xmlFile.close();
}


XmlDefaults::XmlDefaults(QDomNode& node)
{
  thisNode_       	 = node;
  filesPath_      	 = node.toElement().attribute("FilesPath").toStdString();
  geometriesPath_ 	 = node.toElement().attribute("GeometriesPath").toStdString();
  trackFindingAlgorithm_ = node.toElement().attribute("TrackFindingAlgorithm").toStdString();
  trackFittingAlgorithm_ = node.toElement().attribute("TrackFittingAlgorithm").toStdString();
  numberOfEvents_        = node.toElement().attribute("NumberOfEvents").toInt();
  chi2Cut_        	 = node.toElement().attribute("Chi2Cut").toInt();
  trackPoints_    	 = node.toElement().attribute("TrackPoints").toInt();
  maxPlanePoints_ 	 = node.toElement().attribute("MaxPlanePoints").toInt();
  xTolerance_     	 = node.toElement().attribute("XTolerance").toInt();
  yTolerance_     	 = node.toElement().attribute("YTolerance").toInt();

  findDut_        	 = true;
  useEtaFunction_        = true;
  doFineAlignment_       = true;

  if (node.toElement().attribute("FindDut") == "false" || node.toElement().attribute("FindDut") == "False")
    findDut_ = false;

  if (node.toElement().attribute("UseEtaFunction") == "false" || node.toElement().attribute("UseEtaFunction") == "False")
    useEtaFunction_ = false;

  if (node.toElement().attribute("FineAlignment") == "false" || node.toElement().attribute("FineAlignmente") == "False")
    doFineAlignment_ = false;
}


XmlFile::XmlFile(QDomNode& node)
{
  thisNode_     = node;
  fileName_     = node.toElement().attribute("Name")    .toStdString();
  geometryName_ = node.toElement().attribute("Geometry").toStdString();
}




int main (int argc, char** argv)
{
  QCoreApplication app (argc, argv);

  stringstream ss;

  XmlParser theXmlParser;
  std::string configFileName;

  if      (argc == 1) configFileName = std::string("./xml/ExpressConfiguration.xml");
  else if (argc == 2) configFileName = std::string("./xml/") + argv[1];
  else if (argc >  2)
    {
      ss.str("");
      ss << "Usage: ./MonicelliExpress optional(configuration file)";
      STDLINE(ss.str(),ACRed);
      exit(EXIT_SUCCESS);
    }

  ss.str("");
  ss << "Using: " << configFileName << " configuration.";
  STDLINE(ss.str(),ACGreen);

  theXmlParser.parseDocument(configFileName.c_str());
  
  
  const string filesPath            = theXmlParser.getDefaults()->filesPath_;
  const string geometriesPath       = theXmlParser.getDefaults()->geometriesPath_;
  std::string trackFindingAlgorithm = theXmlParser.getDefaults()->trackFindingAlgorithm_;
  std::string trackFittingAlgorithm = theXmlParser.getDefaults()->trackFittingAlgorithm_;
  int    numberOfEvents             = theXmlParser.getDefaults()->numberOfEvents_;
  double chi2Cut                    = theXmlParser.getDefaults()->chi2Cut_;
  int    trackPoints                = theXmlParser.getDefaults()->trackPoints_;
  int    maxPlanePoints             = theXmlParser.getDefaults()->maxPlanePoints_;
  int	 xTolerance                 = theXmlParser.getDefaults()->xTolerance_;
  int	 yTolerance                 = theXmlParser.getDefaults()->yTolerance_;
  bool   findDut                    = theXmlParser.getDefaults()->findDut_;
  bool   useEtaFunction             = theXmlParser.getDefaults()->useEtaFunction_;
  bool   doFineAlignment            = theXmlParser.getDefaults()->doFineAlignment_;


  for (unsigned int f = 0; f < theXmlParser.getFileList().size(); f++)
    {
      string fileName    = filesPath      + theXmlParser.getFileList()[f]->fileName_;
      string geoFileName = geometriesPath + theXmlParser.getFileList()[f]->geometryName_;
      
      fileEater	theFileEater;
      clusterizer theClusterizer;
      trackFitter theTrackFitter;
      HManager theHManager(&theFileEater);
      theFileEater.setHManger(&theHManager);
      trackFinder theTrackFinder(&theTrackFitter);



      // #########################
      // # Parse and make events #
      // #########################
      STDLINE("Parse and make events",ACBlue);

      if (theFileEater.openFile(geoFileName) == "Error!")
  	{
	  STDLINE("Error in geoFileName",ACRed);
	  continue;
  	}   
      theFileEater.setInputFileName(fileName);
      theFileEater.setEventsLimit(numberOfEvents);

      Geometry* theGeometry = theFileEater.getGeometry();
      
      if (!theFileEater.parse())
  	{
	  STDLINE("Error in parsing",ACRed);
	  continue;
  	}
      theHManager.setRunSubDir(theFileEater.openFile(theFileEater.getOutputTreeCompletePath()));
      
      theFileEater.populate();



      // ###############
      // # Clusterizer #
      // ###############
      STDLINE("Clusterizer",ACBlue);

      if (useEtaFunction) theClusterizer.getChargeAsymmetryPlots(theGeometry);
      else                theClusterizer.setUseEtaFunction(false);
      theFileEater.setOperation(&fileEater::updateEvents2,&theClusterizer);
      theFileEater.updateEvents2();



      // ################
      // # Track finder #
      // ################
      STDLINE("Track finder",ACBlue);

      theTrackFinder.setTrackSearchParameters(xTolerance*(1e-4)*CONVF, yTolerance*(1e-4)*CONVF, chi2Cut, trackPoints, maxPlanePoints); // Is this OK ???
      theTrackFinder.setTrackingOperationParameters(trackFindingAlgorithm, trackFittingAlgorithm, findDut);      
      theFileEater.setOperation(&fileEater::updateEvents2,&theTrackFinder);
      theTrackFinder.setOperation(&trackFinder::findAndFitTracks);      
      theFileEater.updateEvents2();



      // ############################
      // # Telescope fine alignment #
      // ############################
      STDLINE("Telescope Fine Alignment",ACBlue);

      aligner *theAlignerTelescope = new aligner(&theFileEater,&theHManager);
      theAlignerTelescope->setAlignmentFitMethodName("Simple");
      theAlignerTelescope->setNumberOfIterations(0);
      
      for (Geometry::iterator it = theGeometry->begin(); it != theGeometry->end(); it++)
	{
	  if (!(*it).second->isDUT()) theAlignerTelescope->setFixParMap((*it).first, 100000);
	  else                        theAlignerTelescope->setFixParMap((*it).first, 111111);
	}
      theAlignerTelescope->setAlignmentPreferences(5, 0, 20., 2, 6, 1, true, "", -1); // Is this OK ???
      theAlignerTelescope->setOperation(&aligner::align);
      
      
      threader *theThreaderTelescopeAlignment = new threader();
      theThreaderTelescopeAlignment->setProcess(theAlignerTelescope);
      theThreaderTelescopeAlignment->start();
      
      while (theThreaderTelescopeAlignment->isRunning()) sleep(1);
      delete theThreaderTelescopeAlignment;
      
      
      aligner::alignmentResultsDef alignmentResultsTelescope = theAlignerTelescope->getAlignmentResults();
      
      for (Geometry::iterator geo = theGeometry->begin(); geo != theGeometry->end(); geo++)
	{
	  Detector* theDetector = theGeometry->getDetector(geo->first);
	  
	  double xPositionCorrection = theDetector->getXPositionCorrection() + alignmentResultsTelescope[geo->first].deltaTx;
	  double yPositionCorrection = theDetector->getYPositionCorrection() + alignmentResultsTelescope[geo->first].deltaTy;
	  double zPositionCorrection = theDetector->getZPositionCorrection() + alignmentResultsTelescope[geo->first].deltaTz;
	  double xRotationCorrection = theDetector->getXRotationCorrection() + alignmentResultsTelescope[geo->first].alpha;
	  double yRotationCorrection = theDetector->getYRotationCorrection() + alignmentResultsTelescope[geo->first].beta;
	  double zRotationCorrection = theDetector->getZRotationCorrection() + alignmentResultsTelescope[geo->first].gamma;
	  
	  theDetector->setXPositionCorrection(xPositionCorrection);
	  theDetector->setYPositionCorrection(yPositionCorrection);
	  theDetector->setZPositionCorrection(zPositionCorrection);
	  theDetector->setXRotationCorrection(xRotationCorrection);
	  theDetector->setYRotationCorrection(yRotationCorrection);
	  theDetector->setZRotationCorrection(zRotationCorrection);
	}
      
      delete theAlignerTelescope;    
      theFileEater.updateGeometry("geometry");
      
      theTrackFinder.setTrackSearchParameters(xTolerance*(1e-4)*CONVF, yTolerance*(1e-4)*CONVF, chi2Cut, trackPoints, maxPlanePoints); // Is this OK ???
      theTrackFinder.setTrackingOperationParameters(trackFindingAlgorithm, trackFittingAlgorithm, findDut);
      theFileEater.setOperation(&fileEater::updateEvents2,&theTrackFinder);
      theTrackFinder.setOperation(&trackFinder::findAndFitTracks);
      theFileEater.updateEvents2();



      // ################
      // # Track finder #
      // ################
      STDLINE("Track finder",ACBlue);

      theTrackFinder.setTrackSearchParameters(xTolerance*(1e-4)*CONVF, yTolerance*(1e-4)*CONVF, chi2Cut, trackPoints, maxPlanePoints); // Is this OK ???
      theTrackFinder.setTrackingOperationParameters(trackFindingAlgorithm, trackFittingAlgorithm, findDut);      
      theFileEater.setOperation(&fileEater::updateEvents2,&theTrackFinder);
      theTrackFinder.setOperation(&trackFinder::findAndFitTracks);      
      theFileEater.updateEvents2();



      // ######################
      // # Fine alignment DUT #
      // ######################
      if (doFineAlignment)
	{
	  for (unsigned int i = 0; i < 1; i++)
	    {
	      ss.clear();
	      ss.str("");
	      ss << "Fine Alignment DUT - step" << i;
	      STDLINE(ss.str().c_str(),ACBlue);

	      for (Geometry::iterator it = theGeometry->begin(); it != theGeometry->end(); it++)
		{
		  if (!(*it).second->isDUT()) continue;
	  
		  string dut = it->first;
		  aligner *theAligner = new aligner(&theFileEater,&theHManager);

		  theAligner->setFixParMap(dut,100011); // Here is where I choose which parameters must be kept constant
		  theAligner->setAlignmentPreferences(10, 8, 20., 2, 8, 1, true, dut, -1);  // Is this OK ???
		  theAligner->setOperation(&aligner::alignDUT);
	  
		  threader *theThreader = new threader();
		  theThreader->setProcess(theAligner);
		  theThreader->start();

		  while (theThreader->isRunning()) sleep(1);

		  aligner::alignmentResultsDef alignmentResults = theAligner->getAlignmentResults();

		  Detector * theDetector = theGeometry->getDetector(dut);

		  double xPositionCorrection = theDetector->getXPositionCorrection() + alignmentResults[dut].deltaTx;
		  double yPositionCorrection = theDetector->getYPositionCorrection() + alignmentResults[dut].deltaTy;
		  double zPositionCorrection = theDetector->getZPositionCorrection() + alignmentResults[dut].deltaTz;
		  double xRotationCorrection = theDetector->getXRotationCorrection() + alignmentResults[dut].alpha;
		  double yRotationCorrection = theDetector->getYRotationCorrection() + alignmentResults[dut].beta;
		  double zRotationCorrection = theDetector->getZRotationCorrection() + alignmentResults[dut].gamma;
	  
		  theDetector->setXPositionCorrection(xPositionCorrection);
		  theDetector->setYPositionCorrection(yPositionCorrection);
		  theDetector->setZPositionCorrection(zPositionCorrection);
		  theDetector->setXRotationCorrection(xRotationCorrection);
		  theDetector->setYRotationCorrection(yRotationCorrection);
		  theDetector->setZRotationCorrection(zRotationCorrection);

		  delete theAligner;
		  delete theThreader;	  
		}



	      // ###################
	      // # Update geometry #
	      // ###################	
	      STDLINE("Update geometry",ACBlue);
	  
	      theFileEater.updateGeometry("geometry");
	    }
	}



      // #######################
      // # Track finder on DUT #
      // #######################
       // Is this OK ???
      STDLINE("Track finder on DUT",ACBlue);
      
      theTrackFinder.setTrackSearchParameters(xTolerance*(1e-4)*CONVF, yTolerance*(1e-4)*CONVF, chi2Cut, trackPoints, maxPlanePoints); // Is this OK ???
      theFileEater.setOperation(&fileEater::updateEvents2,&theTrackFinder);
      theTrackFinder.setOperation(&trackFinder::findDUTCandidates);      
      theFileEater.updateEvents2();	



      // #############
      // # Residuals #
      // #############
      STDLINE("Residuals",ACBlue);

      theTrackFitter.clearSelectedDetectorsList();      
      theTrackFitter.setOperation(&trackFitter::makeFittedTracksResiduals);
      theFileEater.setOperation(&fileEater::updateEvents2,&theTrackFitter);
      theFileEater.updateEvents2();



      // ######################
      // # Copy geometry file #
      // ######################
      STDLINE("Copy geometry file",ACBlue);

      string outputFilePath = filesPath;
      outputFilePath.erase(outputFilePath.length()-8,outputFilePath.length()).append("/MonicelliOutput/");

      string newGeoName  = theXmlParser.getFileList()[f]->fileName_;
      newGeoName.erase(newGeoName.length()-4,newGeoName.length()).append(".geo");

      string copyGeometryCommand = "cp " + outputFilePath + newGeoName + " " + geometriesPath + newGeoName;
      STDLINE(copyGeometryCommand.c_str(),ACBlue);
      system(copyGeometryCommand.c_str());
    }


  return EXIT_SUCCESS;
}
