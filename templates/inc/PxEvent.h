#ifndef _PxSuite_PxEvent_h
#define _PxSuite_PxEvent_h

#include <stdint.h>
#include <vector>
#include <TObject.h>

//namespace PxSuite
//{



struct PxDecodedHit
{
   unsigned int chan; 
   unsigned int chipid;
   unsigned int set;  
   unsigned int strip;
   unsigned int bco;  
   unsigned int adc;  
   unsigned int istrip;
};



class PxEvent : public TObject
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
    PxDecodedHit                     getDecodedHit      (unsigned int position) const;
    const std::vector<PxDecodedHit>& getDecodedHits     (void) const;

    //Setters
    void setTriggerNumber   (int32_t  triggerNumber);
    void setTriggerBcoNumber(int32_t  triggerBcoNumber);
    void setBCONumber       (int64_t  bcoNumber);
    void setRawHit          (uint32_t rawHit);
    void setRawHits         (const std::vector<uint32_t>& rawHits);
    void setDecodedHit      (const PxDecodedHit& PxDecodedHit);
    void setDecodedHits     (const std::vector<PxDecodedHit>& PxDecodedHits);
    PxEvent operator+=(const PxEvent& event);

private:
    int32_t triggerNumber_;//-1 means untriggered event
    int32_t triggerBcoNumber_;//-1 means untriggered event
    int64_t bcoNumber_;    //-1 means no bco information

    std::vector<uint32_t>     rawHits_;
    std::vector<PxDecodedHit> PxDecodedHits_;

    ClassDef(PxEvent,1);
};

//}

#endif
