#include "Firefighter/recoStuff/interface/LeptonjetValueMapProducer.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

LeptonjetValueMapProducer::LeptonjetValueMapProducer( const edm::ParameterSet& ps )
    : fLeptonjetToken( consumes<reco::PFJetCollection>( ps.getParameter<edm::InputTag>( "src" ) ) ),
      fAKJetToken( consumes<reco::PFJetCollection>( ps.getParameter<edm::InputTag>( "ak4jet" ) ) ),
      fJetCorrectorToken( consumes<reco::JetCorrector>( ps.getParameter<edm::InputTag>( "jetCorrector" ) ) ),
      fPFChargedHadronToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "pfAllChargedHadrons" ) ) ),
      fPFNeutralHadronToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "pfAllNeutralHadrons" ) ) ),
      fPFPhotonToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "pfAllPhotons" ) ) ),
      fPFPileUpToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "pfPileUpAllChargedParticles" ) ) ),
      fJetIdSelector( ps.getParameter<std::string>( "jetid" ), true ) {
  produces<edm::ValueMap<float>>( "pfIso" );
  produces<edm::ValueMap<float>>( "minDeltaR" );
  produces<edm::ValueMap<bool>>( "cleaned" );
}

LeptonjetValueMapProducer::~LeptonjetValueMapProducer() = default;

void
LeptonjetValueMapProducer::produce( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  auto vm_pfiso   = make_unique<ValueMap<float>>();
  auto vm_mindr   = make_unique<ValueMap<float>>();
  auto vm_cleaned = make_unique<ValueMap<bool>>();

  e.getByToken( fLeptonjetToken, fLeptonjetHdl );
  assert( fLeptonjetHdl.isValid() );
  e.getByToken( fAKJetToken, fAK4JetHdl );
  assert( fAK4JetHdl.isValid() );
  e.getByToken( fJetCorrectorToken, fJetCorrectorHdl );
  assert( fJetCorrectorHdl.isValid() );
  e.getByToken( fPFChargedHadronToken, fPFChargedHadronHdl );
  assert( fPFChargedHadronHdl.isValid() );
  e.getByToken( fPFNeutralHadronToken, fPFNeutralHadronHdl );
  assert( fPFNeutralHadronHdl.isValid() );
  e.getByToken( fPFPhotonToken, fPFPhotonHdl );
  assert( fPFPhotonHdl.isValid() );
  e.getByToken( fPFPileUpToken, fPFPileUpHdl );
  assert( fPFPileUpHdl.isValid() );

  /*_______________________vm_pfiso______________________*/
  vector<float> v_pfiso{};
  v_pfiso.reserve( fLeptonjetHdl->size() );
  for ( const auto& leptonjet : *fLeptonjetHdl ) {
    vector<reco::PFCandidatePtr> leptonjetcands = leptonjet.getPFConstituents();

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
      if ( find( leptonjetcands.begin(), leptonjetcands.end(), cand.ptr() ) != leptonjetcands.end() )
        continue;
      sumEtPhoton += cand.ptr()->et();
    }

    // sum pt pileup
    double sumPtPileUp( 0. );
    for ( const auto& cand : *fPFPileUpHdl ) {
      if ( deltaR( leptonjet, *( cand.ptr() ) ) > 0.4 )
        continue;
      if ( find( leptonjetcands.begin(), leptonjetcands.end(), cand.ptr() ) != leptonjetcands.end() )
        continue;
      sumPtPileUp += cand.ptr()->pt();
    }

    v_pfiso.emplace_back( ( sumPtChargedHadron + max( 0., sumEtNeutralHadron + sumEtPhoton - 0.5 * sumPtPileUp ) ) / leptonjet.pt() );
  }
  ValueMap<float>::Filler pfIsoFiller( *vm_pfiso );
  pfIsoFiller.insert( fLeptonjetHdl, v_pfiso.begin(), v_pfiso.end() );
  pfIsoFiller.fill();
  e.put( move( vm_pfiso ), "pfIso" );

  /*_______________________vm_mindr______________________*/
  vector<float> v_mindr( fLeptonjetHdl->size(), 999. );
  for ( size_t i( 0 ); i != fLeptonjetHdl->size(); i++ ) {
    float       _mindr( 999. );
    const auto& lj0 = ( *fLeptonjetHdl )[ i ];
    for ( size_t j( 0 ); j != fLeptonjetHdl->size(); j++ ) {
      if ( i == j )
        continue;
      const auto& lj1 = ( *fLeptonjetHdl )[ j ];
      float       _dr = deltaR( lj0, lj1 );
      if ( _dr > _mindr )
        continue;
      _mindr = _dr;
    }
    v_mindr[ i ] = _mindr;
  }
  ValueMap<float>::Filler minDrFiller( *vm_mindr );
  minDrFiller.insert( fLeptonjetHdl, v_mindr.begin(), v_mindr.end() );
  minDrFiller.fill();
  e.put( move( vm_mindr ), "minDeltaR" );

  /*_______________________vm_cleaned______________________*/
  vector<bool>             v_cleaned( fLeptonjetHdl->size(), true );
  vector<Ptr<reco::PFJet>> ak4jets{};
  for ( size_t i( 0 ); i != fAK4JetHdl->size(); i++ ) {
    if ( fJetIdSelector( ( *fAK4JetHdl )[ i ] ) )
      ak4jets.emplace_back( fAK4JetHdl, i );
  }
  for ( size_t i( 0 ); i != fLeptonjetHdl->size(); i++ ) {
    const auto& leptonjet = ( *fLeptonjetHdl )[ i ];
    for ( const auto& ak4jetptr : ak4jets ) {
      const auto& ak4jet = *ak4jetptr;
      auto        jec    = fJetCorrectorHdl->correction( ak4jet );
      if ( ak4jet.pt() * jec < leptonjet.pt() )
        continue;
      if ( deltaR( ak4jet, leptonjet ) > 0.4 )
        continue;
      if ( ( ak4jet.chargedHadronEnergyFraction() + ak4jet.neutralHadronEnergyFraction() ) < 0.5 )
        continue;
      v_cleaned[ i ] = false;
      break;
    }
  }
  ValueMap<bool>::Filler cleanedFiller( *vm_cleaned );
  cleanedFiller.insert( fLeptonjetHdl, v_cleaned.begin(), v_cleaned.end() );
  cleanedFiller.fill();
  e.put( move( vm_cleaned ), "cleaned" );
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( LeptonjetValueMapProducer );