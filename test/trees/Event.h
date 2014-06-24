#include <vector>

#include <TObject.h>

class Event : public TObject
{
 public:
   Event(void)  ;
  ~Event(void) {;}
  
   void             setEventNumber (int    event) ;
   int              getEventNumber (void)         ;
   void             addTrack       (double p)     ;
   void             addVertex      (int    v)     ;
   int              getNTracks     (void)         ;
   std::vector<int> getVertices    (void)         ;
   void             dumpTracks     (void)         ;  
   void             cleanData      (void)         ;
   
   void             setX           (double x)     {myStruct_.x_ = x ;}
   double           x              (void)         {return myStruct_.x_ ;}
   
 private:
   int eventNumber_       ;
   std::vector<double> p_ ;
   std::vector<int> vertices_ ;
   
   struct myStruct_
   {
    double x_ ;
    double y_ ;
   } myStruct_ ;
   
 ClassDef(Event,1)
} ;
