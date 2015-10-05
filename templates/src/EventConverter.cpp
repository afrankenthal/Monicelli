/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include "EventConverter.h"
#include "EventReader.h"
#include "Event.h"
#include "Geometry.h"
#include "MessageTools.h"
#include "Data.h"

#include <TFile.h>
#include <TTree.h>
#include <string>

using namespace std;

//====================================================================
// This class receives a pointer to the eventReader class. This pointer 
// is used to retrieve individual pointers to the objects stored on
// file (theHeader_, theEvent_ and theGeometry_)
//
EventConverter::EventConverter(EventReader * reader ) : 
   theEventReader_ ( reader                         )
  ,theEvent_       ( reader->getEventPointer   ()   )
  ,theHeader_      ( reader->getEventHeaderPointer())
  ,theGeometry_    ( reader->getGeometryPointer()   )
{
}

//====================================================================
// This is the method of the EventConverter class where users are supposed 
// to implement their own private code for analysis. Here you will find 
// an example of how to retrieve and manipulate components of the event
// and the geometry.
//
// NOTE: for a detailed description of the Event class, its public access
//       methods and the meaning of invidual containers, please consult
//       ../include/Event.h and comments therein.
//
void EventConverter::convert()
{
  stringstream plaqName;
  
  string outFileName = theEventReader_->getFileName();
  if(outFileName.find(".root") == string::npos)
  {
  
      ss_.str("") ;
      ss_ << "Can't find .root in file: " << outFileName << ". Aborting...";
      STDLINE(ss_.str(),ACRed);
      exit(EXIT_FAILURE);
  }  
  outFileName.replace(outFileName.find(".root"),5,"_Converted.root");
  if(theEventReader_->getFileName() == outFileName)
  {
    STDLINE("Can't find root in file name", ACRed);
    exit(EXIT_FAILURE);
  }
  TFile* outFile = new TFile(outFileName.c_str(),"RECREATE");
  TTree* outTree = new TTree("CaptanTrack","The Track Tree");
  //TH1F*  dutCharge = new TH1F("dutCharge","dutCharge",2024,0,100000);
  Data   aData;
  aData.branch(outTree);

  unsigned int numberOfEvents = theEventReader_->getNumberOfEvents() ;

  // Retrieve individual events and analyze them individually
  //int badEventsCounter = 0;
  //for(unsigned int e=0; e<numberOfEvents; e++)
  for(unsigned int e=0; e<10; e++)
  {
    theEventReader_->readEvent(e) ;
    if(e % 10000 == 0)
    {
      ss_.str("") ;
      ss_ << "Converting event: " << e << " / " << numberOfEvents ;
      STDLINE(ss_.str(),ACWhite) ;
    }
    
    Event::plaqMapDef  	             & theRawData	      = theEvent_->getRawData();
    Event::clustersMapDef	     & clusters  	      = theEvent_->getClusters();
    Event::clustersHitsMapDef        & clustersHits	      = theEvent_->getClustersHits();
    Event::fittedTracksDef           & fittedTracks           = theEvent_->getFittedTracks();
    Event::chi2VectorDef	     & fittedTracksChi2       = theEvent_->getFittedTracksChi2();
    Event::residualsMapDef	     & fittedTrackResiduals   = theEvent_->getFittedTrackResiduals();
    Event::trackCandidatesDef        & trackCandidates	      = theEvent_->getTrackCandidates();
    //Event::residualsMapDef	     & fittedTrackDeviations  = theEvent_->getFittedTrackDeviations();
    //Event::residualsMapDef	     & fittedTrackPulls       = theEvent_->getFittedTrackPulls();
    Event::fittedTracksCovarianceDef & fittedTracksCovariance = theEvent_->getFittedTracksCovariance();


    if( trackCandidates.size() == 0 ) continue ;

    aData.setEvtId   (e);   
    aData.setNtrack  (trackCandidates.size());  
    if(trackCandidates.size() != fittedTracks.size())
    {
      STDLINE("There is something very wrong in the track sizes", ACRed);
    }
    for(unsigned int t=0; t<trackCandidates.size(); t++)
    {
      std::cout << __PRETTY_FUNCTION__ << "Intercept:  " << fittedTracks[t][1]*10 << std::endl;
      aData.setItrack  (t);
      aData.setNhits   (trackCandidates[t].size());
      aData.setNdof    (trackCandidates[t].size()-4);
      aData.setChi2    (fittedTracksChi2[t]);
      aData.setProb    (fittedTracksChi2[t]);//TO BE FIXED
      aData.setXproj0  (fittedTracks[t][1]*10);
      aData.setYproj0  (fittedTracks[t][3]*10);
      aData.setXslope  (fittedTracks[t][0]);
      aData.setYslope  (fittedTracks[t][2]);
      
      int station   = 0;
      int plaquette = 0;
      
      for(int p=0; p<10; p++)
      {
	plaqName.str("");
	if((p/4)*2 == 4)
	{
	  station = (p/4)*2;
	  plaquette = 1-(p%4);
	}
	else
	{
	  station = (p/4)*2;
	  plaquette = (p%4);
	}
	
	plaqName << "Station: " << station << " - " << "Plaq: " << plaquette;
	
        float row = 0;
	float col = 0;
	double xp, yp, xRes, yRes;//zp 					  	    
        double pixelCenterX; 						  	    
        double pixelCenterY;						  	    
        double xErr = 0.;						  	    
	double yErr = 0.;
	pair<double,double> rc;
	pair<double,double> xyErr;
	Detector* det = theGeometry_->getDetector  (station,plaquette);
	
	if( theGeometry_->getDetector  (station,plaquette) != NULL ) 
	{	
            if( trackCandidates[t].find(plaqName.str()) != trackCandidates[t].end() )
	    {
		int clusterID     = (int)trackCandidates[t][plaqName.str()]["num"];
		
	        aData.setHasHit  (true, p);
	        aData.setBelongsToTrack(true,p);
	        aData.setNpix    ((int)trackCandidates[t][plaqName.str()]["size"], p);
       	        aData.setCharge  (clusters[plaqName.str()][clusterID]["charge"], p);
		
		unsigned int size = (unsigned int)clustersHits[plaqName.str()][clusterID].size();
		
	        for(unsigned int h=0; h<size; h++ )
	        {
	    	     row += (float)clustersHits[plaqName.str()][clusterID][h]["row"];
	    	     col += (float)clustersHits[plaqName.str()][clusterID][h]["col"];
	        }
	    
	        aData.setCol     (col/size, p);
	        aData.setRow     (row/size, p);
	        aData.setXp      (clusters[plaqName.str()][(int)trackCandidates[t][plaqName.str()]["num"]]["x"]*10, p);
	        aData.setYp      (clusters[plaqName.str()][(int)trackCandidates[t][plaqName.str()]["num"]]["y"]*10, p);
	        aData.setXl      (trackCandidates[t][plaqName.str()]["x"]*10, p);
	        aData.setYl      (trackCandidates[t][plaqName.str()]["y"]*10, p);
	        //aData.setXpErr   (trackCandidates[t][plaqName.str()]["xErr"]*10, p);
	        //aData.setYpErr   (trackCandidates[t][plaqName.str()]["yErr"]*10, p);
	        aData.setNdofExcl(trackCandidates[t].size()-6, p);//TO BE FIXED
	        aData.setChi2Excl(fittedTracksChi2[t], p);//TO BE FIXED
		aData.setXresid  (fittedTrackResiduals[t][plaqName.str()].first*10, p);
	        aData.setYresid  (fittedTrackResiduals[t][plaqName.str()].second*10,p);
		std::cout << __PRETTY_FUNCTION__ << "sigma: " << sqrt(fittedTracksCovariance[t](1,1))*10 << std::endl;
		xyErr = det->getTrackErrorsOnPlane(fittedTracks[t],fittedTracksCovariance[t]);
		aData.setXpErr(xyErr.first,p);
		aData.setYpErr(xyErr.second,p);
		    
		if( station == 4 )
		{
		    //STDLINE("I shouldn't be in here!", ACRed);
		    xyErr = det->getTrackErrorsOnPlane(fittedTracks[t],fittedTracksCovariance[t]);
		    aData.setXpErr(xyErr.first,p);
		    aData.setYpErr(xyErr.second,p);
		    det->getPredictedLocal(fittedTracks[t],xp,yp);
		    rc = det->getPixelCellFromLocal(xp, yp);
	            row = rc.first;
	            col = rc.second;
		    aData.setCol(col, p);
                    aData.setRow(row, p);
		    
		    if( col!= -1 && row!=-1)					  	    
                    {   								  	    
	               aData.setXp (xp*10, p);					  	    
		       aData.setYp (yp*10, p);					  	    
		    								  	    
		       pixelCenterX = det->getPixelCenterLocalX((unsigned int)col);			  	    
		       pixelCenterY = det->getPixelCenterLocalY((unsigned int)row);			  	    
		       det->flipPositionLocal(&pixelCenterX, &pixelCenterY, &xErr, &yErr);	    
		       det->flipPositionLocal(&xp, &yp);	    
		    								  	    
		       xRes = (pixelCenterX - xp); 				  	    
		       yRes = (pixelCenterY - yp);
                								  	    
		       aData.setXresid(xRes*10,p); 				  	    
		       aData.setYresid(yRes*10,p);										  	    
		    }
		}
	    }
	    else
	    {		   
	        aData.setHasHit(false,p);
		aData.setBelongsToTrack(false,p);

		det->getPredictedLocal(fittedTracks[t],xp,yp);
  	        rc = det->getPixelCellFromLocal(xp, yp);
	        row = rc.first;
	        col = rc.second;			  
		aData.setNpix(theRawData[plaqName.str()].size(), p);               
										  	    
	        aData.setCol(col, p);						  	    
		aData.setRow(row, p);						  	    
                	
		if(col==-1 || row==-1)
		   continue;
		   	      							  	    								  	    
	        aData.setXp (xp*10, p); 						
		aData.setYp (yp*10, p); 						
											
		pixelCenterX = det->getPixelCenterLocalX((unsigned int)col); 				
		pixelCenterY = det->getPixelCenterLocalY((unsigned int)row); 				
		det->flipPositionLocal(&pixelCenterX, &pixelCenterY, &xErr, &yErr);  	
		det->flipPositionLocal(&xp, &yp);  	
											
		xRes = (pixelCenterX - xp);						
		yRes = (pixelCenterY - yp);
                    									
		aData.setXresid(xRes*10,p);						
		aData.setYresid(yRes*10,p);											

		xyErr = det->getTrackErrorsOnPlane(fittedTracks[t],fittedTracksCovariance[t]);
		aData.setXpErr(xyErr.first,p);
		aData.setYpErr(xyErr.second,p);
		
		if( true )
		{
		    for( vector<map<string , int> >::iterator hits = theRawData[plaqName.str()].begin(); hits != theRawData[plaqName.str()].end(); hits++ )
	            {		
	            	if ( (abs(col - (*hits)["col"]) <= 1) && (abs(row - (*hits)["row"]) <= 1) )
	            	{  
	            	     aData.setHasHit(true,p);			     
		             //aData.setBelongsToTrack(true,p);
	            	     int clusterID = -1;
	            	     for(Event::aClusterHitsMapDef::iterator itC = clustersHits[plaqName.str()].begin(); itC != clustersHits[plaqName.str()].end() && clusterID == -1; itC++ )
		    	     {
		    	       for(Event::hitsDef::iterator itH = itC->second.begin(); itH != itC->second.end() && clusterID == -1; itH++)
		    	       {
		    		 if((*hits)["col"] == (*itH)["col"] && (*hits)["row"] == (*itH)["row"])
		    		 {
		    		   clusterID = itC->first;
		    		 }
		    	       }
		    	     }
		    	     
		    	     unsigned int size = (unsigned int)clustersHits[plaqName.str()][clusterID].size();
		    	     aData.setNpix    (size, p);
      	            	     aData.setCharge  (clusters[plaqName.str()][clusterID]["charge"], p);
		             row = 0;
			     col = 0;
	            	     for(unsigned int h=0; h<size; h++ )
	            	     {
	    	    		  row += (float)clustersHits[plaqName.str()][clusterID][h]["row"];
	    	    		  col += (float)clustersHits[plaqName.str()][clusterID][h]["col"];
	            	     }
	    
	            	     aData.setCol     (col/size, p);
	            	     aData.setRow     (row/size, p);
	            	     //aData.setXp    (clusters[plaqName.str()][(int)trackCandidates[t][plaqName.str()]["num"]]["x"]*10, p);
	            	     //aData.setYp    (clusters[plaqName.str()][(int)trackCandidates[t][plaqName.str()]["num"]]["y"]*10, p);
	            	     //aData.setXl    (trackCandidates[t][plaqName.str()]["x"]*10, p);
	            	     //aData.setYl    (trackCandidates[t][plaqName.str()]["y"]*10, p);
	            	     //aData.setXpErr	(trackCandidates[t][plaqName.str()]["xErr"]*10, p);
	            	     //aData.setYpErr	(trackCandidates[t][plaqName.str()]["yErr"]*10, p);
	            	     //aData.setNdofExcl(trackCandidates[t].size()-6, p);//TO BE FIXED
	            	     //aData.setChi2Excl(fittedTracksChi2[t], p);//TO BE FIXED
	            	     //aData.setXresid  (fittedTrackResiduals[t][plaqName.str()].first*10, p);
	            	     //aData.setYresid  (fittedTrackResiduals[t][plaqName.str()].second*10, p);
		    	     break;
	            	}
	            }
		}

	    }
	}    
	    		
      }	     
      outTree->Fill();
    }
  }

  int station   = 0;
  int plaquette = 0;
  
  for( int i=0; i<4; i++ )
  {
       ss_.str("");
       ss_ << "calibHistos2Dpar" << i;
       calibHistos2D_[i] = new TH2F (ss_.str().c_str(),ss_.str().c_str(),70,  -10,  60, 100,  -10,  90 ) ;
  }
  
    for( int p=0; p<10; p++ )
   {
       plaqName.str("");
       if( (p/4)*2 == 4 )
       {
	   station = (p/4)*2;
	   plaquette = 1-(p%4);
       }
       else
       {
	  station = (p/4)*2;
	  plaquette = (p%4);
       }
	
       plaqName << "Station: " << station << " - " << "Plaq: " << plaquette;
	
       //int row = 0;
       //int col = 0;
	
       if( station == 4 && plaquette == 0 )
       {
           for( int r=0; r<80; r++ )
	   {
	        for( int c=0; c<52; c++)
		{
		     double* par = theGeometry_->getDetector(station,plaquette)->getROC(0)->getCalibrationFunction(r,c);
		     if( par != NULL )
		     {
		         for( int i=0; i<4; i++ )
			 {
			      calibHistos2D_[i]->Fill(r,c,par[i]);
			 }
		     }

		}
	   
	   }
		
       }
  } 
    
  outFile->Write();
  outFile->Close();
  delete outFile;
}
