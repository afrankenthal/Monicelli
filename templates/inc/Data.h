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
 
#ifndef DataClass_h
#define DataClass_h

class TTree;

class Data
{
 public:
  Data();
  ~Data();
  void branch(TTree* tree);
  void setBranchAddress(TTree* tree);

  void  setEvtId   (int   v)	   {evtId_	= v;}
  void  setNtrack  (int   v)	   {ntrack_	= v;}
  void  setItrack  (int   v)	   {itrack_	= v;}
  void  setNhits   (int   v)	   {nhits_	= v;}
  void  setNgoodx  (int   v)	   {ngoodx_	= v;}
  void  setNgoody  (int   v)	   {ngoody_	= v;}
  void  setNdof    (int   v)	   {ndof_	= v;}
  void  setChi2    (float v)	   {chi2_	= v;}
  void  setProb    (float v)	   {prob_	= v;}
  void  setXproj0  (float v)	   {xproj0_	= v;}
  void  setYproj0  (float v)	   {yproj0_	= v;}
  void  setXprojErr(float v)	   {xprojErr_	= v;}
  void  setYprojErr(float v)	   {yprojErr_	= v;}
  void  setXslope  (float v)	   {xslope_	= v;}
  void  setYslope  (float v)	   {yslope_	= v;}
  void  setHasHit  (bool  v, int p){hasHit_[p]  = v;}
  void  setMeasureX(bool  v, int p){measureX_[p]= v;}
  void  setNpix    (int   v, int p){npix_[p]	= v;}
  void  setNcol    (int   v, int p){ncol_[p]	= v;}
  void  setNrow    (int   v, int p){nrow_[p]	= v;}
  void  setCharge  (float v, int p){charge_[p]  = v;}
  void  setCol     (float v, int p){col_[p]	= v;}
  void  setRow     (float v, int p){row_[p]	= v;}
  void  setXp	   (float v, int p){xp_[p]	= v;}
  void  setYp	   (float v, int p){yp_[p]	= v;}
  void  setXl	   (float v, int p){xl_[p]	= v;}
  void  setYl	   (float v, int p){yl_[p]	= v;}
  void  setXpErr   (float v, int p){xpErr_[p]	= v;}
  void  setYpErr   (float v, int p){ypErr_[p]	= v;}
  void  setNdofExcl(int   v, int p){ndofExcl_[p]= v;}
  void  setChi2Excl(float v, int p){chi2Excl_[p]= v;}
  void  setProbExcl(float v, int p){probExcl_[p]= v;}
  void  setXresid  (float v, int p){xresid_[p]  = v;}
  void  setYresid  (float v, int p){yresid_[p]  = v;}
  void  setXtErr   (float v, int p){xtErr_[p]	= v;}
  void  setYtErr   (float v, int p){ytErr_[p]	= v;}
  
  void  setBelongsToTrack(bool v, int p){belongsToTrack_[p] = v;}
 
  int	getEvtId   (void) const {return evtId_      ;}
  int	getNtrack  (void) const {return ntrack_     ;}
  int	getItrack  (void) const {return itrack_     ;}
  int	getNhits   (void) const {return nhits_      ;}
  int	getNgoodx  (void) const {return ngoodx_     ;}
  int	getNgoody  (void) const {return ngoody_     ;}
  int	getNdof    (void) const {return ndof_	    ;}
  float getChi2    (void) const {return chi2_	    ;}
  float getProb    (void) const {return prob_	    ;}
  float getXproj0  (void) const {return xproj0_     ;}
  float getYproj0  (void) const {return yproj0_     ;}
  float getXprojErr(void) const {return xprojErr_   ;}
  float getYprojErr(void) const {return yprojErr_   ;}
  float getXslope  (void) const {return xslope_     ;}
  float getYslope  (void) const {return yslope_     ;}
  bool  getHasHit  (int p)const {return hasHit_[p]  ;}
  bool  getMeasureX(int p)const {return measureX_[p];}
  int	getNpix    (int p)const {return npix_[p]    ;}
  int	getNcol    (int p)const {return ncol_[p]    ;}
  int	getNrow    (int p)const {return nrow_[p]    ;}
  float getCharge  (int p)const {return charge_[p]  ;}
  float getCol     (int p)const {return col_[p]     ;}
  float getRow     (int p)const {return row_[p]     ;}
  float getXp	   (int p)const {return xp_[p]      ;}
  float getYp	   (int p)const {return yp_[p]      ;}
  float getXl	   (int p)const {return xl_[p]      ;}
  float getYl	   (int p)const {return yl_[p]      ;}
  float getXpErr   (int p)const {return xpErr_[p]   ;}
  float getYpErr   (int p)const {return ypErr_[p]   ;}
  int   getNdofExcl(int p)const {return ndofExcl_[p];}
  float getChi2Excl(int p)const {return chi2Excl_[p];}
  float getProbExcl(int p)const {return probExcl_[p];}
  float getXresid  (int p)const {return xresid_[p]  ;}
  float getYresid  (int p)const {return yresid_[p]  ;}
  float getXtErr   (int p)const {return xtErr_[p]   ;}
  float getYtErr   (int p)const {return ytErr_[p]   ;}
  
  bool getBelongsToTrack(int p)const {return belongsToTrack_[p];}

		
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
  
  bool belongsToTrack_[size];
};

#endif

