// g++ -o thisistheend_perfectdata thisistheend_perfectdata.cpp `root-config --cflags --libs` && ./thisistheend_perfectdata

#include <iostream>
#include <sstream>
#include <vector>

#include <math.h>

#include <TApplication.h>
#include <TH1F.h>
#include <TH2D.h>
#include <TRandom.h>
#include <TCanvas.h>
#include <TMatrixD.h>

using namespace std ;

void doIt(void) ; 
void matrici(void) ; 

//==================================================================
int main(int argc, char** argv)
{
  TApplication app("Folders",&argc, argv);
 
//  doIt() ;
  matrici() ;
 
  app.Run() ;
  
  return 0 ;
}

//==================================================================
void doIt(void) 
{
  TCanvas * canvas = new TCanvas("c","c") ;
  
  canvas->Divide(3,4) ;

  TRandom * r = new TRandom() ;
  
  double a = 0 ;

  std::vector<TH1F*> hVector ;

//  TH1F * h = new TH1F("name","title",100,0,100) ;

  std::stringstream name ;

  for(int i=0; i<10; ++i)
  {
    name.str("") ;
    name << "Histogram " << i ;
    hVector.push_back(new TH1F(name.str().c_str(),name.str().c_str(),100,0,100)) ;
  }
    
  int pad = 1 ;

  for(std::vector<TH1F*>::iterator it=hVector.begin(); it!=hVector.end(); ++it)
  {
    for(int i=0; i<10000; ++i)
    {
      (*it)->Fill(r->Gaus(50,10)) ;
    }
  
    canvas->cd(pad++) ;
    (*it)->Draw() ;
   }
}

//==================================================================
void matrici(void) 
{
  TRandom * r = new TRandom() ;

  std::stringstream ss ;
  std::vector<TH2D*>  h2 ;  
  for(int i=0; i<64; ++i)
  {
   ss.str("") ; ss << "Corr " << i ;
   h2.push_back(new TH2D(ss.str().c_str(),ss.str().c_str(), 100,0,0, 100, 0,0 )) ; 
  }

  std::vector<TH1D*>  h1 ;  
  for(int i=0; i<32; ++i)
  {
   ss.str("") ; ss << "Residuals " << i ;
   h1.push_back(new TH1D(ss.str().c_str(),ss.str().c_str(), 100,0,0)) ; 
  }

  
// Real&Fake Rotation Angles and Shifts 
  double ral[8],rbe[8],rga[8],rTx[8],rTy[8],rTz[8],fal[8],fbe[8],fga[8],fTz[8];
// Plane 0  
  ral[0]=-20*M_PI/180;   fal[0]=-20*M_PI/180;
  rbe[0]=0*M_PI/180;    fbe[0]=0*M_PI/180;
  rga[0]=0*M_PI/180;    fga[0]=0*M_PI/180;
  rTx[0]=0;
  rTy[0]=0;
  rTz[0]=43000;		fTz[0]=43000;
// Plane 1  
  ral[1]=-20*M_PI/180;   fal[1]=-20*M_PI/180;
  rbe[1]=0*M_PI/180;    fbe[1]=0*M_PI/180;
  rga[1]=0*M_PI/180;    fga[1]=0*M_PI/180;
  rTx[1]=-40;
  rTy[1]=30;
  rTz[1]=41000;		fTz[1]=41000;
// Plane 2  
  ral[2]=0*M_PI/180;    fal[2]=0*M_PI/180;
  rbe[2]=20*M_PI/180;   fbe[2]=20*M_PI/180;
  rga[2]=0*M_PI/180;    fga[2]=0*M_PI/180;
  rTx[2]=30;
  rTy[2]=-40;
  rTz[2]=27000;		fTz[2]=27000;
// Plane 3  
  ral[3]=0*M_PI/180;    fal[3]=0*M_PI/180;
  rbe[3]=20*M_PI/180;   fbe[3]=20*M_PI/180;
  rga[3]=0*M_PI/180;    fga[3]=0*M_PI/180;
  rTx[3]=40;
  rTy[3]=70;
  rTz[3]=25000;		fTz[3]=25000;
// Plane 4  
  ral[4]=0*M_PI/180;    fal[4]=0*M_PI/180;
  rbe[4]=20*M_PI/180;   fbe[4]=20*M_PI/180;
  rga[4]=0*M_PI/180;    fga[4]=0*M_PI/180;
  rTx[4]=-30;
  rTy[4]=30;
  rTz[4]=-25000;	fTz[4]=-25000;  
// Plane 5  
  ral[5]=0*M_PI/180;   	fal[5]=0*M_PI/180;
  rbe[5]=20*M_PI/180;	fbe[5]=20*M_PI/180;
  rga[5]=0*M_PI/180;    fga[5]=0*M_PI/180;
  rTx[5]=-30;
  rTy[5]=50;
  rTz[5]=-27000;	fTz[5]=-27000;
// Plane 6  
  ral[6]=-20*M_PI/180;   fal[6]=-20*M_PI/180;
  rbe[6]=0*M_PI/180;    fbe[6]=0*M_PI/180;
  rga[6]=0*M_PI/180;    fga[6]=0*M_PI/180;
  rTx[6]=30;
  rTy[6]=-40;
  rTz[6]=-41000;	fTz[6]=-41000;  
// Plane 7  
  ral[7]=20*M_PI/180;   fal[7]=-20*M_PI/180;
  rbe[7]=0*M_PI/180;    fbe[7]=0*M_PI/180;
  rga[7]=0*M_PI/180;    fga[7]=0*M_PI/180;
  rTx[7]=0;
  rTy[7]=0;
  rTz[7]=-43000;	fTz[7]=-43000;
 
     
  
    //  real Inverse Rotation def
    double rRinv[8][3][3];
//  TMatrixD Rinv(3,3);

   //  fake Inverse Rotation def
   double fRinv[8][3][3];
  TMatrixD Rinv(3,3),deltaRinv(3,3),upRinv(3,3);
  
  for(int i=0; i<8; ++i)
  {
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

  //  fake Inverse Rotation
  fRinv[i][0][0]= cos(fbe[i])*cos(fga[i]);
  fRinv[i][0][1]=-cos(fal[i])*sin(fga[i])+sin(fal[i])*sin(fbe[i])*cos(fga[i]);
  fRinv[i][0][2]= sin(fal[i])*sin(fga[i])+cos(fal[i])*sin(fbe[i])*cos(fga[i]);
  fRinv[i][1][0]= cos(fbe[i])*sin(fga[i]);
  fRinv[i][1][1]= cos(fal[i])*cos(fga[i])+sin(fal[i])*sin(fbe[i])*sin(fga[i]);
  fRinv[i][1][2]=-sin(fal[i])*cos(fga[i])+cos(fal[i])*sin(fbe[i])*sin(fga[i]);
  fRinv[i][2][0]=-sin(fbe[i]);
  fRinv[i][2][1]= sin(fal[i])*cos(fbe[i]);
  fRinv[i][2][2]= cos(fal[i])*cos(fbe[i]);
  }
  
  
  // Fit Matrices
  TMatrixD A(14,4);
  TMatrixD ATVA(4,4);
  TMatrixD ATVxy(4,1);
  TMatrixD ATVAinv(4,4);
  TMatrixD fitpar(4,1);
  TMatrixD pred(14,1);
  TMatrixD resid(14,1);
  TMatrixD Vinv(14,14);
  TMatrixD Vinvres(14,1);


  // Align Matrices
  double aTvaxy[8][2][2],aTvinvrxy[8][2][1];
  TMatrixD xyaTva(2,2),xyaTvainv(2,2),xyaTvinvr(2,1),xyalpar(2,1);
  double aTvaall[8][6][6],aTvinvrall[8][6][1];
  TMatrixD allaTva(6,6),allaTvainv(6,6),allaTvinvr(6,1),allalpar(6,1);
  TMatrixD dueaTva(2,2),dueaTvainv(2,2),dueaTvinvr(2,1),duealpar(2,1);
  TMatrixD treaTva(3,3),treaTvainv(3,3),treaTvinvr(3,1),trealpar(3,1);
  TMatrixD cinaTva(5,5),cinaTvainv(5,5),cinaTvinvr(5,1),cinalpar(5,1);
//  TMatrixD aTva7(3,3),aTvainv7(3,3),alpar7(3,1);
//  TMatrixD aTvinvr7(3,1);
  TMatrixD alpar(6,1);
  double atvagamma,atvinvrgamma;
  double c00,c01,c02,c03,c04,c05;
  double c10,c11,c12,c13,c14,c15;
  double deltaTx[8],deltaTy[8],deltaTz[8];
  double alpha[8],beta[8],gamma[8];
  double dalpha,dbeta,dgamma;
  double dx,dy,dz,dalf,dbet,dgam;
//  double beamcenterx[8],beamcentery[8];
  
  double dfNX_dslx,dfNX_dqx,dfNX_dsly,dfNX_dqy,dfNY_dslx,dfNY_dqx,dfNY_dsly,dfNY_dqy,
  dfD_dslx,dfD_dqx,dfD_dsly,dfD_dqy,dpredx_dslx,dpredx_dqx,dpredx_dsly,dpredx_dqy,
  dpredy_dslx,dpredy_dqx,dpredy_dsly,dpredy_dqy,predsigxx,predsigyy;
  double sumchi2;
  
// Hit Buffer

   double xmeas[10000][8],ymeas[10000][8];
  
    // Reset Translations
  
  for(int i=0; i<8; ++i)
       { 
  	deltaTx[i]=0;
	deltaTy[i]=0;
	deltaTz[i]=0;
	alpha[i]=0;
	beta[i]=0;
	gamma[i]=0;
       }
 
  double sx,qx,sy,qy;
  double rNX,rNY,rD,fNX,fNY,fD ;
  
  double sigx[8],sigy[8];
  sigx[0]=5; sigy[0]=2;
  sigx[1]=5; sigy[1]=2;
  sigx[2]=2; sigy[2]=5;
  sigx[3]=2; sigy[3]=5;
  sigx[4]=2; sigy[4]=5;
  sigx[5]=2; sigy[5]=5;
  sigx[6]=5; sigy[6]=2;
  sigx[7]=5; sigy[7]=2;
  
  double sig11inv,sig12inv,sig22inv,sigxx,sigxy,sigyy,dete;
  double chi2;
  double det = 0 ;
  
  double randomx[8],randomy[8];
  
  double fxprime,fyprime,resxprime, resyprime;
  double rxprime[8],ryprime[8];
  double fxglo[7],fyglo[7],fzglo[7];
  double diff;
  
  
// Control Parameters 

//  int ntracks=10000;	// Number of tracks per iteration
  
  int maxtrial=50;	// Number of Iterations for both Trans & Rots  
  
  int ntrial=0;		// Initializes Iteration Counter
  
  int phase=1;		// phase=0 does a preliminary Trans adjustment and then Trans&Rots for maxtrial iterations both
  
  			// phase=1 does Trans&Rots from scratch 
			
			
		       
 // Loop on random tracks to fill track hit buffer
 
 for(int j=0; j<10000; ++j)
 {
   sx=r->Gaus(0,.001);
   qx=r->Gaus(0,1000);
   sy=r->Gaus(0,.001);
   qy=r->Gaus(0,1000);
   
  for(int i=0; i<8; ++i)
  { 
     
   rNX=(sx*rTz[i]+qx)*(rRinv[i][1][1]-sy*rRinv[i][2][1])-(sy*rTz[i]+qy)*(rRinv[i][0][1]-sx*rRinv[i][2][1]);
   rNY=(sy*rTz[i]+qy)*(rRinv[i][0][0]-sx*rRinv[i][2][0])-(sx*rTz[i]+qx)*(rRinv[i][1][0]-sy*rRinv[i][2][0]);
   rD=(rRinv[i][0][0]-sx*rRinv[i][2][0])*(rRinv[i][1][1]-sy*rRinv[i][2][1])-(rRinv[i][1][0]-sy*rRinv[i][2][0])*(rRinv[i][0][1]-sx*rRinv[i][2][1]);
   
   xmeas[j][i]=rNX/rD+rTx[i]+r->Gaus(0,sigx[i]);
   ymeas[j][i]=rNY/rD+rTy[i]+r->Gaus(0,sigy[i]);
    

    
  }
   
 } 
//   
//   for(int i=0; i<8; ++i)
//  { 
//	   
//    beamcenterx[i]=beamcenterx[i]/ntracks;
//    beamcentery[i]=beamcentery[i]/ntracks;
//   
//  } 
//	 cout << "Plane: " << 0 << endl ;
//	 cout << "beamcenterx: " << beamcenterx[0]<< endl ;
//	 cout << "beamcentery: " << beamcentery[0]<< endl ;
//
//	 cout << "Plane: " << 7 << endl ;
//	 cout << "beamcenterx: " << beamcenterx[7]<< endl ;
//	 cout << "beamcentery: " << beamcentery[7]<< endl ;
//
//				 
//	 fxglo[0]=beamcenterx[0]*fRinv[0][0][0]+beamcentery[0]*fRinv[0][0][1];
//	 fyglo[0]=beamcenterx[0]*fRinv[0][1][0]+beamcentery[0]*fRinv[0][1][1];
//	 fzglo[0]=fTz[0]+beamcenterx[0]*fRinv[0][2][0]+beamcentery[0]*fRinv[0][2][1];
// 
//	 fxglo[7]=beamcenterx[7]*fRinv[7][0][0]+beamcentery[7]*fRinv[0][0][1];
//	 fyglo[7]=beamcenterx[7]*fRinv[7][1][0]+beamcentery[7]*fRinv[0][1][1];
//	 fzglo[7]=fTz[7]+beamcenterx[7]*fRinv[7][2][0]+beamcentery[7]*fRinv[7][2][1];
//
//	 sx=(fxglo[0]-fxglo[7])/(fzglo[0]-fzglo[7]);
//	 qx=fxglo[0]-sx*fzglo[0];
//	 sy=(fyglo[0]-fyglo[7])/(fzglo[0]-fzglo[7]);
//	 qy=fyglo[0]-sy*fzglo[0];
//
//	 for(int i=1; i<7; ++i)
// { 
//     
//   fNX=(sx*fTz[i]+qx)*(fRinv[i][1][1]-sy*fRinv[i][2][1])-(sy*fTz[i]+qy)*(fRinv[i][0][1]-sx*fRinv[i][2][1]);
//   fNY=(sy*fTz[i]+qy)*(fRinv[i][0][0]-sx*fRinv[i][2][0])-(sx*fTz[i]+qx)*(fRinv[i][1][0]-sy*fRinv[i][2][0]);
//   fD=(fRinv[i][0][0]-sx*fRinv[i][2][0])*(fRinv[i][1][1]-sy*fRinv[i][2][1])-(fRinv[i][1][0]-sy*fRinv[i][2][0])*(fRinv[i][0][1]-sx*fRinv[i][2][1]);
//   
//   deltaTx[i]=-fNX/fD+beamcenterx[i];
//   deltaTy[i]=-fNY/fD+beamcentery[i];
//   
//	     cout << "Plane: " << i << endl ;
//	 cout << "beamcenterx: " << beamcenterx[i]<< endl ;
//	 cout << "beamcentery: " << beamcentery[i]<< endl ;
//	 cout << "deltaTx: " << deltaTx[i]<< endl ;
//	 cout << "deltaTy: " << deltaTy[i]<< endl ;
//
//   
//  }
//
//  // End of Preliminary Trans Adjustment


        		 
  
  retry:;		// Each iteration starts from here
  
  
  	cout << "ntrial: " << ntrial << endl ;
	sumchi2=0;
  
    // Reset Alignment Matrices
  
  for(int i=0; i<8; ++i)
  { 
	   
   for(int k=0; k<2; ++k)
   {
   	aTvinvrxy[i][k][0]=0;
	
	for(int l=0; l<2; ++l)
   	{
   		aTvaxy[i][k][l]=0;
    	}	
    }
   }

  for(int i=0; i<8; ++i)
  { 
	   
   for(int k=0; k<6; ++k)
   {
   	aTvinvrall[i][k][0]=0;
	
	for(int l=0; l<6; ++l)
   	{
   		aTvaall[i][k][l]=0;
    	}	
    }
   } 
   
     
//  for(int k=0; k<3; ++k)
//  {
//	 
//	 aTvinvr7(k,0)=0;
//
//	 for(int l=0; l<3; ++l)
//	 {
//		 
//		 aTva7(k,l)=0;
//	 }	 
//   }

  
// Loop on random tracks  
  for(int j=0; j<10000; ++j)
  {

// Loop to exclude plane ii from the track fit     
//  for(int ii=1; ii<2; ++ii)			//allinea1!!!!!!!!!
  for(int ii=0; ii<8; ++ii)
     {


// Reset Fit Matrices for next track    
   for(int k=0; k<4; ++k) 
   {
   	ATVxy(k,0)=0;
	for(int l=0; l<4; ++l)
   	{
   		ATVA(k,l)=0;
    	}	
    }
    for(int k=0; k<14; ++k)
   {
	for(int l=0; l<14; ++l)
   	{
   		Vinv(k,l)=0;
    	}	
    }
        
	int ij=-1;

// Loop on all planes but ii for each random track     
  for(int i=0; i<8; ++i)
   {

    
    rxprime[i]= xmeas[j][i]-deltaTx[i];
    ryprime[i]= ymeas[j][i]-deltaTy[i];

    
	if(i != ii)
	{
	
	ij=ij+1;


     	fxglo[ij]=rxprime[i]*fRinv[i][0][0]+ryprime[i]*fRinv[i][0][1];
     	fyglo[ij]=rxprime[i]*fRinv[i][1][0]+ryprime[i]*fRinv[i][1][1];
     	fzglo[ij]=fTz[i]+rxprime[i]*fRinv[i][2][0]+ryprime[i]*fRinv[i][2][1];
    

//	Updating fit matrices

//	Errors
	sigxx=pow(fRinv[i][0][0]*sigx[i],2)+pow(fRinv[i][0][1]*sigy[i],2);
	sigxy=fRinv[i][0][0]*fRinv[i][1][0]*pow(sigx[i],2)+fRinv[i][0][1]*fRinv[i][1][1]*pow(sigy[i],2);
	sigyy=pow(fRinv[i][1][0]*sigx[i],2)+pow(fRinv[i][1][1]*sigy[i],2);
	dete=sigxx*sigyy-sigxy*sigxy;
	sig11inv=sigyy/dete;
	sig12inv=-sigxy/dete;
	sig22inv=sigxx/dete;
	
////////////////////////////////////////////////
//	ATTENZIONE!!!!!!!!!!!!!!!!!!!!!!!	
//	sig12inv=0;
////////////////////////////////////////////////
	
	
//	if(ntrial==0 && j==0)
//	{
//	cout << "Plane: " << i << endl ;
//	cout << "sig11inv: " << sig11inv << endl ;
//	cout << "sig12inv: " << sig12inv << endl ;
//	cout << "sig22inv: " << sig22inv << endl ;
//	}
	
//	A
	A(2*ij,0)=fzglo[ij];
	A(2*ij,1)=1;
	A(2*ij,2)=0;
	A(2*ij,3)=0;
	A(2*ij+1,0)=0;
	A(2*ij+1,1)=0;
	A(2*ij+1,2)=fzglo[ij];
	A(2*ij+1,3)=1;

	
//	Residuals
	resid(2*ij,0)=fxglo[ij];
	resid(2*ij+1,0)=fyglo[ij];
	
//	Vinv	
	Vinv(2*ij,2*ij)=sig11inv;
	Vinv(2*ij,2*ij+1)=sig12inv;
	Vinv(2*ij+1,2*ij)=sig12inv;
	Vinv(2*ij+1,2*ij+1)=sig22inv;
	
//	ATVA	
	ATVA(0,0)=ATVA(0,0)+pow(fzglo[ij],2)*sig11inv;
	ATVA(0,1)=ATVA(0,1)+    fzglo[ij]   *sig11inv;
	ATVA(0,2)=ATVA(0,2)+pow(fzglo[ij],2)*sig12inv;
	ATVA(0,3)=ATVA(0,3)+    fzglo[ij]   *sig12inv;
	ATVA(1,0)=ATVA(0,1);
	ATVA(1,1)=ATVA(1,1)+1		   *sig11inv;
	ATVA(1,2)=ATVA(0,3);
	ATVA(1,3)=ATVA(1,3)+1		   *sig12inv;
	ATVA(2,0)=ATVA(0,2);
	ATVA(2,1)=ATVA(1,2);
	ATVA(2,2)=ATVA(2,2)+pow(fzglo[ij],2)*sig22inv;
	ATVA(2,3)=ATVA(2,3)+    fzglo[ij]   *sig22inv;
	ATVA(3,0)=ATVA(0,3);
	ATVA(3,1)=ATVA(1,3);
	ATVA(3,2)=ATVA(2,3);
	ATVA(3,3)=ATVA(3,3)+1              *sig22inv;
	
//	ATVxy
	ATVxy(0,0)=ATVxy(0,0)+ fzglo[ij]*(fxglo[ij]*sig11inv+fyglo[ij]*sig12inv);
	ATVxy(1,0)=ATVxy(1,0)+(fxglo[ij]          *sig11inv+fyglo[ij]*sig12inv);
	ATVxy(2,0)=ATVxy(2,0)+ fzglo[ij]*(fxglo[ij]*sig12inv+fyglo[ij]*sig22inv);
	ATVxy(3,0)=ATVxy(3,0)+(fxglo[ij]          *sig12inv+fyglo[ij]*sig22inv);
	
	}	// End if(i != ii)
	
    }	// End Loop on all planes but ii for each random track 
    
//	Fit Results
   	double det = 0 ;
  

	ATVAinv = ATVA.Invert(&det) ;
	fitpar=ATVA*ATVxy;
	
//      pred=A*fitpar;
//      resid=resid-pred;
      
//	Vinvres=Vinv*resid;	
//	chi2=0;
//	for(int i=0; i<16; ++i)
//   {
//   	chi2=chi2+resid(i,0)*Vinvres(i,0);
//
//   }
//	if(ntrial == 9)
//	 {
//	      pred=A*fitpar;
//	      resid=resid-pred;
//	      h2[0]->Fill(fyglo[0],resid(0,0)) ;
//	      h2[1]->Fill(fxglo[0],resid(1,0)) ;
//	      h2[2]->Fill(fxglo[0],resid(0,0)) ;
//	      h2[3]->Fill(fyglo[0],resid(1,0)) ;
//	  } 
//
// h1[18]->Fill(chi2/12) ;
// h1[19]->Fill(det) ;
// diff=sx-fitpar(0,0);
// h1[20]->Fill(diff) ;
// diff=qx-fitpar(1,0);
// h1[21]->Fill(diff) ;
// diff=sy-fitpar(2,0);
// h1[22]->Fill(diff) ;
// diff=qy-fitpar(3,0);
// h1[23]->Fill(diff) ;
//
// cout << __LINE__ << endl ;
//	   
//	for(int i=0; i<16; ++i)
// {
//	h1[i]->Fill(resid(i,0)) ;
// }
//  
//  
//	for(int i=0; i<2; ++i)
//    {
//     h2[4*i+0]->Fill(fyglo[i],resid(0,0)) ;
//     h2[4*i+1]->Fill(fxglo[i],resid(1,0)) ;
//     h2[4*i+2]->Fill(fxglo[i],resid(0,0)) ;
//     h2[4*i+3]->Fill(fyglo[i],resid(1,0)) ;
//     }
//     
// } 
   
      
// !!!!!!!!!!ALIGNMENT !!!!!!!!!       
// Prepare plane ii alignment matrices for each random track 

	int i=ii;  // NOTA BENE 
	

 

    fNX=(fitpar(0,0)*fTz[i]+fitpar(1,0))*(fRinv[i][1][1]-fitpar(2,0)*fRinv[i][2][1])-(fitpar(2,0)*fTz[i]+fitpar(3,0))*(fRinv[i][0][1]-fitpar(0,0)*fRinv[i][2][1]);
    fNY=(fitpar(2,0)*fTz[i]+fitpar(3,0))*(fRinv[i][0][0]-fitpar(0,0)*fRinv[i][2][0])-(fitpar(0,0)*fTz[i]+fitpar(1,0))*(fRinv[i][1][0]-fitpar(2,0)*fRinv[i][2][0]);
    fD=(fRinv[i][0][0]-fitpar(0,0)*fRinv[i][2][0])*(fRinv[i][1][1]-fitpar(2,0)*fRinv[i][2][1])-(fRinv[i][1][0]-fitpar(2,0)*fRinv[i][2][0])*(fRinv[i][0][1]-fitpar(0,0)*fRinv[i][2][1]);
    resxprime=rxprime[i]-fNX/fD;
    resyprime=ryprime[i]-fNY/fD;
    
    	if(phase==1 && ntrial ==0)
     {
	h2[i*8+0]->Fill(ryprime[i],resxprime) ;
	h2[i*8+1]->Fill(rxprime[i],resyprime) ;
	h2[i*8+2]->Fill(rxprime[i],resxprime) ;
	h2[i*8+3]->Fill(ryprime[i],resyprime) ;
      }

     if(phase==1 && ntrial == maxtrial-1)
     {
     dfNX_dslx=(fTz[i])*(fRinv[i][1][1]-fitpar(2,0)*fRinv[i][2][1])-(fitpar(2,0)*fTz[i]+fitpar(3,0))*(-fRinv[i][2][1]);
     dfNX_dqx=(fRinv[i][1][1]-fitpar(2,0)*fRinv[i][2][1]);
     dfNX_dsly=(fitpar(0,0)*fTz[i]+fitpar(1,0))*(-fRinv[i][2][1])-(fTz[i])*(fRinv[i][0][1]-fitpar(0,0)*fRinv[i][2][1]);
     dfNX_dqy=-(fRinv[i][0][1]-fitpar(0,0)*fRinv[i][2][1]);
     dfNY_dslx=(fitpar(2,0)*fTz[i]+fitpar(3,0))*(-fRinv[i][2][0])-(fTz[i]+fitpar(1,0))*(fRinv[i][1][0]-fitpar(2,0)*fRinv[i][2][0]);
     dfNY_dqx=-(fRinv[i][1][0]-fitpar(2,0)*fRinv[i][2][0]);
     dfNY_dsly=(fTz[i])*(fRinv[i][0][0]-fitpar(0,0)*fRinv[i][2][0])-(fitpar(0,0)*fTz[i]+fitpar(1,0))*(-fRinv[i][2][0]);
     dfNY_dqy=(fRinv[i][0][0]-fitpar(0,0)*fRinv[i][2][0]);
     dfD_dslx=(-fRinv[i][2][0])*(fRinv[i][1][1]-fitpar(2,0)*fRinv[i][2][1])-(fRinv[i][1][0]-fitpar(2,0)*fRinv[i][2][0])*(-fRinv[i][2][1]);
     dfD_dqx=0;
     dfD_dsly=(fRinv[i][0][0]-fitpar(0,0)*fRinv[i][2][0])*(-fRinv[i][2][1])-(-fRinv[i][2][0])*(fRinv[i][0][1]-fitpar(0,0)*fRinv[i][2][1]);
     dfD_dqy=0;
     dpredx_dslx=dfNX_dslx/fD-fNX/fD*dfD_dslx/fD;
     dpredx_dqx=dfNX_dqx/fD;
     dpredx_dsly=dfNX_dsly/fD-fNX/fD*dfD_dsly/fD;
     dpredx_dqy=dfNX_dqy/fD;
     dpredy_dslx=dfNY_dslx/fD-fNY/fD*dfD_dslx/fD;
     dpredy_dqx=dfNY_dqx/fD;
     dpredy_dsly=dfNY_dsly/fD-fNY/fD*dfD_dsly/fD;
     dpredy_dqy=dfNY_dqy/fD;
     predsigxx=dpredx_dslx*dpredx_dslx*ATVAinv(0,0)+dpredx_dqx*dpredx_dqx*ATVAinv(1,1)+dpredx_dsly*dpredx_dsly*ATVAinv(2,2)+dpredx_dqy*dpredx_dqy*ATVAinv(3,3)+
     	2*dpredx_dslx*dpredx_dqx*ATVAinv(0,1)+2*dpredx_dslx*dpredx_dsly*ATVAinv(0,2)+2*dpredx_dslx*dpredx_dqy*ATVAinv(0,3)+
	2*dpredx_dqx*dpredx_dsly*ATVAinv(1,2)+2*dpredx_dqx*dpredx_dqy*ATVAinv(1,3)+
	2*dpredx_dsly*dpredx_dqy*ATVAinv(2,3);
     predsigyy=dpredy_dslx*dpredy_dslx*ATVAinv(0,0)+dpredy_dqx*dpredy_dqx*ATVAinv(1,1)+dpredy_dsly*dpredy_dsly*ATVAinv(2,2)+dpredy_dqy*dpredy_dqy*ATVAinv(3,3)+
     	2*dpredy_dslx*dpredy_dqx*ATVAinv(0,1)+2*dpredy_dslx*dpredy_dsly*ATVAinv(0,2)+2*dpredy_dslx*dpredy_dqy*ATVAinv(0,3)+
	2*dpredy_dqx*dpredy_dsly*ATVAinv(1,2)+2*dpredy_dqx*dpredy_dqy*ATVAinv(1,3)+
	2*dpredy_dsly*dpredy_dqy*ATVAinv(2,3);
	
	if(j == 1000) 
	{
	
	 cout << "Plane: " <<  ii << endl ;
	 
	 cout << "dfNX_dslx: " <<  dfNX_dslx << endl ;
	 cout << "dfNX_dqx=: " <<  dfNX_dqx << endl ;
	 cout << "dfNX_dsly: " <<  dfNX_dsly << endl ;
	 cout << "dfNX_dqy=: " <<  dfNX_dqy << endl ;
	 cout << "dfNY_dslx: " <<  dfNY_dslx << endl ;
	 cout << "dfNY_dqx=: " <<  dfNY_dqx << endl ;
	 cout << "dfNY_dsly: " <<  dfNY_dsly << endl ;
	 cout << "dfNY_dqy=: " <<  dfNY_dqy << endl ;
	 cout << "dfD_dslx=: " <<  dfD_dslx << endl ;
	 cout << "dfD_dqx=0: " <<  dfD_dqx << endl ;
	 cout << "dfD_dsly=: " <<  dfD_dsly << endl ;
	 cout << "dfD_dqy=0: " <<  dfD_dqy << endl ;
	
	
	

	cout << "predsigxx: " << sqrt(predsigxx) << endl ;
	
	cout << "predsigyy: " << sqrt(predsigyy) << endl ;
	
	}
     predsigxx=predsigxx+sigx[i]*sigx[i];
     predsigyy=predsigyy+sigy[i]*sigy[i];
     sumchi2=sumchi2+resxprime*resxprime/predsigxx+resyprime*resyprime/predsigyy;
     h1[i*4+0]->Fill(resxprime) ; 			//allinea1!!!!!!!!!
     h1[i*4+1]->Fill(resyprime) ; 			//allinea1!!!!!!!!!
     h1[i*4+2]->Fill(resxprime/sqrt(predsigxx)) ;
     h1[i*4+3]->Fill(resyprime/sqrt(predsigyy)) ;
     
     h2[4+8*i+0]->Fill(ryprime[i],resxprime) ;
     h2[4+8*i+1]->Fill(rxprime[i],resyprime) ;
     h2[4+8*i+2]->Fill(rxprime[i],resxprime) ;
     h2[4+8*i+3]->Fill(ryprime[i],resyprime) ;
          }
    if(phase == 0)
    {
  c03=1;
  c04=0;
  c13=0;
  c14=1;
  
  aTvaxy[i][0][0]=aTvaxy[i][0][0]+c03*c03/pow(sigx[i],2)+c13*c13/pow(sigy[i],2);
  aTvaxy[i][0][1]=aTvaxy[i][0][1]+c03*c04/pow(sigx[i],2)+c13*c14/pow(sigy[i],2);
  aTvaxy[i][1][0]=aTvaxy[i][0][1];
  aTvaxy[i][1][1]=aTvaxy[i][1][1]+c04*c04/pow(sigx[i],2)+c14*c14/pow(sigy[i],2);
  
  aTvinvrxy[i][0][0]=aTvinvrxy[i][0][0]+c03*resxprime/pow(sigx[i],2)+c13*resyprime/pow(sigy[i],2);
  aTvinvrxy[i][1][0]=aTvinvrxy[i][1][0]+c04*resxprime/pow(sigx[i],2)+c14*resyprime/pow(sigy[i],2);
  
     }  


    if(phase == 1)
    {

    
                                                                       
    c00=1/fD*(((fRinv[i][1][2]-fitpar(2,0)*fRinv[i][2][2])*(fitpar(0,0)*fTz[i]+fitpar(1,0))-(fRinv[i][0][2]-fitpar(0,0)*fRinv[i][2][2])*(fitpar(2,0)*fTz[i]+fitpar(3,0)))
       -fNX/fD*((fRinv[i][1][2]-fitpar(2,0)*fRinv[i][2][2])*(fRinv[i][0][0]-fitpar(0,0)*fRinv[i][2][0])-(fRinv[i][0][2]-fitpar(0,0)*fRinv[i][2][2])*(fRinv[i][1][0]-fitpar(2,0)*fRinv[i][2][0])));
    c01=1/fD*fNX/fD*((fRinv[i][0][2]-fitpar(0,0)*fRinv[i][2][2])*(fRinv[i][1][1]-fitpar(2,0)*fRinv[i][2][1])-(fRinv[i][1][2]-fitpar(2,0)*fRinv[i][2][2])*(fRinv[i][0][1]-fitpar(0,0)*fRinv[i][2][1]));
    c02=1/fD*((fRinv[i][0][0]-fitpar(0,0)*fRinv[i][2][0])*(fitpar(2,0)*fTz[i]+fitpar(3,0))-(fRinv[i][1][0]-fitpar(2,0)*fRinv[i][2][0])*(fitpar(0,0)*fTz[i]+fitpar(1,0)));
    c03=1;
    c04=0;
    c05=1/fD*((fRinv[i][0][2]-fitpar(0,0)*fRinv[i][2][2])*(fRinv[i][1][1]-fitpar(2,0)*fRinv[i][2][1])-(fRinv[i][1][2]-fitpar(2,0)*fRinv[i][2][2])*(fRinv[i][0][1]-fitpar(0,0)*fRinv[i][2][1]));

    c10=-1/fD*fNY/fD*((fRinv[i][1][2]-fitpar(2,0)*fRinv[i][2][2])*(fRinv[i][0][0]-fitpar(0,0)*fRinv[i][2][0])-(fRinv[i][0][2]-fitpar(0,0)*fRinv[i][2][2])*(fRinv[i][1][0]-fitpar(2,0)*fRinv[i][2][0]));
    c11=1/fD*(((fRinv[i][1][2]-fitpar(2,0)*fRinv[i][2][2])*(fitpar(0,0)*fTz[i]+fitpar(1,0))-(fRinv[i][0][2]-fitpar(0,0)*fRinv[i][2][2])*(fitpar(2,0)*fTz[i]+fitpar(3,0)))
  	+fNY/fD*((fRinv[i][0][2]-fitpar(0,0)*fRinv[i][2][2])*(fRinv[i][1][1]-fitpar(2,0)*fRinv[i][2][1])-(fRinv[i][1][2]-fitpar(2,0)*fRinv[i][2][2])*(fRinv[i][0][1]-fitpar(0,0)*fRinv[i][2][1])));
    c12=1/fD*((fRinv[i][0][1]-fitpar(0,0)*fRinv[i][2][1])*(fitpar(2,0)*fTz[i]+fitpar(3,0))-(fRinv[i][1][1]-fitpar(2,0)*fRinv[i][2][1])*(fitpar(0,0)*fTz[i]+fitpar(1,0)));
    c13=0;
    c14=1;
    c15=1/fD*((fRinv[i][1][2]-fitpar(2,0)*fRinv[i][2][2])*(fRinv[i][0][0]-fitpar(0,0)*fRinv[i][2][0])-(fRinv[i][0][2]-fitpar(0,0)*fRinv[i][2][2])*(fRinv[i][1][0]-fitpar(2,0)*fRinv[i][2][0]));
    

    
     aTvaall[i][0][0]=aTvaall[i][0][0]+c00*c00/pow(sigx[i],2)+c10*c10/pow(sigy[i],2);
     aTvaall[i][0][1]=aTvaall[i][0][1]+c00*c01/pow(sigx[i],2)+c10*c11/pow(sigy[i],2);
     aTvaall[i][0][2]=aTvaall[i][0][2]+c00*c02/pow(sigx[i],2)+c10*c12/pow(sigy[i],2);
     aTvaall[i][0][3]=aTvaall[i][0][3]+c00*c03/pow(sigx[i],2)+c10*c13/pow(sigy[i],2);
     aTvaall[i][0][4]=aTvaall[i][0][4]+c00*c04/pow(sigx[i],2)+c10*c14/pow(sigy[i],2);
     aTvaall[i][0][5]=aTvaall[i][0][5]+c00*c05/pow(sigx[i],2)+c10*c15/pow(sigy[i],2);
     aTvaall[i][1][0]=aTvaall[i][0][1];
     aTvaall[i][1][1]=aTvaall[i][1][1]+c01*c01/pow(sigx[i],2)+c11*c11/pow(sigy[i],2);
     aTvaall[i][1][2]=aTvaall[i][1][2]+c01*c02/pow(sigx[i],2)+c11*c12/pow(sigy[i],2);
     aTvaall[i][1][3]=aTvaall[i][1][3]+c01*c03/pow(sigx[i],2)+c11*c13/pow(sigy[i],2);
     aTvaall[i][1][4]=aTvaall[i][1][4]+c01*c04/pow(sigx[i],2)+c11*c14/pow(sigy[i],2);
     aTvaall[i][1][5]=aTvaall[i][1][5]+c01*c05/pow(sigx[i],2)+c11*c15/pow(sigy[i],2);
     aTvaall[i][2][0]=aTvaall[i][0][2];
     aTvaall[i][2][1]=aTvaall[i][1][2];
     aTvaall[i][2][2]=aTvaall[i][2][2]+c02*c02/pow(sigx[i],2)+c12*c12/pow(sigy[i],2);
     aTvaall[i][2][3]=aTvaall[i][2][3]+c02*c03/pow(sigx[i],2)+c12*c13/pow(sigy[i],2);
     aTvaall[i][2][4]=aTvaall[i][2][4]+c02*c04/pow(sigx[i],2)+c12*c14/pow(sigy[i],2);
     aTvaall[i][2][5]=aTvaall[i][2][5]+c02*c05/pow(sigx[i],2)+c12*c15/pow(sigy[i],2);
     aTvaall[i][3][0]=aTvaall[i][0][3];
     aTvaall[i][3][1]=aTvaall[i][1][3];
     aTvaall[i][3][2]=aTvaall[i][2][3];
     aTvaall[i][3][3]=aTvaall[i][3][3]+c03*c03/pow(sigx[i],2)+c13*c13/pow(sigy[i],2);
     aTvaall[i][3][4]=aTvaall[i][3][4]+c03*c04/pow(sigx[i],2)+c13*c14/pow(sigy[i],2);
     aTvaall[i][3][5]=aTvaall[i][3][5]+c03*c05/pow(sigx[i],2)+c13*c15/pow(sigy[i],2);
     aTvaall[i][4][0]=aTvaall[i][0][4];
     aTvaall[i][4][1]=aTvaall[i][1][4];
     aTvaall[i][4][2]=aTvaall[i][2][4];
     aTvaall[i][4][3]=aTvaall[i][3][4];
     aTvaall[i][4][4]=aTvaall[i][4][4]+c04*c04/pow(sigx[i],2)+c14*c14/pow(sigy[i],2);
     aTvaall[i][4][5]=aTvaall[i][4][5]+c04*c05/pow(sigx[i],2)+c14*c15/pow(sigy[i],2);
     aTvaall[i][5][0]=aTvaall[i][0][5];
     aTvaall[i][5][1]=aTvaall[i][1][5];
     aTvaall[i][5][2]=aTvaall[i][2][5];
     aTvaall[i][5][3]=aTvaall[i][3][5];
     aTvaall[i][5][4]=aTvaall[i][4][5];
     aTvaall[i][5][5]=aTvaall[i][5][5]+c05*c05/pow(sigx[i],2)+c15*c15/pow(sigy[i],2);
  
  
     aTvinvrall[i][0][0]=aTvinvrall[i][0][0]+c00*resxprime/pow(sigx[i],2)+c10*resyprime/pow(sigy[i],2);
     aTvinvrall[i][1][0]=aTvinvrall[i][1][0]+c01*resxprime/pow(sigx[i],2)+c11*resyprime/pow(sigy[i],2);
     aTvinvrall[i][2][0]=aTvinvrall[i][2][0]+c02*resxprime/pow(sigx[i],2)+c12*resyprime/pow(sigy[i],2);
     aTvinvrall[i][3][0]=aTvinvrall[i][3][0]+c03*resxprime/pow(sigx[i],2)+c13*resyprime/pow(sigy[i],2);
     aTvinvrall[i][4][0]=aTvinvrall[i][4][0]+c04*resxprime/pow(sigx[i],2)+c14*resyprime/pow(sigy[i],2);
     aTvinvrall[i][5][0]=aTvinvrall[i][5][0]+c05*resxprime/pow(sigx[i],2)+c15*resyprime/pow(sigy[i],2);
  
    }


  
// !!!!!!!!!!ALIGNMENT !!!!!!!!!


  }	// End Loop to exclude plane ii from the track fit
         
  }	// End Loop on random Tracks
  
// !!!!!!!!!!ALIGNMENT !!!!!!!!!



// Fit for Alignment xy-Translations and update xy-Trans

  for(int i=0; i<8; ++i)
  {
  
      if(phase == 0)
      {
          for(int j=0; j<2; ++j)
          {
	  xyaTvinvr(j,0)=aTvinvrxy[i][j][0];
	  
	  for(int k=0; k<2; ++k)
          {
	  xyaTva(j,k)=aTvaxy[i][j][k];
	  }
	  }
  	  xyaTvainv=xyaTva.Invert(&det) ;
	  xyalpar=xyaTvainv*xyaTvinvr;
	  if(i !=0 && i!=7)
//	  if(i !=0 ) 
//	if(i ==3)
	  {
	// update Trans Offsets
	 deltaTx[i]=deltaTx[i]+xyalpar(0,0);
	 deltaTy[i]=deltaTy[i]+xyalpar(1,0);	  
	  
	  }
	  	 cout << "Plane: " << i << endl ;
	 cout << "xTrans: " << xyalpar(0,0) << endl ;
	 cout << "yTrans: " << xyalpar(1,0) << endl ;
	 cout << "Total xTrans: " << deltaTx[i] << endl ;
	 cout << "Total yTrans: " << deltaTy[i] << endl ;
	 
      }
	 
      if(phase == 1)
      {	
      
	if(i==0 || i==7)	//Lock its geometry 
	{		//No free parameters!
	
	
	
       dx=0;
       dy=0;
       dz=0;

       dalf=0;
       dbet=0;
       dgam=0;

	
	
	
	}
      
      
       
	  
	
	
	
	
	if(i==10)	//enforce delta beta=0 & deltaTx=0 & deltaTy=0 & deltaTz=0
	{		//free parameters are 0,2 only
	
	dueaTva(0,0)=aTvaall[i][0][0];
	dueaTva(0,1)=aTvaall[i][0][2];
	dueaTva(1,0)=aTvaall[i][2][0];
	dueaTva(1,1)=aTvaall[i][2][2];
	
	dueaTvinvr(0,0)=aTvinvrall[i][0][0];
	dueaTvinvr(1,0)=aTvinvrall[i][2][0];
	
        dueaTvainv=dueaTva.Invert(&det) ;
        duealpar=dueaTvainv*dueaTvinvr;
	
	//  delta Inverse Rotation
	deltaRinv(0,0)= cos(0)*cos(duealpar(1,0));
	deltaRinv(0,1)=-cos(duealpar(0,0))*sin(duealpar(1,0))+sin(duealpar(0,0))*sin(0)*cos(duealpar(1,0));
	deltaRinv(0,2)= sin(duealpar(0,0))*sin(duealpar(1,0))+cos(duealpar(0,0))*sin(0)*cos(duealpar(1,0));
	deltaRinv(1,0)= cos(0)*sin(duealpar(1,0));
	deltaRinv(1,1)= cos(duealpar(0,0))*cos(duealpar(1,0))+sin(duealpar(0,0))*sin(0)*sin(duealpar(1,0));
	deltaRinv(1,2)=-sin(duealpar(0,0))*cos(duealpar(1,0))+cos(duealpar(0,0))*sin(0)*sin(duealpar(1,0));
	deltaRinv(2,0)=-sin(0);
	deltaRinv(2,1)= sin(duealpar(0,0))*cos(0);
	deltaRinv(2,2)= cos(duealpar(0,0))*cos(0);       

	Rinv(0,0)=fRinv[i][0][0];
	Rinv(0,1)=fRinv[i][0][1];
	Rinv(0,2)=fRinv[i][0][2];
	Rinv(1,0)=fRinv[i][1][0];
	Rinv(1,1)=fRinv[i][1][1];
	Rinv(1,2)=fRinv[i][1][2];
	Rinv(2,0)=fRinv[i][2][0];
	Rinv(2,1)=fRinv[i][2][1];
	Rinv(2,2)=fRinv[i][2][2];
	
	upRinv=Rinv*deltaRinv;
	
	// update Rot Offsets
		

	fRinv[i][0][0]=upRinv(0,0);
	fRinv[i][0][1]=upRinv(0,1);
	fRinv[i][0][2]=upRinv(0,2);
	fRinv[i][1][0]=upRinv(1,0);
	fRinv[i][1][1]=upRinv(1,1);
	fRinv[i][1][2]=upRinv(1,2);
	fRinv[i][2][0]=upRinv(2,0);
	fRinv[i][2][1]=upRinv(2,1);
	fRinv[i][2][2]=upRinv(2,2);
	
	
	// update Trans Offsets
//	 deltaTx[i]=deltaTx[i]+allalpar(3,0)-fRinv[i][2][0]/fRinv[i][2][2]*allalpar(5,0);
//	 deltaTy[i]=deltaTy[i]+allalpar(4,0)-fRinv[i][2][1]/fRinv[i][2][2]*allalpar(5,0);
//	 deltaTz[i]=deltaTz[i]-allalpar(5,0)/fRinv[i][2][2];	  
//	 fTz[i]=fTz[i]-allalpar(5,0)/fRinv[i][2][2];
	dx=0;
	dy=0;
	dz=0;

	dalf=duealpar(0,0);
	dbet=0;
	dgam=duealpar(1,0);
	
	
	}
	
	if(i==27)	//enforce delta gamma=0 & deltaTx=0 & deltaTy=0 & deltaTz=0
	{		//free parameters are 0,1 only
	
	dueaTva(0,0)=aTvaall[i][0][0];
	dueaTva(0,1)=aTvaall[i][0][1];
	dueaTva(1,0)=aTvaall[i][1][0];
	dueaTva(1,1)=aTvaall[i][1][1];
	
	dueaTvinvr(0,0)=aTvinvrall[i][0][0];
	dueaTvinvr(1,0)=aTvinvrall[i][1][0];
	
        dueaTvainv=dueaTva.Invert(&det) ;
        duealpar=dueaTvainv*dueaTvinvr;
	
	//  delta Inverse Rotation
	deltaRinv(0,0)= cos(duealpar(1,0))*cos(0);
	deltaRinv(0,1)=-cos(duealpar(0,0))*sin(0)+sin(duealpar(0,0))*sin(duealpar(1,0))*cos(0);
	deltaRinv(0,2)= sin(duealpar(0,0))*sin(0)+cos(duealpar(0,0))*sin(duealpar(1,0))*cos(0);
	deltaRinv(1,0)= cos(duealpar(1,0))*sin(0);
	deltaRinv(1,1)= cos(duealpar(0,0))*cos(0)+sin(duealpar(0,0))*sin(duealpar(1,0))*sin(0);
	deltaRinv(1,2)=-sin(duealpar(0,0))*cos(0)+cos(duealpar(0,0))*sin(duealpar(1,0))*sin(0);
	deltaRinv(2,0)=-sin(duealpar(1,0));
	deltaRinv(2,1)= sin(duealpar(0,0))*cos(duealpar(1,0));
	deltaRinv(2,2)= cos(duealpar(0,0))*cos(duealpar(1,0));       

	Rinv(0,0)=fRinv[i][0][0];
	Rinv(0,1)=fRinv[i][0][1];
	Rinv(0,2)=fRinv[i][0][2];
	Rinv(1,0)=fRinv[i][1][0];
	Rinv(1,1)=fRinv[i][1][1];
	Rinv(1,2)=fRinv[i][1][2];
	Rinv(2,0)=fRinv[i][2][0];
	Rinv(2,1)=fRinv[i][2][1];
	Rinv(2,2)=fRinv[i][2][2];
	
	upRinv=Rinv*deltaRinv;
	
	// update Rot Offsets
		

	fRinv[i][0][0]=upRinv(0,0);
	fRinv[i][0][1]=upRinv(0,1);
	fRinv[i][0][2]=upRinv(0,2);
	fRinv[i][1][0]=upRinv(1,0);
	fRinv[i][1][1]=upRinv(1,1);
	fRinv[i][1][2]=upRinv(1,2);
	fRinv[i][2][0]=upRinv(2,0);
	fRinv[i][2][1]=upRinv(2,1);
	fRinv[i][2][2]=upRinv(2,2);
	
	
	// update Trans Offsets
//	 deltaTx[i]=deltaTx[i]+allalpar(3,0)-fRinv[i][2][0]/fRinv[i][2][2]*allalpar(5,0);
//	 deltaTy[i]=deltaTy[i]+allalpar(4,0)-fRinv[i][2][1]/fRinv[i][2][2]*allalpar(5,0);
//	 deltaTz[i]=deltaTz[i]-allalpar(5,0)/fRinv[i][2][2];	  
//	 fTz[i]=fTz[i]-allalpar(5,0)/fRinv[i][2][2];
	dx=0;
	dy=0;
	dz=0;

	dalf=duealpar(0,0);
	dbet=duealpar(1,0);
	dgam=0;
	
	}
	
	
	if(i==27)	//enforce deltaTx=0 & deltaTy=0 & deltaTz=0
	{		//free parameters are 0,1,2
	
	
	for(int j=0; j<3; ++j)

	  {
	treaTvinvr(j,0)=aTvinvrall[i][j][0];
	
	for(int k=0; k<3; ++k)
	{
	treaTva(j,k)=aTvaall[i][j][k];
	}
       }
       treaTvainv=treaTva.Invert(&det) ;
       trealpar=treaTvainv*treaTvinvr;
	  
	
	
	//  delta Inverse Rotation
	deltaRinv(0,0)= cos(trealpar(1,0))*cos(trealpar(2,0));
	deltaRinv(0,1)=-cos(trealpar(0,0))*sin(trealpar(2,0))+sin(trealpar(0,0))*sin(trealpar(1,0))*cos(trealpar(2,0));
	deltaRinv(0,2)= sin(trealpar(0,0))*sin(trealpar(2,0))+cos(trealpar(0,0))*sin(trealpar(1,0))*cos(trealpar(2,0));
	deltaRinv(1,0)= cos(trealpar(1,0))*sin(trealpar(2,0));
	deltaRinv(1,1)= cos(trealpar(0,0))*cos(trealpar(2,0))+sin(trealpar(0,0))*sin(trealpar(1,0))*sin(trealpar(2,0));
	deltaRinv(1,2)=-sin(trealpar(0,0))*cos(trealpar(2,0))+cos(trealpar(0,0))*sin(trealpar(1,0))*sin(trealpar(2,0));
	deltaRinv(2,0)=-sin(trealpar(1,0));
	deltaRinv(2,1)= sin(trealpar(0,0))*cos(trealpar(1,0));
	deltaRinv(2,2)= cos(trealpar(0,0))*cos(trealpar(1,0));       

	Rinv(0,0)=fRinv[i][0][0];
	Rinv(0,1)=fRinv[i][0][1];
	Rinv(0,2)=fRinv[i][0][2];
	Rinv(1,0)=fRinv[i][1][0];
	Rinv(1,1)=fRinv[i][1][1];
	Rinv(1,2)=fRinv[i][1][2];
	Rinv(2,0)=fRinv[i][2][0];
	Rinv(2,1)=fRinv[i][2][1];
	Rinv(2,2)=fRinv[i][2][2];
	
	upRinv=Rinv*deltaRinv;
	
	// update Rot Offsets

		

	fRinv[i][0][0]=upRinv(0,0);
	fRinv[i][0][1]=upRinv(0,1);
	fRinv[i][0][2]=upRinv(0,2);
	fRinv[i][1][0]=upRinv(1,0);
	fRinv[i][1][1]=upRinv(1,1);
	fRinv[i][1][2]=upRinv(1,2);
	fRinv[i][2][0]=upRinv(2,0);
	fRinv[i][2][1]=upRinv(2,1);
	fRinv[i][2][2]=upRinv(2,2);
	
	
//	// update Trans Offsets
//	 deltaTx[i]=deltaTx[i]+allalpar(3,0)-fRinv[i][2][0]/fRinv[i][2][2]*allalpar(5,0);
//	 deltaTy[i]=deltaTy[i]+allalpar(4,0)-fRinv[i][2][1]/fRinv[i][2][2]*allalpar(5,0);
//	 deltaTz[i]=deltaTz[i]-allalpar(5,0)/fRinv[i][2][2];	  
//	 fTz[i]=fTz[i]-allalpar(5,0)/fRinv[i][2][2];

	dx=0;
	dy=0;
	dz=0;
	 
	dalf=trealpar(0,0);
	dbet=trealpar(1,0);
	dgam=trealpar(2,0);
 	

	}
	
	
	if(i==15)	//enforce delta alfa=0 only
	{		//free parameters are 1,2,3,4,5
	
	for(int j=1; j<6; ++j)

	  {
	cinaTvinvr(j-1,0)=aTvinvrall[i][j][0];
	
	for(int k=1; k<6; ++k)
	{
	cinaTva(j-1,k-1)=aTvaall[i][j][k];
	}
       }
       cinaTvainv=cinaTva.Invert(&det) ;
       cinalpar=cinaTvainv*cinaTvinvr;

	//  delta Inverse Rotation
	deltaRinv(0,0)= cos(cinalpar(0,0))*cos(cinalpar(1,0));
	deltaRinv(0,1)=-cos(0)*sin(cinalpar(1,0))+sin(0)*sin(cinalpar(0,0))*cos(cinalpar(1,0));
	deltaRinv(0,2)= sin(0)*sin(cinalpar(1,0))+cos(0)*sin(cinalpar(0,0))*cos(cinalpar(1,0));
	deltaRinv(1,0)= cos(cinalpar(0,0))*sin(cinalpar(1,0));
	deltaRinv(1,1)= cos(0)*cos(cinalpar(1,0))+sin(0)*sin(cinalpar(0,0))*sin(cinalpar(1,0));
	deltaRinv(1,2)=-sin(0)*cos(cinalpar(1,0))+cos(0)*sin(cinalpar(0,0))*sin(cinalpar(1,0));
	deltaRinv(2,0)=-sin(cinalpar(0,0));
	deltaRinv(2,1)= sin(0)*cos(cinalpar(0,0));
	deltaRinv(2,2)= cos(0)*cos(cinalpar(0,0));       

	Rinv(0,0)=fRinv[i][0][0];
	Rinv(0,1)=fRinv[i][0][1];
	Rinv(0,2)=fRinv[i][0][2];
	Rinv(1,0)=fRinv[i][1][0];
	Rinv(1,1)=fRinv[i][1][1];
	Rinv(1,2)=fRinv[i][1][2];
	Rinv(2,0)=fRinv[i][2][0];
	Rinv(2,1)=fRinv[i][2][1];
	Rinv(2,2)=fRinv[i][2][2];
	
	upRinv=Rinv*deltaRinv;

	// update Rot Offsets
		

	fRinv[i][0][0]=upRinv(0,0);
	fRinv[i][0][1]=upRinv(0,1);
	fRinv[i][0][2]=upRinv(0,2);
	fRinv[i][1][0]=upRinv(1,0);
	fRinv[i][1][1]=upRinv(1,1);
	fRinv[i][1][2]=upRinv(1,2);
	fRinv[i][2][0]=upRinv(2,0);
	fRinv[i][2][1]=upRinv(2,1);
	fRinv[i][2][2]=upRinv(2,2);
	
	
	// update Trans Offsets
	  

       deltaTx[i]=deltaTx[i]+cinalpar(2,0)-fRinv[i][2][0]/fRinv[i][2][2]*cinalpar(4,0);
       deltaTy[i]=deltaTy[i]+cinalpar(3,0)-fRinv[i][2][1]/fRinv[i][2][2]*cinalpar(4,0);
       deltaTz[i]=deltaTz[i]-cinalpar(4,0)/fRinv[i][2][2];	
       fTz[i]=fTz[i]-cinalpar(4,0)/fRinv[i][2][2];

	dx=+cinalpar(2,0)-fRinv[i][2][0]/fRinv[i][2][2]*cinalpar(4,0);
	dy=+cinalpar(3,0)-fRinv[i][2][1]/fRinv[i][2][2]*cinalpar(4,0);
	dz=-cinalpar(4,0)/fRinv[i][2][2];        

       
       	dalf=0;
	dbet=cinalpar(0,0);
	dgam=cinalpar(1,0);



	}
	
	
	if(i!=0 &&  i!= 7)	//all free parameters
	{					
	
	
	
	for(int j=0; j<6; ++j)

	  {
	allaTvinvr(j,0)=aTvinvrall[i][j][0];
	
	for(int k=0; k<6; ++k)
	{
	allaTva(j,k)=aTvaall[i][j][k];
	}
       }
       allaTvainv=allaTva.Invert(&det) ;
       allalpar=allaTvainv*allaTvinvr;
	  
	
	
	//  delta Inverse Rotation
	deltaRinv(0,0)= cos(allalpar(1,0))*cos(allalpar(2,0));
	deltaRinv(0,1)=-cos(allalpar(0,0))*sin(allalpar(2,0))+sin(allalpar(0,0))*sin(allalpar(1,0))*cos(allalpar(2,0));
	deltaRinv(0,2)= sin(allalpar(0,0))*sin(allalpar(2,0))+cos(allalpar(0,0))*sin(allalpar(1,0))*cos(allalpar(2,0));
	deltaRinv(1,0)= cos(allalpar(1,0))*sin(allalpar(2,0));
	deltaRinv(1,1)= cos(allalpar(0,0))*cos(allalpar(2,0))+sin(allalpar(0,0))*sin(allalpar(1,0))*sin(allalpar(2,0));
	deltaRinv(1,2)=-sin(allalpar(0,0))*cos(allalpar(2,0))+cos(allalpar(0,0))*sin(allalpar(1,0))*sin(allalpar(2,0));
	deltaRinv(2,0)=-sin(allalpar(1,0));
	deltaRinv(2,1)= sin(allalpar(0,0))*cos(allalpar(1,0));
	deltaRinv(2,2)= cos(allalpar(0,0))*cos(allalpar(1,0));       

	Rinv(0,0)=fRinv[i][0][0];
	Rinv(0,1)=fRinv[i][0][1];
	Rinv(0,2)=fRinv[i][0][2];
	Rinv(1,0)=fRinv[i][1][0];
	Rinv(1,1)=fRinv[i][1][1];
	Rinv(1,2)=fRinv[i][1][2];
	Rinv(2,0)=fRinv[i][2][0];
	Rinv(2,1)=fRinv[i][2][1];
	Rinv(2,2)=fRinv[i][2][2];
	
	upRinv=Rinv*deltaRinv;
	
	// update Rot Offsets

		

	fRinv[i][0][0]=upRinv(0,0);
	fRinv[i][0][1]=upRinv(0,1);
	fRinv[i][0][2]=upRinv(0,2);
	fRinv[i][1][0]=upRinv(1,0);
	fRinv[i][1][1]=upRinv(1,1);
	fRinv[i][1][2]=upRinv(1,2);
	fRinv[i][2][0]=upRinv(2,0);
	fRinv[i][2][1]=upRinv(2,1);
	fRinv[i][2][2]=upRinv(2,2);
	
	
	// update Trans Offsets
	 deltaTx[i]=deltaTx[i]+allalpar(3,0)-fRinv[i][2][0]/fRinv[i][2][2]*allalpar(5,0);
	 deltaTy[i]=deltaTy[i]+allalpar(4,0)-fRinv[i][2][1]/fRinv[i][2][2]*allalpar(5,0);
	 deltaTz[i]=deltaTz[i]-allalpar(5,0)/fRinv[i][2][2];	  
	 fTz[i]=fTz[i]-allalpar(5,0)/fRinv[i][2][2];

	dx=+allalpar(3,0)-fRinv[i][2][0]/fRinv[i][2][2]*allalpar(5,0);
	dy=+allalpar(4,0)-fRinv[i][2][1]/fRinv[i][2][2]*allalpar(5,0);
	dz=-allalpar(5,0)/fRinv[i][2][2];      
	 
	dalf=allalpar(0,0);
	dbet=allalpar(1,0);
	dgam=allalpar(2,0);
 
	
	
	}
	

//	  if(i !=0 && i!=7)
//	  if(i !=0 )
//	  if(i == 1)
//	   {
//	   
//	   
//	 // update Rot Offsets
//
//		 
//
//	 fRinv[i][0][0]=upRinv(0,0);
//	 fRinv[i][0][1]=upRinv(0,1);
//	  fRinv[i][0][2]=upRinv(0,2);
//	  fRinv[i][1][0]=upRinv(1,0);
//	  fRinv[i][1][1]=upRinv(1,1);
//	  fRinv[i][1][2]=upRinv(1,2);
//	  fRinv[i][2][0]=upRinv(2,0);
//	  fRinv[i][2][1]=upRinv(2,1);
//	  fRinv[i][2][2]=upRinv(2,2);
//
//
//	  // update Trans Offsets
//	   deltaTx[i]=deltaTx[i]+allalpar(3,0)-fRinv[i][2][0]/fRinv[i][2][2]*allalpar(5,0);
//	   deltaTy[i]=deltaTy[i]+allalpar(4,0)-fRinv[i][2][1]/fRinv[i][2][2]*allalpar(5,0);
//	   deltaTz[i]=deltaTz[i]-allalpar(5,0)/fRinv[i][2][2];      
//	   fTz[i]=fTz[i]-allalpar(5,0)/fRinv[i][2][2];
//	    


//	fRinv[i][0][0]= cos(fbe[i]+rotalpar(1,0))*cos(fga[i]+rotalpar(2,0));
//	fRinv[i][0][1]=-cos(fal[i]+rotalpar(0,0))*sin(fga[i]+rotalpar(2,0))+sin(fal[i]+rotalpar(0,0))*sin(fbe[i]+rotalpar(1,0))*cos(fga[i]+rotalpar(2,0));
//	fRinv[i][0][2]= sin(fal[i]+rotalpar(0,0))*sin(fga[i]+rotalpar(2,0))+cos(fal[i]+rotalpar(0,0))*sin(fbe[i]+rotalpar(1,0))*cos(fga[i]+rotalpar(2,0));
//	fRinv[i][1][0]= cos(fbe[i]+rotalpar(1,0))*sin(fga[i]+rotalpar(2,0));
//	fRinv[i][1][1]= cos(fal[i]+rotalpar(0,0))*cos(fga[i]+rotalpar(2,0))+sin(fal[i]+rotalpar(0,0))*sin(fbe[i]+rotalpar(1,0))*sin(fga[i]+rotalpar(2,0));
//	fRinv[i][1][2]=-sin(fal[i]+rotalpar(0,0))*cos(fga[i]+rotalpar(2,0))+cos(fal[i]+rotalpar(0,0))*sin(fbe[i]+rotalpar(1,0))*sin(fga[i]+rotalpar(2,0));
//	fRinv[i][2][0]=-sin(fbe[i]+rotalpar(1,0));
//	fRinv[i][2][1]= sin(fal[i]+rotalpar(0,0))*cos(fbe[i]+rotalpar(1,0));
//	fRinv[i][2][2]= cos(fal[i]+rotalpar(0,0))*cos(fbe[i]+rotalpar(1,0));       
	
			
	
		  
	  
//	  }
	  
	  alpha[i]=alpha[i]+dalf;
	  beta[i]=beta[i]+dbet;
	  gamma[i]=gamma[i]+dgam;
	  
	  	 cout << "Plane: " << i << endl ;
	 cout << "alpha: " << dalf << endl ;
	 cout << "beta: " << dbet << endl ;
	 cout << "gamma: " << dgam << endl ;
	 cout << "Total alpha: " << alpha[i] << endl ;
	 cout << "Total beta : " << beta[i] << endl ;
	 cout << "Total gamma: " << gamma[i] << endl ;
	 
	 cout << "xTrans: " << dx << endl ;
	 cout << "yTrans: " << dy << endl ;
	 cout << "zTrans: " << dz << endl ;
	 cout << "Total xTrans: " << deltaTx[i] << endl ;
	 cout << "Total yTrans: " << deltaTy[i] << endl ;	 
	 cout << "Total zTrans: " << deltaTz[i] << endl ;
	 
       if(ntrial == maxtrial-1)
     {
     
     	 dalpha=(asin(upRinv(2,1)/sqrt(1-upRinv(2,0)*upRinv(2,0)))-ral[i])/M_PI*180;
	 dbeta=(asin(-upRinv(2,0))-rbe[i])/M_PI*180;
	 dgamma=(asin(upRinv(1,0)/sqrt(1-upRinv(2,0)*upRinv(2,0)))-rga[i])/M_PI*180;
	 cout << "Plane: " << i << endl ;
	 
	 cout <<  "Delta alpha:"<< dalpha  << endl ;
	 cout <<  "Delta beta :"<< dbeta   << endl ;
	 cout <<  "Delta gamma:"<< dgamma  << endl ;
	 
	 cout <<  "rRinv-fRinv:"<<(rRinv[i][0][0]-upRinv(0,0))<< endl ;
     	 cout <<  "rRinv-fRinv:"<<(rRinv[i][0][1]-upRinv(0,1))<< endl ;
     	 cout <<  "rRinv-fRinv:"<<(rRinv[i][0][2]-upRinv(0,2))<< endl ;
     	 cout <<  "rRinv-fRinv:"<<(rRinv[i][1][0]-upRinv(1,0))<< endl ;
     	 cout <<  "rRinv-fRinv:"<<(rRinv[i][1][1]-upRinv(1,1))<< endl ;
     	 cout <<  "rRinv-fRinv:"<<(rRinv[i][1][2]-upRinv(1,2))<< endl ;
     	 cout <<  "rRinv-fRinv:"<<(rRinv[i][2][0]-upRinv(2,0))<< endl ;
     	 cout <<  "rRinv-fRinv:"<<(rRinv[i][2][1]-upRinv(2,1))<< endl ;
     	 cout <<  "rRinv-fRinv:"<<(rRinv[i][2][2]-upRinv(2,2))<< endl ;
	 
	 cout << "Delta xTr: " << deltaTx[i]-rTx[i] << endl ;
	 cout << "Delta yTr: " << deltaTy[i]-rTy[i] << endl ;
	 cout << "Delta zTr: " <<     fTz[i]-rTz[i] << endl ;
	 
     
     
     }
	 

	 
      } 	 
	 		 
//	   if(i == 1) { 	 
//	  //  delta Inverse Rotation
//	  deltaRinv(0,0)= cos(alpar(1,0))*cos(alpar(2,0));
//	  deltaRinv(0,1)=-cos(alpar(0,0))*sin(alpar(2,0))+sin(alpar(0,0))*sin(alpar(1,0))*cos(alpar(2,0));
//	  deltaRinv(0,2)= sin(alpar(0,0))*sin(alpar(2,0))+cos(alpar(0,0))*sin(alpar(1,0))*cos(alpar(2,0));
//	  deltaRinv(1,0)= cos(alpar(1,0))*sin(alpar(2,0));
//	  deltaRinv(1,1)= cos(alpar(0,0))*cos(alpar(2,0))+sin(alpar(0,0))*sin(alpar(1,0))*sin(alpar(2,0));
//	  deltaRinv(1,2)=-sin(alpar(0,0))*cos(alpar(2,0))+cos(alpar(0,0))*sin(alpar(1,0))*sin(alpar(2,0));
//	  deltaRinv(2,0)=-sin(alpar(1,0));
//	  deltaRinv(2,1)= sin(alpar(0,0))*cos(alpar(1,0));
//	  deltaRinv(2,2)= cos(alpar(0,0))*cos(alpar(1,0));	 
//
//	  Rinv(0,0)=fRinv[i][0][0];
//	  Rinv(0,1)=fRinv[i][0][1];
//	  Rinv(0,2)=fRinv[i][0][2];
//	  Rinv(1,0)=fRinv[i][1][0];
//	  Rinv(1,1)=fRinv[i][1][1];
//	  Rinv(1,2)=fRinv[i][1][2];
//	  Rinv(2,0)=fRinv[i][2][0];
//	  Rinv(2,1)=fRinv[i][2][1];
//	  Rinv(2,2)=fRinv[i][2][2];
//	  
//	  upRinv=Rinv*deltaRinv;
//	  
//	  fRinv[i][0][0]=upRinv(0,0);
//	  fRinv[i][0][1]=upRinv(0,1);
//	  fRinv[i][0][2]=upRinv(0,2);
//	  fRinv[i][1][0]=upRinv(1,0);
//	  fRinv[i][1][1]=upRinv(1,1);
//	  fRinv[i][1][2]=upRinv(1,2);
//	  fRinv[i][2][0]=upRinv(2,0);
//	  fRinv[i][2][1]=upRinv(2,1);
//	  fRinv[i][2][2]=upRinv(2,2);
//	  
//	  // update Trans Offsets
//	  deltaTx[i]=deltaTx[i]+alpar(3,0)-alpar(5,0)*upRinv(2,0)/upRinv(2,2);
//	   deltaTy[i]=deltaTy[i]+alpar(4,0)-alpar(5,0)*upRinv(2,1)/upRinv(2,2);
//	   fTz[i]=fTz[i]-alpar(5,0)/upRinv(2,2);
//	   
//	   }
//	   
//	   cout << "Plane: " << i << endl ;
//	   cout << "alpha: " << alpar(0,0) << endl ;
//	   cout << "beta: " << alpar(1,0) << endl ;
//	   cout << "gamma: " << alpar(2,0) << endl ;
//	   cout << "xTrans: " << alpar(3,0) << endl ;
//	   cout << "yTrans: " << alpar(4,0) << endl ;
//	   cout << "zTrans: " << alpar(5,0) << endl ;
// 
// 
//	  if(i==7){
//	  aTvainv7=aTva7.Invert(&det) ;
//	  alpar7=aTvainv7*aTvinvr7;
//	  cout << "Plane: " << i << endl ;
//	  cout << "alpha: " << alpar7(0,0) << endl ;
//	  cout << "beta: " << alpar7(1,0) << endl ;
//	  cout << "gamma: " << alpar7(2,0) << endl ;
//	  
//	  
//	  }





  }
  
// back to 263 Track Loop if ntrial < 10

	ntrial=ntrial+1; 

	if(ntrial < maxtrial)	goto retry;
  	ntrial=0;
	phase=phase+1;
	if(phase < 2) goto retry;
	
	cout << "sumchi2: " <<     sumchi2 << endl ;
  
// !!!!!!!!!!ALIGNMENT !!!!!!!!!

// std::stringstream stringu ;
// std::vector<TCanvas*>  canvasCorrv ;  
// for(int j=0; j<8; ++j)
// {
//  stringu.str("") ; stringu << "canvasCorr" << j ;
//  canvasCorrv.push_back( new TCanvas(stringu.str(),stringu.str() ) ) ;
//  for(int i=0; i<8; ++i)
//  {
//   canvasCorrv[j]->cd(i+1) ;
//   h2[i]->Draw();
//  } 
//   }
   
   
 
  TCanvas * canvasCorr = new TCanvas("corr","corr") ;
  TCanvas * canvasRes  = new TCanvas("res", "res") ;
 
	 canvasCorr->Divide(16,4) ;
 
	 for(int i=0; i<64; ++i)
	 {
	  canvasCorr->cd(i+1) ;
	  h2[i]->Draw();
	 }
 
  canvasRes->Divide(8,4) ;
  
  for(int i=0; i<32; ++i)
  {
   canvasRes->cd(i+1) ;
   h1[i]->Draw();
  }
  
//   TMatrixD A(4,3) ;
//   TMatrixD B(3,1) ;
//   TMatrixD AT(2,2) ;
//   
//   A(0,0) =  7814;
//   A(0,1) =  1290;
//   A(0,2) =  0   ;
//   A(1,0) =  1290;
//   A(1,1) =  3482;
//   A(1,2) =  0 	;
//   A(2,0) =  0 	;
//   A(2,1) =  0 	;
//   A(2,2) =  7629;
//   A(3,0) =  0   ;
//   A(3,1) =  0   ;
//   A(3,2) =  1237;
//   
//   B(0,0) = 1;
//   B(1,0) = 2;
//   B(2,0) = 3;
//  
//   A.Print(); 
//   
//   B.Print() ;
//   
//   //-------------------------
// 
//   TMatrixD C(4,1) ;
//   
//   C.Mult(A,B) ;
//   
//   C.Print() ;
//   
//   TMatrixD D(4,1) ;
//   
//   D = A*B ;
//   
//   D.Print() ;
//   
//   //-------------------------
//   
//   TMatrixD AT1(2,2) ;
// 
//   AT(0,0) = 1. ;
//   AT(1,0) = 2. ;
//   AT(0,1) = 3. ;
//   AT(1,1) = 4. ;
//   
//   double det = 0 ;
//   
//   AT1 = AT.Invert(&det) ;
//   
//   AT1.Print() ;
//   
//   cout << "Determinant: " << det << endl ;
}

