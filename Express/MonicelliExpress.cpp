#include "fileEater.h"
#include "clusterizer.h"
#include "trackFitter.h"
#include "trackFinder.h"
#include "HManager.h"
#include "Geometry.h"
#include <TApplication.h>

#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <QDomDocument>
//#include <QDomImplementation>
//#include <QDomDocumentType>
#include <QFile>
#include <QString>
#include <QDomNode>

using namespace std;

class XmlDefaults;
class XmlFile;

class XmlParser
{
public:
    XmlParser (void);
    ~XmlParser(void);

    void parseDocument(std::string fileName);

    XmlDefaults*          getDefaults(void){return theDefaults_;}
    std::vector<XmlFile*> getFileList(void){return theFileList_;}

private:
    QDomDocument*  document_;
    QDomNode       rootNode_;

    XmlDefaults*          theDefaults_;
    std::vector<XmlFile*> theFileList_;
    stringstream ss_;
};

class XmlDefaults
{
public:
    XmlDefaults (QDomNode& node);
    ~XmlDefaults(void){;}
    QDomNode&   getNode(void){return thisNode_;}
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

private:
    QDomNode    thisNode_;
};

class XmlFile
{
public:
    XmlFile (QDomNode& node);
    ~XmlFile(void){;}
    QDomNode&   getNode(void){return thisNode_;}
    std::string fileName_;
    std::string geometryName_;

private:
    QDomNode    thisNode_;
};

int main(int argc, char** argv)
{
  stringstream ss;
  TApplication app("App",&argc,argv);
  XmlParser theXmlParser;
  std::string configFileName = "./xml/ExpressConfiguration.xml";
//  std::cout << argc << " " << argv[1] << std::endl;
//  return 1;
  if(argc == 2)
    configFileName = std::string("./xml/") + argv[1];
  else if(argc > 2)
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

  const string filesPath            = theXmlParser.getDefaults()->filesPath_     ;
  const string geometriesPath       = theXmlParser.getDefaults()->geometriesPath_;
  std::string trackFindingAlgorithm = theXmlParser.getDefaults()->trackFindingAlgorithm_;
  std::string trackFittingAlgorithm = theXmlParser.getDefaults()->trackFittingAlgorithm_;
  int    numberOfEvents             = theXmlParser.getDefaults()->numberOfEvents_;
  double chi2Cut                    = theXmlParser.getDefaults()->chi2Cut_       ;
  int    trackPoints                = theXmlParser.getDefaults()->trackPoints_   ;
  int    maxPlanePoints             = theXmlParser.getDefaults()->maxPlanePoints_;
  int	 xTolerance                 = theXmlParser.getDefaults()->xTolerance_    ;
  int	 yTolerance                 = theXmlParser.getDefaults()->yTolerance_    ;
  bool   findDut                    = theXmlParser.getDefaults()->findDut_       ;
  bool   useEtaFunction             = theXmlParser.getDefaults()->useEtaFunction_;
  std::cout << trackFindingAlgorithm << " " << trackPoints << std::endl;
  for(unsigned int f=0; f<theXmlParser.getFileList().size(); f++)
  {
    string fileName    = filesPath      + theXmlParser.getFileList()[f]->fileName_;
    string geoFileName = geometriesPath + theXmlParser.getFileList()[f]->geometryName_;

    fileEater	theFileEater;
    HManager    theHManager(&theFileEater);
    theFileEater.setHManger(&theHManager);
    clusterizer theClusterizer;
    trackFitter theTrackFitter;
    trackFinder theTrackFinder(&theTrackFitter);

    //////////////////////////////////////////
    //Parse and make events
    if(theFileEater.openFile(geoFileName)=="Error!")
    {
       STDLINE("Error in geoFileName",ACRed);
       continue;
    }   
    theFileEater.setInputFileName( fileName );
    theFileEater.setEventsLimit( numberOfEvents );
  
    Geometry* theGeometry = theFileEater.getGeometry();
    
    if(!theFileEater.parse())
    {
       STDLINE("Error in parsing",ACRed);
       continue;
    }
    theHManager.setRunSubDir( theFileEater.openFile(theFileEater.getOutputTreeCompletePath()));
    ////////////////////////////////////////////////////////////


    //////////////////////////////////////////
    //Show beamspots
    theFileEater.populate();
    
    //////////////////////////////////////////
    //////////////////////////////////////////
    //Clusterize
    if(useEtaFunction)
        theClusterizer.getChargeAsymmetryPlots(theGeometry);
    else
        theClusterizer.setUseEtaFunction(false);
    theFileEater.setOperation(&fileEater::updateEvents2,&theClusterizer);
    theFileEater.updateEvents2();
    //////////////////////////////////////////

    //////////////////////////////////////////
    //Track finder
    theTrackFinder.setTrackSearchParameters(xTolerance*(1e-4)*CONVF,
    					    yTolerance*(1e-4)*CONVF,
    					    chi2Cut,
    					    trackPoints,
    					    maxPlanePoints );


    theTrackFinder.setTrackingOperationParameters(trackFindingAlgorithm, trackFittingAlgorithm, findDut);

    theFileEater.setOperation(&fileEater::updateEvents2,&theTrackFinder);
    theTrackFinder.setOperation(&trackFinder::findAndFitTracks);

//     if(trackFindingAlgorithm == "FirstAndLast")
//       if(findDut) 
//         theTrackFinder.setOperation(&trackFinder::findAllFirstAndLast);
//       else				  
//         theTrackFinder.setOperation(&trackFinder::findFirstAndLastTrackCandidates);
//     else if(trackFindingAlgorithm == "RoadSearch")
//       if(findDut) 
//         theTrackFinder.setOperation(&trackFinder::findAllRoadSearch);
//       else				  
//         theTrackFinder.setOperation(&trackFinder::findRoadSearchTrackCandidates);
//     else
//     {
//       ss.str("");
//       ss << "ERROR: Unknown algorithm name: " << trackFindingAlgorithm << ". Possible values are FirstAndLast or RoadSearch only.";
//       STDLINE(ss.str(),ACRed);
//       exit(EXIT_FAILURE);
//     }
    theFileEater.updateEvents2();

    //////////////////////////////////////////
    //Residuals
    theTrackFitter.clearSelectedDetectorsList();

    theTrackFitter.setOperation(&trackFitter::makeFittedTracksResiduals);
    theFileEater.setOperation(&fileEater::updateEvents2,&theTrackFitter);
    theFileEater.updateEvents2();
  }
  
  return EXIT_SUCCESS;
}

//================================================================================
//================================================================================
//================================================================================
XmlParser::XmlParser(void) : document_(0)
{
}

//================================================================================
XmlParser::~XmlParser()
{
    if(document_)
        delete document_ ;
}

//================================================================================
void XmlParser::parseDocument(std::string xmlFileName)
{
    if(document_)
        delete document_;

//    QDomImplementation implementation;
//    QDomDocumentType type = implementation.createDocumentType("ConfigurationFile","MonicelliExpressConfiguration","/home/uplegger/Programming/Monicelli/Express/xml/dtd/ExpressConfiguration.dtd");
    document_ = new QDomDocument( "ConfigurationFile" );
    QFile xmlFile(xmlFileName.c_str());
    if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text ))
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

    for(int f=0; f<fileList.size(); ++f)
    {
        QDomNode fileNode = fileList.at(f);
        if(!fileNode.isComment())
	  theFileList_.push_back(new XmlFile(fileNode));
    }

    xmlFile.close();
}

//================================================================================
//================================================================================
//================================================================================
XmlDefaults::XmlDefaults(QDomNode& node)
{
    thisNode_       	   = node;
    filesPath_      	   = node.toElement().attribute("FilesPath")		.toStdString();
    geometriesPath_ 	   = node.toElement().attribute("GeometriesPath")	.toStdString();
    trackFindingAlgorithm_ = node.toElement().attribute("TrackFindingAlgorithm").toStdString();
    trackFittingAlgorithm_ = node.toElement().attribute("TrackFittingAlgorithm").toStdString();
    numberOfEvents_        = node.toElement().attribute("NumberOfEvents") 	.toInt();
    chi2Cut_        	   = node.toElement().attribute("Chi2Cut")	 	.toInt();
    trackPoints_    	   = node.toElement().attribute("TrackPoints")    	.toInt();
    maxPlanePoints_ 	   = node.toElement().attribute("MaxPlanePoints") 	.toInt();
    xTolerance_     	   = node.toElement().attribute("XTolerance")	 	.toInt();
    yTolerance_     	   = node.toElement().attribute("YTolerance")	 	.toInt();
    findDut_        	   = true;
    useEtaFunction_        = true;
    if(node.toElement().attribute("FindDut") == "false" || node.toElement().attribute("FindDut") == "False" )
      findDut_ = false;
    if(node.toElement().attribute("UseEtaFunction") == "false" || node.toElement().attribute("UseEtaFunction") == "False" )
      useEtaFunction_ = false;
}

//================================================================================
//================================================================================
//================================================================================
XmlFile::XmlFile(QDomNode& node)
{
    thisNode_       = node;
    fileName_       = node.toElement().attribute("Name")    .toStdString();
    geometryName_   = node.toElement().attribute("Geometry").toStdString();
}
