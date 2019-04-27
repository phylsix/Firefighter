#include "Firefighter/recoStuff/interface/CosmicMuonFilter.h"

CosmicMuonFilter::CosmicMuonFilter( const edm::ParameterSet& ps )
    : fMuonToken( consumes<reco::MuonCollection>(
          ps.getParameter<edm::InputTag>( "muons" ) ) ) {}

bool
CosmicMuonFilter::filter( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  e.getByToken( fMuonToken, fMuonHdl );
  assert( fMuonHdl.isValid() );

  const auto& muons = *fMuonHdl;

  // exact 2 muons
  if ( count_if( muons.begin(), muons.end(), []( const auto& mu ) {
         return mu.outerTrack().isNonnull() and
                mu.outerTrack()->extra().isNonnull();
       } ) != 2 )
    return false;

  // one upper hemisphere; one lower hemisphere
  int nUpperHemisphere( 0 );
  int nLowerHemisphere( 0 );
  for ( const auto& mu : muons ) {
    if ( mu.outerTrack().isNull() )
      continue;
    if ( mu.outerTrack()->extra().isNull() )
      continue;

    if ( mu.outerTrack()->outerY() > 0 )
      nUpperHemisphere++;
    if ( mu.outerTrack()->outerY() < 0 )
      nLowerHemisphere++;
  }

  return nUpperHemisphere == 1 and nLowerHemisphere == 1;
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( CosmicMuonFilter );