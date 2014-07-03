#ifndef _PxSuite_PxEvent_h
#define _PxSuite_PxEvent_h

#include <stdint.h>
#include <vector>
#include <TObject.h>
#include <Hit.h>

//namespace PxSuite
//{


class PxEvent //: public TObject
{
public:
    PxEvent(void);
    PxEvent(int64_t bcoNumber);
    virtual ~PxEvent(void);

    //Getters
    int32_t                          getTriggerNumber   (void) const;
    int32_t                          getTriggerBcoNumber(void) const;
    int64_t                          getBCONumber       (void) const;
    unsigned int                     getNumberOfHits    (void) const;
    uint32_t                         getRawHit          (unsigned int position) const;
    const std::vector<uint32_t>&     getRawHits         (void) const;
    Hit                              getDecodedHit      (unsigned int position) const;
    const std::vector<Hit>&          getDecodedHits     (void) const;

    //Setters
    void setTriggerNumber   (int32_t  triggerNumber);
    void setTriggerBcoNumber(int32_t  triggerBcoNumber);
    void setBCONumber       (int64_t  bcoNumber);
    void setRawHit          (uint32_t rawHit);
    void setRawHits         (const std::vector<uint32_t>& rawHits);
    void setDecodedHit      (const Hit& Hit);
    void setDecodedHits     (const std::vector<Hit>& Hits);
    PxEvent operator+=(const PxEvent& event);

    void addTriggerNumberToHits(void);

private:
    int32_t triggerNumber_;//-1 means untriggered event
    int32_t triggerBcoNumber_;//-1 means untriggered event
    int64_t bcoNumber_;    //-1 means no bco information

    std::vector<uint32_t>     rawHits_;
    std::vector<Hit>          decodedHits_;

    //ClassDef(PxEvent,1);
};

//}

#endif
