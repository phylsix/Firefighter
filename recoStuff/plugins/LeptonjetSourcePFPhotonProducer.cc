#include "Firefighter/recoStuff/interface/LeptonjetSourcePFPhotonProducer.h"

#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"

LeptonjetSourcePFPhotonProducer::LeptonjetSourcePFPhotonProducer( const edm::ParameterSet& ps )
    : fPFCandsToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "particleFlowPtrs" ) ) ),
      fPFPhotonIDToken( consumes<edm::ValueMap<bool>>( edm::InputTag( "egmPhotonIDs", "cutBasedPhotonID-Fall17-94X-V2-loose" ) ) ) {
  produces<reco::PFCandidateFwdPtrVector>();
}

LeptonjetSourcePFPhotonProducer::~LeptonjetSourcePFPhotonProducer() = default;

void
LeptonjetSourcePFPhotonProducer::produce( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  auto inclusiveColl = make_unique<reco::PFCandidateFwdPtrVector>();

  e.getByToken( fPFCandsToken, fPFCandsHdl );
  assert( fPFCandsHdl.isValid() );
  e.getByToken( fPFPhotonIDToken, fPFPhotonIDHdl );
  assert( fPFPhotonIDHdl.isValid() );

  for ( const auto& candfwdptr : *fPFCandsHdl ) {
    const auto& candptr = candfwdptr.ptr();

    // is PFPhoton
    if ( candptr->particleId() != reco::PFCandidate::gamma )
      continue;
    if ( candptr->photonRef().isNull() )
      continue;

    // photon id: cutbased-loose
    if ( !( *fPFPhotonIDHdl )[ refToPtr( candptr->photonRef() ) ] )
      continue;

    inclusiveColl->push_back( candfwdptr );
  }

  e.put( move( inclusiveColl ) );
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( LeptonjetSourcePFPhotonProducer );
