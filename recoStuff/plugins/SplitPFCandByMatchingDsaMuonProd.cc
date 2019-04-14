#include "Firefighter/recoStuff/interface/SplitPFCandByMatchingDsaMuonProd.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include <algorithm>

SplitPFCandByMatchingDsaMuonProd::SplitPFCandByMatchingDsaMuonProd(
    const edm::ParameterSet& iC )
    : srcToken_( consumes<reco::PFCandidateFwdPtrVector>(
          iC.getParameter<edm::InputTag>( "src" ) ) ),
      matchedToken_( consumes<reco::PFCandidateFwdPtrVector>(
          iC.getParameter<edm::InputTag>( "matched" ) ) ),
      srcCut_( iC.getParameter<std::string>( "srcPreselection" ) ),
      matcherByTk_( iC.getParameterSet( "matcherByTkParams" ) ),
      matcherByMu_( iC.getParameterSet( "matcherByMuParams" ),
                    consumesCollector() ) {
  produces<reco::PFCandidateFwdPtrVector>( "matched" );
  produces<reco::PFCandidateFwdPtrVector>( "nonMatched" );
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
  using PFCandPtrLink = std::map<reco::PFCandidatePtr, reco::PFCandidatePtr>;

  auto matchCol    = make_unique<reco::PFCandidateFwdPtrVector>();
  auto nonMatchCol = make_unique<reco::PFCandidateFwdPtrVector>();

  iEvent.getByToken( srcToken_, srcHdl_ );
  iEvent.getByToken( matchedToken_, matchedHdl_ );

  assert( srcHdl_.isValid() );
  assert( matchedHdl_.isValid() );

  matcherByMu_.init( iEvent );

  vector<reco::PFCandidatePtr> selectedSrcCands{};
  for ( const auto& srcCandFwdPtr : *srcHdl_ ) {
    const auto& srcCandPtr = srcCandFwdPtr.ptr();
    if ( !srcCut_( *srcCandPtr ) )
      continue;
    if ( srcCandPtr->particleId() != reco::PFCandidate::ParticleType::mu )
      continue;

    selectedSrcCands.emplace_back( srcCandPtr );
  }

  vector<reco::PFCandidatePtr> matchCands{};
  for ( const auto& matchCandFwdPtr : *matchedHdl_ ) {
    matchCands.emplace_back( matchCandFwdPtr.ptr() );
  }

  PFCandPtrLink matchInfo_tk =
      matcherByTk_.match( selectedSrcCands, matchCands );

  PFCandPtrLink matchInfo_mu =
      matcherByMu_.match( selectedSrcCands, matchCands );

  std::set<reco::PFCandidatePtr> matchInfo =
      ff::getMergedMapKeys<PFCandPtrLink>( matchInfo_tk, matchInfo_mu );

  // cout << endl
  //      << "Run" << iEvent.id().run() << "Lumi" << iEvent.luminosityBlock()
  //      << "Event" << iEvent.id().event() << endl;
  for ( const auto& matchCandFwdPtr : *matchedHdl_ ) {
    if ( matchInfo.find( matchCandFwdPtr.ptr() ) == matchInfo.end() ) {
      nonMatchCol->push_back( matchCandFwdPtr );
      // cout << "(" << matchCand->eta() << ", " << matchCand->phi() << ") "
      //      << matchCand->pt() << "GeV" << endl;
    } else
      matchCol->push_back( matchCandFwdPtr );
  }

  iEvent.put( move( matchCol ), "matched" );
  iEvent.put( move( nonMatchCol ), "nonMatched" );
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( SplitPFCandByMatchingDsaMuonProd );
