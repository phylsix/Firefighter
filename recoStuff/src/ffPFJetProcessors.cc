#include "Firefighter/recoStuff/interface/ffPFJetProcessors.h"

#include <algorithm>
#include <numeric>

#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "Firefighter/recoStuff/interface/RecoHelpers.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistance.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"

//-----------------------------------------------------------------------------

std::vector<reco::PFCandidatePtr>
ff::getPFCands( const reco::PFJet& jet ) {
  std::vector<reco::PFCandidatePtr> result = jet.getPFConstituents();
  result.erase(
      std::remove_if( result.begin(), result.end(),
                      []( const auto& cand ) { return cand.isNull(); } ),
      result.end() );

  return result;
}

//-----------------------------------------------------------------------------

std::vector<reco::PFCandidatePtr>
ff::getChargedPFCands( const reco::PFJet& jet ) {
  std::vector<reco::PFCandidatePtr> result = getPFCands( jet );
  result.erase(
      std::remove_if( result.begin(), result.end(),
                      []( const auto& cand ) { return cand->charge() == 0; } ),
      result.end() );

  return result;
}

//-----------------------------------------------------------------------------

std::vector<reco::PFCandidatePtr>
ff::getTrackEmbededPFCands( const reco::PFJet& jet ) {
  std::vector<reco::PFCandidatePtr> result = getChargedPFCands( jet );
  result.erase( std::remove_if( result.begin(), result.end(),
                                []( const auto& cand ) {
                                  return cand->bestTrack() == nullptr;
                                } ),
                result.end() );

  return result;
}

//-----------------------------------------------------------------------------

float
ff::getMinDistAnyTwoTracks( const reco::PFJet& jet, const edm::EventSetup& es ) {
  std::vector<reco::PFCandidatePtr> candsWithTrack = getTrackEmbededPFCands( jet );
  if ( candsWithTrack.size() < 2 ) return -1;

  edm::ESHandle<MagneticField> field_h;
  es.get<IdealMagneticFieldRecord>().get( field_h );
  assert( field_h.isValid() );
  const MagneticField* bField = field_h.product();

  float minDist( 999. );
  for ( size_t i( 0 ); i != candsWithTrack.size(); i++ ) {
    const auto& tk_i = *( candsWithTrack[ i ]->bestTrack() );
    for ( size_t j( i + 1 ); j != candsWithTrack.size(); j++ ) {
      const auto& tk_j = *( candsWithTrack[ j ]->bestTrack() );

      TwoTrackMinimumDistance ttmd;

      bool status = ttmd.calculate( trajectoryStateTransform::initialFreeState( tk_i, bField ),
                                    trajectoryStateTransform::initialFreeState( tk_j, bField ) );
      if ( status ) {
        if ( ttmd.distance() < minDist )
          minDist = ttmd.distance();
      }
    }
  }

  // std::cout << "Two track min distance: " << minDist << std::endl;
  return minDist;
}

//-----------------------------------------------------------------------------

float
ff::getMaxDistAnyTwoTracks( const reco::PFJet& jet, const edm::EventSetup& es ) {
  std::vector<reco::PFCandidatePtr> candsWithTrack = getTrackEmbededPFCands( jet );
  if ( candsWithTrack.size() < 2 ) return -1;

  edm::ESHandle<MagneticField> field_h;
  es.get<IdealMagneticFieldRecord>().get( field_h );
  assert( field_h.isValid() );
  const MagneticField* bField = field_h.product();

  float maxDist( -1 );
  for ( size_t i( 0 ); i != candsWithTrack.size(); i++ ) {
    const auto& tk_i = *( candsWithTrack[ i ]->bestTrack() );
    for ( size_t j( i + 1 ); j != candsWithTrack.size(); j++ ) {
      const auto& tk_j = *( candsWithTrack[ j ]->bestTrack() );

      TwoTrackMinimumDistance ttmd;

      bool status = ttmd.calculate( trajectoryStateTransform::initialFreeState( tk_i, bField ),
                                    trajectoryStateTransform::initialFreeState( tk_j, bField ) );
      if ( status ) {
        if ( ttmd.distance() > maxDist )
          maxDist = ttmd.distance();
      }
    }
  }

  // std::cout << "Two track max distance: " << minDist << std::endl;
  return maxDist;
}

//-----------------------------------------------------------------------------

std::vector<const reco::Track*>
ff::getSelectedTracks(
    const reco::PFJet&                          jet,
    const StringCutObjectSelector<reco::Track>& tkSelector ) {
  std::vector<const reco::Track*> result{};
  result.reserve( jet.chargedMultiplicity() );
  for ( const auto& cand : getTrackEmbededPFCands( jet ) ) {
    const reco::Track* tk = cand->bestTrack();
    if ( tkSelector( *tk ) ) {
      result.push_back( tk );
    }
  }

  return result;
}

//-----------------------------------------------------------------------------

float
ff::chargedMass( const reco::PFJet& jet ) {
  const auto cands = getChargedPFCands( jet );
  if ( cands.empty() )
    return 0.;

  auto result = ( *begin( cands ) )->p4();
  for ( auto canditer( next( begin( cands ) ) ); canditer != end( cands );
        ++canditer )
    result += ( *canditer )->p4();
  return result.M();
}

//-----------------------------------------------------------------------------

int
ff::sumCharge( const reco::PFJet& jet ) {
  std::vector<reco::PFCandidatePtr> cands = getPFCands( jet );

  int result( 0 );
  for ( const auto& c : cands )
    result += c->charge();
  return result;
}

//-----------------------------------------------------------------------------

int
ff::sumPFMuonCharge( const reco::PFJet& jet ) {
  std::vector<reco::PFCandidatePtr> cands = getPFCands( jet );

  int result( 0 );
  for ( const auto& c : cands ) {
    if ( c->particleId() != reco::PFCandidate::mu ) continue;
    result += c->charge();
  }
  return result;
}

//-----------------------------------------------------------------------------

bool
ff::muonInTime( const reco::PFJet& jet, float timeLimit ) {
  // collect muon timing
  std::vector<float> muontimes{};
  for ( const auto& cand : getPFCands( jet ) ) {
    if ( cand->muonRef().isNonnull() and cand->muonRef()->isTimeValid() ) {
      muontimes.push_back( cand->muonRef()->time().timeAtIpInOut );
    }
  }

  // no muons in cands => muonInTime!
  if ( muontimes.empty() )
    return true;

  float muontimeMean =
      std::accumulate( muontimes.begin(), muontimes.end(), 0. ) /
      muontimes.size();
  bool result( true );

  // any time diff larger than limit => break loop
  for ( const auto& t : muontimes ) {
    if ( fabs( t - muontimeMean ) > timeLimit ) {
      result = false;
      break;
    }
  }

  return result;
}

//-----------------------------------------------------------------------------

int
ff::getNumberOfDisplacedStandAloneMuons(
    const reco::PFJet&                        jet,
    const edm::Handle<reco::TrackCollection>& generalTkH ) {
  std::vector<reco::PFCandidatePtr> candsWithTk = getTrackEmbededPFCands( jet );
  return std::count_if( candsWithTk.begin(), candsWithTk.end(),
                        [&generalTkH]( auto cand ) {
                          return cand->trackRef().isNonnull() and
                                 cand->trackRef().id() != generalTkH.id();
                        } );
}

//-----------------------------------------------------------------------------

float
ff::getTkPtSumInCone( const reco::PFJet&                        jet,
                      const edm::Handle<reco::TrackCollection>& tkH,
                      const reco::VertexCollection&             vertices,
                      const float&                              isoRadius ) {
  std::vector<reco::TrackRef> generalTkRefs{};
  for ( size_t i( 0 ); i != tkH->size(); ++i ) {
    reco::TrackRef tk( tkH, i );
    if ( associateWithPrimaryVertex( tk, vertices, true ) )
      generalTkRefs.push_back( tk );
  }

  std::vector<reco::PFCandidatePtr> cands = getTrackEmbededPFCands( jet );

  float ptsum( 0. );
  for ( const auto& tkRef : generalTkRefs ) {
    if ( deltaR( jet, *tkRef ) > isoRadius )
      continue;  // outside radius

    if ( std::find_if( cands.begin(), cands.end(), [&tkRef]( const auto& c ) {
           return c->trackRef() == tkRef;
         } ) != cands.end() )
      continue;  // associated with the jet

    ptsum += tkRef->pt();
  }

  return ptsum;
}

//-----------------------------------------------------------------------------

float
ff::getTkPtRawSumInCone( const reco::PFJet&                        jet,
                         const edm::Handle<reco::TrackCollection>& tkH,
                         const float&                              isoRadius,
                         const float                               minPt ) {
  std::vector<reco::PFCandidatePtr> cands = getTrackEmbededPFCands( jet );

  float tkPtRawSum( 0. );
  for ( size_t i( 0 ); i != tkH->size(); ++i ) {
    reco::TrackRef tk( tkH, i );
    if ( tk->pt() < minPt ) continue;                // less than min pT cut
    if ( deltaR( jet, *tk ) > isoRadius ) continue;  // outside radius
    if ( std::find_if( cands.begin(), cands.end(), [&tk]( const auto& c ) {
           return c->trackRef() == tk;
         } ) != cands.end() )
      continue;  // associated with the jet

    tkPtRawSum += tk->pt();
  }

  return tkPtRawSum;
}

//-----------------------------------------------------------------------------

float
ff::getTkIsolation( const reco::PFJet&                        jet,
                    const edm::Handle<reco::TrackCollection>& tkH,
                    const reco::VertexCollection&             vertices,
                    const float&                              isoRadius ) {
  std::vector<reco::PFCandidatePtr> cands = getTrackEmbededPFCands( jet );

  float notOfCands = getTkPtSumInCone( jet, tkH, vertices, isoRadius );
  float ofCands =
      std::accumulate( cands.begin(), cands.end(), 0.,
                       []( float ptsum, const reco::PFCandidatePtr& jc ) {
                         return ptsum + jc->bestTrack()->pt();
                       } );

  return ( ofCands + notOfCands ) == 0 ? NAN
                                       : notOfCands / ( ofCands + notOfCands );
}

//-----------------------------------------------------------------------------

float
ff::getPfIsolation( const reco::PFJet&                              jet,
                    const edm::Handle<reco::PFCandidateCollection>& pfH,
                    const float&                                    isoRadius ) {
  std::vector<reco::PFCandidatePtr> pfCandPtrs{};
  for ( size_t i( 0 ); i != pfH->size(); ++i )
    pfCandPtrs.emplace_back( pfH, i );

  std::vector<reco::PFCandidatePtr> jetcands = getPFCands( jet );

  float notOfCands( 0. );
  for ( const auto& cand : pfCandPtrs ) {
    if ( deltaR( jet, *cand ) > isoRadius )
      continue;  // outside radius

    if ( std::find_if( jetcands.begin(), jetcands.end(),
                       [&cand]( const auto& jc ) { return jc == cand; } ) !=
         jetcands.end() )
      continue;  // associated with the jet

    notOfCands += cand->energy();
  }

  float ofCands =
      std::accumulate( jetcands.begin(), jetcands.end(), 0.,
                       []( float esum, const reco::PFCandidatePtr& jc ) {
                         return esum + jc->energy();
                       } );

  return ( ofCands + notOfCands ) == 0 ? NAN
                                       : notOfCands / ( ofCands + notOfCands );
}

//-----------------------------------------------------------------------------

float
ff::getPfIsolation( const reco::PFJet&                                jet,
                    const edm::Handle<reco::PFCandidateFwdPtrVector>& pfH,
                    const float&                                      isoRadius ) {
  std::vector<reco::PFCandidatePtr> pfCandPtrs{};
  for ( const auto& cand : *pfH )
    pfCandPtrs.emplace_back( cand.ptr() );

  std::vector<reco::PFCandidatePtr> jetcands = getPFCands( jet );

  float notOfCands( 0. );
  for ( const auto& cand : pfCandPtrs ) {
    if ( deltaR( jet, *cand ) > isoRadius )
      continue;  // outside radius

    if ( std::find_if( jetcands.begin(), jetcands.end(),
                       [&cand]( const auto& jc ) { return jc == cand; } ) !=
         jetcands.end() )
      continue;  // associated with the jet

    notOfCands += cand->energy();
  }

  float ofCands =
      std::accumulate( jetcands.begin(), jetcands.end(), 0.,
                       []( float esum, const reco::PFCandidatePtr& jc ) {
                         return esum + jc->energy();
                       } );

  return ( ofCands + notOfCands ) == 0 ? NAN
                                       : notOfCands / ( ofCands + notOfCands );
}

//-----------------------------------------------------------------------------

float
ff::getNeutralIsolation( const reco::PFJet&                              jet,
                         const edm::Handle<reco::PFCandidateCollection>& pfH,
                         const float&                                    isoRadius ) {
  std::vector<reco::PFCandidatePtr> pfCandPtrs{};
  for ( size_t i( 0 ); i != pfH->size(); ++i )
    pfCandPtrs.emplace_back( pfH, i );

  std::vector<reco::PFCandidatePtr> jetcands = getPFCands( jet );

  float notOfCands( 0. );
  for ( const auto& cand : pfCandPtrs ) {
    if ( cand->charge() != 0 )
      continue;  // charged
    if ( deltaR( jet, *cand ) > isoRadius )
      continue;  // outside radius

    if ( std::find_if( jetcands.begin(), jetcands.end(),
                       [&cand]( const auto& jc ) { return jc == cand; } ) !=
         jetcands.end() )
      continue;  // associated with the jet

    notOfCands += cand->energy();
  }

  float ofCands =
      std::accumulate( jetcands.begin(), jetcands.end(), 0.,
                       []( float esum, const reco::PFCandidatePtr& jc ) {
                         return esum + jc->energy();
                       } );

  return ( ofCands + notOfCands ) == 0 ? NAN
                                       : notOfCands / ( ofCands + notOfCands );
}

//-----------------------------------------------------------------------------

float
ff::getNeutralIsolation( const reco::PFJet&                                jet,
                         const edm::Handle<reco::PFCandidateFwdPtrVector>& pfH,
                         const float&                                      isoRadius ) {
  std::vector<reco::PFCandidatePtr> pfCandPtrs{};
  for ( const auto& cand : *pfH )
    pfCandPtrs.emplace_back( cand.ptr() );

  std::vector<reco::PFCandidatePtr> jetcands = getPFCands( jet );

  float notOfCands( 0. );
  for ( const auto& cand : pfCandPtrs ) {
    if ( cand->charge() != 0 )
      continue;  // charged
    if ( deltaR( jet, *cand ) > isoRadius )
      continue;  // outside radius

    if ( std::find_if( jetcands.begin(), jetcands.end(),
                       [&cand]( const auto& jc ) { return jc == cand; } ) !=
         jetcands.end() )
      continue;  // associated with the jet

    notOfCands += cand->energy();
  }

  float ofCands =
      std::accumulate( jetcands.begin(), jetcands.end(), 0.,
                       []( float esum, const reco::PFCandidatePtr& jc ) {
                         return esum + jc->energy();
                       } );

  return ( ofCands + notOfCands ) == 0 ? NAN
                                       : notOfCands / ( ofCands + notOfCands );
}

//-----------------------------------------------------------------------------

float
ff::getHadronIsolation( const reco::PFJet&                              jet,
                        const edm::Handle<reco::PFCandidateCollection>& pfH,
                        const float&                                    isoRadius ) {
  std::vector<reco::PFCandidatePtr> pfCandPtrs{};
  for ( size_t i( 0 ); i != pfH->size(); ++i )
    pfCandPtrs.emplace_back( pfH, i );

  std::vector<reco::PFCandidatePtr> jetcands = getPFCands( jet );

  double numer( 0. );
  for ( const auto& cand : pfCandPtrs ) {
    auto candpid = cand->particleId();
    if ( candpid == reco::PFCandidate::e or candpid == reco::PFCandidate::mu or candpid == reco::PFCandidate::gamma )
      continue;  // nonelep
    if ( deltaR( jet, *cand ) > isoRadius )
      continue;  // outside radius

    numer += cand->energy();
  }

  return ( jet.energy() ) == 0 ? NAN : float( numer / jet.energy() );
}

//-----------------------------------------------------------------------------

float
ff::getHadronIsolation( const reco::PFJet&                                jet,
                        const edm::Handle<reco::PFCandidateFwdPtrVector>& pfH,
                        const float&                                      isoRadius ) {
  std::vector<reco::PFCandidatePtr> pfCandPtrs{};
  for ( const auto& cand : *pfH )
    pfCandPtrs.emplace_back( cand.ptr() );

  std::vector<reco::PFCandidatePtr> jetcands = getPFCands( jet );

  double numer( 0. );
  for ( const auto& cand : pfCandPtrs ) {
    auto candpid = cand->particleId();
    if ( candpid == reco::PFCandidate::e or candpid == reco::PFCandidate::mu or candpid == reco::PFCandidate::gamma )
      continue;  // nonelep
    if ( deltaR( jet, *cand ) > isoRadius )
      continue;  // outside radius

    numer += cand->energy();
  }

  return ( jet.energy() ) == 0 ? NAN : float( numer / jet.energy() );
}

//-----------------------------------------------------------------------------

std::vector<reco::TransientTrack>
ff::transientTracksFromPFJet(
    const reco::PFJet&                          jet,
    const StringCutObjectSelector<reco::Track>& tkSelector,
    const edm::EventSetup&                      es ) {
  std::vector<reco::PFCandidatePtr> cands = getTrackEmbededPFCands( jet );

  edm::ESHandle<TransientTrackBuilder> theB;
  es.get<TransientTrackRecord>().get( "TransientTrackBuilder", theB );

  std::vector<reco::TransientTrack> t_tks{};
  for ( const auto& c : cands ) {
    if ( !tkSelector( *( c->bestTrack() ) ) )
      continue;
    reco::TransientTrack tt = theB->build( c );
    if ( !tt.isValid() )
      continue;
    t_tks.push_back( tt );
  }

  return t_tks;
}

//-----------------------------------------------------------------------------

bool
ff::muonChargeNeutral( const reco::PFJet&                        jet,
                       const edm::Handle<reco::TrackCollection>& tkH ) {
  int sum_charge( 0 );
  for ( const auto& cand : getPFCands( jet ) ) {
    if ( getCandType( cand, tkH ) == 3 ) sum_charge += cand->charge();
    if ( getCandType( cand, tkH ) == 8 ) sum_charge += cand->charge();
  }

  return sum_charge == 0;
}
