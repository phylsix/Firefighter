#include "Firefighter/ffEvtFilters/interface/LeptonJetPairFilter.h"

#include <cmath>

#include "DataFormats/Math/interface/deltaPhi.h"
#include "Firefighter/recoStuff/interface/ffPFJetProcessors.h"

LeptonJetPairFilter::LeptonJetPairFilter( const edm::ParameterSet& ps )
    : fJetToken( consumes<reco::PFJetCollection>( ps.getParameter<edm::InputTag>( "src" ) ) ),
      fTrackToken( consumes<reco::TrackCollection>( edm::InputTag( "generalTracks" ) ) ),
      fDPhiMin( ps.getParameter<double>( "minDPhi" ) ) {
  assert( fDPhiMin >= 0 and fDPhiMin < M_PI );
}

bool
LeptonJetPairFilter::filter( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  e.getByToken( fJetToken, fJetHdl );
  assert( fJetHdl.isValid() );
  e.getByToken( fTrackToken, fTrackHdl );
  assert( fTrackHdl.isValid() );

  if ( fJetHdl->size() < 2 )
    return false;

  vector<Ptr<reco::PFJet>> jetptrs{};
  for ( size_t i( 0 ); i != fJetHdl->size(); ++i )
    jetptrs.emplace_back( fJetHdl, i );

  sort( jetptrs.begin(), jetptrs.end(), []( const auto& lhs, const auto& rhs ) {
    return lhs->pt() > rhs->pt();
  } );

  // signal selections
  bool dphi_( fabs( deltaPhi( jetptrs[ 0 ]->phi(), jetptrs[ 1 ]->phi() ) ) > fDPhiMin );  // dphi>pi/2
  bool neutralLj0_( ff::muonChargeNeutral( *( jetptrs[ 0 ] ), fTrackHdl ) );              // leading lj is muon charge neutral
  bool neutralLj1_( ff::muonChargeNeutral( *( jetptrs[ 1 ] ), fTrackHdl ) );              // subleading lj is muon charge neutral

  return dphi_ && neutralLj0_ && neutralLj1_;
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( LeptonJetPairFilter );