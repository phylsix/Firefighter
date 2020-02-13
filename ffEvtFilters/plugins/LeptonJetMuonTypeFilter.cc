#include "Firefighter/ffEvtFilters/interface/LeptonJetMuonTypeFilter.h"

#include <numeric>

#include "Firefighter/recoStuff/interface/RecoHelpers.h"
#include "Firefighter/recoStuff/interface/ffPFJetProcessors.h"

LeptonJetMuonTypeFilter::LeptonJetMuonTypeFilter( const edm::ParameterSet& ps )
    : fJetToken( consumes<reco::PFJetCollection>( ps.getParameter<edm::InputTag>( "src" ) ) ),
      fTrackToken( consumes<reco::TrackCollection>( edm::InputTag( "generalTracks" ) ) ),
      fMinCount( ps.getParameter<unsigned int>( "minCount" ) ) {
}

bool
LeptonJetMuonTypeFilter::filter( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;
  using namespace ff;

  e.getByToken( fJetToken, fJetHdl );
  assert( fJetHdl.isValid() );
  e.getByToken( fTrackToken, fTrackHdl );
  assert( fTrackHdl.isValid() );

  if ( fJetHdl->size() < fMinCount ) return false;

  vector<Ptr<reco::PFJet>> jetptrs{};
  for ( size_t i( 0 ); i != fJetHdl->size(); ++i ) {
    const auto& jet = ( *fJetHdl )[ i ];
    vector<int> muonCharges{};
    for ( const auto& cand : getPFCands( jet ) ) {
      if ( getCandType( cand, fTrackHdl ) == 3 ) muonCharges.push_back( cand->charge() );
      if ( getCandType( cand, fTrackHdl ) == 8 ) muonCharges.push_back( cand->charge() );
    }

    if ( muonCharges.empty() ) continue;
    if ( accumulate( muonCharges.begin(), muonCharges.end(), 0 ) != 0 ) continue;
    jetptrs.emplace_back( fJetHdl, i );
  }

  return jetptrs.size() >= fMinCount;
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( LeptonJetMuonTypeFilter );