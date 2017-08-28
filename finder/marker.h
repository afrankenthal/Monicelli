#include <stdint.h>
#include <string>
#include <memory>                       
#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <map>
#include <algorithm>                    
#include <iterator>                     
 
using namespace std;
 
class LCS 
{
 protected:
  // This linked list class is used to trace the LCS candidates
  class Pair 
  {
   public:
    uint32_t         index1;
    uint32_t         index2;
    shared_ptr<Pair> next  ;
 
    Pair(uint32_t         index1,
         uint32_t         index2,
         shared_ptr<Pair> next = nullptr)
      :  index1(index1),
         index2(index2),
         next  (next  )
    {
    }
 
    static shared_ptr<Pair> Reverse(const shared_ptr<Pair> pairs) 
    {
      shared_ptr<Pair> head = nullptr;
      for (auto next = pairs; next != nullptr; next = next->next)
      {
        head = make_shared<Pair>(next->index1, next->index2, head);
      }
      return head;
    }
  };
 
  typedef deque<shared_ptr<Pair> > PAIRS;
  typedef deque<uint32_t>          THRESHOLD;
  typedef deque<uint32_t>          INDEXES;
  typedef map<char, INDEXES>       CHAR2INDEXES;
  typedef deque<INDEXES*>          MATCHES;
 
  //=============================================================================
  uint64_t Pairs(MATCHES& matches, shared_ptr<Pair> *pairs)
  {
    auto trace = pairs != nullptr;
    PAIRS     traces   ;
    THRESHOLD threshold;
 
    uint32_t index1 = 0;
    for (const auto& it1 : matches) 
    {
      if (!it1->empty()) 
      {
        auto dq2   = *it1;
        auto limit = threshold.end();
        for (auto it2 = dq2.begin(); it2 != dq2.end(); it2++) 
        {
          auto index2 = *it2;
          limit       = lower_bound(threshold.begin(), limit, index2);
          auto index3 = distance   (threshold.begin(), limit        );
          auto skip   = (next(it2) != dq2.end())    &&
                        (limit == threshold.begin() || *prev(limit) < *next(it2));
          if (skip) continue;
 
          if (limit == threshold.end()) 
          {
            threshold.push_back(index2);
            limit = prev(threshold.end());
            if (trace) 
            {
              auto prefix = index3 > 0 ? traces[index3 - 1] : nullptr;
              auto last   = make_shared<Pair>(index1, index2, prefix);
              traces.push_back(last);
            }
          }
          else if (index2 < *limit) 
          {
            *limit = index2;
            if (trace) 
            {
              auto prefix = index3 > 0 ? traces[index3 - 1] : nullptr;
              auto last = make_shared<Pair>(index1, index2, prefix);
              traces[index3] = last;
            }
          }
        }
      } 
      index1++;
    }
 
    if (trace) 
    {
      auto last = traces.size() > 0 ? traces.back() : nullptr;
      *pairs = Pair::Reverse(last);
    }
 
    auto length = threshold.size();
    return length;
  }
//=============================================================================
  void Match(      CHAR2INDEXES & indexes,
                   MATCHES      & matches,
             const string       & s1     ,
             const string       & s2     )
  {
    uint32_t index = 0;
    for (const auto& it : s2)
      indexes[it].push_front(index++);
 
    for (const auto& it : s1) 
    {
      auto& dq2 = indexes[it];
      matches.push_back(&dq2);
    }
  }
 
  //=============================================================================
  string Select(shared_ptr<Pair>   pairs ,
                uint64_t           length,
                bool               right ,
                string           & s1    ,
                string           & s2    )
  {
    string buffer;
    buffer.reserve((uint32_t)length);
    uint32_t last1 = -1 ;
    uint32_t last2 = -1 ;
    vector<pair<int,int> > b1 ;
    vector<pair<int,int> > b2 ;

    for (auto next = pairs; next != nullptr; next = next->next) 
    {
      auto c = right ? s2[next->index2] : s1[next->index1];
      buffer.push_back(c);
    }
    
    for (auto next = pairs; next != nullptr; next = next->next) 
    {
     if( next->index1 != last1 + 1 ) b1.push_back(make_pair(last1+1,next->index1 -1)) ;
     last1 = next->index1 ;
     if( next->index2 != last2 + 1 ) b2.push_back(make_pair(last2+1,next->index2 -1)) ;
     last2 = next->index2 ;    
    }

    stringstream ss1, ss2 ;
    bool atL = false ;
    bool atH = false ;
    bool atB = false ;
    
    for(size_t c=0; c<s1.size(); ++c)
    {
     atL = false ;
     atH = false ;
     atB = false ;
     for(size_t i=0; i<b1.size(); ++i)
     {
      if(c >= (size_t)b1[i].first && c <= (size_t)b1[i].second)
      {
       if     ( c == (size_t)b1[i].first &&
                c == (size_t)b1[i].second  ) {atB = true ; break;}
       if     ( c == (size_t)b1[i].first   ) {atL = true ; break;}
       else if( c == (size_t)b1[i].second  ) {atH = true ; break;}
      }
     }
     if( atB ) {ss1 << "<font color=\"#ff21f2\" wheight=\"bold\">"  << s1[c] << "</font>"; continue; }
     if( atL ) {ss1 << "<font color=\"#ff21f2\" wheight=\"bold\">"  << s1[c]             ; continue; }
     if( atH ) {ss1 << s1[c]  << "</font>"                                               ; continue; }
     ss1 << s1[c] ;
    }
    
    for(size_t c=0; c<s2.size(); ++c)
    {
     atL = false ;
     atH = false ;
     atB = false ;
     for(size_t i=0; i<b2.size(); ++i)
     {
      if(c >= (size_t)b2[i].first && c <= (size_t)b2[i].second)
      {
       if     ( c == (size_t)b2[i].first &&
                c == (size_t)b2[i].second  ) {atB = true ; break;}
       if     ( c == (size_t)b2[i].first   ) {atL = true ; break;}
       else if( c == (size_t)b2[i].second  ) {atH = true ; break;}
      }
     }
     if( atB ) {ss2 << "<font color=\"cyan\" wheight=\"bold\">"  << s2[c] << "</font>"; continue; }
     if( atL ) {ss2 << "<font color=\"cyan\" wheight=\"bold\">"  << s2[c]             ; continue; }
     if( atH ) {ss2 << s2[c] << "</font>"                                             ; continue; }
     ss2 << s2[c] ;
    }
    
    s1 = ss1.str() ;
    s2 = ss2.str() ;
    
    return buffer;
  }
 
 public:
  string Correspondence(string & s1,
                        string & s2)
  {
    CHAR2INDEXES indexes;
    MATCHES      matches;
    Match(indexes, matches, s1, s2);
    shared_ptr<Pair> pairs;
    auto length = Pairs(matches, &pairs);
    
    return Select(pairs, length, false, s1, s2);
  }
};
