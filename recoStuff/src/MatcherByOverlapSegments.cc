#include "Firefighter/recoStuff/interface/MatcherByOverlapSegments.h"

#include "DataFormats/Math/interface/deltaR.h"

#include <algorithm>

ff::MatcherByOverlapSegments::MatcherByOverlapSegments(
    const edm::ParameterSet& ps )
    : fDeltaRMax( ps.getParameter<double>( "maxDeltaR" ) ),
      fOverlapRatioMin( ps.getParameter<double>( "minOverlapRatio" ) ),
      fDeltaTMax( ps.getParameter<double>( "maxDeltaT" ) ) {
  setInitBy( ff::ffCandMatcher::InitBy::none );
}

ff::MapLink
ff::MatcherByOverlapSegments::match(
    const PFCandPtrCollection& srcCol,
    const PFCandPtrCollection& matchCol ) const {
  using namespace std;
  ff::MapLink result{};  // dSA --> particleFlow

  // copy and sort src collection by pT from high to low
  vector<reco::PFCandidatePtr> sortedSrcCol{};
  for ( const auto& cand : srcCol ) {
    if ( cand.isNull() or
         ( cand->particleId() != reco::PFCandidate::ParticleType::mu ) or
         cand->muonRef().isNull() or cand->muonRef()->outerTrack().isNull() )
      continue;
    sortedSrcCol.push_back( cand );
  }
  sort( sortedSrcCol.begin(), sortedSrcCol.end(),
        []( const auto& lhs, const auto& rhs ) {
          return lhs->pt() > rhs->pt();
        } );

  // copy and sort match collection by pT from high to low
  vector<reco::PFCandidatePtr> sortedMatchCol{};
  for ( const auto& cand : matchCol ) {
    if ( cand.isNull() or
         ( cand->particleId() != reco::PFCandidate::ParticleType::mu ) or
         cand->muonRef().isNull() or cand->muonRef()->outerTrack().isNull() )
      continue;
    sortedMatchCol.push_back( cand );
  }
  sort( sortedMatchCol.begin(), sortedMatchCol.end(),
        []( const auto& lhs, const auto& rhs ) {
          return lhs->pt() > rhs->pt();
        } );

  // Loop src first, since we trust it more
  for ( const auto& sCand : sortedSrcCol ) {
    reco::MuonRef sMuRef = sCand->muonRef();
    float sTime = sMuRef->isTimeValid() ? sMuRef->time().timeAtIpInOut : NAN;
    vector<int> sCscSegs{}, sDtSegs{};

    for ( const auto& mm : sMuRef->matches() ) {
      for ( const auto& seg : mm.segmentMatches ) {
        if ( seg.cscSegmentRef.isNonnull() )
          sCscSegs.emplace_back( seg.cscSegmentRef.key() );
        if ( seg.dtSegmentRef.isNonnull() )
          sDtSegs.emplace_back( seg.dtSegmentRef.key() );
      }
    }
    sort( sCscSegs.begin(), sCscSegs.end() );
    sort( sDtSegs.begin(), sDtSegs.end() );

    // loop match collection inside
    for ( const auto& mCand : sortedMatchCol ) {
      // skip if already matched
      if ( result.find( mCand ) != result.end() )
        continue;

      reco::MuonRef mMuRef = mCand->muonRef();
      float mTime = mMuRef->isTimeValid() ? mMuRef->time().timeAtIpInOut : NAN;
      vector<int> mCscSegs{}, mDtSegs{};

      for ( const auto& mm : mMuRef->matches() ) {
        for ( const auto& seg : mm.segmentMatches ) {
          if ( seg.cscSegmentRef.isNonnull() )
            mCscSegs.emplace_back( seg.cscSegmentRef.key() );
          if ( seg.dtSegmentRef.isNonnull() )
            mDtSegs.emplace_back( seg.dtSegmentRef.key() );
        }
      }
      sort( mCscSegs.begin(), mCscSegs.end() );
      sort( mDtSegs.begin(), mDtSegs.end() );

      // find the overlapped CSC segs and DT segs ids
      vector<int> interCSC{}, interDT{};
      set_intersection( sCscSegs.begin(), sCscSegs.end(), mCscSegs.begin(),
                        mCscSegs.end(), back_inserter( interCSC ) );
      set_intersection( sDtSegs.begin(), sDtSegs.end(), mDtSegs.begin(),
                        mDtSegs.end(), back_inserter( interDT ) );
      // calculate weighted ratio
      float cscRatio =
          sCscSegs.empty() ? 0. : interCSC.size() / sCscSegs.size();
      float weightCSC =
          ( interCSC.empty() and interDT.empty() )
              ? 0.
              : interCSC.size() / ( interCSC.size() + interDT.size() );
      float dtRatio = sDtSegs.empty() ? 0. : interDT.size() / sDtSegs.size();
      float weightDT =
          ( interCSC.empty() and interDT.empty() )
              ? 0.
              : interDT.size() / ( interCSC.size() + interDT.size() );

      // flags for making decisions
      bool flagDistance = deltaR( *sCand.get(), *mCand.get() ) <= fDeltaRMax;
      bool flagOverlap =
          ( cscRatio * weightCSC + dtRatio * weightDT ) >= fOverlapRatioMin;
      bool flagTime = ( isnan( sTime ) or isnan( mTime ) )
                          ? false
                          : fabs( sTime - mTime ) <= fDeltaTMax;

      if ( ( flagDistance or flagOverlap ) and flagTime ) {
        result.emplace( mCand, sCand );
        break;
      }
    }
  }

  return result;
}
