#include "Firefighter/recoStuff/interface/LeptonjetSourcePFElectronProducer.h"

#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/Math/interface/deltaR.h"

LeptonjetSourcePFElectronProducer::LeptonjetSourcePFElectronProducer( const edm::ParameterSet& ps )
    : fPFCandsToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "particleFlowPtrs" ) ) ),
      fPFElectronIDToken( consumes<edm::ValueMap<bool>>( edm::InputTag( "egmGsfElectronIDs", "cutBasedElectronID-Fall17-94X-V2-loose" ) ) ) {
  produces<reco::PFCandidateFwdPtrVector>( "inclusive" );
  produces<reco::PFCandidateFwdPtrVector>( "nonisolated" );
}

LeptonjetSourcePFElectronProducer::~LeptonjetSourcePFElectronProducer() = default;

void
LeptonjetSourcePFElectronProducer::produce( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  auto inclusiveColl   = make_unique<reco::PFCandidateFwdPtrVector>();
  auto nonisolatedColl = make_unique<reco::PFCandidateFwdPtrVector>();

  e.getByToken( fPFCandsToken, fPFCandsHdl );
  assert( fPFCandsHdl.isValid() );
  e.getByToken( fPFElectronIDToken, fPFElectronIDHdl );
  assert( fPFElectronIDHdl.isValid() );

  for ( const auto& candfwdptr : *fPFCandsHdl ) {
    const auto& candptr = candfwdptr.ptr();

    // is PFElectron
    if ( candptr->particleId() != reco::PFCandidate::e )
      continue;
    if ( candptr->gsfElectronRef().isNull() )
      continue;

    // kinematic cut
    if ( candptr->pt() < 5. )
      continue;
    if ( fabs( candptr->eta() ) > 2.4 )
      continue;

    // electron id: cutbased-loose
    if ( !( *fPFElectronIDHdl )[ refToPtr( candptr->gsfElectronRef() ) ] )
      continue;

    inclusiveColl->push_back( candfwdptr );
  }

  for ( const auto& candfwdptr : *inclusiveColl ) {
    for ( const auto& candfwdptr2 : *inclusiveColl ) {
      if ( candfwdptr.ptr() == candfwdptr2.ptr() )
        continue;
      if ( deltaR( *( candfwdptr.ptr() ), *( candfwdptr2.ptr() ) ) > 0.4 )
        continue;
      if ( ( candfwdptr.ptr()->charge() * candfwdptr2.ptr()->charge() ) != -1 )
        continue;
      nonisolatedColl->push_back( candfwdptr );
      break;
    }
  }

  e.put( move( inclusiveColl ), "inclusive" );
  e.put( move( nonisolatedColl ), "nonisolated" );
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( LeptonjetSourcePFElectronProducer );
