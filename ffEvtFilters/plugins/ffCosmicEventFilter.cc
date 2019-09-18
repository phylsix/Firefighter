#include "Firefighter/ffEvtFilters/interface/ffCosmicEventFilter.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

ffCosmicEventFilter::ffCosmicEventFilter( const edm::ParameterSet& ps )
    : fCosmicToken( consumes<reco::TrackCollection>( ps.getParameter<edm::InputTag>( "src" ) ) ),
      fMinCosAlpha( ps.getParameter<double>( "minCosAlpha" ) ),
      fMaxPairCount( ps.getParameter<int>( "maxPairCount" ) ),
      fTaggingMode( ps.getParameter<bool>( "taggingMode" ) ) {
  assert( fMinCosAlpha >= 0. and fMinCosAlpha <= 1. );
  produces<bool>();
  produces<int>();
}

bool
ffCosmicEventFilter::filter( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  e.getByToken( fCosmicToken, fCosmicHdl );
  assert( fCosmicHdl.isValid() );
  const reco::TrackCollection& cosmicmuons = *fCosmicHdl;

  // unsigned int numparallelpair( 0 );
  fNumParallelPairs = 0;
  for ( size_t i( 0 ); i != cosmicmuons.size(); i++ ) {
    for ( size_t j( i + 1 ); j != cosmicmuons.size(); j++ ) {
      float cosalpha = cosmicmuons[ i ].momentum().Dot( cosmicmuons[ j ].momentum() );
      cosalpha /= cosmicmuons[ i ].momentum().R() * cosmicmuons[ j ].momentum().R();
      if ( fabs( cosalpha ) > fMinCosAlpha )
        fNumParallelPairs++;
    }
  }

  bool result = ( fNumParallelPairs < fMaxPairCount );

  e.put( make_unique<int>( fNumParallelPairs ) );
  e.put( make_unique<bool>( result ) );

  return fTaggingMode || result;
}

DEFINE_FWK_MODULE( ffCosmicEventFilter );