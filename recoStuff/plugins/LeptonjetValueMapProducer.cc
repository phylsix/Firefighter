#include "Firefighter/recoStuff/interface/LeptonjetValueMapProducer.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

LeptonjetValueMapProducer::LeptonjetValueMapProducer( const edm::ParameterSet& ps )
    : fLeptonjetToken( consumes<reco::PFJetCollection>( ps.getParameter<edm::InputTag>( "src" ) ) ),
      fPFChargedHadronToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "pfAllChargedHadrons" ) ) ),
      fPFNeutralHadronToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "pfAllNeutralHadrons" ) ) ),
      fPFPhotonToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "pfAllPhotons" ) ) ),
      fPFPileUpToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "pfPileUpAllChargedParticles" ) ) ) {
  produces<edm::ValueMap<float>>( "pfIso" );
}

LeptonjetValueMapProducer::~LeptonjetValueMapProducer() = default;

void
LeptonjetValueMapProducer::produce( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  auto vm_pfiso = make_unique<ValueMap<float>>();

  e.getByToken( fLeptonjetToken, fLeptonjetHdl );
  assert( fLeptonjetHdl.isValid() );
  e.getByToken( fPFChargedHadronToken, fPFChargedHadronHdl );
  assert( fPFChargedHadronHdl.isValid() );
  e.getByToken( fPFNeutralHadronToken, fPFNeutralHadronHdl );
  assert( fPFNeutralHadronHdl.isValid() );
  e.getByToken( fPFPhotonToken, fPFPhotonHdl );
  assert( fPFPhotonHdl.isValid() );
  e.getByToken( fPFPileUpToken, fPFPileUpHdl );
  assert( fPFPileUpHdl.isValid() );

  vector<float> v_pfiso{};
  v_pfiso.reserve( fLeptonjetHdl->size() );

  for ( const auto& leptonjet : *fLeptonjetHdl ) {
    // sum pt charged hadron
    double sumPtChargedHadron( 0. );
    for ( const auto& cand : *fPFChargedHadronHdl ) {
      if ( deltaR( leptonjet, *( cand.ptr() ) ) > 0.4 )
        continue;
      sumPtChargedHadron += cand.ptr()->pt();
    }

    // sum Et neutral hadron
    double sumEtNeutralHadron( 0. );
    for ( const auto& cand : *fPFNeutralHadronHdl ) {
      if ( deltaR( leptonjet, *( cand.ptr() ) ) > 0.4 )
        continue;
      sumEtNeutralHadron += cand.ptr()->et();
    }

    // sum Et photon
    double sumEtPhoton( 0. );
    for ( const auto& cand : *fPFPhotonHdl ) {
      if ( deltaR( leptonjet, *( cand.ptr() ) ) > 0.4 )
        continue;
      sumEtPhoton += cand.ptr()->et();
    }

    // sum pt pileup
    double sumPtPileUp( 0. );
    for ( const auto& cand : *fPFPileUpHdl ) {
      if ( deltaR( leptonjet, *( cand.ptr() ) ) > 0.4 )
        continue;
      sumPtPileUp += cand.ptr()->pt();
    }

    v_pfiso.emplace_back( ( sumPtChargedHadron + max( 0., sumEtNeutralHadron + sumEtPhoton - 0.5 * sumPtPileUp ) ) / leptonjet.pt() );
  }

  ValueMap<float>::Filler pfIsoFiller( *vm_pfiso );
  pfIsoFiller.insert( fLeptonjetHdl, v_pfiso.begin(), v_pfiso.end() );
  pfIsoFiller.fill();
  e.put( move( vm_pfiso ), "pfIso" );
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( LeptonjetValueMapProducer );