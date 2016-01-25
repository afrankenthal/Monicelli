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
