#include "Firefighter/recoStuff/interface/SplitPFCandsFromDsaMuon.h"

#include "Firefighter/recoStuff/interface/MatcherByExtrapolateTracks.h"
#include "Firefighter/recoStuff/interface/MatcherByOverlapHitPatterns.h"
#include "Firefighter/recoStuff/interface/MatcherByOverlapSegments.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include <algorithm>

SplitPFCandsFromDsaMuon::SplitPFCandsFromDsaMuon( const edm::ParameterSet& iC )
    : srcToken_( consumes<reco::PFCandidateFwdPtrVector>(
          iC.getParameter<edm::InputTag>( "src" ) ) ),
      matchedToken_( consumes<reco::PFCandidateFwdPtrVector>(
          iC.getParameter<edm::InputTag>( "matched" ) ) ),
      srcCut_( iC.getParameter<std::string>( "srcPreselection" ) ) {
  // construct matchers per presence of corresponding parameters
  if ( iC.existsAs<edm::ParameterSet>( "MatcherByExtrapolateTracks" ) ) {
    fMatchers.push_back( std::make_shared<ff::MatcherByExtrapolateTracks>(
        iC.getParameter<edm::ParameterSet>( "MatcherByExtrapolateTracks" ) ) );
  }
  if ( iC.existsAs<edm::ParameterSet>( "MatcherByOverlapHitPatterns" ) ) {
    fMatchers.push_back( std::make_shared<ff::MatcherByOverlapHitPatterns>(
        iC.getParameter<edm::ParameterSet>( "MatcherByOverlapHitPatterns" ) ) );
  }
  if ( iC.existsAs<edm::ParameterSet>( "MatcherByOverlapSegments" ) ) {
    fMatchers.push_back( std::make_shared<ff::MatcherByOverlapSegments>(
        iC.getParameter<edm::ParameterSet>( "MatcherByOverlapSegments" ) ) );
  }

  produces<reco::PFCandidateFwdPtrVector>( "matched" );
  produces<reco::PFCandidateFwdPtrVector>( "nonMatched" );
}

SplitPFCandsFromDsaMuon::~SplitPFCandsFromDsaMuon() = default;

void
SplitPFCandsFromDsaMuon::beginRun( const edm::Run&        iRun,
                                   const edm::EventSetup& iSetup ) {
  for ( const auto& m : fMatchers ) {
    if ( m->getInitBy() == ff::ffCandMatcher::InitBy::run )
      m->init( iRun, iSetup );
  }
}

void
SplitPFCandsFromDsaMuon::produce( edm::Event&            iEvent,
                                  const edm::EventSetup& iSetup ) {
  using namespace std;
  using namespace edm;

  auto matchCol    = make_unique<reco::PFCandidateFwdPtrVector>();
  auto nonMatchCol = make_unique<reco::PFCandidateFwdPtrVector>();

  iEvent.getByToken( srcToken_, srcHdl_ );
  iEvent.getByToken( matchedToken_, matchedHdl_ );

  assert( srcHdl_.isValid() );
  assert( matchedHdl_.isValid() );

  for ( const auto& m : fMatchers ) {
    if ( m->getInitBy() == ff::ffCandMatcher::InitBy::event )
      m->init( iEvent, iSetup );
  }

  // src collections
  vector<reco::PFCandidatePtr> selectedSrcCands{};
  for ( const auto& srcCandFwdPtr : *srcHdl_ ) {
    const auto& srcCandPtr = srcCandFwdPtr.ptr();
    if ( !srcCut_( *srcCandPtr ) )
      continue;
    selectedSrcCands.emplace_back( srcCandPtr );
  }

  // match collections
  vector<reco::PFCandidatePtr> matchCands{};
  for ( const auto& matchCandFwdPtr : *matchedHdl_ ) {
    matchCands.emplace_back( matchCandFwdPtr.ptr() );
  }

  // collect matching results from various matchers
  vector<ff::MapLink> matchResults{};
  for ( const auto& m : fMatchers ) {
    matchResults.push_back( m->match( selectedSrcCands, matchCands ) );
  }

  // merge the matching results into a set
  set<reco::PFCandidatePtr> mergedMatchCands{};
  for ( const auto& mr : matchResults ) {
    for ( const auto& matchsrc : mr ) {
      mergedMatchCands.insert( matchsrc.first );
    }
  }

  // split into two collections
  for ( const auto& matchCandFwdPtr : *matchedHdl_ ) {
    if ( mergedMatchCands.find( matchCandFwdPtr.ptr() ) ==
         mergedMatchCands.end() ) {
      nonMatchCol->push_back( matchCandFwdPtr );
    } else
      matchCol->push_back( matchCandFwdPtr );
  }

  iEvent.put( move( matchCol ), "matched" );
  iEvent.put( move( nonMatchCol ), "nonMatched" );
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( SplitPFCandsFromDsaMuon );
