#include "Firefighter/recoStuff/interface/MatcherByExtrapolatingTracks.h"

#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"

#include <algorithm>

ff::MatcherByExtrapolatingTracks::MatcherByExtrapolatingTracks(
    const edm::ParameterSet& ps ) {
  maxLocalPosDiff_      = ps.getParameter<double>( "maxDeltaLocalPos" );
  maxGlobalMomDeltaR_   = ps.getParameter<double>( "maxDeltaR" );
  maxGlobalMomDeltaEta_ = ps.existsAs<double>( "maxDeltaEta" )
                              ? ps.getParameter<double>( "maxDeltaEta" )
                              : maxGlobalMomDeltaR_;
  maxGlobalMomDeltaPhi_ = ps.existsAs<double>( "maxDeltaPhi" )
                              ? ps.getParameter<double>( "maxDeltaPhi" )
                              : maxGlobalMomDeltaR_;
  maxGlobalDPtRel_   = ps.getParameter<double>( "maxDeltaPtRel" );
  requireSameCharge_ = ps.existsAs<bool>( "requireSameCharge" )
                           ? ps.getParameter<bool>( "requireSameCharge" )
                           : false;
  requireOuterInRange_ = ps.existsAs<bool>( "requireOuterInRange" )
                             ? ps.getParameter<bool>( "requireOuterInRange" )
                             : false;

  std::string sortby = ps.getParameter<std::string>( "sortBy" );
  if ( sortby == "deltaLocalPos" ) {
    sortBy_ = LocalPosDiff;
  } else if ( sortby == "deltaPtRel" ) {
    sortBy_ = GlobalDPtRel;
  } else if ( sortby == "deltaR" ) {
    sortBy_ = GlobalMomDeltaR;
  } else {
    throw cms::Exception( "Configuration" )
        << "Parameter 'sortBy' must be one of: deltaLocalPos, deltaPtRel, "
           "deltaR.\n";
  }
}

void
ff::MatcherByExtrapolatingTracks::init( const edm::EventSetup& es ) {
  tkExtrp_ = std::make_unique<ff::TrackExtrapolator>( es );
}

std::map<reco::PFCandidatePtr, reco::PFCandidatePtr>
ff::MatcherByExtrapolatingTracks::match(
    const std::vector<reco::PFCandidatePtr>& srcCol,
    const std::vector<reco::PFCandidatePtr>& matchCol ) const {
  std::map<reco::PFCandidatePtr, reco::PFCandidatePtr>
      result{};  // dSA --> particleFlow

  /// copy and sort input collection by pT from high to low
  std::vector<reco::PFCandidatePtr> sortedSrcCol{};
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

    FreeTrajectoryState startFTS = trajectoryStateTransform::initialFreeState(
        *srcTk, tkExtrp_->getMagneticField() );

    float                minMatchingMetric( 999. );
    reco::PFCandidatePtr bestMatchedCand;

    for ( const auto& matCand : matchCol ) {
      if ( matCand.isNull() )
        continue;
      const reco::TrackRef matTk = matCand->trackRef();
      if ( matTk.isNull() )
        continue;

      if ( result.find( matCand ) != result.end() )
        continue;

      if ( requireSameCharge_ and srcCand->charge() != matCand->charge() )
        continue;

      TrajectoryStateOnSurface targetInner =
          trajectoryStateTransform::innerStateOnSurface(
              *matTk, *( tkExtrp_->getTrackingGeometry() ),
              tkExtrp_->getMagneticField() );
      TrajectoryStateOnSurface targetOuter =
          trajectoryStateTransform::outerStateOnSurface(
              *matTk, *( tkExtrp_->getTrackingGeometry() ),
              tkExtrp_->getMagneticField() );

      if ( !targetInner.isValid() or !targetOuter.isValid() )
        continue;

      TrajectoryStateOnSurface tsosInner =
          tkExtrp_->propagate( startFTS, targetInner.surface() );
      TrajectoryStateOnSurface tsosOuter =
          tkExtrp_->propagate( startFTS, targetOuter.surface() );

      if ( !tsosInner.isValid() or !tsosOuter.isValid() )
        continue;

      float innerLocalPosDiff =
          ( tsosInner.localPosition() - targetInner.localPosition() ).mag();
      float innerGlobalMomDeltaR =
          deltaR( tsosInner.globalMomentum(), targetInner.globalMomentum() );
      float innerGlobalMomDeltaPhi =
          fabs( deltaPhi( tsosInner.globalMomentum().barePhi(),
                          targetInner.globalMomentum().barePhi() ) );
      float innerGlobalMomDeltaEta = fabs( tsosInner.globalMomentum().eta() -
                                           targetInner.globalMomentum().eta() );
      float innerGlobalDPtRel      = fabs( tsosInner.globalMomentum().perp() -
                                      targetInner.globalMomentum().perp() ) /
                                targetInner.globalMomentum().perp();

      if ( innerLocalPosDiff > maxLocalPosDiff_ or
           innerGlobalMomDeltaR > maxGlobalMomDeltaR_ or
           innerGlobalMomDeltaPhi > maxGlobalMomDeltaPhi_ or
           innerGlobalMomDeltaEta > maxGlobalMomDeltaEta_ or
           innerGlobalDPtRel > maxGlobalDPtRel_ )
        continue;

      float outerLocalPosDiff =
          ( tsosOuter.localPosition() - targetOuter.localPosition() ).mag();
      float outerGlobalMomDeltaR =
          deltaR( tsosOuter.globalMomentum(), targetOuter.globalMomentum() );
      float outerGlobalMomDeltaPhi =
          fabs( deltaPhi( tsosOuter.globalMomentum().barePhi(),
                          targetOuter.globalMomentum().barePhi() ) );
      float outerGlobalMomDeltaEta = fabs( tsosOuter.globalMomentum().eta() -
                                           targetOuter.globalMomentum().eta() );
      float outerGlobalDPtRel      = fabs( tsosOuter.globalMomentum().perp() -
                                      targetOuter.globalMomentum().perp() ) /
                                targetOuter.globalMomentum().perp();

      if ( requireOuterInRange_ and
           ( outerLocalPosDiff > maxLocalPosDiff_ or
             outerGlobalMomDeltaR > maxGlobalMomDeltaR_ or
             outerGlobalMomDeltaPhi > maxGlobalMomDeltaPhi_ or
             outerGlobalMomDeltaEta > maxGlobalMomDeltaEta_ or
             outerGlobalDPtRel > maxGlobalDPtRel_ ) )
        continue;

      float matchingMetric( 999. );
      switch ( sortBy_ ) {
        case LocalPosDiff:
          matchingMetric = ( innerLocalPosDiff + outerLocalPosDiff ) / 2;
          break;
        case GlobalMomDeltaR:
          matchingMetric = ( innerGlobalMomDeltaR + outerGlobalMomDeltaR ) / 2;
          break;
        case GlobalDPtRel:
          matchingMetric = ( innerGlobalDPtRel + outerGlobalDPtRel ) / 2;
          break;
        default:
          break;
      }

      if ( matchingMetric < minMatchingMetric ) {
        minMatchingMetric = matchingMetric;
        bestMatchedCand   = matCand;
      }
    }

    if ( bestMatchedCand.isNull() )
      continue;

    result.emplace( bestMatchedCand, srcCand );
  }

  return result;
}