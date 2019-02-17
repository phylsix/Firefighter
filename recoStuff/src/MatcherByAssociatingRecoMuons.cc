#include "Firefighter/recoStuff/interface/MatcherByAssociatingRecoMuons.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"

#include <algorithm>

ff::MatcherByAssociatingRecoMuons::MatcherByAssociatingRecoMuons(
    const edm::ParameterSet& ps,
    edm::ConsumesCollector&& cc )
    : recoMuon_token_( cc.consumes<reco::MuonCollection>(
          ps.getParameter<edm::InputTag>( "recoMuons" ) ) ),
      minHitsOverlapRatio_( ps.getParameter<double>( "minHitsOverlapRatio" ) ) {
  reset();
}

void
ff::MatcherByAssociatingRecoMuons::init( const edm::Event& e ) {
  reset();

  edm::Handle<reco::MuonCollection> recoMuon_h;
  e.getByToken( recoMuon_token_, recoMuon_h );
  assert( recoMuon_h.isValid() );

  for ( const auto& muon : *recoMuon_h ) {
    const reco::TrackRef innerTk = muon.innerTrack();
    const reco::TrackRef outerTk = muon.outerTrack();

    if ( innerTk.isNull() or outerTk.isNull() )
      continue;

    recoMuonTrackLink_.emplace( innerTk, outerTk );
  }
}

void
ff::MatcherByAssociatingRecoMuons::reset() {
  recoMuonTrackLink_.clear();
}

std::map<reco::PFCandidateRef, reco::PFCandidateRef>
ff::MatcherByAssociatingRecoMuons::match(
    const std::vector<reco::PFCandidateRef>& srcCol,
    const std::vector<reco::PFCandidateRef>& matchCol ) const {
  std::map<reco::PFCandidateRef, reco::PFCandidateRef>
      result{};  // dSA --> particleFlow

  /// copy and sort input collection by pT from high to low
  std::vector<reco::PFCandidateRef> sortedSrcCol{};
  for ( const auto& cand : srcCol )
    sortedSrcCol.push_back( cand );
  std::sort( sortedSrcCol.begin(), sortedSrcCol.end(),
             []( const auto& lhs, const auto& rhs ) {
               return lhs->pt() > rhs->pt();
             } );

  for ( const auto& srcCand : sortedSrcCol ) {
    if ( srcCand.isNull() )
      continue;
    const reco::TrackRef srcTk = srcCand->trackRef();
    if ( srcTk.isNull() )
      continue;

    if ( recoMuonTrackLink_.find( srcTk ) == recoMuonTrackLink_.end() )
      continue;

    const reco::TrackRef& associatedOuterTk = recoMuonTrackLink_.at( srcTk );

    float                minMatchingMetric( 999. );
    reco::PFCandidateRef bestMatchedCand;

    for ( const auto& matCand : matchCol ) {
      if ( matCand.isNull() )
        continue;
      const reco::TrackRef matTk = matCand->trackRef();
      if ( matTk.isNull() )
        continue;

      if ( result.find( matCand ) != result.end() )
        continue;

      bool isOverlapped =
          checkOverlap( associatedOuterTk, matTk, minHitsOverlapRatio_ );
      if ( !isOverlapped )
        continue;

      float ptRel = getPtRel( associatedOuterTk, matTk );
      if ( !std::isnan( ptRel ) and ptRel < minMatchingMetric ) {
        minMatchingMetric = ptRel;
        bestMatchedCand   = matCand;
      }
    }

    if ( bestMatchedCand.isNull() )
      continue;

    result.emplace( bestMatchedCand, srcCand );
  }

  return result;
}

bool
ff::MatcherByAssociatingRecoMuons::checkOverlap( const reco::TrackRef& src,
                                                 const reco::TrackRef& comp,
                                                 const float& minRatio ) const {
  if ( src.isNull() or comp.isNull() )
    return false;

  const reco::HitPattern& srcHp  = src->hitPattern();
  const reco::HitPattern& compHp = comp->hitPattern();

  std::vector<uint16_t> srcHits{};
  for ( int i( 0 ); i != srcHp.numberOfAllHits( reco::HitPattern::TRACK_HITS );
        ++i )
    srcHits.push_back( srcHp.getHitPattern( reco::HitPattern::TRACK_HITS, i ) );

  std::vector<uint16_t> compHits{};
  for ( int i( 0 ); i != compHp.numberOfAllHits( reco::HitPattern::TRACK_HITS );
        ++i )
    compHits.push_back(
        compHp.getHitPattern( reco::HitPattern::TRACK_HITS, i ) );

  return ff::calcOverlap<uint16_t>( srcHits, compHits ) >= minRatio;
}

float
ff::MatcherByAssociatingRecoMuons::getPtRel(
    const reco::TrackRef& src,
    const reco::TrackRef& comp ) const {
  if ( src.isNull() or comp.isNull() )
    return NAN;

  return fabs( src->pt() - comp->pt() ) / src->pt();
}