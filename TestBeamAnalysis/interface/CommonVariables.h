#ifndef CommonVariables_h
#define CommonVariables_h

#define NPLANES 10
#define ROWPITCH .100 
#define COLPITCH .150 
#define ROWEDGE  .200 
#define COLEDGE  .300 



#define XMINBOUND -8.100
#define XMAXBOUND  8.100


#define XCHIP0MIN   4.05
#define XCHIP0MAX  12.15
#define XCHIP1MIN  -4.05
#define XCHIP1MAX   4.05
#define XCHIP2MIN -12.15
#define XCHIP2MAX  -4.05
#define XCHIP3MIN -12.15
#define XCHIP3MAX  -4.05
#define XCHIP4MIN  -4.05
#define XCHIP4MAX   4.05
#define XCHIP5MIN   4.05
#define XCHIP5MAX  12.15


enum planeIDs_t         {PLANE0, PLANE1, PLANE2, PLANE3, PLANE4, PLANE5, PLANE6, PLANE7, DUT1, DUT0};
enum stationIDs_t       {STATION0, STATION1, STATION2};
enum orientationIDs_t   {XPLANE, YPLANE, DUT};


#endif
