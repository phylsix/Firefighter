#include "Firefighter/ffEvtFilters/interface/ProxyEventFilter.h"

#include <numeric>

#include "Firefighter/recoStuff/interface/RecoHelpers.h"
#include "Firefighter/recoStuff/interface/ffPFJetProcessors.h"

ProxyEventFilter::ProxyEventFilter( const edm::ParameterSet& ps )
    : fJetToken( consumes<reco::PFJetCollection>( ps.getParameter<edm::InputTag>( "src" ) ) ),
      fLJSrcToken( consumes<reco::PFCandidateFwdPtrVector>( ps.getParameter<edm::InputTag>( "ljsrc" ) ) ),
      fTrackToken( consumes<reco::TrackCollection>( edm::InputTag( "generalTracks" ) ) ) {
}

bool
ProxyEventFilter::filter( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;
  using namespace ff;

  e.getByToken( fJetToken, fJetHdl );
  assert( fJetHdl.isValid() );
  e.getByToken( fLJSrcToken, fLJSrcHdl );
  assert( fLJSrcHdl.isValid() );
  e.getByToken( fTrackToken, fTrackHdl );
  assert( fTrackHdl.isValid() );

  /**
   * 4mu channel: only 1 muon-type LJ, 0 EGM-type LJ,  + one additional muon
   * 2mu2e channel: 0 muon-type LJ, >0 EGM-type LJ, + one additonal muon
   */

  // First, count how many LJs for each type
  vector<Ptr<reco::PFJet>> EGMLJPtrs{}, MuLJPtrs{};
  for ( size_t i( 0 ); i != fJetHdl->size(); ++i ) {
    const auto& jet = ( *fJetHdl )[ i ];
    vector<int> muonCharges{};
    for ( const auto& cand : getPFCands( jet ) ) {
      if ( getCandType( cand, fTrackHdl ) == 3 ) muonCharges.push_back( cand->charge() );
      if ( getCandType( cand, fTrackHdl ) == 8 ) muonCharges.push_back( cand->charge() );
    }

    if ( muonCharges.empty() )
      EGMLJPtrs.emplace_back( fJetHdl, i );
    else if ( accumulate( muonCharges.begin(), muonCharges.end(), 0 ) == 0 )
      MuLJPtrs.emplace_back( fJetHdl, i );
  }

  if ( MuLJPtrs.size() > 1 ) return false;  // 4mu channel signal region

  if ( MuLJPtrs.size() == 1 ) {  // 4mu channel proxy region
    if ( EGMLJPtrs.size() != 0 ) return false;

    const auto& muLJ = *( MuLJPtrs[ 0 ] );

    vector<reco::PFCandidatePtr> chargedDaughters = getChargedPFCands( muLJ );

    int numAdditionalMuons( 0 );
    for ( const auto& ljsrc : *fLJSrcHdl ) {
      const auto& cand = *( ljsrc.get() );
      if ( cand.particleId() != reco::PFCandidate::mu ) continue;
      if ( find( chargedDaughters.begin(), chargedDaughters.end(), ljsrc.ptr() ) == chargedDaughters.end() ) numAdditionalMuons++;
    }

    return numAdditionalMuons > 0;
  } else {  // 2mu2e channel proxy region
    if ( EGMLJPtrs.size() == 0 ) return false;

    int numAdditionalMuons( 0 );
    for ( const auto& ljsrc : *fLJSrcHdl ) {
      const auto& cand = *( ljsrc.get() );
      if ( cand.particleId() == reco::PFCandidate::mu ) numAdditionalMuons++;
    }

    return numAdditionalMuons > 0;
  }
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( ProxyEventFilter );