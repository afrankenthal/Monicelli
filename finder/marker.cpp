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
 
    Pair(uint32_t index1, uint32_t index2, shared_ptr<Pair> next = nullptr)
      : index1(index1), index2(index2), next(next) 
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
 
  // return the LCS as a linked list of matched index pairs
  uint64_t Pairs(MATCHES& matches, shared_ptr<Pair> *pairs) 
  {
    auto trace = pairs != nullptr;
    PAIRS     traces   ;
    THRESHOLD threshold;
 
    //
    //[Assert]After each index1 iteration threshold[index3] is the least index2
    // such that the LCS of s1[0:index1] and s2[0:index2] has length index3 + 1
    //
    uint32_t index1 = 0;
    for (const auto& it1 : matches) 
    {
      if (!it1->empty()) 
      {
        auto dq2   = *it1;
        auto limit = threshold.end();
        for (auto it2 = dq2.begin(); it2 != dq2.end(); it2++) 
        {
          // Each of the index1, index2 pairs considered here correspond to a match
          auto index2 = *it2;
 
          //
          // Note: The index2 values are monotonically decreasing, which allows the
          // thresholds to be updated in-place.  Monotonicity allows a binary search,
          // implemented here by std::lower_bound().
          //
          limit       = lower_bound(threshold.begin(), limit, index2);
          auto index3 = distance   (threshold.begin(), limit        );
 
          //
          // Look ahead to the next index2 value to optimize space used in the Hunt
          // and Szymanski algorithm.  If the next index2 is also an improvement on
          // the value currently held in threshold[index3], a new Pair will only be
          // superseded on the next index2 iteration.
          //
          // Depending on match redundancy, the number of Pair constructions may be
          // divided by factors ranging from 2 up to 10 or more.
          //
          auto skip = (next(it2) != dq2.end())    &&
                      (limit == threshold.begin() || *prev(limit) < *next(it2));
          if (skip) continue;
 
          if (limit == threshold.end()) 
          {
            // insert case
            threshold.push_back(index2);
            // Refresh limit iterator:
            limit = prev(threshold.end());
            if (trace) 
            {
              auto prefix = index3 > 0 ? traces[index3 - 1] : nullptr;
              auto last = make_shared<Pair>(index1, index2, prefix);
              traces.push_back(last);
            }
          }
          else if (index2 < *limit) 
          {
            // replacement case
            *limit = index2;
            if (trace) 
            {
              auto prefix = index3 > 0 ? traces[index3 - 1] : nullptr;
              auto last = make_shared<Pair>(index1, index2, prefix);
              traces[index3] = last;
            }
          }
        }                                 // next index2
      }
 
      index1++;
    }                                     // next index1
 
    if (trace) 
    {
      auto last = traces.size() > 0 ? traces.back() : nullptr;
      // Reverse longest back-trace
      *pairs = Pair::Reverse(last);
    }
 
    auto length = threshold.size();
    return length;
  }
 
  //
  // Match() avoids incurring m*n comparisons by using the associative
  // memory implemented by CHAR2INDEXES to achieve O(m+n) performance,
  // where m and n are the input lengths.
  //
  // The lookup time can be assumed constant in the case of characters.
  // The symbol space is larger in the case of records; but the lookup
  // time will be O(log(m+n)), at most.
  //
  void Match(CHAR2INDEXES& indexes, MATCHES& matches, const string& s1, const string& s2) 
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
 
  string Select(shared_ptr<Pair> pairs, uint64_t length, bool right, string& s1, string& s2) 
  {
    string buffer;
    buffer.reserve((uint32_t)length);
    int last1 = -1 ; 
    int last2 = -1 ; 
    vector<pair<int,int> > b1 ;
    vector<pair<int,int> > b2 ;

    for (auto next = pairs; next != nullptr; next = next->next) 
    {
      auto c = right ? s2[next->index2] : s1[next->index1];
      cout << __LINE__ << "] " << c << " " << next->index1 << " " << next->index2 << endl ;
      buffer.push_back(c);
    }
    
    for (auto next = pairs; next != nullptr; next = next->next) 
    {
     if( next->index1 != last1 + 1 ) b1.push_back(make_pair(last1+1,next->index1 -1)) ;
     last1 = next->index1 ;

     if( next->index2 != last2 + 1 ) b2.push_back(make_pair(last2+1,next->index2 -1)) ;
     last2 = next->index2 ;    
    }

//     cout << __LINE__ << "] Boundaries: " << endl ;
//     for(int i=0; i<b1.size(); ++i)
//     {
//      cout << __LINE__ << "] " << b1[i].first << " - " << b1[i].second << endl ;
//     }
//     cout << __LINE__ << "] " << endl ;
//     for(int i=0; i<b2.size(); ++i)
//     {
//      cout << __LINE__ << "] " << b2[i].first << " - " << b2[i].second << endl ;
//     }
//     cout << __LINE__ << "] Filter: " << endl ;
    
    stringstream ss1, ss2 ;
    bool atL = false ;
    bool atH = false ;
    bool atB = false ;
    
    for(int c=0; c<s1.size(); ++c)
    {
     atL = false ;
     atH = false ;
     atB = false ;
     for(int i=0; i<b1.size(); ++i)
     {
      if(c >= b1[i].first && c <= b1[i].second) 
      {
       if     ( c == b1[i].first &&
                c == b1[i].second  ) {atB = true ; break;}
       if     ( c == b1[i].first   ) {atL = true ; break;}
       else if( c == b1[i].second  ) {atH = true ; break;}
      }
     }
     if( atB ) {ss1 << "<font>"  << s1[c] << "</font>"; continue; }
     if( atL ) {ss1 << "<font>"  << s1[c]             ; continue; }
     if( atH ) {ss1 << "</font>" << s1[c]             ; continue; }
     ss1 << s1[c] ;
    }
    
    for(int c=0; c<s2.size(); ++c)
    {
     atL = false ;
     atH = false ;
     atB = false ;
     for(int i=0; i<b2.size(); ++i)
     {
      if(c >= b2[i].first && c <= b2[i].second) 
      {
       if     ( c == b2[i].first &&
                c == b2[i].second  ) {atB = true ; break;}
       if     ( c == b2[i].first   ) {atL = true ; break;}
       else if( c == b2[i].second  ) {atH = true ; break;}
      }
     }
     if( atB ) {ss2 << "<font>"  << s2[c] << "</font>"; continue; }
     if( atL ) {ss2 << "<font>"  << s2[c]             ; continue; }
     if( atH ) {ss2 << "</font>" << s2[c]             ; continue; }
     ss2 << s2[c] ;
    }
    
    cout << __LINE__ << "] " << ss1.str() << endl ;
    cout << __LINE__ << "] " << ss2.str() << endl ;
    
    s1 = ss1.str() ;
    s2 = ss2.str() ;
    
    return buffer;
  }
 
public:
  string Correspondence(string& s1, string& s2) 
  {
    CHAR2INDEXES indexes;
    MATCHES      matches;               // holds references into indexes
    Match(indexes, matches, s1, s2);
//     for(CHAR2INDEXES::iterator it=indexes.begin(); it!=indexes.end(); ++it)
//     {
//      cout << __LINE__ << "] " << it->first << endl ;
//      for(INDEXES::iterator jt=it->second.begin(); jt!=it->second.end(); ++jt)
//      {
//       cout << __LINE__ << "]   " << *jt << endl ;      
//      }
//     }
    shared_ptr<Pair> pairs;             // obtain the LCS as index pairs
    auto length = Pairs(matches, &pairs);
    for(MATCHES::iterator it=matches.begin(); it!=matches.end(); ++it)
    {
     
    }
    
    return Select(pairs, length, false, s1, s2);
  }
};

// int main()
// {
// //                         1         2         3         4
// //               -123456789-123456789-123456789-123456789-
//     string s1 = "Io sono pippo e sono amico di Paperino " ;
//     string s2 = "Io sono peppo e non sono amico di Minnie " ;
//     LCS lcs;
//     auto s = lcs.Correspondence(s1, s2);
//     cout << s << endl;
//     cout << s1 << endl;
//     cout << s2 << endl;
// }

