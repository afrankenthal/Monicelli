#ifndef Cuts_h
#define Cuts_h

class DataClass;
class Cuts
{
 public:
  Cuts(DataClass& data);	
//  Cuts(const Cuts& cuts);	
//  Cuts& operator=(const Cuts& cuts);	
  ~Cuts();
  void reset   	   (void       );
  bool ntrack  	   (int tracks );
  bool newEvent	   (void       );
  bool chi2Excl	   (int plane  );
  bool hasHits     (int plane  );
  bool allTrackProj(void       );
  bool hitTrackProj(int plane  );
  bool xResid	   (int plane  );
  bool yResid	   (int plane  );
  bool efficiency  (void);
  bool stationHits (int station);
  
 private:
  //Methods
  double getChi2Ndof(int plane);

  //Variables
  DataClass& theData_;
  int        oldEventId_;
  const double chi2Cut_;
};

#endif
