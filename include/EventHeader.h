/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef _EventHeader_h_
#define _EventHeader_h_

#include <TObject.h>

class EventHeader : public TObject
{
public:
    EventHeader(void);
    ~EventHeader(void);

    void setClustersDone(bool done) ;
    void setTracksFound (bool done) ;

    bool clustersDone   (void     ) ;
    bool tracksFound    (void     ) ;

private:
    bool clustersDone_    ;
    bool tracksFound_     ;

    ClassDef(EventHeader,1);
} ;

#endif // EVENTHEADER_H
