/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef EVENTHEADER_H
#define EVENTHEADER_H

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
