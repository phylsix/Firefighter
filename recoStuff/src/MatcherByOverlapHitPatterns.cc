#include "Firefighter/recoStuff/interface/MatcherByOverlapHitPatterns.h"

ff::MatcherByOverlapHitPatterns::MatcherByOverlapHitPatterns(
    const edm::ParameterSet& ps )
    : fOverlapRatioMin( ps.getParameter<double>( "minOverlapRatio" ) ) {
  setInitBy( ff::ffCandMatcher::InitBy::none );
}

ff::MapLink
ff::MatcherByOverlapHitPatterns::match(
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

  // Loop src first, since we trust them more
  for ( const auto& sCand : sortedSrcCol ) {
    const reco::HitPattern& sHp = sCand->muonRef()->outerTrack()->hitPattern();
    vector<uint16_t>        sHits{};
    for ( int i( 0 ); i != sHp.numberOfAllHits( reco::HitPattern::TRACK_HITS );
          ++i )
      sHits.push_back( sHp.getHitPattern( reco::HitPattern::TRACK_HITS, i ) );
    sort( sHits.begin(), sHits.end() );

    // metric used to select the best matches
    float                minMatchMetric( 999. );
    reco::PFCandidatePtr bestMatchedCand;

    // Loop match collection inside
    for ( const auto& mCand : sortedMatchCol ) {
      // skip if already matched
      if ( result.find( mCand ) != result.end() )
        continue;

      const reco::HitPattern& mHp =
          mCand->muonRef()->outerTrack()->hitPattern();
      vector<uint16_t> mHits{};
      for ( int i( 0 );
            i != mHp.numberOfAllHits( reco::HitPattern::TRACK_HITS ); ++i )
        mHits.push_back( mHp.getHitPattern( reco::HitPattern::TRACK_HITS, i ) );
      sort( mHits.begin(), mHits.end() );

      // find the overlapped hitPattern
      vector<uint16_t> interHits{};
      set_intersection( sHits.begin(), sHits.end(), mHits.begin(), mHits.end(),
                        back_inserter( interHits ) );
      float overlapRatio = sHits.empty() ? 0. : interHits.size() / sHits.size();
      bool  flagOverlap  = overlapRatio >= fOverlapRatioMin;
      if ( !flagOverlap )
        continue;

      // assign the best matches with metric
      float metric = ( sCand->pt() != 0 )
                         ? fabs( mCand->pt() - sCand->pt() ) / sCand->pt()
                         : NAN;
      if ( !isnan( metric ) and metric < minMatchMetric ) {
        minMatchMetric  = metric;
        bestMatchedCand = mCand;
      }
    }

    if ( bestMatchedCand.isNonnull() )
      result.emplace( bestMatchedCand, sCand );
  }

  return result;
}
