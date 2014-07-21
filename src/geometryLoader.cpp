/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include "geometryLoader.h"

//======================================================================
geometryLoader::geometryLoader()
{
    theGeometry_ =  new Geometry();
    theGeometry_->setDUTnumbers(0);
}
//=======================================================================
void geometryLoader::loadXML(std::string configFile)
{
    // Initialize XERCES-C
    try
    {
        XMLPlatformUtils::Initialize();
    }
    catch(const XMLException &toCatch)
    {
        XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
                << "  Exception message:"
                << toCatch.getMessage() << XERCES_STD_QUALIFIER endl;
        exit(-1);
    }

    struct stat fileStatus;
    int returnStatus = stat(configFile.c_str(), &fileStatus);
    if( returnStatus == ENOENT )
    {
        ss_.str("") ; ss_ << "ABORT: Path to " << configFile << " does not exist, or path is an empty string." ;
        STDLINE(ss_.str(),ACRed) ;
        exit(-1) ;
    }
    else if( returnStatus == ENOTDIR )
    {
        ss_.str("") ; ss_ << "ABORT: A component of the path of " << configFile << " is not a directory.";
        STDLINE(ss_.str(),ACRed) ;
        exit(-1) ;
    }
    else if( returnStatus == ELOOP )
    {
        ss_.str("") ; ss_ << "ABORT: Too many symbolic links encountered while traversing the path to " << configFile;
        STDLINE(ss_.str(),ACRed) ;
        exit(-1) ;
    }
    else if( returnStatus == EACCES )
    {
        ss_.str("") ; ss_ << "ABORT: Permission denied to read " << configFile;
        STDLINE(ss_.str(),ACRed) ;
        exit(-1) ;
    }
    else if( returnStatus == ENAMETOOLONG )
    {
        ss_.str("") ; ss_ << "ABORT: Cannot read " << configFile;
        STDLINE(ss_.str(),ACRed) ;
        exit(-1) ;
    }
    else if( returnStatus == -1  )
    {
        ss_.str("") ; ss_ << "ABORT: Could not find configuration file "
                << "'"
                << configFile
                << "'!";
        STDLINE(ss_.str(),ACRed) ;
        exit(-1) ;
    }

    static XercesDOMParser::ValSchemes gValScheme = XercesDOMParser::Val_Auto;   // gValScheme = XercesDOMParser::Val_Never;
    // gValScheme = XercesDOMParser::Val_Auto;
    // gValScheme = XercesDOMParser::Val_Always;

    XercesDOMParser *parser = new XercesDOMParser();
    parser->setValidationScheme(gValScheme);

    DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();
    parser->setErrorHandler(errReporter);

    try
    {
        parser->parse(configFile.c_str());
    }
    catch (const OutOfMemoryException&)
    {
        STDLINE("F A T A L   E R R O R (A): ",ACRed) ;
        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
        exit(-1) ;
    }
    catch (const XMLException& e)
    {
        STDLINE("F A T A L   E R R O R (B): ",ACRed) ;
        XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n   Message: "
                << xercesc::XMLString::transcode(e.getMessage()) << XERCES_STD_QUALIFIER endl;
        exit(-1) ;
    }

    catch (const DOMException& e)
    {
        STDLINE("F A T A L   E R R O R (C): ",ACRed) ;
        const unsigned int maxChars = 2047;
        XMLCh errText[maxChars + 1];

        XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << configFile << "'\n"
                << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;

        if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
            XERCES_STD_QUALIFIER cerr << "Message is: " << xercesc::XMLString::transcode(errText) << XERCES_STD_QUALIFIER endl;

        exit(-1) ;
    }

    catch (...)
    {
        STDLINE("F A T A L   E R R O R (D): ",ACRed) ;
        XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n " << XERCES_STD_QUALIFIER endl;
        exit(-1) ;
    }

    STDLINE("Configuration successfully parsed",ACGreen) ;

    DOMDocument * xmlDoc  = parser->getDocument();
    testBeamGeometryXml_  = xmlDoc->getDocumentElement();

    if( !testBeamGeometryXml_ )
    {
        STDLINE("F A T A L   E R R O R: ",ACRed) ;
        XERCES_STD_QUALIFIER cerr << "Empty configuration file\n " << XERCES_STD_QUALIFIER endl;
        XERCES_STD_QUALIFIER cerr << "This should never occur, the parser should have already detected this error!\n " << XERCES_STD_QUALIFIER endl;
        exit(-1) ;
    }

    theGeometry_->setGeometryFileName(configFile);
}
//===================================================================================================================
void  geometryLoader::fillGeometry(std::string toRead)
{
    toRead_ = toRead;
    theGeometry_->clear();
    this->acquireInfo(testBeamGeometryXml_) ;
    for(Geometry::iterator git=theGeometry_->begin(); git!=theGeometry_->end(); git++)
        git->second->setupVariables();
}
//===================================================================================================================
void geometryLoader::acquireInfo(DOMElement * element)
{
    //  static const boost::regex matchDoubleQuotes(".*?\"(.*)\".*", boost::regex::perl);

    DOMNodeList*      children  = element->getChildNodes();
    const  XMLSize_t  nodeCount = children->getLength()   ;
    double rowPitch=0,colPitch=0;
    int dutNumbers=0;

    std::string  parentTagName = XMLString::transcode(element->getTagName()) ;

    std::map<std::string, std::string> keyValue ;

    for( XMLSize_t j = 0; j < nodeCount; ++j )
    {
        DOMNode* currentNode = children->item(j);

        if( !(currentNode->getNodeType() &&
              currentNode->getNodeType() == DOMNode::ELEMENT_NODE )) continue ; // is element otherwise close the recursive stack

        DOMElement      * currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode ); // Found node which is an Element. Re-cast node as such.
        DOMNamedNodeMap * attList	     = currentNode->getAttributes() ;
        std::string       tagName	     = XMLString::transcode(currentElement->getTagName()) ;
        std::string       textContent    = "" ;
        std::string       textWithBlanks = "" ;

        if(currentNode->getTextContent())
            textWithBlanks = XMLString::transcode(currentElement->getTextContent()) ;

        textContent = this->stripBlanks(textWithBlanks) ;

        keyValue.clear() ;
        bool used = true;

        for(unsigned int i=0; i<attList->getLength(); ++i) // Set attibutes apart in a temporary hash map
        {
            if(                                                  attList->item(i)->getTextContent() )
                keyValue[                   XMLString::transcode(attList->item(i)->   getNodeName())] =
                        this->stripBlanks( XMLString::transcode(attList->item(i)->getTextContent()));
        }


        if( tagName == "testBeamGeometry" )
        {
            //theGeometry_->                keyValue["id" ]           ;
            //theGeometry_->       ( this->toLower(keyValue["date"]) );
            STDLINE("Entered " + tagName,ACYellow) ;
        }

        if( tagName == "stations" )
        {
            ss_.str("");
            ss_ << "stations in use: " << keyValue["inUse" ];
            STDLINE(ss_.str(),ACYellow) ;
        }

        if( tagName == "station" )
        {
            if(this->toLower(keyValue["used"])=="yes")
                station_    = keyValue["id"];
            else                   used        = false         ;
        }

        if( tagName == "detectors" )
        {
            ss_.str("");
            ss_ << "detectors in use: " << keyValue["inUse" ];
            STDLINE(ss_.str(),ACYellow) ;
        }

        if( tagName == "detector" )
        {
            if( this->toLower(keyValue["used"])=="yes" )
            {
                ss_.str("");
                ss_ << "Station: " << station_ << " - " << "Plaq: " << keyValue["id"];
                currentPlaqID_ = ss_.str();
                theGeometry_->addDetector( currentPlaqID_ );

                STDLINE(keyValue["name"] + " detector id: " + currentPlaqID_,ACYellow) ;

                theGeometry_->getDetector( currentPlaqID_ )->setName(keyValue["name"]);
                if(keyValue["name"] == "FSSR2")
                    theGeometry_->getDetector( currentPlaqID_ )->setIsStrip(true);

                //std::cout << __PRETTY_FUNCTION__ << theGeometry_->getDetector( currentPlaqID_ )->isStrip() << std::endl;

                if( this->toLower(keyValue["isDUT"]) == "yes" )
                {
                    theGeometry_->getDetector( currentPlaqID_ )->setDUT();
                    dutNumbers++;
                    theGeometry_->setDUTnumbers(dutNumbers);
                }
            }
            else used=false;
        }

        if( tagName == "largeGranularity" )
        {
            STDLINE("Rotations relative to " + keyValue["relativeRotations"],ACYellow) ;
        }

        if( tagName == "xBackFlipped"  && this->validContent(tagName,textContent))
        {
            if( this->toLower(textContent) == "yes" || this->toLower(textContent) == "true")
            {
                STDLINE("get Detector",ACYellow) ;
                theGeometry_->getDetector( currentPlaqID_ )->setXBackFlipped(true );
                STDLINE("cleared",ACYellow) ;
            }
            else   theGeometry_->getDetector( currentPlaqID_ )->setXBackFlipped(false);
        }

        if( tagName == "yBackFlipped"  && this->validContent(tagName,textContent))
        {
            if( this->toLower(textContent) == "yes" || this->toLower(textContent) == "true")
                theGeometry_->getDetector( currentPlaqID_ )->setYBackFlipped(true );
            else   theGeometry_->getDetector( currentPlaqID_ )->setYBackFlipped(false);
        }

        if( tagName == "xPosition"  && this->validContent(tagName,textContent))
            theGeometry_->getDetector( currentPlaqID_ )->setXPosition(Utils::toDouble(textContent)*CONVF);

        if( tagName == "yPosition"  && this->validContent(tagName,textContent))
            theGeometry_->getDetector( currentPlaqID_ )->setYPosition(Utils::toDouble(textContent)*CONVF);

        if( tagName == "zPosition"  && this->validContent(tagName,textContent))
            theGeometry_->getDetector( currentPlaqID_ )->setZPosition(Utils::toDouble(textContent)*CONVF);

        if( tagName == "xRotation"  && this->validContent(tagName,textContent))
            theGeometry_->getDetector( currentPlaqID_ )->setXRotation( Utils::toDouble(textContent));

        if( tagName == "yRotation"  && this->validContent(tagName,textContent))
            theGeometry_->getDetector( currentPlaqID_ )->setYRotation( Utils::toDouble(textContent));

        if( tagName == "zRotation"  && this->validContent(tagName,textContent))
            theGeometry_->getDetector( currentPlaqID_ )->setZRotation( Utils::toDouble(textContent) );

        if( toRead_ == "correction" || toRead_ == "all" )
        {
            if( tagName == "fineGranularity" )
            {
                STDLINE("Reading fineGranularity",ACGreen);
                STDLINE("Rotations relative to " + keyValue["relativeRotations"],ACYellow) ;
            }

            if( tagName == "xPosCorrection"  && this->validContent(tagName,textContent))
                theGeometry_->getDetector( currentPlaqID_ )->setXPositionCorrection(Utils::toDouble(textContent)*CONVF);

            if( tagName == "xPositionError"  && this->validContent(tagName,textContent))
                theGeometry_->getDetector( currentPlaqID_ )->setXPositionError(Utils::toDouble(textContent)*CONVF);

            if( tagName == "yPosCorrection"  && this->validContent(tagName,textContent))
                theGeometry_->getDetector( currentPlaqID_ )->setYPositionCorrection(Utils::toDouble(textContent)*CONVF);

            if( tagName == "yPositionError"  && this->validContent(tagName,textContent))
                theGeometry_->getDetector( currentPlaqID_ )->setYPositionError(Utils::toDouble(textContent)*CONVF);

            if( tagName == "zPosCorrection"  && this->validContent(tagName,textContent))
                theGeometry_->getDetector( currentPlaqID_ )->setZPositionCorrection(Utils::toDouble(textContent)*CONVF);

            if( tagName == "zPositionError"  && this->validContent(tagName,textContent))
                theGeometry_->getDetector( currentPlaqID_ )->setZPositionError(Utils::toDouble(textContent)*CONVF);

            if( tagName == "xRotationCorrection"  && this->validContent(tagName,textContent))
                theGeometry_->getDetector( currentPlaqID_ )->setXRotationCorrection( Utils::toDouble(textContent) );

            if( tagName == "xRotationCorrectionError"  && this->validContent(tagName,textContent))
                theGeometry_->getDetector( currentPlaqID_ )->setXRotationCorrectionError( Utils::toDouble(textContent) );

            if( tagName == "yRotationCorrection"  && this->validContent(tagName,textContent))
                theGeometry_->getDetector( currentPlaqID_ )->setYRotationCorrection( Utils::toDouble(textContent) );

            if( tagName == "yRotationCorrectionError"  && this->validContent(tagName,textContent))
                theGeometry_->getDetector( currentPlaqID_ )->setYRotationCorrectionError( Utils::toDouble(textContent) );

            if( tagName == "zRotationCorrection"  && this->validContent(tagName,textContent))
                theGeometry_->getDetector( currentPlaqID_ )->setZRotationCorrection( Utils::toDouble(textContent) );

            if( tagName == "zRotationCorrectionError"  && this->validContent(tagName,textContent))
                theGeometry_->getDetector( currentPlaqID_ )->setZRotationCorrectionError( Utils::toDouble(textContent) );
        }

        if( tagName == "ROCs" )
        {
            STDLINE("ROCs in use: " + keyValue["inUse"],ACYellow) ;
            theGeometry_->getDetector( currentPlaqID_ )->setXNumberOfROCs( Utils::toInt(keyValue["xChipsNumber"]) );
            theGeometry_->getDetector( currentPlaqID_ )->setYNumberOfROCs( Utils::toInt(keyValue["yChipsNumber"]) );
        }

        if( tagName == "ROC" )
        {
            if( this->toLower(keyValue["used"])=="yes" )
            {
                currentROC_ = Utils::toInt(keyValue["pos"]);
                theGeometry_->getDetector( currentPlaqID_ )->addROC( Utils::toInt(keyValue["pos"]),
                                                                     Utils::toInt(keyValue["id"] ) );
            }
            else used=false;
        }

        if ( tagName == "calibrationFilePath" && this->validContent(tagName,textContent))
            theGeometry_->getDetector( currentPlaqID_ )->getROC( currentROC_ )->setCalibrationFilePath(textContent);

        if ( tagName == "standardRowPitch" && this->validContent(tagName,textContent)) rowPitch = Utils::toDouble(textContent)*CONVF;
        if ( tagName == "standardColPitch" && this->validContent(tagName,textContent))
        {
            colPitch = Utils::toDouble(textContent)*CONVF;
            ss_.str("");
            ss_ << ACCyan   << ACBold << "Pitch. Row : " << ACWhite << ACBold << rowPitch
                    << ACCyan   << ACBold << " Col: "        << ACWhite << ACBold << colPitch
                    << ACYellow << ACBold << " (tens of microns)";
            STDLINE(ss_.str(),ACRed) ;
            theGeometry_->getDetector( currentPlaqID_ )->getROC( currentROC_ )->setStandardPixPitch(rowPitch,colPitch);
        }

        if ( tagName == "nonStandardRowPitch" && this->validContent(tagName,textContent) )
            theGeometry_->getDetector( currentPlaqID_ )->getROC( currentROC_ )->setOneRowPitch(Utils::toInt(keyValue["rowNum"]),
                                                                                               Utils::toDouble(textContent)*CONVF     );


        if ( tagName == "nonStandardColPitch" && this->validContent(tagName,textContent) )
            theGeometry_->getDetector( currentPlaqID_ )->getROC( currentROC_ )->setOneColPitch(Utils::toInt(keyValue["colNum"]),
                                                                                               Utils::toDouble(textContent)*CONVF     );


        if( tagName == "MAX_ROWS"  && this->validContent(tagName,textContent))
        {
            theGeometry_->getDetector( currentPlaqID_ )->getROC( currentROC_ )->setNumberOfRows(Utils::toInt(textContent)+1 );
        }
        if( tagName == "MAX_COLS"  && this->validContent(tagName,textContent))
        {
            theGeometry_->getDetector( currentPlaqID_ )->getROC( currentROC_ )->setNumberOfCols(Utils::toInt(textContent)+1 );
        }
        if( tagName == "orientation"  && this->validContent(tagName,textContent))
        {
            theGeometry_->getDetector( currentPlaqID_ )->getROC( currentROC_ )->setOrientation( Utils::toInt(textContent) );
            ss_.str("") ; ss_ << ACCyan  << ACBold << "Orientation: " << ACWhite << textContent
                    << ACYellow << ACBold << " (degrees)";
            STDLINE(ss_.str(),ACGreen) ;
        }
        //closingFlag_ = false ;
        //STDLINE(tagName,ACRed) ;
        if ( used ) this->acquireInfo(currentElement) ;
        //STDLINE(tagName,ACGreen) ;
        //closingFlag_ = true ;
    }
}

//=================================================================
bool geometryLoader::validContent(std::string tagName, std::string content)
{
    bool isValid = false ;

    if( content == "" )
    {
        ss_.str("") ;
        ss_ << ACRed    << ACBold << ACReverse
                << "FATAL:"
                << ACPlain  << ACWhite << ACBold
                << " Tag "
                << ACYellow << ACBold
                << tagName
                << ACPlain  << ACWhite << ACBold
                << " has an empty content" ;
        STDLINE(ss_.str(), ACWhite) ;
        exit(-1) ;
    }

    isValid = true ;

    return isValid ;
}
//=================================================================
std::string geometryLoader::stripBlanks(std::string theString)
{
    return boost::algorithm::trim_right_copy(boost::algorithm::trim_left_copy(theString)) ;
}

//=================================================================
std::string geometryLoader::toLower(std::string theString)
{
    return boost::algorithm::to_lower_copy(theString) ;
}
