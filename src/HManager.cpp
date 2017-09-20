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

#include "HManager.h"

//=============================================================================
HManager::HManager(fileEater *theFileEater)
{
    ss_.str("");
    ss_ << getenv("MonicelliDir")  << "/tmp/" << "tempCache.root";

    theFileEater_     = theFileEater ;
    restrictSearch_   = true ;
    theGeometry_      = NULL;
    gROOT->Reset() ;
    mainCacheFolder_  = gROOT->GetRootFolder()->AddFolder("MainFolder","Main Folder") ;
    currentFolder_    = mainCacheFolder_;
    runSubFolder_     = mainCacheFolder_;
    mainCacheFolder_->SetOwner(kTRUE);

    emptyTH1I_        = new TH1I() ;
    emptyTH1I_->SetName("emptyTH1IHManager");
    emptyTH2I_        = new TH2I() ;
    emptyTH2I_->SetName("emptyTH2IHManager");
}

//=============================================================================
HManager::~HManager(void)
{
    //    STDLINE("HManager::~HManager",ACRed);
    delete emptyTH2I_;
    delete emptyTH1I_;
    //    STDLINE("HManager::~HManager",ACGreen);
}

//=============================================================================
HManager::stringVDef HManager::fillResiduals (Event* theEvent   ,
                                              int    trackNum   ,
                                              int    clusterSize,
                                              bool   clear      )
{
    stringVDef fullPaths ;

    fullPaths.push_back(std::string(X_U_RESIDUALS )) ;
    fullPaths.push_back(std::string(Y_U_RESIDUALS )) ;

    if(clear)
    {
        theGeometry_ = theFileEater_->getGeometry();
        for(Geometry::iterator git=theGeometry_->begin(); git!=theGeometry_->end(); git++)
        {
            TH1 *histo = (TH1*)this->getHistogram(fullPaths[0],(*git).first);
            histo->Reset("M");
            histo->GetXaxis()->UnZoom();
            histo      = (TH1*)this->getHistogram(fullPaths[1],(*git).first);
            histo->Reset("M");
            histo->GetXaxis()->UnZoom();
        }
    }
    else
    {
        TH1D    *histoDevX,*histoDevY ;
        int     lastTrack = trackNum+1;
        Event::residualsMapDef &residuals = theEvent->getFittedTrackResiduals();

        if(trackNum < 0)
        {
            trackNum  = 0               ;
            lastTrack = residuals.size();
        }

        for(int tr = trackNum; tr < lastTrack; tr++ )
        {
            if( residuals.find(tr) == residuals.end() ) continue;

            for(Event::trackResidualsMapDef::iterator res=residuals[tr].begin(); res!=residuals[tr].end(); res++)
            {
                ss_.str(""); ss_ << fullPaths[0]  << "/" << (*res).first;
                if ( !runSubFolder_->FindObject(ss_.str().c_str()) )
                {
                    histoDevX =  new TH1D((*res).first.c_str(),(*res).first.c_str(),1602,-400,400);
                    histoDevX->GetXaxis()->SetTitle("X Residual (10um)");
                    histoDevX->GetYaxis()->SetTitle("Counts")           ;
                    histoDevX->SetDirectory(0);
                    this->addItem(fullPaths[0],histoDevX);

                    histoDevY =  new TH1D((*res).first.c_str(),(*res).first.c_str(),1602,-400,400);
                    histoDevY->GetXaxis()->SetTitle("Y Residual (10um)");
                    histoDevY->GetYaxis()->SetTitle("Counts")           ;
                    histoDevY->SetDirectory(0);
                    this->addItem(fullPaths[1],histoDevY);

                    STDLINE("Booked unconstrained residual histogram for " + (*res).first,ACWhite);
                }
                else
                {
                    histoDevX = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());
                    ss_.str(""); ss_ << fullPaths[1] << "/" << (*res).first;
                    histoDevY = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());
                }

                //Event::alignedHitsCandidateMapDef &track = theEvent->getTrackCandidates()[tr];
                if( clusterSize < 0 || clusterSize == theEvent->getTrackCandidates()[tr][(*res).first]["size"])
                {
                    //                    if(track[res->first]["charge"]>30000)//myTry
                    //                    {
                    histoDevX->Fill((*res).second.first);
                    histoDevY->Fill((*res).second.second);
                    //                    }
                }
            }
        }
    }
    return fullPaths;
}
//============================================================================
HManager::stringVDef       HManager::fillDeviations (Event* theEvent   ,
                                                     int    trackNum   ,
                                                     int    clusterSize,
                                                     bool   clear      )
{
    stringVDef fullPaths ;

    fullPaths.push_back(std::string(X_C_RESIDUALS      )) ;
    fullPaths.push_back(std::string(Y_C_RESIDUALS      )) ;

    if(clear)
    {
        theGeometry_ = theFileEater_->getGeometry();
        for(Geometry::iterator git=theGeometry_->begin(); git!=theGeometry_->end(); git++)
        {
            TH1 *histo = (TH1*)this->getHistogram(fullPaths[0], (*git).first );
            histo->Reset("M");
            histo->GetXaxis()->UnZoom();
            histo      = (TH1*)this->getHistogram(fullPaths[1],(*git).first );
            histo->Reset("M");
            histo->GetXaxis()->UnZoom();
        }
    }
    else
    {
        TH1D    *histoDevX,*histoDevY ;
        int     lastTrack = trackNum+1;
        Event::residualsMapDef residuals = theEvent->getFittedTrackDeviations();

        if(trackNum < 0)
        {
            trackNum  = 0               ;
            lastTrack = residuals.size();
        }

        for(int tr = trackNum; tr < lastTrack; tr++ )
        {
            for(Event::trackResidualsMapDef::iterator res=residuals[tr].begin(); res!=residuals[tr].end(); res++)
            {
                ss_.str(""); ss_ << fullPaths[0]  << "/" << (*res).first;
                if ( !runSubFolder_->FindObject(ss_.str().c_str()) )//if he doesn't find the plaq histogram
                {
                    histoDevX =  new TH1D((*res).first.c_str(),(*res).first.c_str(),8001,-4000,4000);
                    histoDevX->GetXaxis()->SetTitle("X Deviation (10um)");
                    histoDevX->GetYaxis()->SetTitle("tracks")            ;
                    histoDevX->SetDirectory(0);

                    this->addItem(fullPaths[0],histoDevX);
                    histoDevY =  new TH1D((*res).first.c_str(),(*res).first.c_str(),8001,-4000,4000);
                    histoDevY->GetXaxis()->SetTitle("Y Deviation (10um)");
                    histoDevY->GetYaxis()->SetTitle("tracks")            ;
                    histoDevY->SetDirectory(0);
                    this->addItem(fullPaths[1],histoDevY);

                    STDLINE("Booked constrained residual histogram for " + (*res).first,ACWhite);
                }
                else
                {
                    histoDevX = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());
                    ss_.str(""); ss_ << fullPaths[1] << "/" << (*res).first;
                    histoDevY = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());
                }

                if( clusterSize < 0 || clusterSize == theEvent->getTrackCandidates()[tr][(*res).first]["size"])
                {
                    histoDevX->Fill( (*res).second.first  );
                    histoDevY->Fill( (*res).second.second );
                }
            }
        }
    }
    return fullPaths;
}
//============================================================================
HManager::stringVDef     HManager::fillScatterResidual  (Event *theEvent   ,
                                                         int    trackNum   ,
                                                         int    clusterSize,
                                                         bool   clear      )
{
    stringVDef fullPaths ;

    fullPaths.push_back(std::string(X_RES_Y_POS_2D  )) ;
    fullPaths.push_back(std::string(Y_RES_X_POS_2D  )) ;
    fullPaths.push_back(std::string(X_RES_X_POS_2D  )) ;
    fullPaths.push_back(std::string(Y_RES_Y_POS_2D  )) ;

    if(clear)
    {
        theGeometry_ = theFileEater_->getGeometry();
        for(Geometry::iterator git=theGeometry_->begin(); git!=theGeometry_->end(); git++)
        {
            TH2* histo2= (TH2*)this->getHistogram(fullPaths[0], (*git).first);
            histo2->Reset("M");
            histo2     = (TH2*)this->getHistogram(fullPaths[1], (*git).first);
            histo2->Reset("M");
            histo2     = (TH2*)this->getHistogram(fullPaths[2], (*git).first);
            histo2->Reset("M");
            histo2     = (TH2*)this->getHistogram(fullPaths[3], (*git).first);
            histo2->Reset("M");
        }
    }
    else
    {
        TH2D    *histoResXvsY  , *histoResYvsX  ;
        TH2D    *histoResXvsX  , *histoResYvsY  ;

        Event::residualsMapDef &residuals = theEvent->getFittedTrackResiduals();

        int     lastTrack = trackNum+1;

        if(trackNum < 0)
        {
            trackNum  = 0               ;
            lastTrack = residuals.size();
        }

        for(int tr = trackNum; tr < lastTrack; tr++ )
        {
            if( residuals.find(tr) == residuals.end() ) continue;

            for(Event::trackResidualsMapDef::iterator res=residuals[tr].begin(); res!=residuals[tr].end(); res++)
            {
                ss_.str(""); ss_ << fullPaths[0]  << "/" << (*res).first;
                if ( !runSubFolder_->FindObject(ss_.str().c_str()) )//if he doesn't find the plaq histogram
                {
                    double length = theGeometry_->getMaxDetectorsLength();
                    //                    int pixNum;
                    //                    if ( theGeometry_->getMaxColsNum() > theGeometry_->getMaxRowsNum() ) pixNum = theGeometry_->getMaxColsNum();
                    //                    else                                                                 pixNum = theGeometry_->getMaxRowsNum();
                    //x vs y plots
                    histoResXvsY = new TH2D((*res).first.c_str(),(*res).first.c_str(),400,-length   ,+length,
                                            2000,-0.1*CONVF,+0.1*CONVF);
                    histoResXvsY->GetYaxis()->SetTitle("X Residual (10um)");
                    histoResXvsY->GetXaxis()->SetTitle("Y Position (10um)");
                    histoResXvsY->SetDirectory(0);
                    this->addItem(fullPaths[0] , histoResXvsY);

                    histoResYvsX = new TH2D((*res).first.c_str(),(*res).first.c_str(),400,-length   ,+length,
                                            2000,-0.1*CONVF,+0.1*CONVF);
                    histoResYvsX->GetYaxis()->SetTitle("Y Residual (10um)");
                    histoResYvsX->GetXaxis()->SetTitle("X Position (10um)");
                    histoResYvsX->SetDirectory(0);
                    this->addItem(fullPaths[1], histoResYvsX);


                    histoResXvsX = new TH2D((*res).first.c_str(),(*res).first.c_str(),400,-length   ,+length,
                                            400,-0.1*CONVF,+0.1*CONVF);
                    histoResXvsX->GetYaxis()->SetTitle("X Residual (10um)");
                    histoResXvsX->GetXaxis()->SetTitle("X Position (10um)");
                    histoResXvsX->SetDirectory(0);
                    this->addItem(fullPaths[2], histoResXvsX);

                    histoResYvsY = new TH2D((*res).first.c_str(),(*res).first.c_str(),400,-length   ,+length,
                                            400,-0.1*CONVF,+0.1*CONVF);
                    histoResYvsY->GetYaxis()->SetTitle("Y Residual (10um)");
                    histoResYvsY->GetXaxis()->SetTitle("Y Position (10um)");
                    histoResYvsY->SetDirectory(0);
                    this->addItem(fullPaths[3], histoResYvsY);

                    STDLINE("Filling " + (*res).first,ACPurple);
                }
                else
                {
                    histoResXvsY = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str());
                    ss_.str(""); ss_ << fullPaths[1] << "/" << (*res).first;
                    histoResYvsX = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str());
                    ss_.str(""); ss_ << fullPaths[2]  << "/" << (*res).first;
                    histoResXvsX = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str());
                    ss_.str(""); ss_ << fullPaths[3] << "/" << (*res).first;
                    histoResYvsY = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str());
                }

                if( clusterSize < 0 || clusterSize == theEvent->getTrackCandidates()[tr][(*res).first]["size"])
                {
                    histoResXvsY->Fill( theEvent->getTrackCandidates()[tr][(*res).first]["y"], (*res).second.first );
                    histoResYvsX->Fill( theEvent->getTrackCandidates()[tr][(*res).first]["x"], (*res).second.second);
                    histoResXvsX->Fill( theEvent->getTrackCandidates()[tr][(*res).first]["x"], (*res).second.first );
                    histoResYvsY->Fill( theEvent->getTrackCandidates()[tr][(*res).first]["y"], (*res).second.second);
                }
            }
        }
    }
    return fullPaths;
}
//===========================================================================
HManager::stringVDef HManager::makeMeanScatterResidual  (HManager::stringVDef histoTypes, bool redo)
{
    stringVDef fullPaths ;

    fullPaths.push_back(std::string(X_RES_Y_POS_MEAN      )) ;
    fullPaths.push_back(std::string(Y_RES_X_POS_MEAN      )) ;
    fullPaths.push_back(std::string(X_RES_X_POS_MEAN      )) ;
    fullPaths.push_back(std::string(Y_RES_Y_POS_MEAN      )) ;

    theGeometry_ = theFileEater_->getGeometry();

    if ( runSubFolder_->FindObject( fullPaths[0].c_str() ) && redo )
    {
        for(unsigned int i=0; i < fullPaths.size(); i++)
        {
            currentFolder_= (TFolder*)runSubFolder_->FindObject(fullPaths[i].c_str() );
            currentFolder_->SetOwner(kTRUE);
            runSubFolder_->RecursiveRemove(currentFolder_);
        }
        STDLINE("Resetting histograms...", ACPurple);
    }

    for(Geometry::iterator git=theGeometry_->begin(); git!=theGeometry_->end(); git++)
    {
        std::vector< TH2*  >   histoVec ;
        std::vector< TH1D* >   meanVec  ;
        for(unsigned int i=0; i < fullPaths.size(); i++)
        {
            histoVec.push_back( (TH2*)this->getHistogram(histoTypes[i] , (*git).first) );
        }

        //x vs y plots
        TH1D *histoX;
        for(unsigned int i=0; i < histoVec.size(); i++)
        {
            histoX =  new TH1D((*git).first.c_str(),(*git).first.c_str()               ,
                               histoVec[i]->GetXaxis()->GetNbins()   ,
                               histoVec[i]->GetXaxis()->GetXmin()    ,
                               histoVec[i]->GetXaxis()->GetXmax()     );
            histoX->GetYaxis()->SetTitle( histoVec[i]->GetYaxis()->GetTitle() );
            histoX->GetXaxis()->SetTitle( histoVec[i]->GetXaxis()->GetTitle() );
            histoX->SetDirectory(0);
            this->addItem(fullPaths[i] , histoX );

            meanVec.push_back( histoX );
        }

        STDLINE("Filling " + (*git).first,ACWhite);

        for(int binx=1; binx <= histoVec[0]->GetXaxis()->GetNbins(); binx++ )
        {
            std::vector< double > mean, tot, err;
            for(unsigned int i=0; i < histoVec.size(); i++)
            {
                mean.push_back( 0 );
                tot.push_back ( 0 );
                err.push_back ( 0 );
            }

            for(unsigned int i=0; i < histoVec.size(); i++)
            {
                for(int biny=1; biny <= histoVec[i]->GetYaxis()->GetNbins(); biny++)
                {
                    mean[i]+= histoVec[i]->GetYaxis()->GetBinCenter(biny)*
                            histoVec[i]->GetBinContent(binx,biny);

                    tot[i] += histoVec[i]->GetBinContent(binx,biny);

                    if( histoVec[i]->GetBinContent(binx,biny)!=0 )
                        err[i] += 1./histoVec[i]->GetBinContent(binx,biny);
                }
            }

            for(unsigned int i=0; i < mean.size(); i++)
            {
                if(tot[i] > 0)
                {
                    mean[i] = mean[i]/tot[i];
                    meanVec[i]->SetBinContent( binx, mean[i] );
                    meanVec[i]->SetBinError(binx, sqrt(1./err[i]));
                }
            }
        }
    }
    return fullPaths;
}
//===========================================================================
HManager::stringVDef HManager::fillAlignmentResults(std::string detector,
                                                    int         clusterSizeX, int    clusterSizeY,
                                                    double      resX        , double sigx2       ,
                                                    double      resY        , double sigy2       ,
                                                    double      xPrime      , double yPrime      ,
                                                    bool        clear                       )
{
    stringVDef fullPaths ;

    //RESIDUALS X
    fullPaths.push_back(std::string( ALIGN_OUT_X_RES_ALL_SIZE       )) ;
    fullPaths.push_back(std::string( ALIGN_OUT_X_RES_SIZE_1         )) ;
    fullPaths.push_back(std::string( ALIGN_OUT_X_RES_SIZE_2         )) ;
    //RESIDUALS Y
    fullPaths.push_back(std::string( ALIGN_OUT_Y_RES_ALL_SIZE       )) ;
    fullPaths.push_back(std::string( ALIGN_OUT_Y_RES_SIZE_1         )) ;
    fullPaths.push_back(std::string( ALIGN_OUT_Y_RES_SIZE_2         )) ;

    //PULLS X
    fullPaths.push_back(std::string( ALIGN_OUT_X_PULL_ALL_SIZE      )) ;
    fullPaths.push_back(std::string( ALIGN_OUT_X_PULL_SIZE_1        )) ;
    fullPaths.push_back(std::string( ALIGN_OUT_X_PULL_SIZE_2        )) ;
    //PULLS Y
    fullPaths.push_back(std::string( ALIGN_OUT_Y_PULL_ALL_SIZE      )) ;
    fullPaths.push_back(std::string( ALIGN_OUT_Y_PULL_SIZE_1        )) ;
    fullPaths.push_back(std::string( ALIGN_OUT_Y_PULL_SIZE_2        )) ;

    //CORRELATION XY
    fullPaths.push_back(std::string( ALIGN_OUT_X_RES_Y_POS_ALL_SIZE )) ;
    fullPaths.push_back(std::string( ALIGN_OUT_X_RES_Y_POS_SIZE_1   )) ;
    fullPaths.push_back(std::string( ALIGN_OUT_X_RES_Y_POS_SIZE_2   )) ;
    //CORRELATION YX
    fullPaths.push_back(std::string( ALIGN_OUT_Y_RES_X_POS_ALL_SIZE )) ;
    fullPaths.push_back(std::string( ALIGN_OUT_Y_RES_X_POS_SIZE_1   )) ;
    fullPaths.push_back(std::string( ALIGN_OUT_Y_RES_X_POS_SIZE_2   )) ;

    //CORRELATION XX
    fullPaths.push_back(std::string( ALIGN_OUT_X_RES_X_POS_ALL_SIZE )) ;
    fullPaths.push_back(std::string( ALIGN_OUT_X_RES_X_POS_SIZE_1   )) ;
    fullPaths.push_back(std::string( ALIGN_OUT_X_RES_X_POS_SIZE_2   )) ;
    //CORRELATION YY
    fullPaths.push_back(std::string( ALIGN_OUT_Y_RES_Y_POS_ALL_SIZE )) ;
    fullPaths.push_back(std::string( ALIGN_OUT_Y_RES_Y_POS_SIZE_1   )) ;
    fullPaths.push_back(std::string( ALIGN_OUT_Y_RES_Y_POS_SIZE_2   )) ;

    if(clear)
    {
        theGeometry_ = theFileEater_->getGeometry();
        for(unsigned int i=0; i < fullPaths.size(); i++)
        {
            for(Geometry::iterator git=theGeometry_->begin(); git!=theGeometry_->end(); git++)
            {
                if(detector != "" && detector != (*git).first) continue;
                TH1 *histo = (TH1*)this->getHistogram(fullPaths[i], (*git).first );
                histo->Reset("M");
                if(gPad!=NULL) histo->GetXaxis()->UnZoom();
            }
        }
    }
    else
    {
        TH1D    *histoPullsXAll , *histoPullsXSize1 , *histoPullsXSize2  ;
        TH1D    *histoPullsYAll , *histoPullsYSize1 , *histoPullsYSize2  ;
        TH1D    *histoResXAll   , *histoResXSize1   , *histoResXSize2    ;
        TH1D    *histoResYAll   , *histoResYSize1   , *histoResYSize2    ;
        TH2D    *XresYcoHistoAll, *XresYcoHistoSize1, *XresYcoHistoSize2 ;
        TH2D    *YresXcoHistoAll, *YresXcoHistoSize1, *YresXcoHistoSize2 ;
        TH2D    *XresXcoHistoAll, *XresXcoHistoSize1, *XresXcoHistoSize2 ;
        TH2D    *YresYcoHistoAll, *YresYcoHistoSize1, *YresYcoHistoSize2 ;

        ss_.str(""); ss_ << fullPaths[0]  << "/" << detector;
        if ( !runSubFolder_->FindObject(ss_.str().c_str()) )//if he doesn't find the plaq histogram
        {
            /////////////////////////////////////X Residuals//////////////////////////////

            histoResXAll =  new TH1D(detector.c_str(),(detector + ": X Residual").c_str(), 125, -50, 50);
            histoResXAll->GetXaxis()->SetTitle("X Residual");
            histoResXAll->GetYaxis()->SetTitle("tracks");
            histoResXAll->SetDirectory(0);
            this->addItem(fullPaths[0],histoResXAll);

            histoResXSize1 =  new TH1D(detector.c_str(),(detector + ": X Residual clusters size 1").c_str(), 125, -50, 50);
            histoResXSize1->GetXaxis()->SetTitle("X Residual");
            histoResXSize1->GetYaxis()->SetTitle("tracks");
            histoResXSize1->SetDirectory(0);
            this->addItem(fullPaths[1],histoResXSize1);

            histoResXSize2 =  new TH1D(detector.c_str(),(detector + ": X Residual clusters size 2").c_str(), 125, -50, 50);
            histoResXSize2->GetXaxis()->SetTitle("X Residual");
            histoResXSize2->GetYaxis()->SetTitle("tracks");
            histoResXSize2->SetDirectory(0);
            this->addItem(fullPaths[2],histoResXSize2);

            /////////////////////////////////////Y Residuals//////////////////////////////

            histoResYAll =  new TH1D(detector.c_str(),(detector + ": Y Residual").c_str(), 125, -50, 50);
            histoResYAll->GetXaxis()->SetTitle("Y Residual");
            histoResYAll->GetYaxis()->SetTitle("tracks");
            histoResYAll->SetDirectory(0);
            this->addItem(fullPaths[3],histoResYAll);

            histoResYSize1 =  new TH1D(detector.c_str(),(detector + ": Y Residual clusters size 1").c_str(), 125, -50, 50);
            histoResYSize1->GetXaxis()->SetTitle("Y Residual");
            histoResYSize1->GetYaxis()->SetTitle("tracks");
            histoResYSize1->SetDirectory(0);
            this->addItem(fullPaths[4],histoResYSize1);

            histoResYSize2 =  new TH1D(detector.c_str(),(detector + ": Y Residual clusters size 2").c_str(), 125, -50, 50);
            histoResYSize2->GetXaxis()->SetTitle("Y Residual");
            histoResYSize2->GetYaxis()->SetTitle("tracks");
            histoResYSize2->SetDirectory(0);
            this->addItem(fullPaths[5],histoResYSize2);

            /////////////////////////////////////X Pulls//////////////////////////////

            histoPullsXAll =  new TH1D(detector.c_str(),(detector + ": X Pull").c_str(), 250, -5, 5);
            histoPullsXAll->GetXaxis()->SetTitle("X Pull");
            histoPullsXAll->GetYaxis()->SetTitle("tracks");
            histoPullsXAll->SetDirectory(0);
            this->addItem(fullPaths[6],histoPullsXAll);

            histoPullsXSize1 =  new TH1D(detector.c_str(),(detector + ": X Pull clusters size 1").c_str(),250, -5, 5 );
            histoPullsXSize1->GetXaxis()->SetTitle("X Pull");
            histoPullsXSize1->GetYaxis()->SetTitle("tracks");
            histoPullsXSize1->SetDirectory(0);
            this->addItem(fullPaths[7],histoPullsXSize1);

            histoPullsXSize2 =  new TH1D(detector.c_str(),(detector + ": X Pull clusters size 2").c_str(),250, -5, 5 );
            histoPullsXSize2->GetXaxis()->SetTitle("X Pull");
            histoPullsXSize2->GetYaxis()->SetTitle("tracks");
            histoPullsXSize2->SetDirectory(0);
            this->addItem(fullPaths[8],histoPullsXSize2);

            /////////////////////////////////////Y Pulls//////////////////////////////

            histoPullsYAll =  new TH1D(detector.c_str(),(detector + ": Y Pull").c_str(),250, -5, 5 );
            histoPullsYAll->GetXaxis()->SetTitle("Y Pull");
            histoPullsYAll->GetYaxis()->SetTitle("tracks");
            histoPullsYAll->SetDirectory(0);
            this->addItem(fullPaths[9],histoPullsYAll);

            histoPullsYSize1 =  new TH1D(detector.c_str(),(detector + ": Y Pull clusters size 1").c_str(),250, -5, 5 );
            histoPullsYSize1->GetXaxis()->SetTitle("Y Pull");
            histoPullsYSize1->GetYaxis()->SetTitle("tracks");
            histoPullsYSize1->SetDirectory(0);
            this->addItem(fullPaths[10],histoPullsYSize1);

            histoPullsYSize2 =  new TH1D(detector.c_str(),(detector + ": Y Pull clusters size 2").c_str(),250, -5, 5 );
            histoPullsYSize2->GetXaxis()->SetTitle("Y Pull");
            histoPullsYSize2->GetYaxis()->SetTitle("tracks");
            histoPullsYSize2->SetDirectory(0);
            this->addItem(fullPaths[11],histoPullsYSize2);

            ////////////////////////////////////////////////////////////////////////////////////

            double length = theGeometry_->getMaxDetectorsLength();

            ////////////////////////////////XY Correlation//////////////////////////////////////

            XresYcoHistoAll =  new TH2D(detector.c_str(),(detector + ": X Residual vs Y Coordinate").c_str(), 400,-length,+length, 2000,-0.1*CONVF,+0.1*CONVF);
            XresYcoHistoAll->GetXaxis()->SetTitle("Y Coordinate (10um)");
            XresYcoHistoAll->GetYaxis()->SetTitle("X Residual (10um)");
            XresYcoHistoAll->SetDirectory(0);
            this->addItem(fullPaths[12],XresYcoHistoAll);

            XresYcoHistoSize1 =  new TH2D(detector.c_str(),(detector + ": X Residual vs Y Coordinate cluster size 1").c_str(), 400,-length,+length, 2000,-0.1*CONVF,+0.1*CONVF);
            XresYcoHistoSize1->GetXaxis()->SetTitle("Y Coordinate (10um)");
            XresYcoHistoSize1->GetYaxis()->SetTitle("X Residual (10um)");
            XresYcoHistoSize1->SetDirectory(0);
            this->addItem(fullPaths[13],XresYcoHistoSize1);

            XresYcoHistoSize2 =  new TH2D(detector.c_str(),(detector + ": X Residual vs Y Coordinate cluster size 2").c_str(), 400,-length,+length, 2000,-0.1*CONVF,+0.1*CONVF);
            XresYcoHistoSize2->GetXaxis()->SetTitle("Y Coordinate (10um)");
            XresYcoHistoSize2->GetYaxis()->SetTitle("X Residual (10um)");
            XresYcoHistoSize2->SetDirectory(0);
            this->addItem(fullPaths[14],XresYcoHistoSize2);

            ////////////////////////////////YX Correlation//////////////////////////////////////

            YresXcoHistoAll =  new TH2D(detector.c_str(),(detector + ": Y Residual vs X Coordinate").c_str(), 400,-length,+length, 2000,-0.1*CONVF,+0.1*CONVF);
            YresXcoHistoAll->GetXaxis()->SetTitle("X Coordinate (10um)");
            YresXcoHistoAll->GetYaxis()->SetTitle("Y Residual (10um)")           ;
            YresXcoHistoAll->SetDirectory(0);
            this->addItem(fullPaths[15],YresXcoHistoAll);

            YresXcoHistoSize1 =  new TH2D(detector.c_str(),(detector + ": Y Residual vs X Coordinate cluster size 1").c_str(), 400,-length,+length, 2000,-0.1*CONVF,+0.1*CONVF);
            YresXcoHistoSize1->GetXaxis()->SetTitle("X Coordinate (10um)");
            YresXcoHistoSize1->GetYaxis()->SetTitle("Y Residual (10um)")           ;
            YresXcoHistoSize1->SetDirectory(0);
            this->addItem(fullPaths[16],YresXcoHistoSize1);

            YresXcoHistoSize2 =  new TH2D(detector.c_str(),(detector + ": Y Residual vs X Coordinate cluster size 2").c_str(), 400,-length,+length, 2000,-0.1*CONVF,+0.1*CONVF);
            YresXcoHistoSize2->GetXaxis()->SetTitle("X Coordinate (10um)");
            YresXcoHistoSize2->GetYaxis()->SetTitle("Y Residual (10um)")           ;
            YresXcoHistoSize2->SetDirectory(0);
            this->addItem(fullPaths[17],YresXcoHistoSize2);

            ////////////////////////////////XX Correlation//////////////////////////////////////

            XresXcoHistoAll =  new TH2D(detector.c_str(),(detector + ": X Residual vs X Coordinate").c_str(), 400,-length,+length, 2000,-0.1*CONVF,+0.1*CONVF);
            XresXcoHistoAll->GetXaxis()->SetTitle("X Coordinate (10um)");
            XresXcoHistoAll->GetYaxis()->SetTitle("X Residual (10um)")           ;
            XresXcoHistoAll->SetDirectory(0);
            this->addItem(fullPaths[18],XresXcoHistoAll);

            XresXcoHistoSize1 =  new TH2D(detector.c_str(),(detector + ": X Residual vs X Coordinate cluster size 1").c_str(), 400,-length,+length, 2000,-0.1*CONVF,+0.1*CONVF);
            XresXcoHistoSize1->GetXaxis()->SetTitle("X Coordinate (10um)");
            XresXcoHistoSize1->GetYaxis()->SetTitle("X Residual (10um)")           ;
            XresXcoHistoSize1->SetDirectory(0);
            this->addItem(fullPaths[19],XresXcoHistoSize1);

            XresXcoHistoSize2 =  new TH2D(detector.c_str(),(detector + ": X Residual vs X Coordinate cluster size 2").c_str(), 400,-length,+length, 2000,-0.1*CONVF,+0.1*CONVF);
            XresXcoHistoSize2->GetXaxis()->SetTitle("X Coordinate (10um)");
            XresXcoHistoSize2->GetYaxis()->SetTitle("X Residual (10um)")           ;
            XresXcoHistoSize2->SetDirectory(0);
            this->addItem(fullPaths[20],XresXcoHistoSize2);

            ////////////////////////////////YY Correlation//////////////////////////////////////

            YresYcoHistoAll =  new TH2D(detector.c_str(),(detector + ": Y Residual vs Y Coordinate").c_str(), 400,-length,+length, 2000,-0.1*CONVF,+0.1*CONVF);
            YresYcoHistoAll->GetXaxis()->SetTitle("Y Coordinate (10um)");
            YresYcoHistoAll->GetYaxis()->SetTitle("Y Residual (10um)")           ;
            YresYcoHistoAll->SetDirectory(0);
            this->addItem(fullPaths[21],YresYcoHistoAll);

            YresYcoHistoSize1 =  new TH2D(detector.c_str(),(detector + ": Y Residual vs Y Coordinate cluster size 1").c_str(), 400,-length,+length, 2000,-0.1*CONVF,+0.1*CONVF);
            YresYcoHistoSize1->GetXaxis()->SetTitle("Y Coordinate (10um)");
            YresYcoHistoSize1->GetYaxis()->SetTitle("Y Residual (10um)")           ;
            YresYcoHistoSize1->SetDirectory(0);
            this->addItem(fullPaths[22],YresYcoHistoSize1);

            YresYcoHistoSize2 =  new TH2D(detector.c_str(),(detector + ": Y Residual vs Y Coordinate cluster size 2").c_str(), 400,-length,+length, 2000,-0.1*CONVF,+0.1*CONVF);
            YresYcoHistoSize2->GetXaxis()->SetTitle("Y Coordinate (10um)");
            YresYcoHistoSize2->GetYaxis()->SetTitle("Y Residual (10um)")           ;
            YresYcoHistoSize2->SetDirectory(0);
            this->addItem(fullPaths[23],YresYcoHistoSize2);

            STDLINE("Filling " + detector,ACWhite);
        }
        else
        {
            ss_.str(""); ss_ << fullPaths[0] << "/" << detector;
            histoResXAll = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());
            ss_.str(""); ss_ << fullPaths[1] << "/" << detector;
            histoResXSize1 = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());
            ss_.str(""); ss_ << fullPaths[2] << "/" << detector;
            histoResXSize2 = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());

            ss_.str(""); ss_ << fullPaths[3] << "/" << detector;
            histoResYAll = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());
            ss_.str(""); ss_ << fullPaths[4] << "/" << detector;
            histoResYSize1 = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());
            ss_.str(""); ss_ << fullPaths[5] << "/" << detector;
            histoResYSize2 = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());

            ss_.str(""); ss_ << fullPaths[6] << "/" << detector;
            histoPullsXAll = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());
            ss_.str(""); ss_ << fullPaths[7] << "/" << detector;
            histoPullsXSize1 = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());
            ss_.str(""); ss_ << fullPaths[8] << "/" << detector;
            histoPullsXSize2 = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());

            ss_.str(""); ss_ << fullPaths[9] << "/" << detector;
            histoPullsYAll = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());
            ss_.str(""); ss_ << fullPaths[10] << "/" << detector;
            histoPullsYSize1 = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());
            ss_.str(""); ss_ << fullPaths[11] << "/" << detector;
            histoPullsYSize2 = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());

            ss_.str(""); ss_ << fullPaths[12]  << "/" << detector;
            XresYcoHistoAll = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str());
            ss_.str(""); ss_ << fullPaths[13]  << "/" << detector;
            XresYcoHistoSize1 = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str());
            ss_.str(""); ss_ << fullPaths[14]  << "/" << detector;
            XresYcoHistoSize2 = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str());

            ss_.str(""); ss_ << fullPaths[15] << "/" << detector;
            YresXcoHistoAll = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str());
            ss_.str(""); ss_ << fullPaths[16] << "/" << detector;
            YresXcoHistoSize1 = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str());
            ss_.str(""); ss_ << fullPaths[17] << "/" << detector;
            YresXcoHistoSize2 = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str());

            ss_.str(""); ss_ << fullPaths[18]  << "/" << detector;
            XresXcoHistoAll = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str());
            ss_.str(""); ss_ << fullPaths[19]  << "/" << detector;
            XresXcoHistoSize1 = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str());
            ss_.str(""); ss_ << fullPaths[20]  << "/" << detector;
            XresXcoHistoSize2 = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str());

            ss_.str(""); ss_ << fullPaths[21] << "/" << detector;
            YresYcoHistoAll = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str());
            ss_.str(""); ss_ << fullPaths[22] << "/" << detector;
            YresYcoHistoSize1 = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str());
            ss_.str(""); ss_ << fullPaths[23] << "/" << detector;
            YresYcoHistoSize2 = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str());
        }

        histoResXAll   ->Fill( resX              );
        histoResYAll   ->Fill( resY              );
        histoPullsXAll ->Fill( resX/sqrt(sigx2)  );
        histoPullsYAll ->Fill( resY/sqrt(sigy2)  );
        XresYcoHistoAll->Fill( yPrime, resX      );
        YresXcoHistoAll->Fill( xPrime, resY      );
        XresXcoHistoAll->Fill( xPrime, resX      );
        YresYcoHistoAll->Fill( yPrime, resY      );

        if(clusterSizeX == 1)
        {
            histoResXSize1   ->Fill( resX              );
            histoPullsXSize1 ->Fill( resX/sqrt(sigx2)  );
        }
        else if(clusterSizeX == 2)
        {
            histoResXSize2   ->Fill( resX              );
            histoPullsXSize2 ->Fill( resX/sqrt(sigx2)  );
        }
        if(clusterSizeY == 1)
        {
            histoResYSize1   ->Fill( resY              );
            histoPullsYSize1 ->Fill( resY/sqrt(sigy2)  );
        }
        else if(clusterSizeY == 2)
        {
            histoResYSize2   ->Fill( resY              );
            histoPullsYSize2 ->Fill( resY/sqrt(sigy2)  );
        }
        if(clusterSizeX == 1 && clusterSizeY == 1)
        {
            XresYcoHistoSize1->Fill( yPrime, resX      );
            YresXcoHistoSize1->Fill( xPrime, resY      );
            XresXcoHistoSize1->Fill( xPrime, resX      );
            YresYcoHistoSize1->Fill( yPrime, resY      );
        }
        else if((clusterSizeX == 2 && clusterSizeY == 1) || (clusterSizeY == 2 && clusterSizeX == 1))
        {
            XresYcoHistoSize2->Fill( yPrime, resX      );
            YresXcoHistoSize2->Fill( xPrime, resY      );
            XresXcoHistoSize2->Fill( xPrime, resX      );
            YresYcoHistoSize2->Fill( yPrime, resY      );
        }
    }
    return fullPaths;
}
//===========================================================================
HManager::stringVDef  HManager::fillPulls (Event* theEvent   ,
                                           int    trackNum   ,
                                           int    clusterSize,
                                           bool   clear      )
{
    stringVDef fullPaths ;

    fullPaths.push_back(std::string(X_PULLS                      )) ;
    fullPaths.push_back(std::string(Y_PULLS                      )) ;

    if(clear)
    {
        theGeometry_ = theFileEater_->getGeometry();
        for(Geometry::iterator git=theGeometry_->begin(); git!=theGeometry_->end(); git++)
        {
            TH1 *histo = (TH1*)this->getHistogram(fullPaths[0], (*git).first );
            histo->Reset("M");
            histo->GetXaxis()->UnZoom();
            histo      = (TH1*)this->getHistogram(fullPaths[1],(*git).first );
            histo->Reset("M");
            histo->GetXaxis()->UnZoom();
        }
    }
    else
    {
        TH1D    *histoDevX,*histoDevY ;
        int     lastTrack = trackNum+1;
        Event::residualsMapDef &pulls = theEvent->getFittedTrackPulls();

        if(trackNum < 0)
        {
            trackNum  = 0               ;
            lastTrack = pulls.size();
        }

        for(int tr = trackNum; tr < lastTrack; tr++ )
        {
            if( pulls.find(tr) == pulls.end() ) continue;

            Event::alignedHitsCandidateMapDef &track = theEvent->getTrackCandidates()[tr];
            for(Event::trackResidualsMapDef::iterator pul=pulls[tr].begin();pul!=pulls[tr].end(); pul++)
            {
                ss_.str("");
                ss_ << fullPaths[0]  << "/" << pul->first;
                if ( !runSubFolder_->FindObject(ss_.str().c_str()) )
                {
                    //Locked pull distributions range to -5;5

                    histoDevX =  new TH1D(pul->first.c_str(),pul->first.c_str(), 250, -5, 5);
                    histoDevX->GetXaxis()->SetTitle("X Pull");
                    histoDevX->GetYaxis()->SetTitle("tracks")           ;
                    histoDevX->SetDirectory(0);
                    this->addItem(fullPaths[0],histoDevX);

                    histoDevY =  new TH1D(pul->first.c_str(),pul->first.c_str(), 250, -5, 5);
                    histoDevY->GetXaxis()->SetTitle("Y Pull");
                    histoDevY->GetYaxis()->SetTitle("tracks")           ;
                    histoDevY->SetDirectory(0);
                    this->addItem(fullPaths[1],histoDevY);
                    STDLINE("Filling " + (*pul).first,ACWhite);
                }
                else
                {
                    ss_.str("");
                    ss_ << fullPaths[0]  << "/" << pul->first;
                    histoDevX = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());
                    ss_.str("");
                    ss_ << fullPaths[1] << "/" << pul->first;
                    histoDevY = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str());
                }

                if( clusterSize <= 0 || clusterSize == track[pul->first]["size"])
                {
                    //                    if(track[pul->first]["charge"]>30000)//myTry
                    //                    {
                    histoDevX->Fill( pul->second.first  );
                    histoDevY->Fill( pul->second.second );
                    //                    }
                }
            }
        }
    }
    return fullPaths;
}
//============================================================================
HManager::stringVDef  HManager::clearAlignmentResults (std::string detector)
{
    return this->fillAlignmentResults(detector, 0, 0, 0, 0, 0, 0, 0, 0, true);
}
//============================================================================
HManager::stringVDef  HManager::clearResiduals ()
{
    Event *event = NULL;
    return this->fillResiduals(event, -1,-1, true);
}
//============================================================================
HManager::stringVDef  HManager::clearDeviations ()
{
    Event *event = NULL;
    return this->fillDeviations(event, -1,-1, true);
}
//============================================================================
HManager::stringVDef  HManager::clearPulls ()
{
    Event *event = NULL;
    return this->fillPulls(event, -1,-1, true);
}
//============================================================================
HManager::stringVDef  HManager::clearScatterResidual ()
{
    Event *event = NULL;
    return this->fillScatterResidual(event, -1, -1, true);
}

//=============================================================================
std::string HManager::makeRawEvent(  Event * theEvent )
{
    std::string fullPath = std::string("rawEvent");

    if ( runSubFolder_->FindObject(fullPath.c_str())!=0 )
    {
        currentFolder_= (TFolder*)runSubFolder_->FindObject(fullPath.c_str());
        currentFolder_->SetOwner(kTRUE);
        STDLINE("DYING HERE",ACRed);
        //STDLINE(fullPath,ACPurple);
        //currentFolder_->ls();
        runSubFolder_->RecursiveRemove(currentFolder_);
        //STDLINE("NOT DEAD",ACRed);
    }
    currentFolder_ = runSubFolder_->AddFolder(fullPath.c_str(), fullPath.c_str() );

    Event::plaqMapDef &plaqMap = theEvent->getRawData();
    theGeometry_  = theFileEater_->getGeometry();

    int maxCols = theGeometry_->getMaxColsNum();
    int maxRows = theGeometry_->getMaxRowsNum();

    if ( maxCols < maxRows ) maxCols = maxRows;

    std::map<std::string,TH2I*>  vetH;

    for(Event::plaqMapDef::iterator it=plaqMap.begin(); it!=plaqMap.end(); ++it)
    {
        vetH[(*it).first] = new TH2I((*it).first.c_str(), (*it).first.c_str(), maxCols, 0, maxCols, maxCols, 0, maxCols);
        vetH[(*it).first]->GetYaxis()->SetTitle("row");
        vetH[(*it).first]->GetXaxis()->SetTitle("col");
        vetH[(*it).first]->SetDirectory(0);
        currentFolder_->Add( vetH[(*it).first] );
        STDLINE("Filling " + (*it).first,ACWhite);
        for (unsigned int i=0; i<(*it).second.size(); i++)
        {
            unsigned int row      = (*it).second[i]["row"];
            unsigned int col      = (*it).second[i]["col"];

            theGeometry_->getDetector( (*it).first )->flipPixel( &row, &col );

            vetH[(*it).first]->Fill( col , row, (*it).second[i]["adc"] );
        }
    }

    return fullPath;
}
//=========================================================================
std::string  HManager::makeClusterEvent( Event *theEvent )
{
    std::string fullPath = std::string("clusterEvent");

    if ( runSubFolder_->FindObject(fullPath.c_str())!=0 )
    {
        currentFolder_= (TFolder*)runSubFolder_->FindObject(fullPath.c_str());
        currentFolder_->SetOwner(kTRUE);
        STDLINE("DYING HERE",ACRed);
        //STDLINE(fullPath,ACPurple);
        //currentFolder_->ls();
        runSubFolder_->RecursiveRemove(currentFolder_);
        //STDLINE("NOT DEAD",ACRed);
    }
    currentFolder_ = runSubFolder_->AddFolder(fullPath.c_str(),  fullPath.c_str() );

    Event::clustersMapDef clustersMap = theEvent->getClusters();
    theGeometry_  = theFileEater_->getGeometry();

    std::map<std::string,TH2I*>  vetH;

    for(Event::clustersMapDef::iterator it=clustersMap.begin(); it!=clustersMap.end(); ++it)
    {

        vetH[(*it).first] = new TH2I((*it).first.c_str(), (*it).first.c_str(), 1000 ,
                                     /*-theGeometry_->getMaxDetectorsLength()*/0      ,
                                     theGeometry_->getMaxDetectorsLength()      ,
                                     1000 ,
                                     /*-theGeometry_->getMaxDetectorsLength()*/0      ,
                                     theGeometry_->getMaxDetectorsLength()       );
        vetH[(*it).first]->GetYaxis()->SetTitle("Y (10um)");
        vetH[(*it).first]->GetXaxis()->SetTitle("X (10um)");
        vetH[(*it).first]->SetDirectory(0);
        currentFolder_->Add( vetH[(*it).first] );
        STDLINE("Filling " + (*it).first,ACWhite);
        for (Event::aClusterMapDef::iterator cl=(*it).second.begin(); cl!=(*it).second.end(); cl++)
        {
            double x      = (*cl).second["x"];
            double y      = (*cl).second["y"];

            vetH[(*it).first]->Fill( x , y );
        }
    }

    return fullPath;
}
//===========================================================================
HManager::stringVDef HManager::makeTrackEvent( Event *theEvent )
{
    std::string path = "trackEvent";
    xyStringPairDef xyHisto = std::make_pair("Xview", "Yview");

    HManager::stringVDef xyCompletePath;
    ss_.str("");
    ss_ << path << "/" << xyHisto.first;
    xyCompletePath.push_back(ss_.str());
    ss_.str("");
    ss_ << path << "/" << xyHisto.second;
    xyCompletePath.push_back(ss_.str());

    if ( runSubFolder_->FindObject( path.c_str() )!=0 )
    {
        currentFolder_= (TFolder*)runSubFolder_->FindObject( path.c_str() );
        currentFolder_->SetOwner(kTRUE);
        runSubFolder_->RecursiveRemove(currentFolder_);
    }

    theGeometry_  = theFileEater_->getGeometry();

    TH1D  *vetX,*vetY;

    vetX = new TH1D(xyHisto.first.c_str(), xyHisto.first.c_str(), 2000, -100000, +100000);
    vetX->GetYaxis()->SetTitle("X (10um)");
    vetX->GetXaxis()->SetTitle("Z (10um)");
    vetX->SetDirectory(0);
    this->addItem(path,vetX);

    vetY = new TH1D(xyHisto.second.c_str(), xyHisto.second.c_str(), 2000, -100000, +100000);
    vetY->GetYaxis()->SetTitle("Y (10um)");
    vetY->GetXaxis()->SetTitle("Z (10um)");
    vetY->SetDirectory(0);
    this->addItem(path,vetY);

    Event::trackCandidatesDef trackCandidates = theEvent->getTrackCandidates();
    Event::clustersMapDef    &clusters        = theEvent->getClusters();

    for(unsigned int tr=0; tr < trackCandidates.size(); tr++)
    {
        STDLINE("track found",ACGreen);
        Event::alignedHitsCandidateMapDef trackHits = trackCandidates[tr];
        Event::fittedTracksDef fittedTracks = theEvent->getFittedTracks();
        Event::chi2VectorDef   chi2         = theEvent->getFittedTracksChi2();


        ss_.str("");
        ss_ << "Track n: " << tr;
        ss_ << " chi2: " << chi2[tr];
        STDLINE(ss_.str(),ACYellow);

        for(Event::alignedHitsCandidateMapDef::iterator it=trackHits.begin(); it!=trackHits.end(); it++)
        {
            int binx = vetX->Fill(theGeometry_->getDetector( (*it).first )->getZPositionTotal(), (*it).second["x"]);
            vetX->SetBinError(binx,(*it).second["xErr"]);
            int biny = vetY->Fill(theGeometry_->getDetector( (*it).first )->getZPositionTotal(), (*it).second["y"]);
            vetY->SetBinError(biny,(*it).second["yErr"]);

            //           double xtry = clusters[(*it).first][(int)(*it).second["cluster ID"]]["x"];
            //           double ytry = clusters[(*it).first][(int)(*it).second["cluster ID"]]["y"];
            //           double ztry = 0;
            //           double tmp = 0;

            //           theGeometry_->getDetector( (*it).first )->fromDetectorToGlobal(&xtry, &ytry, &ztry, &tmp, &tmp, &tmp);

            ss_.str("");
            ss_ << (*it).first << ": "
                << " Translations x: " << theGeometry_->getDetector( (*it).first )->getXPosition()
                << " y: " << theGeometry_->getDetector( (*it).first )->getYPosition();
            //STDLINE(ss_.str(),ACCyan);

            ss_.str("");
            ss_ << "x prime: "  << clusters[(*it).first][(int)(*it).second["cluster ID"]]["x"] - theGeometry_->getDetector( (*it).first )->getXPosition()
                    << " y prime: " << clusters[(*it).first][(int)(*it).second["cluster ID"]]["y"] - theGeometry_->getDetector( (*it).first )->getYPosition()
                    << " z center: " << theGeometry_->getDetector( (*it).first )->getZPositionTotal()
                    << " x: "  << /*xtry*/(*it).second["x"]
                    << " y: "  << /*ytry*/(*it).second["y"]
                    << " z: "  << (*it).second["z"];
            STDLINE(ss_.str(),ACPurple);


        }

        TF1 *fitX = new TF1("fitx", "x*[0]+[1]", -100000, 100000) ;

        fitX->SetParameters(fittedTracks[tr][0],fittedTracks[tr][1]);
        fitX->SetLineColor(2);
        vetX->GetListOfFunctions()->Add(fitX);

        TF1 *fitY = new TF1("fity", "x*[0]+[1]", -100000, 100000) ;
        fitY->SetParameters(fittedTracks[tr][2],fittedTracks[tr][3]);
        fitY->SetLineColor(2);
        vetY->GetListOfFunctions()->Add(fitY);

        ss_.str("");
        ss_ << "Fit Results sx: "  << fittedTracks[tr][0]
            << " qx: " << fittedTracks[tr][1]
            << " sy: " << fittedTracks[tr][2]
            << " qy: " << fittedTracks[tr][3];
        STDLINE(ss_.str(), ACYellow);

    }

    return xyCompletePath;
}
//===========================================================================
HManager::stringVDef HManager::makeFittedTracksBeam(std::vector<int> selectedEvents )
{
    stringVDef fullPaths ;
    //    std::string histoType1 = "fittedTracksBeam";
    //    std::string histoType2 = "trackHitsBeam";

    fullPaths.push_back(TRACKS_INTERSECTION_BEAM_SPOTS);
    fullPaths.push_back(TRACKS_HITS_BEAM_SPOTS        );

    //    TFolder *trackHitsFolder, *fittedTracksFolder;

    //    if ( !runSubFolder_->FindObject(histoType1.c_str()) )
    //    {
    //        fittedTracksFolder = runSubFolder_->AddFolder(histoType1.c_str(), histoType1.c_str());
    //        trackHitsFolder    = runSubFolder_->AddFolder(histoType2.c_str(), histoType2.c_str());
    //    }
    //    else
    //    {
    //       fittedTracksFolder = (TFolder*)runSubFolder_->FindObject(histoType1.c_str());
    //       trackHitsFolder    = (TFolder*)runSubFolder_->FindObject(histoType2.c_str());
    //    }

    theGeometry_  = theFileEater_->getGeometry();

    double length = theGeometry_->getMaxDetectorsLength();

    std::map<std::string,TH2D*> fittedTrackBS, trackHitsBS;

    //for(int ev=0; ev < theFileEater_->getEventsNumber(); ev++)
    for(std::vector<int>::iterator ev=selectedEvents.begin(); ev!=selectedEvents.end(); ev++)
    {
        Event *theEvent = theFileEater_->getEvent( (*ev) );
        Event::fittedTracksDef tracks = theEvent->getFittedTracks();
        Event::trackCandidatesDef alignedHitsCandidate = theEvent->getTrackCandidates();

        for(unsigned int tr=0; tr < tracks.size(); tr++)
        {
            for (Geometry::iterator gIt=theGeometry_->begin(); gIt!=theGeometry_->end(); gIt++)
            {
                if ( fittedTrackBS.count((*gIt).first)==0 )//if he doesn't find the plaq histogram in the map he create it
                {
                    fittedTrackBS[(*gIt).first] = new TH2D((*gIt).first.c_str(), (*gIt).first.c_str(), 1000, -length, +length,
                                                           1000, -length, +length);
                    fittedTrackBS[(*gIt).first]->GetYaxis()->SetTitle("Y (10um)");
                    fittedTrackBS[(*gIt).first]->GetXaxis()->SetTitle("X (10um)");
                    fittedTrackBS[(*gIt).first]->SetDirectory(0);
                    //fittedTracksFolder->Add( fittedTrackBS[(*gIt).first] );
                    this->addItem(fullPaths[0],fittedTrackBS[(*gIt).first]);

                    trackHitsBS[(*gIt).first] = new TH2D((*gIt).first.c_str(), (*gIt).first.c_str(), 1000, -length, +length,
                                                         1000, -length, +length);
                    trackHitsBS[(*gIt).first]->GetYaxis()->SetTitle("Y (10um)");
                    trackHitsBS[(*gIt).first]->GetXaxis()->SetTitle("X (10um)");
                    trackHitsBS[(*gIt).first]->SetDirectory(0);
                    //trackHitsFolder->Add( trackHitsBS[(*gIt).first] );
                    this->addItem(fullPaths[1],trackHitsBS[(*gIt).first]);
                    STDLINE("Filling " + (*gIt).first, ACWhite);

                    ss_.str("");
                    ss_ << (*gIt).first << " posx: "<< theGeometry_->getDetector((*gIt).first)->getXPosition()
                        << " posy: "<< theGeometry_->getDetector((*gIt).first)->getYPosition();
                    STDLINE(ss_.str(), ACCyan);
                }
                double x= tracks[tr][0]*theGeometry_->getDetector((*gIt).first)->getZPositionTotal() + tracks[tr][1];
                double y= tracks[tr][2]*theGeometry_->getDetector((*gIt).first)->getZPositionTotal() + tracks[tr][3];

                fittedTrackBS[(*gIt).first]->Fill(x,y);
                trackHitsBS  [(*gIt).first]->Fill(alignedHitsCandidate[tr][(*gIt).first]["x"],alignedHitsCandidate[tr][(*gIt).first]["y"]);
            }
        }
    }

    return fullPaths;
}
//===========================================================================
void HManager::markClusters( Event *theEvent )
{
    Event::clustersMapDef &clusters    = theEvent->getClusters();
    theGeometry_  = theFileEater_->getGeometry();

    for(Event::clustersMapDef::iterator det=clusters.begin(); det!=clusters.end(); ++det)
    {
        for (Event::aClusterMapDef::iterator cluster=(*det).second.begin(); cluster!=(*det).second.end(); cluster++)
        {
            ss_.str("");
            ss_ << "Found cluster " <<  (*cluster).first
                << " in " <<  (*det).first ;
            STDLINE(ss_.str(),ACCyan);

            double y    = (*cluster).second["y"]   ;
            double x    = (*cluster).second["x"]   ;
            double yErr = (*cluster).second["yErr"];
            double xErr = (*cluster).second["xErr"];

            //theGeometry_->getDetector( (*det).first )->flipBackPositionLocal(&x, &y, &xErr, &yErr);
            //theGeometry_->getDetector( (*det).first )->transalteXY(&x, &y);
            ss_.str("");
            ss_ << "x: " <<  x << "+-" << xErr << "\ty: " << y << "+-" << yErr ;
            STDLINE(ss_.str(),ACRed)
        }
    }
}
//===========================================================================
void HManager::markClustersHits( Event *theEvent )
{
    Event::clustersHitsMapDef &clustersHits = theEvent->getClustersHits();
    theGeometry_  = theFileEater_->getGeometry();

    for(Event::clustersHitsMapDef::iterator det=clustersHits.begin(); det!=clustersHits.end(); ++det)
    {
        //this->getHistogram("rawEvent", (*det).first );

        for (Event::aClusterHitsMapDef::iterator cluster=(*det).second.begin(); cluster!=(*det).second.end(); cluster++)
        {
            ss_.str("");
            ss_ << "Found cluster " <<  (*cluster).first
                << " of size " << (*cluster).second.size()
                << " in " <<  (*det).first << " formed from:";
            STDLINE(ss_.str(),ACCyan);
            for(Event::hitsDef::iterator hit=(*cluster).second.begin(); hit!=(*cluster).second.end(); hit++ )
            {
                unsigned int row = (*hit)["row"];
                unsigned int col = (*hit)["col"];
                //              ss_.str("");
                //              ss_ << "Pixel row: " <<  row << " col: " << col  ;
                //              STDLINE(ss_.str(),ACYellow) ;
                //theGeometry_->getDetector( (*det).first )->flipPixel( &row, &col );
                ss_.str("");
                ss_ << "Pixel row: " <<  row << " col: " << col  << " adc: " << (*hit)["adc"];

                ROC *roc = theGeometry_->getDetector((*det).first)->convertPixelToROC(&row, &col);
                int el;
                roc->calibratePixel(row, col, (*hit)["adc"], el);

                ss_ << " el: " << el;
                STDLINE(ss_.str(),ACRed) ;
            }
        }
    }
}

//===========================================================================
HManager::stringVDef HManager::makeElectronDistribution(std::vector<int> selectedEvents,int clusterSize)
{
    HManager::stringVDef fullPath;
    fullPath.push_back(SELECTED_EV_CHARGE_DISTR );

    if ( runSubFolder_->FindObject( fullPath[0].c_str() )!=0 )
    {
        currentFolder_= (TFolder*)runSubFolder_->FindObject( fullPath[0].c_str() );
        currentFolder_->SetOwner(kTRUE);
        runSubFolder_->RecursiveRemove(currentFolder_);
    }

    std::map<std::string,TH1I*> electronDistribution;

    theGeometry_ = theFileEater_->getGeometry();

    for(std::vector<int>::iterator ev=selectedEvents.begin(); ev!=selectedEvents.end(); ++ev)
    {
        if( *ev % 1000 == 0 ) {ss_.str(""); ss_ << "Processing event " << *ev ; STDSNAP(ss_.str(),ACPurple) ;}

        Event *theEvent = theFileEater_->getEvent( (*ev) );
        Event::clustersHitsMapDef &clusterHits = theEvent->getClustersHits()   ;
        Event::trackCandidatesDef &tracks      = theEvent->getTrackCandidates() ;

        for(unsigned int tr=0; tr < tracks.size(); tr++)
        {
            for(Event::alignedHitsCandidateMapDef::iterator det=tracks[tr].begin(); det!=tracks[tr].end(); ++det)
            {
                if ( electronDistribution.count((*det).first)==0 )
                {
                    //ss_.str(""); ss_ << path << "/" << (*det).first ;
                    electronDistribution[(*det).first] = new TH1I((*det).first.c_str(),(*det).first.c_str(),2024,0,100000);
                    electronDistribution[(*det).first]->GetXaxis()->SetTitle("Electrons");
                    electronDistribution[(*det).first]->SetDirectory(0);
                    this->addItem( fullPath[0], electronDistribution[(*det).first]);
                }

                double totADC = 0;
                if (clusterSize < 0 ||
                        (unsigned int)clusterSize == clusterHits[(*det).first][(int)det->second["cluster ID"]].size() )
                {
                    for(unsigned int i=0; i < clusterHits[(*det).first][(int)det->second["cluster ID"]].size(); i++)
                    {
                        unsigned int row = clusterHits[(*det).first][(int)det->second["cluster ID"]][i]["row"];
                        unsigned int col = clusterHits[(*det).first][(int)det->second["cluster ID"]][i]["col"];
                        ROC *roc = theGeometry_->getDetector((*det).first)->convertPixelToROC(&row, &col);
                        int charge;
                        bool isCalibrated = roc->calibratePixel(row, col, clusterHits[(*det).first][(int)det->second["cluster ID"]][i]["adc"], charge);
                        if(isCalibrated)
                            totADC+=charge;
                    }
                    electronDistribution[(*det).first]->Fill( totADC );
                }
            }
        }
    }
    std::cout << std::endl ;

    return fullPath ;
}
//===========================================================================
HManager::stringVDef HManager::storeCustomTObject(TObject *object, std::string objectType)
{
    //    if ( runSubFolder_->FindObject(objectType.c_str())!=0 )
    //            currentFolder_= (TFolder*)runSubFolder_->FindObject(objectType.c_str());
    //    else    currentFolder_ = runSubFolder_->AddFolder(objectType.c_str(), objectType.c_str() );

    //    currentFolder_->Add( object );

    this->addItem(objectType, object);

    HManager::stringVDef fullPath;
    fullPath.push_back(objectType);

    return fullPath;
}

//============================================================================
TObject* HManager::getHistogram(std::string histogramType, int station, int plaq)
{
    ss_.str("");
    ss_ << "Station: " << station << " - " << "Plaq: " << plaq;
    return this->getHistogram(histogramType, ss_.str());
}

//============================================================================
TObject* HManager::getHistogram(std::string histogramType, std::string detectorID)
{
    if ( !runSubFolder_->FindObject( histogramType.c_str() ) )
    {
        //ss_.str("") ; ss_ << histogramType << " folder not found" ;
        //STDLINE(ss_.str(),ACRed) ;
        return emptyTH1I_;
    }
    currentFolder_ = (TFolder*)runSubFolder_->FindObject( histogramType.c_str() );
    //ss_.str("");
    //ss_ << "Successfully entered in " << currentFolder_->GetName();
    //STDLINE(ss_.str(),ACGreen)

    if ( !currentFolder_->FindObject( detectorID.c_str() ) )
        return emptyTH1I_;

    return currentFolder_->FindObject( detectorID.c_str() );
}

//============================================================================
TObject* HManager::getHistogram(std::string histogramPath)
{
    if ( !runSubFolder_->FindObject( histogramPath.c_str() ) )
    {
        ss_.str("") ; ss_ << histogramPath << " histogram not found" ;
        STDLINE(ss_.str(),ACRed) ;
        return emptyTH1I_;
    }

    return runSubFolder_->FindObject( histogramPath.c_str() );
}
//===================================================================
void HManager::setRunSubDir(std::string runSubDirName)
{

    if ( !mainCacheFolder_->FindObject( runSubDirName.c_str() ) )
    {
        runSubFolder_ = mainCacheFolder_->AddFolder( runSubDirName.c_str(), runSubDirName.c_str() );
        ss_.str("") ;
        ss_ << "Adding the "
            << ACBlue  << ACBold
            << runSubDirName
            << ACWhite << ACBold
            << " folder to the ROOT memory-resident tree" ;
        STDLINE(ss_.str(),ACWhite) ;
    }
    else
    {
        runSubFolder_ = (TFolder*)mainCacheFolder_->FindObject( runSubDirName.c_str() );
        ss_.str("") ;
        ss_ << "Chaining the "
            << ACBlue  << ACBold
            << runSubDirName
            << ACWhite << ACBold
            << " folder to the ROOT memory-resident tree" ;
        STDLINE(ss_.str(),ACWhite) ;
    }

}
//===================================================================
TFile * HManager::getFileHandle(std::string fullPath)
{
    return theFileEater_->getFileHandle(fullPath) ;
}

//===================================================================
HManager::stringVDef HManager::splitPath(std::string s)
{
    stringVDef path ;

    boost::regex re("\\/");
    boost::sregex_token_iterator i(s.begin(), s.end(), re, -1);
    boost::sregex_token_iterator j;

    while(i != j) {path.push_back( *i++ ) ;}

    return path ;
}

//===================================================================
std::string HManager::addItem(std::string fullPath , TObject * object)
{
    bool debug = false ;

    TFolder * target = (TFolder *)runSubFolder_->FindObject(fullPath.c_str()) ;

    if( target == NULL ) // The target folder does not seem to exist in memory
    {
        if( debug ) {ss_.str(""); ss_  << "Path "
                                       << fullPath
                                       << " not found : explore point of broken link"
                                       << std::endl ; STDLINE(ss_.str(),ACWhite)}

        stringVDef path = splitPath(fullPath) ; // Split the full path of the target folder:

        std::string partialPath = ""      ;     // the idea is to explore the requested target folder path to establish were to
        std::string lastPath    = path[0] ;     // attach the first missing piece to the already partially existing path.
        std::string lastName    = "root"  ;

        for (stringVDef::iterator it=path.begin(); it!=path.end(); ++it)
        {
            partialPath += *it ;

            if( debug ) {ss_.str(""); ss_ << "Searching for "
                                          << partialPath
                                          << " in "
                                          << runSubFolder_->GetName()
                                          << std::endl ; STDLINE(ss_.str(),ACWhite) }
            target = (TFolder *)runSubFolder_->FindObject(partialPath.c_str()) ; // Get a pointer to the i-th element of the
            // requested target folder path

            if( target == NULL ) // If i-th element does not exist, it is time to create it and attach it to the last valid parent
            {
                std::string thisName = *(it) ;

                if( debug ) {ss_.str(""); ss_ << "Iterative searching for "
                                              << lastPath
                                              << " in "
                                              << runSubFolder_->GetName()
                                              << std::endl ; STDLINE(ss_.str(),ACWhite) }
                target = (TFolder *)runSubFolder_->FindObject(lastPath.c_str()) ;
                if( target == NULL )
                {
                    if( debug ) {ss_.str(""); ss_ << "Could not find  "
                                                  << lastPath
                                                  << " in "
                                                  << runSubFolder_->GetName()
                                                  << " <- define this as target"
                                                  << std::endl ; STDLINE(ss_.str(),ACWhite) }
                    target = (TFolder *)runSubFolder_ ;
                }
                if( debug ) {ss_.str(""); ss_ << "Adding:  "
                                              << thisName
                                              << " to "
                                              << target->GetName()
                                              << std::endl ; STDLINE(ss_.str(),ACWhite) }
                target->AddFolder(thisName.c_str(),thisName.c_str()) ;
                lastPath = target->GetName() ;
            }
            else
            {
                if( debug ) {ss_.str(""); ss_ << "Path "
                                              << partialPath
                                              << " already exists in memory. Ptr: "
                                              << target
                                              << std::endl ; STDLINE(ss_.str(),ACWhite) }
            }
            lastPath     = partialPath ;
            partialPath += std::string("/") ;
        }
    }

    target = (TFolder *)runSubFolder_->FindObject(fullPath.c_str()) ;
    //debug = true;
    TObject* oldObject = 0;
    if( (oldObject = target->FindObjectAny(object->GetName())))
    {
        if( debug )
        {
            ss_.str("");
            ss_ << object->GetName()
                << " already exists in "
                << target->GetName();
            STDLINE(ss_.str(),ACWhite)
        }
        //oldObject->Delete();

    }
    if( debug )
    {
        ss_.str("");
        ss_ << "Finally adding "
            << object->GetName()
            << " to "
            << target->GetName()
            << " target ptr: "
            << target
            << " histo ptr: "
            << object
            << " in "
            << fullPath;
        STDLINE(ss_.str(),ACWhite);
    }
    target->Add(object) ;
    if( debug && fullPath == "Kalman/Smoothing/Pulls/X")
    {
        STDLINE(fullPath,ACRed) ;
        target->ls() ;
    }
    if( debug && fullPath == "Kalman/Filtering/Pulls/X")
    {
        STDLINE(fullPath,ACGreen) ;
        target->ls() ;
    }

    return fullPath ;
}
//===============================================================================
int HManager::getMaxIterations()
{
    return theFileEater_->getEventsNumber();
}
//================================================================================
std::string HManager::getLabel (void )
{
    if(processOperation_   == &HManager::eventsCycle)
    {
     if(      subProcessFunction_ == &HManager::makeResidualDistributions)
     {
        return "Creating residual distributions" ;
     }
     else if( subProcessFunction_ == &HManager::makeAdcDistributions2    )
     {
        return "Creating charge distributions" ;
     }
     else if( subProcessFunction_ == &HManager::makeBeamSpots2           )
     {
        return "Creating beam spot distorbutions and projections" ;
     }
    }
    return "Histogramming" ;
}
//===============================================================================
bool HManager::execute()
{
    if(processOperation_)
    {
        histoPaths_ = (this->*processOperation_)();
        return true;
    }
    else           return false   ;
}
//==============================================================================
void HManager::setOperation(multiProcess_function operation, HManager_Make_function function)
{
    processOperation_  =  operation;
    subProcessFunction_=  function ;
}
//===============================================================================
HManager::stringVDef HManager::eventsCycle()
{
    Event*     theEvent = 0    ;
    stringVDef fullPaths       ;
    bool       add      = false;

    theGeometry_ = theFileEater_->getGeometry();
    for(currentIteration_=0; currentIteration_<theFileEater_->getEventsNumber(); currentIteration_++)
    {
        if(currentIteration_ % 1000 == 0)
        {
            ss_.str("");
            ss_ << "Processing event: " << currentIteration_;
            STDSNAP(ss_.str(),ACGreen);
        }
        theEvent = theFileEater_->getEvent(currentIteration_);

        if(fullPaths.size() == 0)
            fullPaths = (this->*subProcessFunction_)(theEvent,add);
        else
                        (this->*subProcessFunction_)(theEvent,add);

        if(!add) {std::cout << std::endl ;break;}
    }
    std::cout << std::endl ;

    return fullPaths ;
}
/*
//===================================================================================================
HManager::stringVDef HManager::eventsFilter()
{
    stringVDef fullPaths       ;
    bool       add  = false    ;

    theGeometry_ = theFileEater_->getGeometry();

    int totalFound           =0;
    int eventsWithTracksFound=0;
    int selected             =0;

    selectedEvents_.clear();

    for(int ev=0; ev<theFileEater_->getEventsNumber(); ++ev)
    {
        ss_.str("");
        ss_ << "processing event: " << ev;
        if(ev % 1000 == 0) STDLINE(ss_.str(),ACGreen);

        Event *theEvent = theFileEater_->getEvent(ev);
        Event::trackCandidatesDef &trackCandidates = theEvent->getTrackCandidates();
        Event::clustersMapDef     &clusters        = theEvent->getClusters()       ;

        int pass=0;

        //found tracks filter
        if ( maxTracksFilter_ > 0 && trackCandidates.size() > (unsigned int)maxTracksFilter_ ) continue;

        //solved clusters filter
        if ( minPlanesFilter_ > 0 )
        {
            if (clusters.size() < (unsigned int)minPlanesFilter_) continue;

            for(std::set<std::string>::iterator git=selectedDetectors_.begin(); git!=selectedDetectors_.end(); git++)
            {
                if( maxClustersFilter_ > 0 )
                {
                    if ( clusters[(*git)].size() >  0  &&
                         clusters[(*git)].size() <= (unsigned int)maxClustersFilter_ ) ++pass;
                }
                else
                    ++pass;
            }
            if ( pass < minPlanesFilter_ ) continue;
        }

        //found tracks filter
        if ( chi2filter_ > 0                    ||
             minTrackHitsFilter_ > 0            ||
             slopeCuts_                         ||
             minTrackHitsOfClusterSize_>0         )
        {
            bool found          = false;

            for(unsigned int tr=0; tr < trackCandidates.size(); tr++)
            {
                //chi2/DOF filter
                if( chi2filter_ > 0 && theEvent->getFittedTracksChi2()[tr] > chi2filter_   ) continue;

                Event::fittedTracksDef &fittedTracks = theEvent->getFittedTracks();
                if( slopeCuts_                                   &&
                        ( fittedTracks[tr][0] < xLowerSlopeLimit_  ||
                          fittedTracks[tr][0] > xHigherSlopeLimit_ ||
                          fittedTracks[tr][2] < yLowerSlopeLimit_  ||
                          fittedTracks[tr][2] > yHigherSlopeLimit_   ) ) continue;

                if( minTrackHitsFilter_ > 0 || minTrackHitsOfClusterSize_ > 0 )
                {
                    int pass = 0;
                    int cPass= 0;

                    for(std::set<std::string>::iterator git=selectedDetectors_.begin(); git!=selectedDetectors_.end(); git++)
                    {
                        if( trackCandidates[tr].find((*git)) != trackCandidates[tr].end() )
                        {
                            pass++ ;
                            if( clusterSizeFilter_.find((int)trackCandidates[tr][(*git)]["size"])
                                    != clusterSizeFilter_.end() )  cPass++;
                        }
                    }
                    if(minTrackHitsFilter_ > 0        && pass  < minTrackHitsFilter_       ) continue;
                    if(minTrackHitsOfClusterSize_ > 0 && cPass < minTrackHitsOfClusterSize_) continue;
                }
                fullPaths = (this->*subProcessFunction_)(theEvent,add);
                totalFound++           ;
                found = true           ;
            }
            if(found)
            {
                selected++                   ;
                eventsWithTracksFound++      ;
                selectedEvents_.push_back(ev);
            }
        }
        else
        {
            fullPaths = (this->*subProcessFunction_)(theEvent,add);
            selected++;
            int found = trackCandidates.size();
            totalFound += found;
            if (found > 0 )
            {
                eventsWithTracksFound++;
                selectedEvents_.push_back(ev);
            }
        }

        ss_.str("");
        ss_ << eventsWithTracksFound << "/" << selected;
        STDLINE(ss_.str(),ACCyan)
                double tracksPerEvent = (1.*totalFound) / (1.*eventsWithTracksFound);
        ss_.str("");
        ss_ << tracksPerEvent;
        ss_ << " tracks found" ;
        STDLINE(ss_.str(),ACCyan)

                if(!add)  break;
        currentIteration_ = ev;
    }

    return fullPaths ;
}
*/
//======================================================================================
void HManager::clearAllFilters()
{
    selectedDetectors_.clear();
    maxTracksFilter_           = -1;
    minPlanesFilter_           = -1;
    chi2filter_                = -1;
    minTrackHitsFilter_        = -1;
    maxClustersFilter_         = -1;
    maxClusterSizeFilter_      = -1;
    onlyClusterSizeFilter_     = -1;
    maxPlanePointsFilter_      = -1;
}

//======================================================================================
void HManager::setEventFilters(int maxTracks,
                               int minPlanesWithHits,
                               int maxPointsPerDetector)
{
    if(minPlanesWithHits > 0)
    {
        minPlanesFilter_   = minPlanesWithHits   ;
        maxClustersFilter_ = maxPointsPerDetector;
    }
    else
    {
        minPlanesFilter_   = -1;
        maxClustersFilter_ = -1;
    }
    maxTracksFilter_ = maxTracks;
}

//====================================================================================================
HManager::stringVDef HManager::makeClusterPlots2(Event * theEvent, bool &add)
{
    stringVDef fullPaths;
    fullPaths.push_back(std::string(CLUSTERS_OCCUPANCY        )) ;
    fullPaths.push_back(std::string(CLUSTERS_BEAM_PROFILES_X  )) ;
    fullPaths.push_back(std::string(CLUSTERS_BEAM_PROFILES_Y  )) ;
    fullPaths.push_back(std::string(CLUSTERS_BEAM_SPOTS       )) ;

    if( !redo_ && !add && runSubFolder_->FindObject(fullPaths[0].c_str()) )  return fullPaths;

    TH1D* clusterOccupancy = 0;
    TH2D* vet2H            = 0;
    TH1D* vetXH            = 0;
    TH1D* vetYH            = 0;

    double length = theGeometry_->getMaxDetectorsLength();

    Event::clustersMapDef     &clusters    = theEvent->getClusters()     ;
    for(Event::clustersMapDef::iterator det=clusters.begin(); det!=clusters.end(); ++det)
    {
        ss_.str(""); ss_ << fullPaths[1]  << "/" << det->first;
        if ( (vetXH = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str())) == 0 )
        {
            vetXH = new TH1D((*det).first.c_str(), det->first.c_str(),
                             theGeometry_->getDetector(det->first)->getNumberOfCols(true),
                             0,theGeometry_->getDetector(det->first)->getDetectorLengthX(true));
            vetXH->GetXaxis()->SetTitle("X (10um)");
            vetXH->SetDirectory(0);
            this->addItem( fullPaths[1], vetXH );
        }
        else if(redo_) vetXH->Reset();

        ss_.str(""); ss_ << fullPaths[2]  << "/" << det->first;
        if ( (vetYH = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str())) == 0 )
        {
            vetYH = new TH1D((*det).first.c_str(),(*det).first.c_str(),
                             theGeometry_->getDetector(det->first)->getNumberOfRows(true),
                             0,theGeometry_->getDetector(det->first)->getDetectorLengthY(true));
            vetYH->GetXaxis()->SetTitle("Y (10um)");
            vetYH->SetDirectory(0);
            this->addItem( fullPaths[2], vetYH );
        }
        else if(redo_) vetYH->Reset();

        ss_.str(""); ss_ << fullPaths[3]  << "/" << det->first;
        if ( (vet2H = (TH2D*)runSubFolder_->FindObject(ss_.str().c_str())) == 0 )
        {
            vet2H = new TH2D((*det).first.c_str(),(*det).first.c_str(),100,0,length,100,0,length);
            vet2H->GetYaxis()->SetTitle("Y (10um)");
            vet2H->GetXaxis()->SetTitle("X (10um)");
            vet2H->SetDirectory(0);
            this->addItem( fullPaths[3], vet2H );
        }
        else if(redo_) vet2H->Reset();

        ss_.str(""); ss_ << fullPaths[0]  << "/" << det->first;
        if ( (clusterOccupancy = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str())) == 0 )
        {
            clusterOccupancy = new TH1D((*det).first.c_str(),det->first.c_str(),50,0,50);
            clusterOccupancy->GetXaxis()->SetTitle("Clusters size");
            clusterOccupancy->SetDirectory(0);
            this->addItem( fullPaths[0], clusterOccupancy);
        }
        else if(redo_) clusterOccupancy->Reset();

        for (Event::aClusterMapDef::iterator cluster=(*det).second.begin(); cluster!=(*det).second.end(); cluster++)
        {
            double y    = (*cluster).second["y"]   ;
            double x    = (*cluster).second["x"]   ;
            vet2H->Fill(x,y);
            vetXH->Fill(x);
            vetYH->Fill(y);
        }
        clusterOccupancy->Fill((*det).second.size()) ;
    }

    add   = true ;
    redo_ = false;
    return fullPaths ;
}

//===========================================================================
void HManager::setResidualFilters(std::string type, double maxChi2, int maxTracks, int maxPlanePoints, int minPoints, int maxClusterSize, int onlyClusterSize)
{
    residualsToPlot_          = type;
    chi2filter_               = maxChi2;
    maxTracksFilter_          = maxTracks;
    maxPlanePointsFilter_     = maxPlanePoints;
    minTrackHitsFilter_       = minPoints;
    maxClusterSizeFilter_     = maxClusterSize;
    onlyClusterSizeFilter_    = onlyClusterSize;
}

//===========================================================================
void HManager::resetResidualCounters(void)
{
    totalTracksFound_           = 0;
    totalEventsWithTracksFound_ = 0;
}

//===========================================================================
HManager::stringVDef HManager::makeResidualDistributions(Event* theEvent, bool& add)
{
    add = true;
    stringVDef fullPaths ;

    //Max tracks per event filter
    if( maxTracksFilter_ > 0 && theEvent->getTrackCandidates().size() > (unsigned int)maxTracksFilter_)
        return fullPaths;

    //Max clusters on a plane
    bool nextEvent = false;
    if( maxPlanePointsFilter_ > 0 )
        for(Geometry::iterator git=theGeometry_->begin(); git!=theGeometry_->end(); git++)
        {
            if ( (int)theEvent->getClusters()[git->first].size() >  maxPlanePointsFilter_)
            {
                nextEvent = true;
                break;
            }
        }
    if(nextEvent) return fullPaths;

    //Track quality filters
    if ( chi2filter_            > 0 ||
         minTrackHitsFilter_    > 0 ||
         maxClusterSizeFilter_  > 0 ||
         onlyClusterSizeFilter_ > 0   )
    {
        bool goodEvent = false;
        for(unsigned int tr=0; tr < theEvent->getTrackCandidates().size(); tr++)
        {
            //chi2/DOF filter
            if( chi2filter_ > 0 && theEvent->getFittedTracksChi2()[tr] > chi2filter_ ) continue;

            //Checking for minimum hits requirement and max cluster size
            if( minTrackHitsFilter_ > 0 || maxClusterSizeFilter_ > 0 )
            {
                int pass    = 0;
                int purePass= 0;
                for(Geometry::iterator git=theGeometry_->begin(); git!=theGeometry_->end(); git++)
                {
                    if( selectedDetectors_.find(git->first) == selectedDetectors_.end() )
                    {
                        if( theEvent->getTrackCandidates()[tr].count( git->first ) != 0 )
                        {
                            ++pass;
                            if( theEvent->getTrackCandidates()[tr][git->first]["size"] <= maxClusterSizeFilter_)
                                ++purePass;
                        }
                    }
                }
                if( minTrackHitsFilter_ > 0 && pass < minTrackHitsFilter_) continue;
                //If I am here I already passed the min points selection, if active.
                if( maxClusterSizeFilter_ > 0 && purePass != pass ) continue;
            }
            int  clusterSizeSel = -1   ;
            if( onlyClusterSizeFilter_ > 0 )
                clusterSizeSel  = onlyClusterSizeFilter_;
            if( residualsToPlot_ == "showScatterResiduals" )  fullPaths = this->fillScatterResidual( theEvent, (int)tr, clusterSizeSel);
            if( residualsToPlot_ == "showResiduals"        )  fullPaths = this->fillResiduals      ( theEvent, (int)tr, clusterSizeSel);
            if( residualsToPlot_ == "showDeviations"       )  fullPaths = this->fillDeviations     ( theEvent, (int)tr, clusterSizeSel);
            if( residualsToPlot_ == "showPulls"            )  fullPaths = this->fillPulls          ( theEvent, (int)tr, clusterSizeSel);
            ++totalTracksFound_;
            goodEvent = true;
        }
        if(goodEvent)
        {
            ++totalEventsWithTracksFound_;
            //            ss_.str("");
            //            ss_ << ev;
            //            ui->eventDisplayTrackFitterEventsSelected->addItem( QString::fromStdString(ss_.str()) );
            //            selectedEvents_.push_back(ev);
        }
    }
    else
    {
        if( residualsToPlot_ == "showScatterResiduals" )  fullPaths = this->fillScatterResidual( theEvent );
        if( residualsToPlot_ == "showResiduals"        )  fullPaths = this->fillResiduals      ( theEvent );
        if( residualsToPlot_ == "showDeviations"       )  fullPaths = this->fillDeviations     ( theEvent );
        if( residualsToPlot_ == "showPulls"            )  fullPaths = this->fillPulls          ( theEvent );
        int nTracks = theEvent->getTrackCandidates().size();
        totalTracksFound_ += nTracks;
        if (nTracks > 0 )
        {
            totalEventsWithTracksFound_++;
            //                    ss_.str("");
            //                    ss_ << ev;
            //                    ui->eventDisplayTrackFitterEventsSelected->addItem( QString::fromStdString(ss_.str()) );
            //                    selectedEvents_.push_back(ev);
        }
        //theHManager_->fillResiduals( theEvent->makeFittedTrackDeviations(), theEvent );
    }
    return fullPaths;
}

//===========================================================================
HManager::stringVDef HManager::makeAdcDistributions2(Event * theEvent, bool &add )
{
    stringVDef fullPaths ;
    fullPaths.push_back(std::string(ADC_DISTRIBUTIONS         )) ;
    fullPaths.push_back(std::string(FULL_CHARGE_DISTRIBUTIONS )) ;

    if( !redo_ && !add && runSubFolder_->FindObject(fullPaths[0].c_str()) )  return fullPaths;

    TH1I* electronDistribution = 0;
    TH1I* adcDistribution      = 0;

    Event::clustersHitsMapDef &clusterHits = theEvent->getClustersHits();
    for(Event::clustersHitsMapDef::iterator det=clusterHits.begin(); det!=clusterHits.end(); ++det)
    {
        ss_.str(""); ss_ << fullPaths[0]  << "/" << det->first;
        if ( (adcDistribution  = (TH1I*)runSubFolder_->FindObject(ss_.str().c_str())) == 0 )
        {
            adcDistribution = new TH1I((*det).first.c_str(),(*det).first.c_str(),1024,0,1024);
            adcDistribution->GetXaxis()->SetTitle("adc value");
            adcDistribution->SetDirectory(0);
            this->addItem( fullPaths[0], adcDistribution);
        }
        else if(redo_) adcDistribution->Reset();

        ss_.str(""); ss_ << fullPaths[1]  << "/" << det->first;
        if ( (electronDistribution = (TH1I*)runSubFolder_->FindObject(ss_.str().c_str())) == 0 )
        {
            electronDistribution = new TH1I((*det).first.c_str(),(*det).first.c_str(),500,0,100000);
            electronDistribution->GetXaxis()->SetTitle("Electrons");
            electronDistribution->SetDirectory(0);
            this->addItem( fullPaths[1], electronDistribution);
        }
        else if(redo_) electronDistribution->Reset();

        double totADC = 0;
        for (Event::aClusterHitsMapDef::iterator aClusterHit=(*det).second.begin(); aClusterHit!=(*det).second.end(); aClusterHit++)
        {
            for(unsigned int i=0; i < (*aClusterHit).second.size(); i++)
            {
                adcDistribution->Fill( (*aClusterHit).second[i]["adc"] );

                unsigned int row = (*aClusterHit).second[i]["row"];
                unsigned int col = (*aClusterHit).second[i]["col"];
                ROC *roc = theGeometry_->getDetector((*det).first)->convertPixelToROC(&row, &col);
                int charge;
                bool isCalibrated = roc->calibratePixel(row, col, (*aClusterHit).second[i]["adc"], charge, theGeometry_->getDetector(det->first)->isDUT());

                ss_.str("");
                ss_ << "newAdc[0] " << charge;
                //if(i%1000 == 0) STDLINE(ss_.str(),ACPurple);
                if(isCalibrated)
                    totADC += charge;
            }
            electronDistribution->Fill( totADC );
        }
    }

    add   = true ;
    redo_ = false;
    return fullPaths ;
}
//=============================================================================
HManager::stringVDef HManager::makeHitsFreq2(Event * theEvent, bool &add )
{
    stringVDef fullPaths ;
    fullPaths.push_back(std::string(HITS_FREQUENCY)) ;

    if( !redo_ && !add && runSubFolder_->FindObject(fullPaths[0].c_str()) )  return fullPaths;

    TH1I*  vetH = 0;

    Event::plaqMapDef &plaqMap = theEvent->getRawData();
    for(Event::plaqMapDef::iterator it=plaqMap.begin(); it!=plaqMap.end(); ++it)
    {
        ss_.str(""); ss_ << fullPaths[0]  << "/" << it->first;
        if ( (vetH = (TH1I*)runSubFolder_->FindObject(ss_.str().c_str())) == 0 )
        {
            vetH = new TH1I((*it).first.c_str(),(*it).first.c_str(),50,0,50);
            vetH->GetXaxis()->SetTitle("Num of Hits")  ;
            vetH->GetYaxis()->SetTitle("Num of Events");
            vetH->SetDirectory(0);
            this->addItem(fullPaths[0], vetH);
            STDLINE("Filling " + (*it).first,ACWhite);
        }
        else if(redo_) vetH->Reset();

        vetH->Fill((*it).second.size());
    }

    add   = true ;
    redo_ = false;
    return fullPaths;
}
//===============================================================================
HManager::stringVDef HManager::makeDUTprojections2(Event * theEvent, bool &add, std::vector<std::string> DUTs)
{
    stringVDef fullPaths ;
    fullPaths.push_back(std::string(DUT_PROJ )) ;

    if( !redo_ && !add && runSubFolder_->FindObject(fullPaths[0].c_str()) )  return fullPaths;

    TH2I* vetH = 0   ;

    Event::plaqMapDef &plaqMap = theEvent->getRawData();

    //select only events with hits on at least one DUT
    bool skip = true;
    for(unsigned int det=0; det < DUTs.size(); det++)
    {
        if( plaqMap.count( DUTs[det] ) != 0) skip = false;
    }
    if ( skip ) return fullPaths;

    for(Event::plaqMapDef::iterator it=plaqMap.begin(); it!=plaqMap.end(); ++it)
    {
        ss_.str(""); ss_ << fullPaths[0]  << "/" << it->first;
        if ( (vetH = (TH2I*)runSubFolder_->FindObject(ss_.str().c_str())) == 0 )
        {
            int maxCols = theGeometry_->getMaxColsNum();
            int maxRows = theGeometry_->getMaxRowsNum();
            if ( maxCols < maxRows ) maxCols = maxRows;
            vetH = new TH2I((*it).first.c_str(), (*it).first.c_str(), maxCols, 0, maxCols, maxCols, 0, maxCols );
            vetH->GetYaxis()->SetTitle("row");
            vetH->GetXaxis()->SetTitle("col");
            vetH->SetDirectory(0);
            this->addItem(fullPaths[0],vetH);
            STDLINE("Filling " + (*it).first,ACWhite);
        }
        else if(redo_) vetH->Reset();

        for (unsigned int i=0; i<(*it).second.size(); i++)
        {
            unsigned int row      = (*it).second[i]["row"];
            unsigned int col      = (*it).second[i]["col"];

            theGeometry_->getDetector( (*it).first )->flipPixel( &row, &col );

            vetH->Fill( col , row );
        }
    }

    add   = true ;
    redo_ = false;
    return fullPaths;
}

//============================================================================
HManager::stringVDef  HManager::makeTracksDistr2(Event * theEvent, bool &add )
{
    std::string histoName = "Chi2_Distribution";
    HManager::xyStringPairDef histoNames;
    histoNames.first = "X_Slope_Distribution";
    histoNames.second= "Y_Slope_Distribution";

    std::string histoType = TRACK_DISTRIBUTIONS;

    HManager::stringVDef fullPaths ;
    ss_.str(""); ss_ << histoType << "/" << histoName;
    fullPaths.push_back(ss_.str());
    ss_.str(""); ss_ << histoType << "/" << histoNames.first;
    fullPaths.push_back(ss_.str());
    ss_.str(""); ss_ << histoType << "/" << histoNames.second;
    fullPaths.push_back(ss_.str());

    if( !redo_ && !add && runSubFolder_->FindObject(fullPaths[0].c_str()) )  return fullPaths;

    TH1D* histo  = 0;
    TH1D* histoX = 0;
    TH1D* histoY = 0;

    if ( (histo = (TH1D*)runSubFolder_->FindObject(fullPaths[0].c_str())) == 0 )
    {
        histo = new TH1D(histoName.c_str(), histoName.c_str(), 20000, 0, 500 );
        histo->GetXaxis()->SetTitle("chi2/DOF"  );
        histo->GetYaxis()->SetTitle("tracks");
        histo->SetDirectory(0);
        this->addItem( histoType, histo );
        STDLINE("Filling " + histoName,ACWhite);
    }
    else if(redo_) histo->Reset();

    if ( (histoX = (TH1D*)runSubFolder_->FindObject(fullPaths[1].c_str())) == 0 )
    {
        histoX = new TH1D(histoNames.first.c_str(), histoNames.first.c_str(), 10000, -0.03, 0.03 );
        histoX->GetXaxis()->SetTitle("X Slope");
        histoX->GetYaxis()->SetTitle("tracks" );
        histoX->SetDirectory(0);
        this->addItem( histoType, histoX );
        STDLINE("Filling " + histoNames.first ,ACWhite);
    }
    else if(redo_) histoX->Reset();

    if( (histoY = (TH1D*)runSubFolder_->FindObjectAny(fullPaths[2].c_str())) == 0 )
    {
        histoY = new TH1D(histoNames.second.c_str(), histoNames.second.c_str(), 10000, -0.03, 0.03 );
        histoY->GetXaxis()->SetTitle("Y Slope");
        histoY->GetYaxis()->SetTitle("tracks" );
        histoY->SetDirectory(0);
        this->addItem( histoType, histoY );
        STDLINE("Filling " + histoNames.second,ACWhite);
    }
    else if(redo_) histoY->Reset();

    Event::chi2VectorDef              & chi2Vet      =  theEvent->getFittedTracksChi2()     ;
    Event::fittedTracksDef            & fittedTracks =  theEvent->getFittedTracks()         ;
    //Event::trackCandidatesDef         & tracks       =  theEvent->getTrackCandidates()      ;
    //Event::fittedTrackCovarianceDef   & covMat       =  theEvent->getFittedTrackCovariance();

    for (unsigned int tr=0; tr < chi2Vet.size(); tr++)
    {
        //        unsigned int purePass = 0;
        //        for(Geometry::iterator git=theGeometry_->begin(); git!=theGeometry_->end(); git++)
        //        {
        //              if ( tracks[tr][(*git).first]["size"] >  0 &&
        //                   tracks[tr][(*git).first]["size"] <= 2 ) purePass++;
        //        }
        //        if( purePass < theGeometry_->getDetectorsNumber(true) ) continue;

        histo->Fill( chi2Vet[tr] );
        histoX->Fill( fittedTracks[tr][0] );
        histoY->Fill( fittedTracks[tr][2] );
    }

    add   = true ;
    redo_ = false;
    return fullPaths;
}
//===========================================================================
HManager::stringVDef HManager::makeTrackErrorsOnDUTs2 (Event * theEvent, bool &add)
{
    stringVDef fullPaths ;
    fullPaths.push_back(std::string(X_TRACK_ERRORS_ON_DUT )) ;
    fullPaths.push_back(std::string(Y_TRACK_ERRORS_ON_DUT )) ;

    if( !redo_ && !add  && runSubFolder_->FindObject(fullPaths[0].c_str()) )  return fullPaths;

    TH1D* vetHx = 0;
    TH1D* vetHy = 0;

    Event::fittedTracksDef           &fittedTracks =  theEvent->getFittedTracks()          ;
    Event::fittedTracksCovarianceDef &covMat       =  theEvent->getFittedTracksCovariance();

    for (unsigned int tr=0; tr < fittedTracks.size(); tr++)
    {
        for(Geometry::iterator duts=theGeometry_->begin(); duts!=theGeometry_->end(); duts++)
        {
            if( !duts->second->isDUT() ) continue;

            ss_.str(""); ss_ << fullPaths[0]  << "/" << duts->first;
            if( (vetHx = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str())) == 0 )
            {
                vetHx = new TH1D(duts->first.c_str(), duts->first.c_str(), 400, 0, 20 );
                vetHx->GetXaxis()->SetTitle("X Track Errors on DUT (#mum)");
                vetHx->GetYaxis()->SetTitle("# of tracks" );
                vetHx->GetXaxis()->SetRangeUser(0,10);
                vetHx->SetDirectory(0);
                this->addItem(fullPaths[0] , vetHx );
            }
            else if(redo_) vetHx->Reset();

            ss_.str(""); ss_ << fullPaths[1]  << "/" << duts->first;
            if( (vetHy = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str())) == 0 )
            {
                vetHy = new TH1D(duts->first.c_str(), duts->first.c_str(), 400, 0, 20 );
                vetHy->GetXaxis()->SetTitle("Y Track Errors on DUT (#mum)");
                vetHy->GetYaxis()->SetTitle("# of tracks" );
                vetHy->GetXaxis()->SetRangeUser(0,10);
                vetHy->SetDirectory(0);
                this->addItem(fullPaths[1] , vetHy );
            }
            else if(redo_) vetHy->Reset();

            Detector::xyPair xyPair = duts->second->getTrackErrorsOnPlane(fittedTracks[tr], covMat[tr]);
            if(duts->first.c_str() == std::string("Station: 4 - Plaq: 1"))
            {
                vetHx->Fill( 10*sqrt(covMat[tr](1,1)) );
                vetHy->Fill( 10*sqrt(covMat[tr](3,3)) );
            }
            else
            {
                vetHx->Fill( 10*sqrt(xyPair.first ) );
                vetHy->Fill( 10*sqrt(xyPair.second) );
            }
        }
    }

    add  = true ;
    redo_= false;
    return fullPaths;
}

//=============================================================================
HManager::stringVDef HManager::makeBeamSpots2(Event * theEvent, bool &add)
{
    HManager::stringVDef fullPaths;
    fullPaths.push_back(std::string(BEAM_SPOTS        ));
    fullPaths.push_back(std::string(BEAM_PROFILES_X   ));
    fullPaths.push_back(std::string(BEAM_PROFILES_Y   ));

    theGeometry_ = theFileEater_->getGeometry();

    if( !redo_ && !add && runSubFolder_->FindObject(fullPaths[0].c_str()) )  return fullPaths;

    TH2I* flippedBeamSpotsHistos = 0;
    TH1D* xProfileHistos         = 0;
    TH1D* yProfileHistos         = 0;

    Detector*    aDetector   = 0;
    std::string  aDetectorName  ;
    bool         enoughHits  = true ;

    Event::plaqMapDef& plaqMap = theEvent->getRawData();

    unsigned int planesWithAtLeastAHit = 0;
    if( restrictSearch_ )
    {
        for(Geometry::iterator det=theGeometry_->begin(); det!=theGeometry_->end(); det++)
        {
            aDetectorName = det->first;
            Event::plaqMapDef::iterator pM;
            if( (pM = plaqMap.find(aDetectorName)) != plaqMap.end())
            {
                if( pM->second.size() > 0 ) planesWithAtLeastAHit++ ;
//                ss_.str(""); ss_ << "    "
//                                 << aDetectorName
//                                 << " has "
//                                 << pM->second.size()
//                                 << " hits" ;
//                STDLINE(ss_.str(),ACWhite) ;
            }
        }

        if(planesWithAtLeastAHit < theGeometry_->getDetectorsNumber(true) )
        {
            enoughHits = false ;
//            ss_.str(""); ss_ << "Event has only "
//                             << planesWithAtLeastAHit
//                             << " planes with a hit" ;
//            STDLINE(ss_.str(),ACRed) ;
        }
    }

    for(Geometry::iterator det=theGeometry_->begin(); det!=theGeometry_->end(); det++)
    {
        aDetectorName = det->first;
        aDetector     = det->second;

        ss_.str(""); ss_ << fullPaths[0]  << "/" << aDetectorName;
        if( (flippedBeamSpotsHistos = (TH2I*)runSubFolder_->FindObjectAny(ss_.str().c_str())) == 0 )
        {
            int          maxCols = theGeometry_->getMaxColsNum();
            int          maxRows = theGeometry_->getMaxRowsNum();
            if ( maxCols < maxRows ) maxCols = maxRows;
            //beam spot
            ss_.str("") ; ss_ << "Beam spot: " << aDetectorName ;
            flippedBeamSpotsHistos = new TH2I(aDetectorName.c_str(), ss_.str().c_str(), maxCols, 0, maxCols, maxRows, 0, maxRows);
            flippedBeamSpotsHistos->SetDirectory(0);
            flippedBeamSpotsHistos->GetYaxis()->SetTitle("Y (10um)");
            flippedBeamSpotsHistos->GetXaxis()->SetTitle("X (10um)");
            this->addItem(fullPaths[0],flippedBeamSpotsHistos);
        }
        else if(redo_) flippedBeamSpotsHistos->Reset();

        ss_.str(""); ss_ << fullPaths[1]  << "/" << aDetectorName;
        if((xProfileHistos = (TH1D*)runSubFolder_->FindObjectAny(ss_.str().c_str())) == 0)
        {
            //profile X
            double xPixPitches[ aDetector->getNumberOfCols()+1 ];
            for( unsigned int i=0; i<aDetector->getNumberOfCols(); i++  )
            {
                xPixPitches[i] = aDetector->getPixelLowEdgeLocalX(i);
            }
            xPixPitches[aDetector->getNumberOfCols()] = aDetector->getDetectorLengthX();

            xProfileHistos = new TH1D( aDetectorName.c_str(), aDetectorName.c_str(), aDetector->getLastCol(), xPixPitches);
            xProfileHistos->GetYaxis()->SetTitle("Contents (area normalized)");
            xProfileHistos->GetXaxis()->SetTitle("X (10um)");
            xProfileHistos->SetDirectory(0);
            this->addItem(fullPaths[1],xProfileHistos);
        }
        else if(redo_) xProfileHistos->Reset();

        ss_.str(""); ss_ << fullPaths[2]  << "/" << aDetectorName;
        if((yProfileHistos = (TH1D*)runSubFolder_->FindObjectAny(ss_.str().c_str())) == 0)
        {
            //profile Y
            double yPixPitches[ aDetector->getNumberOfRows()+1 ];
            for( unsigned int i=0; i<aDetector->getNumberOfRows(); i++  )
            {
                yPixPitches[i] = aDetector->getPixelLowEdgeLocalY(i);
            }
            yPixPitches[aDetector->getNumberOfRows()] = aDetector->getDetectorLengthY();
            yProfileHistos = new TH1D( aDetectorName.c_str(), aDetectorName.c_str(), aDetector->getLastRow(), yPixPitches);
            yProfileHistos->GetYaxis()->SetTitle("Contents (area normalized)");
            yProfileHistos->GetXaxis()->SetTitle("Y (10um)");
            yProfileHistos->SetDirectory(0);
            this->addItem(fullPaths[2],yProfileHistos);
        }
        else if(redo_) yProfileHistos->Reset();

        if( !enoughHits ) continue ;

        Event::plaqMapDef::iterator pM;
        if( (pM = plaqMap.find(aDetectorName)) != plaqMap.end())
        {
            for (unsigned int i=0; i<pM->second.size(); i++)
            {
                unsigned int row = pM->second[i]["row"];
                unsigned int col = pM->second[i]["col"];

                double y    = aDetector->getPixelLowEdgeLocalY(row);
                double x    = aDetector->getPixelLowEdgeLocalX(col);

                double area = aDetector->getPixelPitchLocalY(row)  *
                              aDetector->getPixelPitchLocalX(col)  ;

                xProfileHistos->Fill( x , 1/area );
                yProfileHistos->Fill( y , 1/area );

                aDetector->flipPixel( &row, &col );

                flippedBeamSpotsHistos->Fill( col , row );
            }
        }
    }

    add   = true ;
    redo_ = false;
    return fullPaths;
}
//====================================================================================================
HManager::stringVDef HManager::makeKalmanResidualPlots(trackFitter::residualsVDef & theResiduals)
{
    static HManager::stringVDef fullPaths;
    static bool first = true ;
    if( first )
    {
        first = false ;
        fullPaths.push_back(std::string(KALMAN_FILTER_RES_X  ));
        fullPaths.push_back(std::string(KALMAN_FILTER_RES_Y  ));
        fullPaths.push_back(std::string(KALMAN_FILTER_PULLS_X));
        fullPaths.push_back(std::string(KALMAN_FILTER_PULLS_Y));
        fullPaths.push_back(std::string(KALMAN_FILTER_CHI2   ));
        fullPaths.push_back(std::string(KALMAN_SMOOTH_RES_X  ));
        fullPaths.push_back(std::string(KALMAN_SMOOTH_RES_Y  ));
        fullPaths.push_back(std::string(KALMAN_SMOOTH_PULLS_X));
        fullPaths.push_back(std::string(KALMAN_SMOOTH_PULLS_Y));
        fullPaths.push_back(std::string(KALMAN_SMOOTH_CHI2   ));
    }
    trackFitter::residualsStruct residuals ;

    TH1D * theH ;
    vector<TH1D*> plots ;

    ss_.str(""); ss_ << "Data size: " << theResiduals.size();
    STDLINE(ss_.str(),ACGreen) ;

    for(trackFitter::residualsVDef::iterator it  = theResiduals.begin();
                                             it != theResiduals.end()  ;
                                           ++it)
    {
        residuals = *it ;
//        cout << "plaqID   : " << residuals.plaqID    << endl ;
//        cout << "trackN   : " << residuals.trackN    << endl ;
//        cout << "resX     : " << residuals.resX      << endl ;
//        cout << "resY     : " << residuals.resY      << endl ;
//        cout << "pullX    : " << residuals.pullX     << endl ;
//        cout << "pullY    : " << residuals.pullY     << endl ;
//        cout << "chi2     : " << residuals.chi2      << endl ;
//        cout << "direction: " << residuals.direction << endl ;
//        cout << "----------------------------------" << endl ;

        int offset = 0 ;
        if( residuals.direction == "smoothing" ) offset = fullPaths.size() / 2;

        ss_.str(""); ss_ << fullPaths[offset]  << "/" << residuals.plaqID;
        if ( (theH = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str()) ) == 0 )
        {
            theH = new TH1D(residuals.plaqID.c_str(),residuals.plaqID.c_str(),100,-10,10);
            theH->GetXaxis()->SetTitle("X Residual (10um)");
            theH->SetDirectory(0);
            this->addItem(fullPaths[offset], theH);
            plots.push_back(theH) ;
        }
        else
        {
            theH->Fill(residuals.resX) ;
        }

        ss_.str(""); ss_ << fullPaths[1+offset]  << "/" << residuals.plaqID;
        if ( (theH = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str()) ) == 0 )
        {
            theH = new TH1D(residuals.plaqID.c_str(),residuals.plaqID.c_str(),100,-10,10);
            theH->GetXaxis()->SetTitle("Y Residual (10um)");
            theH->SetDirectory(0);
            this->addItem(fullPaths[1+offset], theH);
            plots.push_back(theH) ;
        }
        else
        {
            theH->Fill(residuals.resY) ;
        }

        ss_.str(""); ss_ << fullPaths[2+offset]  << "/" << residuals.plaqID;
        if ( (theH = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str()) ) == 0 )
        {
            theH = new TH1D(residuals.plaqID.c_str(),residuals.plaqID.c_str(),100,-10,10);
            theH->GetXaxis()->SetTitle("X Pull");
            theH->SetDirectory(0);
            this->addItem(fullPaths[2+offset], theH);
            plots.push_back(theH) ;
        }
        else
        {
            theH->Fill(residuals.pullX) ;
        }

        ss_.str(""); ss_ << fullPaths[3+offset]  << "/" << residuals.plaqID;
        if ( (theH = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str()) ) == 0 )
        {
            theH = new TH1D(residuals.plaqID.c_str(),residuals.plaqID.c_str(),100,-10,10);
            theH->GetXaxis()->SetTitle("Y Pull");
            theH->SetDirectory(0);
            this->addItem(fullPaths[3+offset], theH);
            plots.push_back(theH) ;
        }
        else
        {
            theH->Fill(residuals.pullY) ;
        }

        ss_.str(""); ss_ << fullPaths[4+offset]  << "/" << residuals.plaqID;
        if ( (theH = (TH1D*)runSubFolder_->FindObject(ss_.str().c_str()) ) == 0 )
        {
            theH = new TH1D(residuals.plaqID.c_str(),residuals.plaqID.c_str(),100,0,10);
            theH->GetXaxis()->SetTitle("Chi2");
            theH->SetDirectory(0);
            this->addItem(fullPaths[4+offset], theH);
//            plots.push_back(theH) ; // Do not store chi2 distributions (gaussian fit does not make sense)
        }
        else
        {
            theH->Fill(residuals.chi2) ;
        }
    }

    for(vector<TH1D*>::iterator it  = plots.begin();
                                it != plots.end()  ;
                              ++it)
    {
        STDLINE((*it)->GetName(),ACWhite) ;
        if( (*it)->GetMean() != 0)
        {
            (*it)->Fit("gaus") ;
        }
    }

    return fullPaths ;
}
