// g++ -o smatrix smatrix.cpp `root-config --cflags --libs` && ./smatrix

#include <iostream>

#include <iomanip>
#include <map>
#include <ostream>
#include <sstream>
#include <string>

#include <Math/SMatrix.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TRandom.h>

#define DIM 4

typedef ROOT::Math::SMatrix<double,DIM,DIM> matrixDef ;
typedef std::vector<matrixDef> matrixVectorDef ;

std::vector<int>    failures_ ;
std::vector<double> scale_    ;

std::map<int,TH1D*> m_ ;
std::map<int,TH1D*> i_ ;

TH1D * d_ ;

void doIt() ;

void print(            std::string sm, 
                       matrixDef & matrix) ;
void computeDivergence(std::string type,
                       int bin,
                       matrixDef & matrix1,
                       matrixDef & matrix2) ;

int main(int argc, char** argv)
{
  TApplication app("App",&argc, argv);
 
  doIt() ;
 
  app.Run() ;
}

//==================================================================
void doIt() 
{
 std::stringstream ss ;
 std::stringstream sh ;

 TRandom * r = new TRandom() ;

 matrixVectorDef matrices ;
 matrixVectorDef inverses ;
 
 double scale     = 100.   ;
 double precision = scale / 10. ;
 int    nx        = 5 ;
 int    ny        = 4 ;
 int    steps     = nx * ny ;
 double parallel  = 1 ;

 matrixDef  A ; 
 matrixDef IA ; 
 
 TCanvas * canvasM = new TCanvas("cM","cM") ;
 TCanvas * canvasI = new TCanvas("cI","cI") ;
 TCanvas * canvasD = new TCanvas("cD","cD") ;
 
 canvasM->Divide(nx,ny) ;
 canvasI->Divide(nx,ny) ;

 d_ = new TH1D("Determinant","Determinant",steps,0,steps) ;

 double s = scale ; 
 for(int r=0; r<DIM; ++r)
 {
  for(int c=0; c<DIM; ++c)
  {
    s /= 10 ;
    int rc = r*10 + c ;
    ss.str("") ; ss << "M r: " << r << " c: " << c << " scale: " << s  ;
    m_[rc] = new TH1D(ss.str().c_str(),ss.str().c_str(),steps,0,steps) ;
    ss.str("") ; ss << "I r: " << r << " c: " << c << " scale: " << s  ;
    i_[rc] = new TH1D(ss.str().c_str(),ss.str().c_str(),steps,0,steps) ;
  }
 }

 steps = 2 ;
 int CONV = 1 ;
 
 for( int i=0; i<steps; ++i)
 {
  if( i == 1 ) CONV = 1000 ;

  scale /= 10 ; precision = scale / 10. ;

//         781434255.698529        12900843.8728485                       0                       0
//         12900843.8728485        348261.188226009                       0                       0
//                        0                       0         762943044.40029        12377625.1183082
//                        0                       0        12377625.1183082        334130.592973887


  A(0,0) =  781434255.698529*CONV ;
  A(0,1) =  12900843.8728485*CONV ;
  A(0,2) =  0 ;
  A(0,3) =  0 ;
  A(1,0) =  12900843.8728485*CONV ;
  A(1,1) =  348261.188226009*CONV ;
  A(1,2) =  0 ;
  A(1,3) =  0 ;
  A(2,0) =  0 ;
  A(2,1) =  0 ;
  A(2,2) =  762943044.40029 *CONV ;
  A(2,3) =  12377625.1183082*CONV ;
  A(3,0) =  0 ;
  A(3,1) =  0 ;
  A(3,2) =  12377625.1183082*CONV ;
  A(3,3) =  334130.592973887*CONV ;
 
  int failure = -23 ;
 
  IA = A.Inverse(failure) ; std::cout << std::endl ;

  double determinant ;
  bool notSingular = A.Det2(determinant) ;
  
  d_->SetBinContent(i,fabs(determinant)) ;

  std::cout << std::endl ;
  std::cout << __LINE__ 
            << "]\t=============================================================\n"
            << "Iteration: " 
            << i
            << " Scale: "
            << std::setprecision(15)  
            << scale  
            << " Precision: " 
            << precision
            << " Determinant: "
            << determinant
            << " Failure: " 
            << failure 
            << std::endl ;

  print("Matrix  A ", A ) ;
  print("Inverse IA", IA) ;
  
  failures_.push_back(failure) ;
 
  matrices.push_back(A) ;
  inverses.push_back(IA) ;

 }

 canvasD->cd() ;
 gPad->SetLogy(1) ;
 d_->Draw() ;
 
 for(int i=0; i<(int)matrices.size()-1; ++i)
 {
  std::cout << "\f===========================================================================================================" << std::endl ;
  ss.str("") ; ss << "Matrix "  << i ;
  print(ss.str(), matrices[i]) ;
  ss.str("") ; ss << "Matrix "  << i+1 ;
  print(ss.str(), matrices[i+1]) ;
  ss.str("") ; ss << "Inverse " << i ;
  print(ss.str(), inverses[i]) ;
  ss.str("") ; ss << "Inverse " << i+1 ;
  print(ss.str(), inverses[i+1]) ;
  std::cout << "Failure: " << failures_[i] << std::endl ;
  computeDivergence("Matrix",  i,matrices[i],matrices[i+1]) ;
  computeDivergence("Inverse", i,inverses[i],inverses[i+1]) ;
 }

 int pad = 1 ;
 for(int r=0; r<DIM; ++r)
 {
  for(int c=0; c<DIM; ++c)
  {
   int rc = r*10 + c ;
   canvasM->cd(pad++) ;
   gPad->SetLogy(1) ;
   m_[rc]->Draw() ;
  }
 }

 pad = 1 ;
 for(int r=0; r<DIM; ++r)
 {
  for(int c=0; c<DIM; ++c)
  {
   int rc = r*10 + c ;
   canvasI->cd(pad++) ;
   gPad->SetLogy(1) ;
   i_[rc]->Draw() ;
  }
 }
}

//==================================================================
void print(std::string sm, ROOT::Math::SMatrix<double,DIM,DIM> & matrix)
{
  std::cout << sm << std::endl ;
  for(int r=0; r<DIM; ++r)
  {
   for(int c=0; c<DIM; ++c)
   {
     std::cout << std::setw(24) << std::setprecision(15) << matrix(r,c) ;
   }
   std::cout << std::endl ;
  }
  std::cout << std::endl ;
}

//==================================================================
void computeDivergence(std::string type,
                       int bin,
                       ROOT::Math::SMatrix<double,DIM,DIM> & matrix1,
                       ROOT::Math::SMatrix<double,DIM,DIM> & matrix2)
{
  std::cout << "Type: " << type << " at bin " << bin << std::endl ;
  std::cout << "                  Matrix[" << bin << "]              Matrix[" 
            << bin + 1
            << "]             Percentage"
            << std::endl ;
  for(int r=0; r<DIM; ++r)
  {
   for(int c=0; c<DIM; ++c)
   {
     double percent = ( matrix1(r,c) - matrix2(r,c) ) / matrix1(r,c) ;
     std::cout << " r: " << r 
               << " c: " << c 
               << " " 
               << std::setw(22) << std::setprecision(15) << std::scientific
               << matrix1(r,c)  << " "
               << std::setw(22) << std::setprecision(15) << std::scientific
               << matrix2(r,c)  << " "
               << std::setw(22) << std::setprecision(15) << std::scientific
               << percent 
               << std::endl ;
    int rc = r*10 + c ;
    if( type == std::string("Matrix" ) ) m_[rc]->SetBinContent(bin+1,fabs(percent+.1)) ;
    if( type == std::string("Inverse") ) i_[rc]->SetBinContent(bin+1,fabs(percent+.1)) ;
   }
  }
  std::cout << std::endl ;
}                       
