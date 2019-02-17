#include "Firefighter/recoStuff/interface/SplitPFCandByMatchingDsaMuonProd.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include <algorithm>

SplitPFCandByMatchingDsaMuonProd::SplitPFCandByMatchingDsaMuonProd(
    const edm::ParameterSet& iC )
    : srcToken_( consumes<reco::PFCandidateCollection>(
          iC.getParameter<edm::InputTag>( "src" ) ) ),
      matchedToken_( consumes<reco::PFCandidateCollection>(
          iC.getParameter<edm::InputTag>( "matched" ) ) ),
      srcCut_( iC.getParameter<std::string>( "srcPreselection" ) ),
      matcherByTk_( iC.getParameterSet( "matcherByTkParams" ) ),
      matcherByMu_( iC.getParameterSet( "matcherByMuParams" ),
                    consumesCollector() ) {
  produces<reco::PFCandidateCollection>( "matched" );
  produces<reco::PFCandidateCollection>( "nonMatched" );
}

SplitPFCandByMatchingDsaMuonProd::~SplitPFCandByMatchingDsaMuonProd() = default;

void
SplitPFCandByMatchingDsaMuonProd::beginRun( const edm::Run&        iRun,
                                            const edm::EventSetup& iSetup ) {
  matcherByTk_.init( iSetup );
}

void
SplitPFCandByMatchingDsaMuonProd::produce( edm::Event&            iEvent,
                                           const edm::EventSetup& iSetup ) {
  using namespace std;
  using namespace edm;
  using PFCandRefLink = std::map<reco::PFCandidateRef, reco::PFCandidateRef>;

  auto matchCol    = make_unique<reco::PFCandidateCollection>();
  auto nonMatchCol = make_unique<reco::PFCandidateCollection>();

  iEvent.getByToken( srcToken_, srcHdl_ );
  iEvent.getByToken( matchedToken_, matchedHdl_ );

  assert( srcHdl_.isValid() && matchedHdl_.isValid() );

  matcherByMu_.init( iEvent );

  vector<reco::PFCandidateRef> selectedSrcCands{};
  for ( size_t isrc( 0 ); isrc != srcHdl_->size(); ++isrc ) {
    const auto& srcCand = ( *srcHdl_ )[ isrc ];
    if ( !srcCut_( srcCand ) )
      continue;
    if ( srcCand.particleId() != reco::PFCandidate::ParticleType::mu )
      continue;

    selectedSrcCands.emplace_back( srcHdl_, isrc );
  }

  vector<reco::PFCandidateRef> matchCands{};
  for ( size_t imat( 0 ); imat != matchedHdl_->size(); ++imat ) {
    matchCands.emplace_back( matchedHdl_, imat );
  }

  PFCandRefLink matchInfo_tk =
      matcherByTk_.match( selectedSrcCands, matchCands );

  PFCandRefLink matchInfo_mu =
      matcherByMu_.match( selectedSrcCands, matchCands );

  std::set<reco::PFCandidateRef> matchInfo =
      ff::getMergedMapKeys<PFCandRefLink>( matchInfo_tk, matchInfo_mu );

  for ( const auto& matchCand : matchCands ) {
    if ( matchInfo.find( matchCand ) == matchInfo.end() )
      nonMatchCol->push_back( *matchCand );
    else
      matchCol->push_back( *matchCand );
  }

  iEvent.put( move( matchCol ), "matched" );
  iEvent.put( move( nonMatchCol ), "nonMatched" );
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( SplitPFCandByMatchingDsaMuonProd );