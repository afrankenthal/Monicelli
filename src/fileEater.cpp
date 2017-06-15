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
 
#include <iomanip>
#include <sstream>

#include <time.h>
#include <QTime>
#include <QRegExp>
#include <QStringList>

#include "fileEater.h"
#include "MessageTools.h"
#include "verbosity.h"

#include <boost/filesystem/operations.hpp>

#define GEOMETRY_TREE_NAME "Geometry"

//============================================================================
bool fileEater::execute(void)
{
    STDLINE("fileEater process has started", ACGreen) ;
    return (this->*processOperation_)();
}

//=============================================================================
fileEater::fileEater(void) :
    inputGeometryTree_  (0)
  , inputEventTree_     (0)
  , inputEventHeader_   (0)
  , emptyH_             (new TH2I())
  , theBeamSimulator_   (new beamSimulator())
  , theEvent_           (new Event())
  , theEventHeader_     (new EventHeader())
  , theGeometryLoader_  (new geometryLoader())
  , theGeometry_        (theGeometryLoader_->getGeometry())
  , histogramsFolder_   (new TFolder("histograms","histograms"))
  , writeOutFile_       (false)
  , eventsLimit_        (-1)
  , currentVerbosity_   (kLOW)
  , currentAction_      ("Idle")
{
    inputFileName_    = "" ;
    emptyH_->SetName("emptyFileEater");
    //gROOT->Reset() ;
    outputTreePath_   = getenv("MonicelliOutputDir");
    //create directories
    beamSpots_        = histogramsFolder_->AddFolder("originalBeamSpots", "Beam spot (high resolution, 1 bin per pixel)");
    lowResBeamSpots_  = histogramsFolder_->AddFolder("lowResBeamSpots"  , "Beam spot (low resolution)"                  );
    flippedBeamSpots_ = histogramsFolder_->AddFolder("flippedBeamSpots" , "Flipped beam spot"                           );
    beamProfiles_     = histogramsFolder_->AddFolder("beamProfiles"     , "Beam spot profiles"                          );
    xProfiles_        = beamProfiles_    ->AddFolder("Xprofiles"        , "X profiles"                                  );
    yProfiles_        = beamProfiles_    ->AddFolder("Yprofiles"        , "Y profiles"                                  );
    histogramsFolder_->SetOwner(kTRUE);

}
//=============================================================================
fileEater::~fileEater(void)
{
    STDLINE("entering fileEater destructor... ", ACRed);

    inputTreesMap_.clear()  ;
    delete theBeamSimulator_;
    //delete theClusterizer_  ;
    delete theEvent_        ;
    delete theEventHeader_  ;
    delete theGeometry_     ;
    //delete theTrackFinder_  ;
    //delete theTrackFitter_  ;
    delete emptyH_          ;
}

//=============================================================================
void fileEater::setVerbosity(int verbosity)
{
    currentVerbosity_ = verbosity;
    ss_.str("") ;
    ss_ << "Setting verbosity to " << currentVerbosity_ ;
    STDLINE(ss_.str(),ACCyan) ;
}

//=============================================================================
fileEater::fileEater(int argc, char ** argv)
{
    std::string inputMergedFile;
    if (argc<2)
    {
        STDLINE("USAGE:\n$./Puzzle \"./mergedFileName.txt\" ", ACWhite);
        inputMergedFile ="./summary.txt";
        STDLINE(std::string("Reading default merged file: ") + inputMergedFile, ACYellow) ;
    }
    else
    {
        inputMergedFile = argv[1];
        STDLINE(std::string("Reading merged file: ") + inputMergedFile, ACWhite) ;
    }

    //inputMergedFile_ = new std::ifstream(inputMergedFile_.c_str(), std::ifstream::in );

    ss_.str(""); ss_ << "Once more: " << inputMergedFile ; STDLINE(ss_.str(),ACPurple) ;
    this->openFile(inputMergedFile) ;
}

//=============================================================================
std::string fileEater::openFile(std::string inputFile)
{
    std::string fileName = inputFile ;
    if(fileName == "No file loaded" ) return "";
    boost::cmatch what;
    static const boost::regex exp(".*?[\\/](\\w+)\\.(dat|txt|root|xml|nhd|geo)", boost::regex::perl);

    ss_.str("") ; ss_ << "Trying to open " << inputFile ; STDLINE(ss_.str(),ACPurple) ;

    if( boost::regex_match(inputFile.c_str(), what, exp, boost::match_extra) )
    {
        inputFileName_ = fileName ;
        fileName       = what[1] ;
        fileFormat_    = std::string(what[2]);

        if (fileFormat_=="root") // =============== R O O T  F I L E ======================
        {
            //open event root file
            if( inputTreesMap_.find(inputFile)==inputTreesMap_.end() )
            {
                if(inputTreesMap_.size() > 40) inputTreesMap_.erase(inputTreesMap_.begin());
                inputTreesMap_[inputFile] = new TFile( inputFile.c_str(), "read" );
                STDLINE(std::string("Successfully opened ") + inputFile, ACGreen)  ;
            }
            if( !inputTreesMap_[inputFile]->IsOpen() )
            {
                STDLINE(std::string("Could not open file ") + inputFile, ACRed) ;
                exit(-1) ;
            }

            QString inputFileName = fileName.c_str();

            std::stringstream eventsTreeName;
            std::stringstream eventsHeaderName;
            eventsTreeName   << inputFileName.toStdString() << ".root" << "Events";
            eventsHeaderName << inputFileName.toStdString() << ".root" << "Header";

            inputEventTree_ = (TTree*)inputTreesMap_[inputFile]->Get(eventsTreeName.str().c_str()) ;

            if ( inputEventTree_ )
            {
                inputEventTree_    ->SetBranchAddress("EventBranch", &theEvent_      );

                //STDLINE(eventsHeaderName.str(),ACPurple) ;
                inputEventHeader_ = (TTree*)inputTreesMap_[inputFile]->Get(eventsHeaderName.str().c_str()) ;
                if( inputEventHeader_ )
                {
                    inputEventHeader_->SetBranchAddress("EventHeader", &theEventHeader_);
                    ss_.str("");
                    ss_ << "Found " << inputEventHeader_->GetEntries() << " entry in EventHeader";
                    STDLINE(ss_.str(),ACGreen) ;
                    inputEventHeader_->GetEntry(0);

                    if(theEventHeader_->clustersDone())   {STDLINE("Clusters already built and available",      ACGreen);}
                    else                                  {STDLINE("No clusters available so far",              ACRed  );}

                    if(theEventHeader_->tracksFound())    {STDLINE("Already reconstructed tracks found",        ACGreen);}
                    else                                  {STDLINE("No reconstructed tracks found so far",      ACRed  );}

                    //                 if(theEventHeader_->uResidualsComputed())
                    //                 {
                    //                   STDLINE("Already reconstructed tracks found",           ACGreen) ;
                    //                 }
                    //                 else
                    //                 {
                    //                   STDLINE("No reconstructed tracks found so far", ACRed  ) ;
                    //                 }
                }
                else
                {
                    STDLINE("WARNING: No header found",ACRed) ;
                }
                ss_.str("");
                ss_ << "Found " << inputEventTree_  ->GetEntries() << " events";
                STDLINE(ss_.str(),ACGreen) ;

                //open geometry root file
                boost::regex ext("\\.root", boost::regex::perl);
                std::string geometryFileName = std::string( boost::regex_replace(inputFile, ext, ".geo") );

                if ( this->openGeometryFile(geometryFileName) )
                {
                    fileFormat_="rootTree";
                    inputTreesMap_[inputFile]->cd();
                }
                else
                {
                    STDLINE("ERROR: Problems occurring with the opening of geometry file: " + geometryFileName, ACRed);
                }
            }
            else
            {
                STDLINE("No valid trees found in file " + inputFile, ACRed);
                STDLINE("A valid file name must be of the form: Run*_Merged.root ", ACGreen);
                exit(0) ;
            }
        }
        else if( fileFormat_=="geo" ) // =============== G E O  F I L E ======================
        {
            this->openGeometryFile(inputFile);
        }
        else if( fileFormat_=="xml" ) // =============== X M L  F I L E ======================
        {
            theGeometry_->clear();
            theGeometryLoader_->loadXML( inputFile )        ;
            theGeometryLoader_->fillGeometry("all")         ;
            theGeometry_ = theGeometryLoader_->getGeometry();
            updateGeometry("geometry");
        }
        else if( fileFormat_=="nhd" )   // =============== N H D  F I L E ======================
        {
            ss_.str("");
            ss_ << fileName <<  ".root";
            //theBeamSimulator_->growFakeTree(inputFile, outputTreePath_, ss_.str(), theEvent_, theGeometry_);
            theBeamSimulator_->growFakeTree2(9999,4,4,outputTreePath_, ss_.str(),theEvent_,theGeometry_);

            //fill geometry tree file
            ss_.str("");
            ss_ << fileName <<  ".geo";
            std::string outputGeometryTreeFileName = ss_.str();

            ss_.str("");
            ss_ << outputTreePath_ << "/" << outputGeometryTreeFileName;
            TFile outputGeometryTreeFile( ss_.str().c_str(), "recreate");

            TTree  outputGeometryTree( GEOMETRY_TREE_NAME, "A tree with a Geometry class")  ;
            //Fill Geometry Tree                                             ;
            outputGeometryTree.Branch("GeometryBranch" , "Geometry", &theGeometry_ , 16000, 0 ) ;                                                               ;
            outputGeometryTree.Fill();

            outputGeometryTreeFile.Write() ;
            outputGeometryTreeFile.Close() ;
        }
        else                         // =============== B I N A R Y   O R   A S C I I   F I L E  ======================
        {
            inputFile_ = new std::ifstream(inputFile.c_str(), std::ifstream::in );

            if( !inputFile_->is_open() )
            {
                STDLINE(std::string("Could not open file ") + inputFile, ACRed) ;
                exit(-1) ;
            }


            STDLINE(std::string("Successfully opened ") + inputFile, ACGreen)   ;

            ss_.str("");
            ss_ << fileName << ".root";
            outputTreeFileName_ = ss_.str();
        }
    }
    else
    {
        STDLINE(std::string("Could not open file ") + inputFile, ACRed) ;
        STDLINE("Invalid file name syntax or path!", ACRed) ;
    }

    return fileName;
}

//=============================================================================
bool fileEater::openGeometryFile(std::string geometryFileName)
{
    theGeometry_->clear();
    if( inputTreesMap_.find(geometryFileName)==inputTreesMap_.end() && inputTreesMap_.size() > 40)
        inputTreesMap_.erase(inputTreesMap_.begin());

    inputTreesMap_[geometryFileName] = new TFile( geometryFileName.c_str(), "read" );

// The following lines are useless: the error has apparantly no meaning!...
//    if( inputTreesMap_[geometryFileName]->GetErrno() != 0 )
//    {
//        ss_.str("");
//        ss_ << "File "
//            << geometryFileName
//            << " generates errors ("
//            << inputTreesMap_[geometryFileName]->GetErrno()
//            << "): see STDOUT";
//        STDLINE(ss_.str(),ACRed) ;
//        return false;
//    }

    if( !inputTreesMap_[geometryFileName]->IsOpen() )
    {
        STDLINE(std::string("Could not open file ") + geometryFileName, ACRed) ;
        return false;
    }
    else
        STDLINE(std::string("Successfully opened ") + geometryFileName, ACGreen)  ;

    //get geometry root tree
    inputGeometryTree_ = (TTree*)inputTreesMap_[geometryFileName]->Get(GEOMETRY_TREE_NAME);
    if ( inputGeometryTree_ )
    {
        theGeometry_->clear();
        inputGeometryTree_->SetBranchAddress("GeometryBranch" , &theGeometry_   );
        inputGeometryTree_->GetEntry(0); //it takes long time...

        //this implementation require geometry version changes:
        //adding the function and the variable to the geometry header.
        //users may need to reparse all their files with an xml geometry.
        if(theGeometry_->calibrationDone()){STDLINE("Calibrations already performed and available",ACGreen);}
        else                               {STDLINE("No calibrations available so far",            ACRed  );}



        return true;
    }
    else
    {
        STDLINE("WARNING: No geometry Tree found",ACRed) ;
        return false;
    }
}

//=============================================================================
bool fileEater::parse(void)
{
    bool success = false;

    if( theGeometry_->empty() )
    {
        STDLINE("ERROR: no geometry file was declared or file is empty!!", ACRed) ;
        return success ;
    }

    ss_.str(""); ss_ << "Opening to parse " << inputFileName_ ; STDLINE(ss_.str(),ACPurple) ;
    this->openFile( inputFileName_ );

    if (fileFormat_ == "dat" || fileFormat_ == "txt")
    {
        //success = parseBinary() ;
        success = this->growMagicTree2(true );
    }

    return success ;
}
//==============================================================================
std::map<std::string,TH2I*> & fileEater::getHistograms (void)
{
    return beamSpotsH_;
}
//================================================================================
void fileEater::populate(void)
{
    ss_.str("")  ;
    ss_ << outputTreePath_ << "/" << outputTreeFileName_ ;

    STDLINE(ss_.str(),ACPurple) ;
    this->openFile(ss_.str());
    if( fileFormat_!="rootTree" ) return ;
    inputGeometryTree_->GetEntry(0);
    int cols = theGeometry_->getMaxColsNum();
    int rows = theGeometry_->getMaxRowsNum();
    for (Geometry::iterator it=theGeometry_->begin(); it!=theGeometry_->end(); ++it)
    {
        if ( beamSpotsH_.find((*it).first) !=  beamSpotsH_.end() && beamSpotsH_[(*it).first] != 0)
            beamSpotsH_[(*it).first] ->Reset();
        else
            beamSpotsH_[(*it).first] = new TH2I((*it).first.c_str(), (*it).first.c_str(), cols, 0, cols, rows, 0, rows);
    }
    int pitStop = inputEventTree_->GetEntries() / 30 ;
    for( int ev=0; ev< inputEventTree_->GetEntries(); ++ev)
    {
        inputEventTree_->GetEntry(ev)  ;
        Event::plaqMapDef plaqMap = theEvent_ ->getRawData();

        if( ev % pitStop == 0 )
        {
            ss_.str("") ;
            ss_ << "Processing event " << ev ;
            STDSNAP(ss_.str(),ACWhite) ;
        }

        for(Event::plaqMapDef::iterator it2=plaqMap.begin(); it2!=plaqMap.end(); ++it2)
        {
            for (unsigned int i=0; i<(*it2).second.size(); i++)
                beamSpotsH_[(*it2).first]->Fill((*it2).second[i]["col"],(*it2).second[i]["row"]);
        }
    }
    std::cout << std::endl;
}
//=================================================================================================
bool fileEater::growMagicTree2(bool BinaryFile)
{
    //fill event tree file
    std::stringstream outputEventTreeCompletePath;
    outputEventTreeCompletePath << outputTreePath_ << "/" << outputTreeFileName_ ;    

    if ( inputTreesMap_.count(outputEventTreeCompletePath.str())!=0 )
    {
        inputTreesMap_[outputEventTreeCompletePath.str()]->Close() ;
        inputTreesMap_.erase( outputEventTreeCompletePath.str() )  ;
    }
    STDLINE(std::string("Opening: ") + outputEventTreeCompletePath.str(),ACPurple);
    TFile* outputEventTreeFile = TFile::Open( outputEventTreeCompletePath.str().c_str(), "RECREATE");

    ss_.str("");
    ss_ << outputTreeFileName_ << "Events";

    TTree* outputEventTree = new TTree( ss_.str().c_str(), "A tree with an Event class"  )  ;

    ss_.str("");
    ss_ << outputTreeFileName_ << "Header";

    TTree* outputEventHeader = new TTree( ss_.str().c_str(), "A tree with an Event Header"  )  ;

    //Fill Event Tree
    theEvent_->clear();
    outputEventTree  ->Branch("EventBranch", "Event",       &theEvent_,       16000, 0 ) ;
    outputEventHeader->Branch("EventHeader", "EventHeader", &theEventHeader_, 16000, 0 ) ;

    theEventHeader_->setClustersDone   (false);
    theEventHeader_->setTracksFound    (false);

    outputEventHeader->Fill();

    //int  nLine     = 0    ;

    beamSpotsH_.clear()       ;
    xProfileH_.clear()        ;
    yProfileH_.clear()        ;
    plaqMap_.clear()          ;
    flippedBeamSpotsH_.clear();

    if(writeOutFile_)
    {
        STDLINE("Writing ASCII output file: " + outputFileName_, ACRed) ;
        outputFile_.open( outputFileName_.c_str() );
    }

    if(BinaryFile)
    {
        this->parseBinary3(outputEventTree);
    }
    else
    {
        //timeStamp = this->parseMagicASCII (trig,nByte,nLine);
        STDLINE("Can't parse an ASCII file", ACRed);
        return false;
    }

    STDLINE("",ACWhite) ;

    if(writeOutFile_) outputFile_.close();

    outputEventTreeFile->Write();
    histogramsFolder_  ->Write();
    outputEventTreeFile->Close();

    this->makeGeometryTreeFile( outputEventTreeCompletePath.str() );
    //outputTreeFile.Close() ;//When another root file is open:
    //Fatal in <operator delete>: unreasonable size (57563656) aborting
    //or Fatal in <operator delete>: storage area overwritten aborting
    //Probably cause can you only close a pointer?

    return true;
}
//=============================================================================
void fileEater::makeGeometryTreeFile(std::string eventFileCompletePath)
{
    //fill geometry tree file
    boost::regex ext("\\.root", boost::regex::perl);
    std::string outputGeometryTreeFileName = boost::regex_replace(eventFileCompletePath, ext, ".geo");

    if ( inputTreesMap_.count(outputGeometryTreeFileName)!=0 )
    {
        inputTreesMap_[outputGeometryTreeFileName]->Close() ;
        inputTreesMap_.erase( outputGeometryTreeFileName )  ;
    }
    STDLINE(outputGeometryTreeFileName,ACYellow);
    TFile  outputGeometryTreeFile( outputGeometryTreeFileName.c_str(), "recreate");
    TTree  outputGeometryTree    ( GEOMETRY_TREE_NAME, "A tree with a Geometry class")  ;
    //Fill Geometry Tree                                             ;
    outputGeometryTree.Branch("GeometryBranch" , "Geometry" , &theGeometry_ , 16000, 0 ) ;
    outputGeometryTree.Fill();
    outputGeometryTreeFile.Write() ;
    //histogramsFolder_->Write()     ;
    openGeometryFile(outputGeometryTreeFileName);
}
//=============================================================================
bool fileEater::parseBinary3(TTree* tree)
{
    const int    dataSize    = 8;
    int          counts      = 0;
    long long    data        = 0;
    long long    orderedData = 0;
    long long    tmpData     = 0;
    int          nByte       = 0;
    bool         timeStamp   = false;
    int          adc         = 0;
    unsigned int col         = 0;
    unsigned int row         = 0;
    int          chip        = 0;
    int          dataDebug   = 0;
    int          module      = 0;
    int          trig        = 0;
    int          station     = 0;
    int          dataType    = 0;
    Event::aHitDef aHit;

    while( inputFile_->good() )
    {
        inputFile_->read( (char*)(&data),sizeof(long long) );
        nByte+=sizeof(long long);

        if(data == -1)
        {
            timeStamp = true;
            continue;
        }
        if( ( timeStamp || !inputFile_->good() ) )
        {

            theEvent_->setUTC(data);
            if(writeOutFile_)
                outputFile_ << std::dec << "Time: " << (data>>32) << (data & 0xffffffff) << std::endl;
            timeStamp = false;

            if( !plaqMap_.empty() )
            {
                if( counts++ % 1000 == 0 )
                {
                    ss_.str("");
                    ss_ << "trigger number " << trig << " (event " << counts - 1 << ")";
                    //std::cout << __PRETTY_FUNCTION__ << ss_ << " Printout " << std::endl;
                    STDSNAP("Building event for " + ss_.str() + "\n",ACGreen) ;
                }
                theEvent_->setRawData(trig, plaqMap_);
                //theEvent_->setRawData(counts, plaqMap_);
                //std::cout << __PRETTY_FUNCTION__ << theEvent_->getTrigger() << std::endl;
                tree->Fill();
                plaqMap_.clear();

                if( eventsLimit_ > 0 )
                {
                    currentIteration_ = counts;
                    if( counts >= eventsLimit_ ) break;
                }
                else currentIteration_ = nByte;
            }
        }
        else
        {

            orderedData = 0;

            for(int byte=0; byte < dataSize; byte++)
            {
                tmpData = 0;
                tmpData = ((data >> ((dataSize-byte-1)*8))& 0xff) << (byte*8);
                orderedData += tmpData;
            }

            dataType     =  (orderedData >> 60) & 0xf;
            station      =  (orderedData >> 56) & 0xf;

            if(dataType==0) //PSI46 pixels or DUTs
            //if(station == 0 || station == 2 || station == 4)
            {
                dataDebug    =  (orderedData >> 28) & 0x1    ;
                module       =  (orderedData >> 29) & 0x7    ;
                trig         =  (orderedData >> 32) & 0xfffff;
                if(dataDebug == 0)
                {
                    adc          =   orderedData & 0x3ff         ;
                    col          =  (orderedData >> 10) & 0x3f   ;
                    row          =  (orderedData >> 16) & 0x7f   ;
                    chip         =  (orderedData >> 23) & 0xf    ;
                }
                else
                {
                    adc          =  ((orderedData        & 0xf) + ((orderedData>>5  & 0xf)<<4))*4;
                    row          =  ((orderedData >> 9)  & 0x7) + (orderedData>>12 & 0x7)*6 + (orderedData>>15 & 0x7)*36;
                    col          =  ((orderedData >> 18) & 0x7) + (orderedData>>21 & 0x7)*6;
                    col = col*2 + row%2;
                    row = 80 - (row/2);
                    chip         =  ((orderedData >> 24) & 0xf);
                    //std::stringstream ss;
                    //ss.str("");
                    //ss << "Station: " << station << " chip: " << chip << " row: " << row << " col: " << col << " adc: " << adc;
                    //STDLINE(ss.str(), ACRed);
                }
                //if(station == 4)  //LUIGINO COMMENTS: what is this for...? using the strips I think I gotta remove it!
                //{
                //  theEvent_->setTimestamp       (orderedData & 0xffffffff );
                //  theEvent_->setBubbleSignal    ((orderedData >> 53) & 0x1);
                //  theEvent_->setBubbleAltSignal ((orderedData >> 52) & 0x1);
                //continue;//THIS ONE MUST BE REMOVED FOR REGULAR OPERATIONS
                //}
            }
            else if (dataType==1) //strips
            //else if(station == 5 || station == 6 || station == 7)//Strip data
            {
                dataDebug    = 0;
                trig         = (orderedData >> 32) & 0xfffff;
                module       = (orderedData >> 27) & 0x7;
                chip         = (orderedData >> 24) & 0x7;
                row          = 320;
                int strip    = (orderedData >> 17) & 0x0f;
                int set      = (orderedData >> 12) & 0x1f;
                col          = computeSensorStrip(chip, set, strip);
                adc          = (orderedData >> 1) & 0x7;
                chip         = 0;

//                std::cout << __PRETTY_FUNCTION__
//                          << "Adc: " << adc
//                          << " Set: " << set
//                          << " Strip: " << strip
//                          << " Module: " << module
//                          << " Chip: " << chip
//                          << " Col: " << col
//                          << std::endl;
            }
            else if (dataType==2) //vipic
            //else if(station == 5 || station == 6 || station == 7)//Strip data
            {
                dataDebug    = 0;
                trig         = (orderedData >> 32) & 0xfffff;
                module       = 0;
                chip         = 0;
//                int count    = (orderedData>>16)&0x0f;
//                int bco      = (orderedData>>8)&0xff;
                int chan     = (orderedData>>28)&0x0f;
                int pixel    = (orderedData>>20)&0xff;
                row          = pixel%64;
                col          = pixel/64 + (chan*4);

                std::cout << __PRETTY_FUNCTION__ << "Vipic row: " << row << " col: " << col << std::endl;
            }
            else
            {
                std::stringstream ss;
                ss.str("");
                ss << "Unrecognized data type:  " << dataType;
                //ss << "Unrecognized station number:  " << station;
                STDLINE(ss.str(), ACRed);
            }


            if(writeOutFile_)
            {
                outputFile_ << std::hex
                            << "data: "
                            << std::setw(8) << std::setfill('0')
                            << ((data>>32) & 0xffffffff)
                            << std::setw(8) << std::setfill('0')
                            << (data & 0xffffffff)
                            << " ordered:"
                            << std::setw(8) << std::setfill('0')
                            << ((orderedData>>32) & 0xffffffff)
                            << std::setw(8) << std::setfill('0')
                            << (orderedData & 0xffffffff);
                outputFile_ <<  std::dec
                             << " data type: " << dataType
                             << " trig: "      << trig
                             << " row: "	   << row
                             << " col: "	   << col
                             << " chip: "      << chip
                             << " module: "    << module
                             << " adc: "	   << adc
                             << " station: "   << station
                             << std::endl;
            }

            ss_.str("");
            ss_ << "Station: " << station << " - " << "Plaq: " << module;
            //STDLINE(ss_.str(), ACGreen) ;

            if( !theGeometry_->getDetector(ss_.str())                                           ||
                !theGeometry_->getDetector(ss_.str())->getROC(chip)                             ||
                !(row  < theGeometry_->getDetector(ss_.str())->getROC(chip)->getNumberOfRows()) ||
                !(col  < theGeometry_->getDetector(ss_.str())->getROC(chip)->getNumberOfCols()) )
            {
                if( currentVerbosity_ > 1 )
                {
                    std::string detName = ss_.str();
                    ss_.str("");
                    ss_ << ACRed << ACBold
                        << "WARNING: "
                        << ACWhite << ACBold
                        << "Incorrect data at block: "
                        << nByte
                        << " Station: " << station << " - " << "Plaq: " << module
                        << " Roc: "   << chip
                        << ", Row: "  << row
                        << ", Col: "  << col;
                    //<< " nrows: " << theGeometry_->getDetector(detName)->getROC(chip)->getNumberOfRows()
                    //<< " ncols: " << theGeometry_->getDetector(detName)->getROC(chip)->getNumberOfCols();
                    STDLINE(ss_.str(), ACCyan) ;
                }
                continue;
            }

            theGeometry_->getDetector(station, module)->convertPixelFromROC(theGeometry_->getDetector(station, module)->getROC(chip), &row, &col);

            //push back values
            aHit["row"] = row  ;
            aHit["col"] = col  ;
            aHit["adc"] = adc  ;
            aHit["dataType"] = dataType;
            //std::cout << __PRETTY_FUNCTION__ << "Data Type: " << dataType << " Size: " << aHit.size() << std::endl;
            plaqMap_[ss_.str()].push_back( aHit ) ;

            this->fillMagicPlaqComposition(station, module, row, col);
        }
    }
    std::cout << std::endl ;

    return true;
}

//============================================================================
bool fileEater::parseMagicASCII(int &trig, int &nByte, int &nLine)
{

    std::string line = "" ;
    getline(*inputFile_, line);
    nByte+=line.size()*sizeof(char)+1;
    nLine++;

    boost::cmatch what;
    ss_.str("") ;
    ss_ << "data:\\s+"
        << "(\\w+)"            // [1]
        << "\\s+ordered:\\s*"
        << "(\\w+)"            // [2]
        << "\\s+trig:\\s+"
        << "(\\d+)"            // [3] Trigger number
        << "\\s+row:\\s+"
        << "(\\d+)"            // [4] Row number
        << "\\s+col:\\s+"
        << "(\\d+)"            // [5] Column number
        << "\\s+chip:\\s+"
        << "(\\d+)"            // [6] Chip ID
        << "\\s+plaq:\\s+"
        << "(\\d+)"            // [7] Plaquette number
        << "\\s+adc:\\s+"
        << "(\\d+)"            // [8] ADC pulse height
        << "\\s+station:\\s+"
        << "(\\d+)"   ;        // [9] Station number
    static const boost::regex exp(ss_.str(), boost::regex::perl);

    ss_.str("") ;
    ss_ << "Time:\\s+(\\d+)" ;
    static const boost::regex ttempExp(ss_.str(), boost::regex::perl);

    if( boost::regex_match(line.c_str(), what, exp, boost::match_extra) )
    {
        unsigned int row = Utils::toInt(what[4]) ;
        unsigned int col = Utils::toInt(what[5]) ;
        int chip         = Utils::toInt(what[6]) ;
        int adc          = Utils::toInt(what[8]) ;
        int station      = Utils::toInt(what[9]) ;
        int module       = Utils::toInt(what[7]) ;

        if( Utils::toInt(what[3]) < trig)//what we wanna do with this?
        {
            ss_.str("");
            ss_ << "WARNING: Bad trigger information at line: " << nByte;
            STDLINE(ss_.str(),            ACRed) ;
        }

        ss_.str("");
        ss_ << "Station: " << station << " - " << "Plaq: " << module;

        if( !(row  < theGeometry_->getDetector(ss_.str())->getROC(chip)->getNumberOfRows()) ||
            !(col  < theGeometry_->getDetector(ss_.str())->getROC(chip)->getNumberOfCols()) ||
            !        theGeometry_->getDetector(ss_.str())                                   ||
            !        theGeometry_->getDetector(ss_.str())->getROC(chip))
        {
            ss_.str("");
            ss_ << "WARNING: Bad data at line: " << nLine;
            STDLINE(ss_.str(),            ACRed) ;
            STDLINE(std::string(what[0]), ACRed) ;
            return false;
        }

        trig = Utils::toInt(what[3]) ;

        theGeometry_->getDetector(station, module)->convertPixelFromROC(theGeometry_->getDetector(station, module)->getROC(chip),
                                                                        &row                                                    ,
                                                                        &col                                                   );
        //push back values
        Event::aHitDef aHit;
        aHit["row"] = row  ;
        aHit["col"] = col  ;
        aHit["adc"] = adc  ;

        plaqMap_[ss_.str()].push_back( aHit ) ;

        this->fillMagicPlaqComposition(station, module, row, col);

        return false;
    }
    else if( boost::regex_match(line.c_str(), what, ttempExp, boost::match_extra) ) return true;

    return false;
}
//============================================================================
void fileEater::fillMagicPlaqComposition( int station, int module, unsigned int row, unsigned int col )
{
    ss_.str("");
    ss_ << "Station: " << station << " - " << "Plaq: " << module;

    std::stringstream ss ; ss << "LR " << ss_.str() ;

    Detector *detector = theGeometry_->getDetector(ss_.str());
    //real time beam spot filling
    if ( !(beamSpotsH_.find(ss_.str()) != beamSpotsH_.end()) )
    {
        int cols = detector->getNumberOfCols();
        int rows = detector->getNumberOfRows();
        beamSpotsH_[ss_.str()] = new TH2I(ss_.str().c_str(), ss_.str().c_str(), cols, 0, cols, rows, 0, rows);
        beamSpotsH_[ss_.str()]->SetDirectory(0);
        beamSpots_->Add( beamSpotsH_[ss_.str()] );
    }
    beamSpotsH_[ss_.str()]->Fill(col,row);
    /*
    //detector->flipPixel( &row, &col );

    //append histograms to the root file
    //flipped beam spots
    int maxCols = theGeometry_->getMaxColsNum();
    int maxRows = theGeometry_->getMaxRowsNum();

    if ( maxCols < maxRows ) maxCols = maxRows;

    if ( flippedBeamSpotsH_.count(ss_.str())==0 )
    {
        flippedBeamSpotsH_[ss_.str()] = new TH2I(ss_.str().c_str(), ss_.str().c_str(), maxCols, 0, maxCols,
                                                                                       maxCols, 0, maxCols );
        flippedBeamSpotsH_[ss_.str()]->SetDirectory(0);
        flippedBeamSpots_->Add( flippedBeamSpotsH_[ss_.str()] );
        STDLINE("Filling " + ss_.str(),ACWhite);
    }
    //profiles
    if ( xProfileH_.count(ss_.str())==0 )
    {
        double xPixPitches[ detector->getNumberOfCols() ];
        double yPixPitches[ detector->getNumberOfRows() ];

        for( unsigned int i=0; i<detector->getNumberOfCols(); i++  )
        {
            xPixPitches[i] = detector->getPixelLowEdgeX(i);
        }
        xPixPitches[detector->getNumberOfCols()] = detector->getDetectorLengthX();

        for( unsigned int i=0; i<detector->getNumberOfRows(); i++  )
        {
            yPixPitches[i] = detector->getPixelLowEdgeY(i);
        }
        yPixPitches[detector->getNumberOfRows()] = detector->getDetectorLengthY();

        xProfileH_[ss_.str()] = new TH1D( ss_.str().c_str(), ss_.str().c_str(), detector->getLastCol(), xPixPitches);
        xProfileH_[ss_.str()]->SetDirectory(0) ;
        xProfiles_->Add( xProfileH_[ss_.str()] );
        yProfileH_[ss_.str()] = new TH1D( ss_.str().c_str(), ss_.str().c_str(), detector->getLastRow(), yPixPitches);
        yProfileH_[ss_.str()]->SetDirectory(0) ;
        yProfiles_->Add( yProfileH_[ss_.str()] );
        STDLINE("Filling " + ss_.str(),ACWhite);
    }

//STDLINE("",ACWhite) ;
    double y    = detector->getPixelLowEdgeY(row);
    double x    = detector->getPixelLowEdgeX(col);
//STDLINE("",ACWhite) ;
    double area = detector->getPixelPitchY(row)*
                  detector->getPixelPitchX(col);

//STDLINE("",ACWhite) ;
    xProfileH_[ss_.str()]->Fill( x , 1./area );
    yProfileH_[ss_.str()]->Fill( y , 1./area );

//STDLINE("",ACWhite) ;
    detector->flipPixel( &row, &col );

//STDLINE("",ACWhite) ;
    flippedBeamSpotsH_[ss_.str()]->Fill( col , row );
//STDLINE("",ACWhite) ;
*/
}
//=================================================================================================
void fileEater::closeFile()
{
    if(fileFormat_ == "dat" || fileFormat_ == "txt")
    {
        if(inputFile_->is_open())
        {
            inputFile_->close() ;
        }
    }
    else if(fileFormat_ == "root")
    {
        //inputTreeFile_->Close();
        inputTreesMap_.clear();
    }
}
//=============================================================================
Geometry* fileEater::getGeometry(void)
{
    //  if( fileFormat_=="rootTree" )
    //  {
    //     inputGeometryTree_->GetEntry(0);
    //  }

    return theGeometry_;
}
//=============================================================================
Event* fileEater::getEvent(int event)
{
    if( fileFormat_!="rootTree" ) return NULL;

    inputEventTree_->GetEntry(event) ;
    return theEvent_;
}
//============================================================================
bool fileEater::updateEvents2(void)
{
    ss_.str("");
    ss_ << inputEventTree_->GetCurrentFile()->GetName();
    std::string fileName = ss_.str();

    ss_.str("");
    ss_ << "./tmp/TreeClassExpanded_" <<  time(NULL) << "_" << fileName.substr(fileName.find_last_of('/')+1);
    std::string  copyCacheFileName = ss_.str();

    TFile *copyCacheFile = new TFile(copyCacheFileName.c_str(),"recreate") ;
    STDLINE("Cloning event tree... " ,ACCyan);
    TTree *newEventTree   = inputEventTree_  ->CloneTree(0);
    TTree *newEventHeader = NULL;
    if(inputEventHeader_!=NULL) newEventHeader = inputEventHeader_->CloneTree(0);
    STDLINE("Event tree cloned!" ,ACCyan);

    if(inputEventHeader_!=NULL) inputEventHeader_->GetEntry(0) ;

    int eventsNum = inputEventTree_->GetEntries();
    ss_.str(""); ss_ << "Events found in input file: " <<  eventsNum; STDLINE(ss_.str(),ACPurple) ;

    for( int ev=0; ev< eventsNum; ++ev)
    {
        inputEventTree_->GetEntry(ev) ;
        for(unsigned int i=0; i<currentSubProcess_.size(); i++)
        {
            currentSubProcess_[i]->execute(theEvent_,theGeometry_);
            if( ev%1000 == 0 )
            {
                ss_.str("");
                ss_ << ACPurple << ACBold
                    << "Processing event "
                    << ACCyan
                    << ev
                    << ACWhite << ACBold
                    << " ("
                    << currentSubProcess_[i]->getLabel()
                    << ")" ;
                STDSNAP(ss_.str(),ACPurple) ;
            }
        }
        currentIteration_ = ev;

        newEventTree->Fill() ;
    }
    std::cout << std::endl ;

    if(inputEventHeader_!=NULL)
    {
        for(unsigned int i=0; i<currentSubProcess_.size(); i++)
        {
            currentSubProcess_[i]->setHeader(theEventHeader_);
        }
        newEventHeader->Fill() ;
        //newEventHeader->AutoSave();
    }
    //newEventTree   ->AutoSave();
    newEventHeader->Write();
    newEventTree  ->Write();

    if ( inputTreesMap_[ fileName ]->FindObjectAny("histograms") )
        inputTreesMap_[ fileName ]->FindObjectAny("histograms")->Write();

    copyCacheFile->Close();
    delete copyCacheFile ;

    inputTreesMap_[ fileName ]->Close();
    inputTreesMap_.erase( fileName );

    ss_.str("");
    ss_ <<"mv " << copyCacheFileName << " " << fileName;
    system(ss_.str().c_str() ) ;
    STDLINE(ss_.str(),ACCyan)  ;
    this->openFile( fileName );

    return true;
}

//============================================================================
int    fileEater::getMaxIterations     (void                             )
{
    if(processOperation_ == &fileEater::parse)
    {
        if(eventsLimit_ <= 0) return boost::filesystem::file_size(inputFileName_.c_str() );
        else                  return eventsLimit_;
    }
    if(processOperation_ == &fileEater::updateEvents2) return this->getEventsNumber();
    else
    {
        if(eventsLimit_ <= 0) return boost::filesystem::file_size(inputFileName_.c_str() );
        else                  return (eventsLimit_);
    }

}

//============================================================================
int   fileEater::getCurrentIteration  (void                             )
{
    //    int iter = currentSubProcess_->getCurrentIteration();
    //    if( iter < 0 ) return currentIteration_;
    //    else           return iter             ;
    return currentIteration_;
}

//============================================================================
bool fileEater::updateGeometry(std::string what)
{
    std::stringstream ss;
    ss << "Input geometry tree pointer: " << inputGeometryTree_;
    STDLINE(ss.str(),ACPurple);
    if(!inputGeometryTree_) return false;
    std::string whatLowed = boost::algorithm::to_lower_copy(what);

    ss_.str("");
    ss_ << inputGeometryTree_->GetCurrentFile()->GetName();
    std::string fileName = ss_.str();
    ss_.str("");
    ss_ << "./tmp/TreeClassExpanded_" <<  time(NULL) << "_" << fileName.substr(fileName.find_last_of('/')+1);
    //ss_ << "./tmp/" << "treeClassExpanded" << time(NULL) <<".root";
    std::string  copyCacheFileName = ss_.str();
    TFile *copyCacheFile   = new TFile(copyCacheFileName.c_str(),"recreate") ;
    TTree *newGeometryTree = inputGeometryTree_->CloneTree(0) ;
    STDLINE("Geometry tree cloned!" ,ACCyan);

    newGeometryTree->Fill();

    newGeometryTree->AutoSave();

    //    if ( inputTreesMap_[ fileName ]->FindObjectAny("histograms") )
    //                                       inputTreesMap_[ fileName ]->FindObjectAny("histograms")->Write();

    copyCacheFile->Close();
    delete copyCacheFile ;

    inputTreesMap_[ fileName ]->Close();
    inputTreesMap_.erase( fileName );

    theGeometry_->dump();
    ss_.str("");
    ss_ <<"mv " << copyCacheFileName << " " << fileName;
    system(ss_.str().c_str() ) ;
    STDLINE(ss_.str(),ACCyan)  ;
    this->openFile( fileName );

    return true;
}
//============================================================================
int fileEater::getEventsNumber()
{
    if( !inputEventTree_) return 0;
    return inputEventTree_->GetEntries();
}
/*
//============================================================================
void fileEater::fillPlaqComposition(unsigned int row, unsigned int col, int adc, int station, int module, int trig)
{
    ss_.str("");
    ss_ << "Station: " << station << " - " << "Plaq: " << module;

    std::stringstream ss ; ss << "LR " << ss_.str() ;

    Detector *detector = theGeometry_->getDetector(ss_.str());
    //real time beam spot filling
    if ( beamSpotsH_.count(ss_.str())==0 )
    {
        int cols = detector->getNumberOfCols();
        int rows = detector->getNumberOfRows();
        beamSpotsH_[ss_.str()] = new TH2I(ss_.str().c_str(), ss_.str().c_str(), cols,   0, cols, rows,   0, rows);
        beamSpotsH_[ss_.str()]->SetDirectory(0);
        beamSpots_->Add( beamSpotsH_[ss_.str()] );

        STDLINE("Filling " + ss_.str(),ACWhite);

        beamSpotsH_[ss.str()] = new TH2I(ss.str().c_str(),   ss.str().c_str(),  cols/2, 0, cols, rows/2, 0, rows);
        beamSpotsH_[ss.str()]->SetDirectory(0);
        lowResBeamSpots_->Add( beamSpotsH_[ss.str()] );

    }
    beamSpotsH_[ss_.str()]->Fill(col,row);
    beamSpotsH_[ss .str()]->Fill(col,row);

    //detector->flipPixel( &row, &col );

    //push back values
    Event::aHitDef aHit;
    aHit["row"] = row  ;
    aHit["col"] = col  ;
    aHit["adc"] = adc  ;

    rawDataMap_[trig][ss_.str()].push_back( aHit )   ;
    currentProcessingStep_ = (int)rawDataMap_.size() ;

    //append histograms to the root file
    //flipped beam spots
    int maxCols = theGeometry_->getMaxColsNum();
    int maxRows = theGeometry_->getMaxRowsNum();

    if ( maxCols < maxRows ) maxCols = maxRows;

    if ( flippedBeamSpotsH_.count(ss_.str())==0 )
    {
        flippedBeamSpotsH_[ss_.str()] = new TH2I(ss_.str().c_str(), ss_.str().c_str(), maxCols, 0, maxCols,
                                                                                       maxCols, 0, maxCols );
        flippedBeamSpotsH_[ss_.str()]->SetDirectory(0);
        flippedBeamSpots_->Add( flippedBeamSpotsH_[ss_.str()] );
        STDLINE("Filling " + ss_.str(),ACWhite);
    }
    //profiles
    if ( xProfileH_.count(ss_.str())==0 )
    {
        double xPixPitches[ detector->getNumberOfCols() ];
        double yPixPitches[ detector->getNumberOfRows() ];

        for( unsigned int i=0; i<detector->getNumberOfCols(); i++  )
        {
            xPixPitches[i] = detector->getPixelLowEdgeX(i);
        }
        xPixPitches[detector->getNumberOfCols()] = detector->getDetectorLengthX();

        for( unsigned int i=0; i<detector->getNumberOfRows(); i++  )
        {
            yPixPitches[i] = detector->getPixelLowEdgeY(i);
        }
        yPixPitches[detector->getNumberOfRows()] = detector->getDetectorLengthY();

        xProfileH_[ss_.str()] = new TH1D( ss_.str().c_str(), ss_.str().c_str(), detector->getLastCol(), xPixPitches);
        xProfileH_[ss_.str()]->SetDirectory(0) ;
        xProfiles_->Add( xProfileH_[ss_.str()] );
        yProfileH_[ss_.str()] = new TH1D( ss_.str().c_str(), ss_.str().c_str(), detector->getLastRow(), yPixPitches);
        yProfileH_[ss_.str()]->SetDirectory(0) ;
        yProfiles_->Add( yProfileH_[ss_.str()] );
        STDLINE("Filling " + ss_.str(),ACWhite);
    }

    double y    = detector->getPixelLowEdgeY(row);
    double x    = detector->getPixelLowEdgeX(col);

    double area = detector->getPixelPitchY(row)*
                  detector->getPixelPitchX(col);

    xProfileH_[ss_.str()]->Fill( x , 1./area );
    yProfileH_[ss_.str()]->Fill( y , 1./area );

    detector->flipPixel( &row, &col );

    flippedBeamSpotsH_[ss_.str()]->Fill( col , row );
}
*/
//================================================================================
TFile * fileEater::getFileHandle(std::string fullPath)
{
    fileHandleDef::iterator fh = inputTreesMap_.find(fullPath) ;
    if( fh == inputTreesMap_.end() )
    {
        ss_.str("") ;
        ss_ << ACRed << ACBold << "WARNING:" << ACPlain << " File " << fullPath << " not currently open" ;
        STDLINE(ss_.str(),ACWhite) ;
        return NULL ;
    }

    return fh->second ;
}

//================================================================================
fileEater::subProcessVDef fileEater::getCurrentSubProcess()
{
    //    if( !currentSubProcess_.empty() ) return currentSubProcess_;
    //    else                              return this              ;
    return currentSubProcess_;
}

//================================================================================
std::string  fileEater::getLabel (void )
{
    if(processOperation_ == &fileEater::parse              ) return  "File parsing" ;
    if(processOperation_ == &fileEater::fullReconstruction )
    {
        ss_.str("");
        ss_ << inputFileName_ << " full reconstruction";
        return ss_.str();
    }
    if(processOperation_ == &fileEater::updateEvents2 && !currentSubProcess_.empty())
    {
        ss_.str("");
        for(unsigned int i=0; i<currentSubProcess_.size(); ++i)
        {
            if(i!=0) ss_ << "\n";
            ss_ << currentSubProcess_[i]->getLabel();
        }
        return ss_.str();
    }
    else
        return NO_PROCESS_LABEL_ ;
}

//================================================================================
std::string  fileEater::getOutputTreeCompletePath()
{
    ss_.str("");
    ss_ << outputTreePath_ << "/" << outputTreeFileName_ ;
    return ss_.str();
}

//================================================================================
void  fileEater::setOperation(multiProcess_function function, subProcess<Event, Geometry> *theSubProcess)
{
    processOperation_  = function     ;
    this->setSubProcess(theSubProcess);
}

//================================================================================
void fileEater::setSubProcess (subProcess<Event,Geometry> * theSubProcess)
{
    currentSubProcess_.clear();
    currentSubProcess_.push_back(theSubProcess);
}

//================================================================================
void fileEater::clearSubProcess (void)
{
    currentSubProcess_.clear();
}

//===============================================================================
void fileEater::addSubProcess (subProcess<Event,Geometry> * theSubProcess)
{
    currentSubProcess_.push_back(theSubProcess);
}

//==============================================================================
bool fileEater::fullReconstruction()
{
    if( this->parse() )
    {
        this->openFile(this->getOutputTreeCompletePath()) ;
        return this->updateEvents2()                      ;
    }
    else return false;
}

//==============================================================================
int fileEater::computeSensorStrip(int chip,int set,int strip)
{
  if (chip < 1 || chip > 5)
    return -1000;
  if (set < 0 || set > 31)
    return -1000;
  if (strip < 0 || strip > 15)
    return -1000;
  static unsigned char set_number[] = { 255, 255, 255, 255, 255, 255, 255, 255,
                    255, 255,   0,   1,   4,   5,   3,   2,
                    255, 255,  12,  13,   8,   9,  11,  10,
                    255, 255,  15,  14,   7,   6, 255, 255  };
  static unsigned char strip_number[] = { 255, 255, 255, 255,
                                          255,   0,   2,   1,
                                          255,   6,   4,   5,
                                          255,   7,   3, 255  };

  return 128*(chip-1)+set_number[set]*8+strip_number[strip];
}
