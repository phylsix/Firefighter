#include "Firefighter/recoStuff/interface/LeptonjetSourcePFMuonProducer.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/MuonReco/interface/Muon.h"

LeptonjetSourcePFMuonProducer::LeptonjetSourcePFMuonProducer( const edm::ParameterSet& ps )
    : fPFCandsToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "particleFlowPtrs" ) ) ) {
  produces<reco::PFCandidateFwdPtrVector>( "inclusive" );
  produces<reco::PFCandidateFwdPtrVector>( "nonisolated" );
}

LeptonjetSourcePFMuonProducer::~LeptonjetSourcePFMuonProducer() = default;

void
LeptonjetSourcePFMuonProducer::produce( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  auto inclusiveColl   = make_unique<reco::PFCandidateFwdPtrVector>();
  auto nonisolatedColl = make_unique<reco::PFCandidateFwdPtrVector>();

  e.getByToken( fPFCandsToken, fPFCandsHdl );
  assert( fPFCandsHdl.isValid() );

  for ( const auto& candfwdptr : *fPFCandsHdl ) {
    const auto& candptr = candfwdptr.ptr();

    // is PFMuon
    if ( candptr->particleId() != reco::PFCandidate::mu ) continue;
    if ( candptr->muonRef().isNull() ) continue;

    // kinematic cut
    if ( candptr->pt() < 5. ) continue;
    if ( fabs( candptr->eta() ) > 2.4 ) continue;

    // muon id + iso cut
    const auto& muon = *( candptr->muonRef() );
    if ( !muon.passed( reco::Muon::CutBasedIdLoose ) ) continue;
    // if ( !muon.passed( reco::Muon::PFIsoLoose ) ) continue;

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

DEFINE_FWK_MODULE( LeptonjetSourcePFMuonProducer );
