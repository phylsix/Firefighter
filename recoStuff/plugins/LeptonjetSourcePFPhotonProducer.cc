#include "Firefighter/recoStuff/interface/LeptonjetSourcePFPhotonProducer.h"

#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/Math/interface/deltaR.h"

LeptonjetSourcePFPhotonProducer::LeptonjetSourcePFPhotonProducer( const edm::ParameterSet& ps )
    : fPFCandsToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "particleFlowPtrs" ) ) ),
      fIdName( ps.getParameter<std::string>( "idName" ) ),
      fPFPhotonIDToken( consumes<edm::ValueMap<bool>>( edm::InputTag( "egmPhotonIDs", fIdName ) ) ) {
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
    if ( candptr->particleId() != reco::PFCandidate::gamma ) continue;
    if ( candptr->photonRef().isNull() ) continue;
    const auto& supercluster = candptr->photonRef()->superCluster();
    if ( supercluster.isNull() ) continue;

    // kinematic cut; same with POG syst unc. lowest value
    if ( candptr->pt() < 20. ) continue;
    if ( fabs( supercluster->eta() ) > 2.5 ) continue;

    // photon id: cutbased-loose
    if ( !( *fPFPhotonIDHdl )[ refToPtr( candptr->photonRef() ) ] ) continue;

    inclusiveColl->push_back( candfwdptr );
  }

  e.put( move( inclusiveColl ) );
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( LeptonjetSourcePFPhotonProducer );
