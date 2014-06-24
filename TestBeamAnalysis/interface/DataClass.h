#ifndef DataClass_h
#define DataClass_h

class TTree;

class DataClass
{
 public:
  DataClass();
  ~DataClass();
  void branch(TTree* tree);
  void setBranchAddress(TTree* tree);

  const int&   getEvtId   (void) const {return evtId_      ;}
  const int&   getNtrack  (void) const {return ntrack_     ;}
  const int&   getItrack  (void) const {return itrack_     ;}
  const int&   getNhits   (void) const {return nhits_      ;}
  const int&   getNgoodx  (void) const {return ngoodx_     ;}
  const int&   getNgoody  (void) const {return ngoody_     ;}
  const int&   getNdof    (void) const {return ndof_       ;}
  const float& getChi2    (void) const {return chi2_       ;}
  const float& getProb    (void) const {return prob_       ;}
  const float& getXproj0  (void) const {return xproj0_     ;}
  const float& getYproj0  (void) const {return yproj0_     ;}
  const float& getXprojErr(void) const {return xprojErr_   ;}
  const float& getYprojErr(void) const {return yprojErr_   ;}
  const float& getXslope  (void) const {return xslope_     ;}
  const float& getYslope  (void) const {return yslope_     ;}
  const bool&  getHasHit  (int p)const {return hasHit_[p]  ;}
  const bool&  getMeasureX(int p)const {return measureX_[p];}
  const int&   getNpix    (int p)const {return npix_[p]    ;}
  const int&   getNcol    (int p)const {return ncol_[p]    ;}
  const int&   getNrow    (int p)const {return nrow_[p]    ;}
  const float& getCharge  (int p)const {return charge_[p]  ;}
  const float& getCol     (int p)const {return col_[p]     ;}
  const float& getRow     (int p)const {return row_[p]     ;}
  const float& getXp      (int p)const {return xp_[p]      ;}
  const float& getYp      (int p)const {return yp_[p]      ;}
  const float& getXl      (int p)const {return xl_[p]      ;}
  const float& getYl      (int p)const {return yl_[p]      ;}
  const float& getXpErr   (int p)const {return xpErr_[p]   ;}
  const float& getYpErr   (int p)const {return ypErr_[p]   ;}
  const int&   getNdofExcl(int p)const {return ndofExcl_[p];}
  const float& getChi2Excl(int p)const {return chi2Excl_[p];}
  const float& getProbExcl(int p)const {return probExcl_[p];}
  const float& getXresid  (int p)const {return xresid_[p]  ;}
  const float& getYresid  (int p)const {return yresid_[p]  ;}
  const float& getXtErr   (int p)const {return xtErr_[p]   ;}
  const float& getYtErr   (int p)const {return ytErr_[p]   ;}
  
  bool  getBelongsToTrack(int p)const {return belongsToTrack_[p];}
  //float getXresidCalc    (int p)const {return xresidCalc_[p]    ;}
  //float getYresidCalc    (int p)const {return yresidCalc_[p]    ;}
 
  const float& getMaxCol  (int p, TTree* tree) ;
  const float& getMaxRow  (int p, TTree* tree) ;
  		
 private:
  enum {size = 10};	
   
  int   evtId_;
  int   ntrack_;
  int   itrack_;
  int   nhits_;
  int   ngoodx_;
  int   ngoody_;
  int   ndof_;
  float chi2_;
  float prob_;
  float xproj0_;
  float yproj0_;
  float xprojErr_;
  float yprojErr_;
  float xslope_;
  float yslope_;
  bool  hasHit_  [size];
  bool  measureX_[size];
  int   npix_    [size];
  int   ncol_    [size];
  int   nrow_    [size];
  float charge_  [size];
  float col_     [size];
  float row_     [size];
  float xp_      [size];
  float yp_      [size];
  float xl_      [size];
  float yl_      [size];
  float xpErr_   [size];
  float ypErr_   [size];
  int   ndofExcl_[size];
  float chi2Excl_[size];
  float probExcl_[size];
  float xresid_  [size];
  float yresid_  [size];
  float xtErr_   [size];
  float ytErr_   [size];
  
  bool  belongsToTrack_[size];
  //float xresidCalc_    [size];
  //float yresidCalc_    [size];
  
  float MaxCol_ ;
  float MaxRow_ ;
};

#endif

