#include "Firefighter/recoStuff/interface/JetConstituentSubtractor.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "RecoJets/JetProducers/interface/JetSpecific.h"

JetConstituentSubtractor::JetConstituentSubtractor(
    const edm::ParameterSet& iC )
    : jetToken_( consumes<reco::PFJetCollection>(
          iC.getParameter<edm::InputTag>( "jet" ) ) ),
      candCut_( iC.getParameter<std::string>( "cut" ) ) {
  produces<reco::PFJetCollection>();
}

JetConstituentSubtractor::~JetConstituentSubtractor() = default;

void
JetConstituentSubtractor::produce( edm::Event&            iEvent,
                                   edm::EventSetup const& iSetup ) {
  using namespace std;
  using namespace edm;

  auto newJetCol = make_unique<reco::PFJetCollection>();

  iEvent.getByToken( jetToken_, jetH_ );
  assert( jetH_.isValid() );

  for ( const auto& oldj : *jetH_ ) {
    auto                      oldjConstituents = oldj.getJetConstituents();
    reco::PFJet::Constituents newjConstituents{};
    newjConstituents.reserve( oldjConstituents.size() );

    for ( const auto& oldjc : oldjConstituents ) {
      if ( !( oldjc.isNonnull() && oldjc.isAvailable() ) )
        continue;
      const reco::PFCandidate* asPFCand =
          dynamic_cast<const reco::PFCandidate*>( oldjc.get() );
      if ( asPFCand && candCut_( *asPFCand ) ) {
        newjConstituents.emplace_back( oldjc );
      }
    }

    if ( newjConstituents.size() == 0 )
      continue;

    math::XYZTLorentzVector newjp4 = ( *std::cbegin( newjConstituents ) )->p4();
    for ( auto ic( std::next( newjConstituents.begin() ) );
          ic != std::end( newjConstituents ); ++ic ) {
      newjp4 += ( *ic )->p4();
    }

    reco::PFJet newj;
    reco::writeSpecific( newj, newjp4, oldj.vertex(), newjConstituents,
                         iSetup );
    newJetCol->emplace_back( newj );
  }

  iEvent.put( move( newJetCol ) );
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( JetConstituentSubtractor );