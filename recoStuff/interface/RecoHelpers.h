#ifndef recoStuff_RecoHelpers_H
#define recoStuff_RecoHelpers_H

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include <set>
#include <map>
#include <algorithm>


/** some helper functions that can be applied across
 * different plugins
 */

namespace ff {

  bool genAccept(reco::GenParticle const&);

  template<typename T>
  std::set<T>
  getMergedSetFromVectors(const std::vector<T>& A,
                          const std::vector<T>& B)
  {
    std::set<T> result{};
    std::set_union(
      A.begin(), A.end(),
      B.begin(), B.end(),
      std::inserter(result, result.begin())
    );

    return result;
  }


  template<typename T>
  std::set<typename T::key_type>
  getMergedMapKeys(const T& A,
                   const T& B)
  {
    std::vector<typename T::key_type> keyA, keyB;
    for (const auto& entry : A) keyA.push_back(entry.first);
    for (const auto& entry : B) keyB.push_back(entry.first);

    return ff::getMergedSetFromVectors<typename T::key_type>(keyA, keyB);
  }


  template<typename T>
  float
  calcOverlap(const std::vector<T>& src,
              const std::vector<T>& comp)
  {
    if (src.empty()) return 0.;

    std::vector<bool> maskbits(src.size(), false);

    for (const auto& c : comp)
    {
      auto srcIt(src.begin());
      auto maskIt(maskbits.begin());

      for (; srcIt!=src.end(); ++srcIt, ++maskIt)
      {
        if (*maskIt) continue;

        if (*srcIt == c)
        {
          *maskIt = true;
          break;
        }
      }
    }

    return (float)std::count(maskbits.begin(), maskbits.end(), true)/maskbits.size();
  }
  
}

#endif