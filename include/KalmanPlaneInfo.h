#ifndef KALMANPLANEINFO_H
#define KALMANPLANEINFO_H

#include <map>
#include <vector>
#include <string>
#include <TVectorT.h>
#include <TMatrixTSym.h>
//#include <TObject.h>
//#include <utility>

class KalmanPlaneInfo// : public TObject
{
public:

    KalmanPlaneInfo(void);
   ~KalmanPlaneInfo(void);

    void                                                   setPosPlane	       (unsigned int                                 posPlane	      	                 ) {posPlane_	       = posPlane	       ;}
    void                                                   setNegPlane	       (unsigned int                                 negPlane		  	                 ) {negPlane_	       = negPlane	       ;}
    void                                                   setKalmanFilterOrder(std::vector<std::pair<double, std::string> > kalmanFilterOrder                   ) {kalmanFilterOrder_ = kalmanFilterOrder ;}
    void                                                   setH  	           (TVectorT<double>    h        ,std::string plaqID                                 ) {h_        [plaqID].ResizeTo(4);
                                                                                                                                                                    h_        [plaqID] = h                 ;}
    void                                                   setHx 	           (TVectorT<double>    hx       ,std::string plaqID                                 ) {hx_	      [plaqID].ResizeTo(4);
                                                                                                                                                                    hx_	      [plaqID] = hx                ;}
    void                                                   setHy 	           (TVectorT<double>    hy       ,std::string plaqID                                 ) {hy_	      [plaqID].ResizeTo(4);
                                                                                                                                                                    hy_	      [plaqID] = hy                ;}
    void                                                   setTrackCov	       (TMatrixTSym<double> trackCov ,std::string plaqID                                 ) {trackCov_ [plaqID].ResizeTo(4,4);
                                                                                                                                                                    trackCov_ [plaqID] = trackCov	       ;}
    void                                                   setTrackCovx        (TMatrixTSym<double> trackCovx,std::string plaqID                                 ) {trackCovx_[plaqID].ResizeTo(4,4);
                                                                                                                                                                    trackCovx_[plaqID] = trackCovx         ;}
    void                                                   setTrackCovy        (TMatrixTSym<double> trackCovy,std::string plaqID                                 ) {trackCovy_[plaqID].ResizeTo(4,4);
                                                                                                                                                                    trackCovy_[plaqID] = trackCovy         ;}
    void                                                   setOffset	       (double 	            offset   ,std::string plaqID                                 ) {offset_   [plaqID] = offset	           ;}
    void                                                   setOffsetx	       (double 	            offsetx  ,std::string plaqID                                 ) {offsetx_  [plaqID] = offsetx	       ;}
    void                                                   setOffsety	       (double 	            offsety  ,std::string plaqID                                 ) {offsety_  [plaqID] = offsety	       ;}
    
    unsigned int				                           getPosPlane	       (void                                                                             ) {return posPlane_	                   ;}
    unsigned int				                           getNegPlane	       (void                                                                             ) {return negPlane_	                   ;}
    const std::vector<std::pair<double, std::string> >   & getKalmanFilterOrder(void                                                                             ) {return kalmanFilterOrder_              ;}
    const TVectorT<double>                               & getH  	           (std::string plaqID                                                               ) {return h_        [plaqID]		       ;}
    const TVectorT<double>                               & getHx 	           (std::string plaqID                                                               ) {return hx_       [plaqID]  	           ;}
    const TVectorT<double>                               & getHy 	           (std::string plaqID                                                               ) {return hy_       [plaqID]  	           ;}
    const TMatrixTSym<double>                            & getTrackCov	       (std::string plaqID                                                               ) {return trackCov_ [plaqID]	           ;}
    const TMatrixTSym<double>                            & getTrackCovx        (std::string plaqID                                                               ) {return trackCovx_[plaqID]	           ;}
    const TMatrixTSym<double>                            & getTrackCovy        (std::string plaqID                                                               ) {return trackCovy_[plaqID]	           ;}
    double                                                 getOffset	       (std::string plaqID                                                               ) {return offset_   [plaqID]	           ;}
    double                                                 getOffsetx	       (std::string plaqID                                                               ) {return offsetx_  [plaqID]	           ;}
    double                                                 getOffsety	       (std::string plaqID                                                               ) {return offsety_  [plaqID]	           ;}
    		     																	
private:																		
															  				
    unsigned int                                 posPlane_	       ;
    unsigned int                                 negPlane_	       ;
    std::vector<std::pair<double, std::string> > kalmanFilterOrder_;
    std:: map< std::string,TVectorT<double> >    h_		           ;
    std:: map< std::string,TVectorT<double> >    hx_		       ;
    std:: map< std::string,TVectorT<double> >    hy_		       ;
    std:: map< std::string,TMatrixTSym<double> > trackCov_	       ;
    std:: map< std::string,TMatrixTSym<double> > trackCovx_	       ;
    std:: map< std::string,TMatrixTSym<double> > trackCovy_	       ;
    std:: map< std::string, double>              offset_ 	       ;
    std:: map< std::string, double>              offsetx_	       ;
    std:: map< std::string, double>              offsety_	       ;

    //ClassDef(KalmanPlaneInfo,1)

};

#endif // KALMANPLANEINFO_H
