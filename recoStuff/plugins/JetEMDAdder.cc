#include "Firefighter/recoStuff/interface/JetEMDAdder.h"

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include <cmath>

JetEMDAdder::JetEMDAdder( const edm::ParameterSet& iC )
    : fJetToken( consumes<reco::PFJetCollection>(
          iC.getParameter<edm::InputTag>( "src" ) ) ),
      fSjetToken( consumes<reco::JetView>(
          iC.getParameter<edm::InputTag>( "matched" ) ) ),
      fJetsjetMapToken( consumes<edm::Association<reco::PFJetCollection>>(
          iC.getParameter<edm::InputTag>( "associationMap" ) ) ) {
  produces<edm::ValueMap<float>>( "momentum" );
  produces<edm::ValueMap<float>>( "energy" );
}

JetEMDAdder::~JetEMDAdder() = default;

void
JetEMDAdder::produce( edm::Event& iEvent, edm::EventSetup const& iSetup ) {
  using namespace std;
  using namespace edm;

  auto momentumDistVM = make_unique<ValueMap<float>>();
  auto energyDistVM   = make_unique<ValueMap<float>>();

  iEvent.getByToken( fJetToken, fJetHdl );
  assert( fJetHdl.isValid() );
  iEvent.getByToken( fSjetToken, fSjetHdl );
  assert( fSjetHdl.isValid() );
  iEvent.getByToken( fJetsjetMapToken, fJetsjetMapHdl );
  assert( fJetsjetMapHdl.isValid() );

  const auto& jetSjetMap = *fJetsjetMapHdl;

  vector<float> momentumDistValues( fJetHdl->size(), NAN );
  vector<float> energyDistValues( fJetHdl->size(), NAN );

  //-------------------- fill ---------------------

  for ( size_t i( 0 ); i != fSjetHdl->size(); ++i ) {
    Ptr<reco::Jet>   sjet( fSjetHdl, i );
    Ptr<reco::PFJet> jet = refToPtr( jetSjetMap[ sjet ] );
    if ( jet.isNull() )
      continue;

    const std::vector<reco::CandidatePtr>& sjdaus = sjet->daughterPtrVector();

    float lambda_( NAN );
    float epsilon_( NAN );

    if ( sjdaus.size() > 0 ) {
      lambda_ = log( 1 - sjdaus[ 0 ]->pt() / sjet->pt() );
      if ( sjdaus.size() == 1 )
        epsilon_ = 1.;
      else if ( sjdaus.size() == 2 )
        epsilon_ = ( sjdaus[ 0 ]->energy() * sjdaus[ 1 ]->energy() ) /
                   ( sjet->energy() * sjet->energy() );
      else {
        epsilon_ = ( sjdaus[ 0 ]->energy() * sjdaus[ 1 ]->energy() +
                     sjdaus[ 0 ]->energy() * sjdaus[ 2 ]->energy() +
                     sjdaus[ 1 ]->energy() * sjdaus[ 2 ]->energy() ) /
                   ( sjet->energy() * sjet->energy() );
      }
    }

    momentumDistValues[ jet.key() ] = lambda_;
    energyDistValues[ jet.key() ]   = epsilon_;
  }
  //-------------------- fin ---------------------

  ValueMap<float>::Filler mfiller( *momentumDistVM );
  mfiller.insert( fJetHdl, momentumDistValues.begin(),
                  momentumDistValues.end() );
  mfiller.fill();
  ValueMap<float>::Filler efiller( *energyDistVM );
  efiller.insert( fJetHdl, energyDistValues.begin(), energyDistValues.end() );
  efiller.fill();

  iEvent.put( move( momentumDistVM ), "momentum" );
  iEvent.put( move( energyDistVM ), "energy" );
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( JetEMDAdder );