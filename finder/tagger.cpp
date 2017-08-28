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
