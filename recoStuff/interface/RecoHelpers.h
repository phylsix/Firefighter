#ifndef recoStuff_RecoHelpers_H
#define recoStuff_RecoHelpers_H

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/Measurement1D.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "RecoVertex/VertexPrimitives/interface/VertexState.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"

#include <set>
#include <map>
#include <algorithm>
#include <numeric>


/** some helper functions that can be applied across
 * different plugins
 */

namespace ff {

  bool genAccept(reco::GenParticle const&);

  // same functionality as IPTools::absoluteImpactParameter,
  // replace reco::Vertex with VertexState
  std::pair<bool, Measurement1D>
  absoluteImpactParameter(const TrajectoryStateOnSurface&,
                          const VertexState&,
                          VertexDistance&);


  // same functionality as IPTools::absoluteImpactParameter3D,
  // replace reco::Vertex with VertexState
  std::pair<bool, Measurement1D>
  absoluteImpactParameter3D(const reco::TransientTrack&,
                            const VertexState&);


  // same functionality as IPTools::absoluteTransverseImpactParameter,
  // replace reco::Vertex with VertexState
  std::pair<bool, Measurement1D>
  absoluteTransverseImpactParameter(const reco::TransientTrack&,
                                    const VertexState&);


  template<typename T>
  T
  medianValue(const std::vector<T>& v)
  {
    std::vector<T> vcopy;
    std::copy(v.begin(), v.end(), back_inserter(vcopy));
    size_t vsize = vcopy.size();

    T res;
    if (vsize==0)
    {
      res = NAN;
    } else
    {
      std::sort(vcopy.begin(), vcopy.end());
      if (vsize%2==0)
      {
        res = (vcopy[vsize/2-1]+vcopy[vsize/2])/2;
      } else
      {
        res = vcopy[vsize/2];
      }
    }

    return res;
  }


  template<typename T>
  float
  averageValue(const std::vector<T>& v)
  {
    float res;
    if (v.size()==0)
    {
      res = NAN;
    } else
    {
      res = std::accumulate(v.begin(), v.end(), 0)/v.size();
    }

    return res;
  }

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