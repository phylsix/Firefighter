#ifndef recoStuff_RecoHelpers_H
#define recoStuff_RecoHelpers_H

#include <algorithm>
#include <cmath>
#include <iterator>
#include <map>
#include <numeric>
#include <set>

#include "DataFormats/GeometryCommonDetAlgo/interface/Measurement1D.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicVertex.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/VertexPrimitives/interface/VertexState.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"

/** some helper functions that can be applied across
 * different plugins
 */

namespace ff {

using Point = math::XYZPointF;

bool
genAccept( reco::GenParticle const& );

// same functionality as IPTools::absoluteImpactParameter,
// replace reco::Vertex with VertexState
std::pair<bool, Measurement1D>
absoluteImpactParameter( const TrajectoryStateOnSurface&,
                         const VertexState&,
                         VertexDistance& );

// same functionality as IPTools::absoluteImpactParameter3D,
// replace reco::Vertex with VertexState
std::pair<bool, Measurement1D>
absoluteImpactParameter3D( const reco::TransientTrack&, const VertexState& );

// same functionality as IPTools::absoluteTransverseImpactParameter,
// replace reco::Vertex with VertexState
std::pair<bool, Measurement1D>
absoluteTransverseImpactParameter( const reco::TransientTrack&,
                                   const VertexState& );

reco::PFCandidatePtr
getCandWithMaxPt( const std::vector<reco::PFCandidatePtr>& );

int
getCandType( const reco::PFCandidatePtr&,
             const edm::Handle<reco::TrackCollection>& );

float
getMuonIsolationValue( const reco::Muon& );

/**
 * @brief Estimate vertex as the median value of reference points of tracks of
 * the jet.
 *
 * @param pTks vector of tracks.
 * @return Point
 */
Point
estimatedVertexFromMedianReferencePoints(
    const std::vector<const reco::Track*>& pTks );

/**
 * @brief Estimate vertex as the average value of reference points of tracks
 * of the jet.
 *
 * @param pTks vector of tracks.
 * @return Point
 */
Point
estimatedVertexFromAverageReferencePoints(
    const std::vector<const reco::Track*>& pTks );

std::pair<TransientVertex, float>
kalmanVertexFromTransientTracks( const std::vector<reco::TransientTrack>&,
                                 const edm::ParameterSet& );

std::pair<KinematicVertex, float>
kinematicVertexFromTransientTracks( const std::vector<reco::TransientTrack>& );

Measurement1D
signedDistanceXY( const reco::Vertex&,
                  const VertexState&,
                  const GlobalVector& );
Measurement1D
signedDistance3D( const reco::Vertex&,
                  const VertexState&,
                  const GlobalVector& );

float
cosThetaOfJetPvXY( const reco::Vertex&,
                   const VertexState&,
                   const GlobalVector& );

float
cosThetaOfJetPv3D( const reco::Vertex&,
                   const VertexState&,
                   const GlobalVector& );

float
impactDistanceXY( const reco::Vertex&,
                  const VertexState&,
                  const GlobalVector& );

float
impactDistance3D( const reco::Vertex&,
                  const VertexState&,
                  const GlobalVector& );

//-----------------------------------------------------------------------------

template <typename T>
T
medianValue( const std::vector<T>& v ) {
  std::vector<T> vcopy;
  std::copy( v.begin(), v.end(), back_inserter( vcopy ) );
  size_t vsize = vcopy.size();

  T res;
  if ( vsize == 0 ) {
    res = NAN;
  } else {
    std::sort( vcopy.begin(), vcopy.end() );
    if ( vsize % 2 == 0 ) {
      res = ( vcopy[ vsize / 2 - 1 ] + vcopy[ vsize / 2 ] ) / 2;
    } else {
      res = vcopy[ vsize / 2 ];
    }
  }

  return res;
}

template <typename T>
float
averageValue( const std::vector<T>& v ) {
  float res;
  if ( v.size() == 0 ) {
    res = NAN;
  } else {
    res = std::accumulate( v.begin(), v.end(), 0. ) / v.size();
  }

  return res;
}

template <typename T>
std::set<T>
getMergedSetFromVectors( const std::vector<T>& A, const std::vector<T>& B ) {
  std::set<T> result{};
  std::set_union( A.begin(), A.end(), B.begin(), B.end(),
                  std::inserter( result, result.begin() ) );

  return result;
}

template <typename T>
std::set<typename T::key_type>
getMergedMapKeys( const T& A, const T& B ) {
  std::vector<typename T::key_type> keyA, keyB;
  for ( const auto& entry : A )
    keyA.push_back( entry.first );
  for ( const auto& entry : B )
    keyB.push_back( entry.first );

  return ff::getMergedSetFromVectors<typename T::key_type>( keyA, keyB );
}

template <typename T>
float
calcOverlap( const std::vector<T>& src, const std::vector<T>& comp ) {
  // if ( src.empty() )
  //   return 0.;

  // std::vector<bool> maskbits( src.size(), false );

  // for ( const auto& c : comp ) {
  //   auto srcIt( src.begin() );
  //   auto maskIt( maskbits.begin() );

  //   for ( ; srcIt != src.end(); ++srcIt, ++maskIt ) {
  //     if ( *maskIt )
  //       continue;

  //     if ( *srcIt == c ) {
  //       *maskIt = true;
  //       break;
  //     }
  //   }
  // }

  // return (float)std::count( maskbits.begin(), maskbits.end(), true ) /
  //        maskbits.size();
  if ( src.size() + comp.size() == 0 )
    return 0.;

  std::vector<T> srcCopy( src );
  std::vector<T> compCopy( comp );
  std::sort( srcCopy.begin(), srcCopy.end() );
  std::sort( compCopy.begin(), compCopy.end() );
  std::vector<T> v_intersection;
  std::set_intersection( srcCopy.begin(), srcCopy.end(), compCopy.begin(),
                         compCopy.end(), std::back_inserter( v_intersection ) );
  return (float)v_intersection.size() / std::min( src.size(), comp.size() );
}

template <typename T>
double
calculateStandardDeviation( const std::vector<T>& v ) {
  using namespace std;
  vector<T> vcopy{};
  copy_if( v.begin(), v.end(), back_inserter( vcopy ),
           []( const auto& n ) { return !isnan( n ); } );
  if ( vcopy.empty() )
    return NAN;

  double sum  = (double)accumulate( vcopy.begin(), vcopy.end(), 0.0 );
  double mean = (double)sum / vcopy.size();

  vector<double> diff( vcopy.size() );
  transform( vcopy.begin(), vcopy.end(), diff.begin(),
             [mean]( double x ) { return x - mean; } );
  double sq_sum = inner_product( diff.begin(), diff.end(), diff.begin(), 0.0 );

  return sqrt( sq_sum / vcopy.size() );
}

}  // namespace ff

#endif
