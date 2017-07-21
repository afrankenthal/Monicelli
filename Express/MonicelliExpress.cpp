#include "fileEater.h"
#include "clusterizer.h"
#include "trackFitter.h"
#include "trackFinder.h"
#include "HManager.h"
#include "Geometry.h"
#include "aligner.h"
#include "threader.h"
#include "maintabs.h"

#include <TApplication.h>

#include <QDomDocument>
#include <QFile>
#include <QString>
#include <QDomNode>

#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>


// @@@ Hard coded parameters @@@
#define DUTfreePLANES 100011 // Define the fix [1] and free [0] parameters [z,y,x,gamma,beta,alpha]
#define DORAWALIGNMENT true  // Find the transverse position of the beamspot
#define CHI2RAWALIGN 60.0    // Track Chi2 for raw alignment
#define NTELEALIGN 2         // Maximum telescope fine alignments
#define DUT2STEPS true       // Do DUT alignment in 2 steps: (1) only translations, (2) translations + angles
#define LARGEROD 10000.      // DUT larger rod search
#define COPYGEOFILE false    // Copy geo file into geometry directory
// ============================


class XmlDefaults;
class XmlFile;


//=======================================================================
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


//=======================================================================
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
  bool        doDUTFineAlignment_;
  bool        doTelescopeFineAlignment_;
  
private:
  QDomNode thisNode_;
};


//=======================================================================
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


//=======================================================================
XmlParser::XmlParser(void) : document_(0)
{
}


//=======================================================================
XmlParser::~XmlParser()
{
  if (document_) delete document_;
}


//=======================================================================
void XmlParser::parseDocument(std::string xmlFileName)
{
  if (document_) delete document_;
  
  document_ = new QDomDocument("ConfigurationFile");
  QFile xmlFile(xmlFileName.c_str());

  if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      STDLINE(std::string("Could not open ") + xmlFile.fileName().toStdString(),ACRed);
      return;
    }
  
  QString errMsg = "";
  int line;
  int col;

  if (!document_->setContent(&xmlFile, true , &errMsg, &line, &col))
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


//=======================================================================
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

  findDut_        	    = true;
  useEtaFunction_           = true;
  doDUTFineAlignment_       = true;
  doTelescopeFineAlignment_ = true;

  if (node.toElement().attribute("FindDut") == "false" || node.toElement().attribute("FindDut") == "False")
    findDut_ = false;

  if (node.toElement().attribute("UseEtaFunction") == "false" || node.toElement().attribute("UseEtaFunction") == "False")
    useEtaFunction_ = false;

  if (node.toElement().attribute("FineAlignment") == "false" || node.toElement().attribute("FineAlignmente") == "False")
    doDUTFineAlignment_ = false;

  if (node.toElement().attribute("FineTelescopeAlignment") == "false" || node.toElement().attribute("FineTelescopeAlignmente") == "False")
    doTelescopeFineAlignment_ = false;
}


//=======================================================================
XmlFile::XmlFile(QDomNode& node)
{
  thisNode_     = node;
  fileName_     = node.toElement().attribute("Name").toStdString();
  geometryName_ = node.toElement().attribute("Geometry").toStdString();
}




//=======================================================================
int main (int argc, char** argv)
{
  stringstream ss;

  gROOT->SetBatch(true);
  TApplication tApp("App",&argc,argv);
  STDLINE("=== Using a TApplication only ===" ,ACRed);

  XmlParser theXmlParser;
  std::string configFileName;

  if      (argc == 1) configFileName = std::string("./xml/ExpressConfiguration.xml");
  else if (argc == 2) configFileName = std::string("./xml/") + argv[1];
  else if (argc >  2)
    {
      ss.clear();
      ss.str("");
      ss << "Usage: ./MonicelliExpress optional(configuration file)";
      STDLINE(ss.str(),ACRed);

      exit(EXIT_SUCCESS);
    }

  ss.clear();
  ss.str("");
  ss << "Using: " << configFileName << " configuration.";
  STDLINE(ss.str(),ACGreen);

  theXmlParser.parseDocument(configFileName.c_str());
  
  
  const string filesPath                = theXmlParser.getDefaults()->filesPath_;
  const string geometriesPath           = theXmlParser.getDefaults()->geometriesPath_;
  std::string  trackFindingAlgorithm    = theXmlParser.getDefaults()->trackFindingAlgorithm_;
  std::string  trackFittingAlgorithm    = theXmlParser.getDefaults()->trackFittingAlgorithm_;
  int          numberOfEvents           = theXmlParser.getDefaults()->numberOfEvents_;
  double       chi2Cut                  = theXmlParser.getDefaults()->chi2Cut_;
  int          trackPoints              = theXmlParser.getDefaults()->trackPoints_;
  int          maxPlanePoints           = theXmlParser.getDefaults()->maxPlanePoints_;
  int	       xTolerance               = theXmlParser.getDefaults()->xTolerance_;
  int	       yTolerance               = theXmlParser.getDefaults()->yTolerance_;
  bool         findDut                  = theXmlParser.getDefaults()->findDut_;
  bool         useEtaFunction           = theXmlParser.getDefaults()->useEtaFunction_;
  bool         doDUTFineAlignment       = theXmlParser.getDefaults()->doDUTFineAlignment_;
  bool         doTelescopeFineAlignment = theXmlParser.getDefaults()->doTelescopeFineAlignment_;


  for (unsigned int f = 0; f < theXmlParser.getFileList().size(); f++)
    {
      string fileName    = filesPath      + theXmlParser.getFileList()[f]->fileName_;
      string geoFileName = geometriesPath + theXmlParser.getFileList()[f]->geometryName_;

      fileEater	theFileEater;
      HManager theHManager(&theFileEater);
      theFileEater.setHManger(&theHManager);
      clusterizer theClusterizer;
      trackFitter theTrackFitter;
      trackFinder theTrackFinder(&theTrackFitter);
      aligner theTelescopeAligner(&theFileEater,&theHManager);
      aligner theDUTAligner(&theFileEater,&theHManager);



      // #########################
      // # Parse and make events #
      // #########################
      STDLINE("",ACBlue);
      STDLINE("Parse and Make Events",ACBlue);

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



      // ###############
      // # Clusterizer #
      // ###############
      STDLINE("Clusterizer",ACBlue);

      if (useEtaFunction) theClusterizer.getChargeAsymmetryPlots(theGeometry);
      else                theClusterizer.setUseEtaFunction(false);
      theFileEater.setOperation(&fileEater::updateEvents2,&theClusterizer);
      theFileEater.updateEvents2();



      // #################
      // # Raw alignment #
      // #################
      if (DORAWALIGNMENT == true)
	{
	  STDLINE("Raw Alignment",ACBlue);
	  
	  HManager::stringVDef histoType;
	  fitter theFitter;
	  TH1* histo;
	  TF1* fitFunc;
	  bool reDo = true;
	  double xPosition;
	  double yPosition;
	  double xPositionErr;
	  double yPositionErr;

	  for (int myIt = 0; myIt < theFileEater.getEventsNumber(); myIt++)
	    histoType = theHManager.makeBeamSpots2(theFileEater.getEvent(myIt), reDo);

	  for (Geometry::iterator it = theGeometry->begin(); it != theGeometry->end(); it++)
            {
	      histo = (TH1*)theHManager.getHistogram(histoType[1], (*it).first);
	      theFitter.gaussFit(histo);

	      fitFunc      = (TF1*)histo->FindObject("gausFitFunc");
	      xPosition    = fitFunc->GetParameter(0);
	      xPositionErr = fitFunc->GetParError(0) / 2.;


	      histo = (TH1*)theHManager.getHistogram(histoType[2], (*it).first);
	      theFitter.gaussFit(histo);

	      fitFunc      = (TF1*)histo->FindObject("gausFitFunc");
	      yPosition    = fitFunc->GetParameter(0);
	      yPositionErr = fitFunc->GetParError(0) / 2.;


	      theGeometry->getDetector(it->first)->flipPositionLocal(&xPosition, &yPosition, &xPositionErr, &yPositionErr);
	      theGeometry->getDetector(it->first)->setXPosition(xPosition);
	      theGeometry->getDetector(it->first)->setXPositionError(xPositionErr);
	      theGeometry->getDetector(it->first)->setYPosition(yPosition);
	      theGeometry->getDetector(it->first)->setYPositionError(yPositionErr);
	    }



	  // ###################
	  // # Update geometry #
	  // ###################	
	  STDLINE("Update Geometry",ACBlue);
	  
	  theFileEater.updateGeometry("geometry");



	  // ################
	  // # Track finder #
	  // ################
	  STDLINE("Track Finder",ACBlue);
	  
	  theTrackFinder.setTrackSearchParameters(xTolerance*(1e-4)*CONVF, yTolerance*(1e-4)*CONVF, chi2Cut, trackPoints, maxPlanePoints);
	  theTrackFinder.setTrackingOperationParameters(trackFindingAlgorithm, trackFittingAlgorithm, findDut);
	  theFileEater.setOperation(&fileEater::updateEvents2,&theTrackFinder);
	  theTrackFinder.setOperation(&trackFinder::findAndFitTracks);
	  theFileEater.updateEvents2();



	  // ################
	  // # Slope finder #
	  // ################
	  STDLINE("Slope Finder",ACBlue);

	  reDo = true;
	  double slopeX;
	  double slopeY;

	  for (int myIt = 0; myIt < theFileEater.getEventsNumber(); myIt++)
	    histoType = theHManager.makeTracksDistr2(theFileEater.getEvent(myIt), reDo);


	  histo = (TH1*)theHManager.getHistogram(histoType[1]);
	  theFitter.gaussFit(histo);

	  fitFunc = (TF1*)histo->FindObject("gausFitFunc");
	  slopeX = fitFunc->GetParameter(0);

	  
	  histo = (TH1*)theHManager.getHistogram(histoType[2]);
	  theFitter.gaussFit(histo);

	  fitFunc = (TF1*)histo->FindObject("gausFitFunc");
	  slopeY = fitFunc->GetParameter(0);


	  for (Geometry::iterator it = theGeometry->begin(); it != theGeometry->end(); it++)
	    {
	      xPosition = sin(atan(slopeX)) * theGeometry->getDetector(it->first)->getZPositionTotal();
	      yPosition = sin(atan(slopeY)) * theGeometry->getDetector(it->first)->getZPositionTotal();

	      theGeometry->getDetector(it->first)->setXPositionCorrection(theGeometry->getDetector(it->first)->getXPositionCorrection() + xPosition);
	      theGeometry->getDetector(it->first)->setYPositionCorrection(theGeometry->getDetector(it->first)->getYPositionCorrection() + yPosition);
	    }



	  // ###################
	  // # Update geometry #
	  // ###################	
	  STDLINE("Update Geometry",ACBlue);
	  
	  theFileEater.updateGeometry("geometry");
	}




      unsigned int nAlign = doTelescopeFineAlignment;
      if (DORAWALIGNMENT == true) nAlign = 2;
      for (unsigned int i = 0; i < nAlign; i++)
	{
	  // ################
	  // # Track finder #
	  // ################
	  STDLINE("Track Finder",ACBlue);
	  
	  theTrackFinder.setTrackSearchParameters(xTolerance*(1e-4)*CONVF, yTolerance*(1e-4)*CONVF, chi2Cut, trackPoints, maxPlanePoints);
	  theTrackFinder.setTrackingOperationParameters(trackFindingAlgorithm, trackFittingAlgorithm, findDut);
	  theFileEater.setOperation(&fileEater::updateEvents2,&theTrackFinder);
	  theTrackFinder.setOperation(&trackFinder::findAndFitTracks);
	  theFileEater.updateEvents2();



	  // ############################
	  // # Telescope fine alignment #
	  // ############################
	      STDLINE("Telescope Fine Alignment",ACBlue);
	      
	      theTelescopeAligner.setAlignmentFitMethodName("Simple");
	      theTelescopeAligner.setNumberOfIterations(0);
	      
	      for (Geometry::iterator it = theGeometry->begin(); it != theGeometry->end(); it++)
		{
		  if (!(*it).second->isDUT()) theTelescopeAligner.setFixParMap((*it).first, 100000);
		  else                        theTelescopeAligner.setFixParMap((*it).first, 111111);
		}
	      if ((DORAWALIGNMENT == true) && (i == 0))
		theTelescopeAligner.setAlignmentPreferences(5, 0, CHI2RAWALIGN, 2, trackPoints, 1, true, "", numberOfEvents);
	      else
		theTelescopeAligner.setAlignmentPreferences(5, 0, chi2Cut, 2, trackPoints, 1, true, "", numberOfEvents);
	      // #############################
	      // # Parameter meaning:        #
	      // #############################
	      // # - max trials              #
	      // # - fine alignment strategy #
	      // # - chi2cut                 #
	      // # - max cluster size        #
	      // # - min points              #
	      // # - max tracks / ev         #
	      // # - no diagonal clusters    #
	      // # - alignment select        #
	      // # - nEvents                 #
	      // #############################
	      theTelescopeAligner.setOperation(&aligner::align);
	      theTelescopeAligner.align();
	      
	      
	      aligner::alignmentResultsDef alignmentResultsTelescope = theTelescopeAligner.getAlignmentResults();
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



	      // ###################
	      // # Update geometry #
	      // ###################	
	      STDLINE("Update Geometry",ACBlue);
	      
	      theFileEater.updateGeometry("geometry");	     
	}	      



      // ################
      // # Track finder #
      // ################
      STDLINE("Track Finder",ACBlue);
      
      theTrackFinder.setTrackSearchParameters(xTolerance*(1e-4)*CONVF, yTolerance*(1e-4)*CONVF, chi2Cut, trackPoints, maxPlanePoints);
      theTrackFinder.setTrackingOperationParameters(trackFindingAlgorithm, trackFittingAlgorithm, findDut);
      theFileEater.setOperation(&fileEater::updateEvents2,&theTrackFinder);
      theTrackFinder.setOperation(&trackFinder::findAndFitTracks);
      theFileEater.updateEvents2();



      // ######################
      // # Fine alignment DUT #
      // ######################
      if (doDUTFineAlignment == true)
	{
	  if (DUT2STEPS == true)
	    {
	      // #########################################
	      // # Track finder on DUT: large rod search #
	      // #########################################
	      STDLINE("Track Finder on DUT: large rod search",ACBlue);
	      
	      theTrackFinder.setTrackSearchParameters(LARGEROD*(1e-4)*CONVF, LARGEROD*(1e-4)*CONVF, chi2Cut, trackPoints, maxPlanePoints);
	      theFileEater.setOperation(&fileEater::updateEvents2,&theTrackFinder);
	      theTrackFinder.setOperation(&trackFinder::findDUTCandidates);
	      theFileEater.updateEvents2();



	      // ###############################
	      // # Fine alignment DUT: only XY #
	      // ###############################
	      STDLINE("Fine Alignment DUT: only XY",ACBlue);
	      
	      for (Geometry::iterator it = theGeometry->begin(); it != theGeometry->end(); it++)
		{
		  if (!(*it).second->isDUT()) continue;
		  
		  string dut = it->first;
		  
		  theDUTAligner.setFixParMap(dut,100111); // Here is where I choose which parameters must be kept constant
		  theDUTAligner.setAlignmentPreferences(5, 0, CHI2RAWALIGN, 2, trackPoints, 1, true, dut, numberOfEvents);
		  theDUTAligner.setOperation(&aligner::alignDUT);
	          theDUTAligner.alignDUT();

		  aligner::alignmentResultsDef alignmentResults = theDUTAligner.getAlignmentResults();
		  Detector* theDetector = theGeometry->getDetector(dut);
		  
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
		}



	      // ###################
	      // # Update geometry #
	      // ###################	
	      STDLINE("Update Geometry",ACBlue);

	      theFileEater.updateGeometry("geometry");



	      // #######################
	      // # Track finder on DUT #
	      // #######################
	      STDLINE("Track Finder on DUT",ACBlue);
	      
	      theTrackFinder.setTrackSearchParameters(xTolerance*(1e-4)*CONVF, yTolerance*(1e-4)*CONVF, chi2Cut, trackPoints, maxPlanePoints);
	      theFileEater.setOperation(&fileEater::updateEvents2,&theTrackFinder);
	      theTrackFinder.setOperation(&trackFinder::findDUTCandidates);
	      theFileEater.updateEvents2();
	    }
	  
	  
	  
	  // ######################
	  // # Fine alignment DUT #
	  // ######################
	  STDLINE("Fine Alignment DUT: XY + angles",ACBlue);

	  for (Geometry::iterator it = theGeometry->begin(); it != theGeometry->end(); it++)
	    {
	      if (!(*it).second->isDUT()) continue;
	  
	      string dut = it->first;

	      theDUTAligner.setFixParMap(dut,DUTfreePLANES); // Here is where I choose which parameters must be kept constant
	      theDUTAligner.setAlignmentPreferences(5, 0, chi2Cut, 2, trackPoints, 1, true, dut, numberOfEvents);
	      theDUTAligner.setOperation(&aligner::alignDUT);
	      theDUTAligner.alignDUT();

	      aligner::alignmentResultsDef alignmentResults = theDUTAligner.getAlignmentResults();
	      Detector* theDetector = theGeometry->getDetector(dut);

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
	    }



	  // ###################
	  // # Update geometry #
	  // ###################	
	  STDLINE("Update Geometry",ACBlue);
	  
	  theFileEater.updateGeometry("geometry");



	  // #######################
	  // # Track finder on DUT #
	  // #######################
	  STDLINE("Track Finder on DUT: final",ACBlue);

	  theTrackFinder.setTrackSearchParameters(xTolerance*(1e-4)*CONVF, yTolerance*(1e-4)*CONVF, chi2Cut, trackPoints, maxPlanePoints);
	  theFileEater.setOperation(&fileEater::updateEvents2,&theTrackFinder);
	  theTrackFinder.setOperation(&trackFinder::findDUTCandidates);
	  theFileEater.updateEvents2();
	}



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
      if (COPYGEOFILE == true)
	{
	  STDLINE("Copy Geometry File",ACBlue);
	  
	  string outputFilePath = filesPath;
	  outputFilePath.erase(outputFilePath.length()-8,outputFilePath.length()).append("/MonicelliOutput/");
	  
	  string newGeoName = theXmlParser.getFileList()[f]->fileName_;
	  newGeoName.erase(newGeoName.length()-4,newGeoName.length()).append(".geo");
	  
	  string copyGeometryCommand = "cp " + outputFilePath + newGeoName + " " + geometriesPath + newGeoName;
	  STDLINE(copyGeometryCommand.c_str(),ACBlue);
	  system(copyGeometryCommand.c_str());
	}
    }
  return EXIT_SUCCESS;
}
