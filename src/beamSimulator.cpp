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
 
#include "beamSimulator.h"

bool beamSimulator::growFakeTree(std::string inputFileName, std::string outputTreePath , std::string outputTreeName,
                                 Event *theEvent, Geometry*                                             )
{
    ss_.str("");
    ss_ << outputTreePath << "/" << outputTreeName;
    TFile outputEventTreeFile   ( ss_.str().c_str(), "recreate");

    ss_.str("");
    ss_ << outputTreeName << "Events";
    TTree  outputEventTree   ( ss_.str().c_str(), "A tree with an Event class"  )  ;

    //Fill Event Tree
    outputEventTree.Branch("EventBranch"    , "Event"    , &theEvent    , 16000, 0 )   ;

    //open the generated file
    std::ifstream inputFile;
    inputFile.open(inputFileName.c_str(), std::ios::binary);

    if( !inputFile.is_open() )
    {
        STDLINE(std::string("Could not open file ") + inputFileName, ACRed) ;
        return false;
    }

    int nPlanes = 0;
    inputFile.read((char*)&nPlanes,1);

    ss_.str("");
    ss_ << "Number of planes: " << nPlanes;
    STDLINE(ss_.str(),ACGreen);

    int dummy = 0;
    int plane = 0;
    double x,y;
    int count = 0;

    while( inputFile.good() )
    {
        inputFile.read((char*)&dummy,1);
        count++;
        //        ss_.str("");
        //        ss_ << "Dummy: " << dummy;
        //        STDLINE(ss_.str(),ACGreen);
        Event::clustersMapDef   &clustersMap = theEvent->getClusters();

        for(int p=0; p<nPlanes; p++)
        {
            inputFile.read((char*)&plane,1);
            inputFile.read((char*)&x,sizeof(double));
            inputFile.read((char*)&y,sizeof(double));

            ss_.str("");
            ss_ << "Detector: " << plane << " x: " << x << " y: " << y;
            //if(count%1000 == 0) {STDLINE(ss_.str(),ACGreen);}

            int station, plaq;
            if(plane < 4)
            {
                station = 0    ;
                plaq    = plane;
            }
            else if (plane < 8)
            {
                station = 2      ;
                plaq    = plane-4;
            }
            else
            {
                station = 4      ;
                plaq    = plane-8;
            }
            ss_.str("");
            ss_ << "Station: " << station << " - " << "Plaq: " << plaq;

            if(x<0 || y<0) continue;

            x     = x/10.    ;
            y     = y/10.    ;
            double xErr  = xErr_/10.;
            double yErr  = yErr_/10.;

            //theGeometry->getDetector(ss_.str())->flipPositionLocal(&x,&y,&xErr,&yErr);
            clustersMap[ss_.str()][count]["x"]     = x   ;
            clustersMap[ss_.str()][count]["y"]     = y   ;
            clustersMap[ss_.str()][count]["xErr"]  = xErr;
            clustersMap[ss_.str()][count]["yErr"]  = yErr;
        }

            ss_.str("");
            ss_ << count;
            if(count%1 == 0)
            {
                if(count%1000 == 0)  STDLINE(ss_.str(),ACGreen);
                outputEventTree.Fill();
                theEvent->clear();
            }

    }

    outputEventTreeFile.Write() ;
    outputEventTreeFile.Close() ;

    return true;
}
//========================================================================================================
bool beamSimulator::growFakeTree2(int trackNum, double xErr, double yErr, std::string outputTreePath ,
                                  std::string outputTreeName, Event *theEvent, Geometry *theGeometry  )
{
    TRandom * r = new TRandom() ;

    // Real&Fake Rotation Angles and Shifts
    double ral[9],rbe[9],rga[9],rTx[9],rTy[9],zPos[9];
    // Plane 0
    ral[0]=-26*M_PI/180;
    rbe[0]=0*M_PI/180;
    rga[0]=0*M_PI/180;
    rTx[0]=1200;
    rTy[0]=800;
    // Plane 1
    ral[1]=-26*M_PI/180;
    rbe[1]=0*M_PI/180;
    rga[1]=0*M_PI/180;
    rTx[1]=1400;
    rTy[1]=900;
    // Plane 2
    ral[2]=0*M_PI/180;
    rbe[2]=26*M_PI/180;
    rga[2]=0*M_PI/180;
    rTx[2]=900;
    rTy[2]=1400;
    // Plane 3
    ral[3]=0*M_PI/180;
    rbe[3]=26*M_PI/180;
    rga[3]=0*M_PI/180;
    rTx[3]=800;
    rTy[3]=1500;
    // Plane 4
    ral[4]=0*M_PI/180;
    rbe[4]=26*M_PI/180;
    rga[4]=0*M_PI/180;
    rTx[4]=800;
    rTy[4]=1500;
    // Plane 5
    ral[5]=0*M_PI/180;
    rbe[5]=26*M_PI/180;
    rga[5]=0*M_PI/180;
    rTx[5]=800;
    rTy[5]=1500;
    // Plane 6
    ral[6]=-26*M_PI/180;
    rbe[6]=0*M_PI/180;
    rga[6]=0*M_PI/180;
    rTx[6]=1500;
    rTy[6]=800;
    // Plane 7
    ral[7]=-26*M_PI/180;
    rbe[7]=0*M_PI/180;
    rga[7]=0*M_PI/180;
    rTx[7]=1500;
    rTy[7]=800;

    // Plane 8 DUT
    ral[8]=0*M_PI/180;
    rbe[8]=0*M_PI/180;
    rga[8]=0*M_PI/180;
    rTx[8]=200      ;
    rTy[8]=200      ;

    //  real Inverse Rotation def
    double rRinv[9][3][3];

    for(int i=0; i<9; ++i)
    {
        int plaq=-1, station=-1;
        if( i==0 )      {station = 0; plaq = 0; }
        else if( i==1 ) {station = 0; plaq = 1; }
        else if( i==2 ) {station = 0; plaq = 2; }
        else if( i==3 ) {station = 0; plaq = 3; }
        else if( i==4 ) {station = 2; plaq = 0; }
        else if( i==5 ) {station = 2; plaq = 1; }
        else if( i==6 ) {station = 2; plaq = 2; }
        else if( i==7 ) {station = 2; plaq = 3; }
        else if( i==8 ) {station = 4; plaq = 0; }

        ss_.str("");
        ss_ << "Station: " << station << " - " << "Plaq: " << plaq;

        //  real Inverse Rotation
        rRinv[i][0][0]= cos(rbe[i])*cos(rga[i]);
        rRinv[i][0][1]=-cos(ral[i])*sin(rga[i])+sin(ral[i])*sin(rbe[i])*cos(rga[i]);
        rRinv[i][0][2]= sin(ral[i])*sin(rga[i])+cos(ral[i])*sin(rbe[i])*cos(rga[i]);
        rRinv[i][1][0]= cos(rbe[i])*sin(rga[i]);
        rRinv[i][1][1]= cos(ral[i])*cos(rga[i])+sin(ral[i])*sin(rbe[i])*sin(rga[i]);
        rRinv[i][1][2]=-sin(ral[i])*cos(rga[i])+cos(ral[i])*sin(rbe[i])*sin(rga[i]);
        rRinv[i][2][0]=-sin(rbe[i]);
        rRinv[i][2][1]= sin(ral[i])*cos(rbe[i]);
        rRinv[i][2][2]= cos(ral[i])*cos(rbe[i]);

        double rz = 0;
        //if(i!=0 && i!=7) rz = r->Gaus(0,1000);
        zPos[i] = theGeometry->getDetector(ss_.str())->getZPositionTotal()+rz;
    }

    ss_.str("");
    ss_ << outputTreePath << "/" << outputTreeName;
    TFile outputEventTreeFile   ( ss_.str().c_str(), "recreate");

    ss_.str("");
    ss_ << outputTreeName << "Events";
    TTree  outputEventTree   ( ss_.str().c_str(), "A tree with an Event class"  )  ;

    //Fill Event Tree
    outputEventTree.Branch("EventBranch"    , "Event"    , &theEvent    , 16000, 0 )   ;

    Event::clustersMapDef   &clustersMap = theEvent->getClusters();
    for(int j=0; j<trackNum; ++j)
    {
        double sx=r->Gaus(0,.0001);
        double qx=r->Gaus(0,300);
        double sy=r->Gaus(0,.0001);
        double qy=r->Gaus(0,300);

        bool out = false;
        for(int i=0; i<9; ++i)
        {
            int plaq=-1, station=-1;
            if( i==0 )      {station = 0; plaq = 0; }
            else if( i==1 ) {station = 0; plaq = 1; }
            else if( i==2 ) {station = 0; plaq = 2; }
            else if( i==3 ) {station = 0; plaq = 3; }
            else if( i==4 ) {station = 2; plaq = 0; }
            else if( i==5 ) {station = 2; plaq = 1; }
            else if( i==6 ) {station = 2; plaq = 2; }
            else if( i==7 ) {station = 2; plaq = 3; }
            else if( i==8 ) {station = 4; plaq = 0; }

            ss_.str("");
            ss_ << "Station: " << station << " - " << "Plaq: " << plaq;

            double rNX=(sx*zPos[i]+qx)*(rRinv[i][1][1]-sy*rRinv[i][2][1])-(sy*zPos[i]+qy)*(rRinv[i][0][1]-sx*rRinv[i][2][1]);
            double rNY=(sy*zPos[i]+qy)*(rRinv[i][0][0]-sx*rRinv[i][2][0])-(sx*zPos[i]+qx)*(rRinv[i][1][0]-sy*rRinv[i][2][0]);
            double rD=(rRinv[i][0][0]-sx*rRinv[i][2][0])*(rRinv[i][1][1]-sy*rRinv[i][2][1])-(rRinv[i][1][0]-sy*rRinv[i][2][0])*(rRinv[i][0][1]-sx*rRinv[i][2][1]);

            clustersMap[ss_.str()][j]["x"]     = rNX/rD+rTx[i]+r->Gaus(0,xErr);
            clustersMap[ss_.str()][j]["y"]     = rNY/rD+rTy[i]+r->Gaus(0,yErr);
            clustersMap[ss_.str()][j]["xErr"]  = xErr;
            clustersMap[ss_.str()][j]["yErr"]  = yErr;

            if(!theGeometry->getDetector(ss_.str())->isDUT() && !out)
            {
                if( clustersMap[ss_.str()][j]["x"]<0    ||
                    clustersMap[ss_.str()][j]["x"]>theGeometry->getDetector(ss_.str())->getDetectorLengthX(true) ||
                    clustersMap[ss_.str()][j]["y"]<0    ||
                    clustersMap[ss_.str()][j]["y"]>theGeometry->getDetector(ss_.str())->getDetectorLengthY(true)   )
                {
                    trackNum++;
                    //out = true;
                    break;
                }
            }
        }
        ss_.str("");
        ss_ << j;
        if(j%1 == 0)
        {
            if(j%1000 == 0)  STDLINE(ss_.str(),ACGreen);
            outputEventTree.Fill();
            theEvent->clear();
        }
    }

    outputEventTreeFile.Write() ;
    outputEventTreeFile.Close() ;
    delete r;

    return true;
}
