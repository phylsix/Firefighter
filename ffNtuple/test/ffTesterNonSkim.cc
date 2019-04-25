// framework headers
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
// dataformats
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonEnergy.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "Firefighter/recoStuff/interface/RecoHelpers.h"

#include <algorithm>
#include <map>

class ffTesterNonSkim : public edm::one::EDAnalyzer<edm::one::SharedResources> {
 public:
  explicit ffTesterNonSkim( const edm::ParameterSet& );
  ~ffTesterNonSkim() = default;

  static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

 private:
  virtual void beginJob() override;
  virtual void analyze( const edm::Event&, const edm::EventSetup& ) override;
  virtual void endJob() override;

  edm::EDGetTokenT<reco::MuonCollection>  fMuonToken;
  edm::EDGetTokenT<reco::TrackCollection> fDSAToken;
  edm::EDGetTokenT<reco::TrackCollection> fSAToken;
  edm::EDGetTokenT<reco::TrackCollection> fSAVtxToken;

  edm::Handle<reco::MuonCollection>  fMuonHdl;
  edm::Handle<reco::TrackCollection> fDSAHdl;
  edm::Handle<reco::TrackCollection> fSAHdl;
  edm::Handle<reco::TrackCollection> fSAVtxHdl;

  void compareDSATrackId( const edm::Event& e );

  edm::EDGetTokenT<reco::PFCandidateCollection> fParticleFlowToken;
  edm::Handle<reco::PFCandidateCollection>      fParticleFlowHdl;

  void testParticleFlowRef( const edm::Event& e );

  void testDSAHitpattern( const edm::Event& e );

  void dSARecoMuonMatching( const edm::Event& e );

  using trackRefMap = std::map<reco::TrackRef, reco::TrackRef>;
  trackRefMap buildTrackLinkByDeltaR( const std::vector<reco::TrackRef>& src,
                                      const std::vector<reco::TrackRef>& dest,
                                      float metric = 0.3 ) const;
  trackRefMap buildTrackLinkByHitPatternOverlap(
      const std::vector<reco::TrackRef>& src,
      const std::vector<reco::TrackRef>& dest,
      float                              metric = 0.75 ) const;

  std::vector<uint16_t> getHitInfo( const reco::HitPattern& hp ) const;

  void dSARecoMuonOuterTrackSize( const edm::Event& e );

  edm::EDGetTokenT<reco::MuonCollection> fCosmicMuonToken;
  edm::EDGetTokenT<reco::MuonCollection> fCosmicMuonOneLegToken;
  edm::Handle<reco::MuonCollection>      fCosmicMuonHdl;
  edm::Handle<reco::MuonCollection>      fCosmicMuonOneLegHdl;

  void cosmicMuonTiming( const edm::Event& e );
};

ffTesterNonSkim::ffTesterNonSkim( const edm::ParameterSet& iC ) {
  fMuonToken = consumes<reco::MuonCollection>( edm::InputTag( "muons" ) );
  fDSAToken  = consumes<reco::TrackCollection>(
      edm::InputTag( "displacedStandAloneMuons" ) );
  fSAToken =
      consumes<reco::TrackCollection>( edm::InputTag( "standAloneMuons" ) );
  fSAVtxToken = consumes<reco::TrackCollection>(
      edm::InputTag( "standAloneMuons", "UpdatedAtVtx" ) );

  fParticleFlowToken =
      consumes<reco::PFCandidateCollection>( edm::InputTag( "particleFlow" ) );

  fCosmicMuonToken =
      consumes<reco::MuonCollection>( edm::InputTag( "muonsFromCosmics" ) );
  fCosmicMuonOneLegToken =
      consumes<reco::MuonCollection>( edm::InputTag( "muonsFromCosmics1Leg" ) );
}

void
ffTesterNonSkim::analyze( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  // compareDSATrackId( e );
  // testParticleFlowRef( e );
  // testDSAHitpattern( e );
  // dSARecoMuonMatching( e );
  dSARecoMuonOuterTrackSize( e );
  // cosmicMuonTiming( e );

  cout << "++++++++++++++++++++++++++++++++++" << endl;
}

void
ffTesterNonSkim::beginJob() {}
void
ffTesterNonSkim::endJob() {}

void
ffTesterNonSkim::compareDSATrackId( const edm::Event& e ) {
  using namespace std;

  e.getByToken( fMuonToken, fMuonHdl );
  assert( fMuonHdl.isValid() );
  e.getByToken( fDSAToken, fDSAHdl );
  assert( fDSAHdl.isValid() );
  e.getByToken( fSAToken, fSAHdl );
  assert( fSAHdl.isValid() );
  e.getByToken( fSAVtxToken, fSAVtxHdl );
  assert( fSAVtxHdl.isValid() );

  cout << "displacedStandAloneMuon id: " << fDSAHdl.id() << endl;
  cout << "standAloneMuon id: " << fSAHdl.id() << endl;
  cout << "standAloneMuon_UpdatedAtVtx id: " << fSAVtxHdl.id() << endl;
  for ( const auto& muon : *fMuonHdl ) {
    if ( muon.outerTrack().isNull() )
      continue;
    cout << muon.outerTrack().id() << ", ";
  }
  cout << endl;
}

void
ffTesterNonSkim::testParticleFlowRef( const edm::Event& e ) {
  using namespace std;

  e.getByToken( fParticleFlowToken, fParticleFlowHdl );
  assert( fParticleFlowHdl.isValid() );

  for ( const auto& cand : *fParticleFlowHdl ) {
    if ( abs( cand.pdgId() ) == 13 ) {
      cout << cand.pdgId() << "/";
      if ( cand.muonRef().isNonnull() ) {
        cout << cand.muonRef().id() << "/" << cand.muonRef().key();
        if ( cand.muonRef()->isTimeValid() ) {
          cout << "|" << cand.muonRef()->time().timeAtIpInOut;
        }
      } else {
        cout << "NULL";
      }
      cout << ", ";
    }

    if ( abs( cand.pdgId() ) == 11 ) {
      cout << cand.pdgId() << "/";
      if ( cand.gsfElectronRef().isNonnull() ) {
        cout << cand.gsfElectronRef().id() << "/"
             << cand.gsfElectronRef().key();
      } else {
        cout << "NULL";
      }
      cout << ", ";
    }

    if ( abs( cand.pdgId() ) == 22 ) {
      if ( cand.photonRef().isNonnull() ) {
        cout << cand.pdgId() << "/" << cand.photonRef().id() << "/"
             << cand.photonRef().key() << ", ";
      } else {
        // cout << cand.pdgId() << "/" << "NULL, ";
      }
    }
  }
  cout << endl;
}

void
ffTesterNonSkim::testDSAHitpattern( const edm::Event& e ) {
  using namespace std;

  e.getByToken( fDSAToken, fDSAHdl );
  assert( fDSAHdl.isValid() );

  for ( const auto& dsa : *fDSAHdl ) {
    const reco::HitPattern& hp = dsa.hitPattern();
    hp.print( reco::HitPattern::TRACK_HITS );
  }
  cout << "++++++++++++++++++++++++++++++++++" << endl;
}

void
ffTesterNonSkim::dSARecoMuonMatching( const edm::Event& e ) {
  using namespace std;

  e.getByToken( fDSAToken, fDSAHdl );
  assert( fDSAHdl.isValid() );
  e.getByToken( fMuonToken, fMuonHdl );
  assert( fMuonHdl.isValid() );

  map<reco::TrackRef, reco::TrackRef> recoMuonOuterToInnerTrackLink{};
  for ( const auto& mu : *fMuonHdl ) {
    if ( mu.outerTrack().isNull() )
      continue;
    if ( mu.innerTrack().isNull() )
      continue;
    recoMuonOuterToInnerTrackLink.emplace( mu.outerTrack(), mu.innerTrack() );
  }

  std::vector<reco::TrackRef> dSARefs{}, recoMuonOuterTrackRefs{};
  for ( size_t i( 0 ); i != fDSAHdl->size(); ++i ) {
    dSARefs.emplace_back( fDSAHdl, i );
  }
  for ( const auto& mu : *fMuonHdl ) {
    if ( mu.outerTrack().isNonnull() )
      recoMuonOuterTrackRefs.push_back( mu.outerTrack() );
  }

  map<reco::TrackRef, reco::TrackRef> linkBuiltByDeltaR =
      buildTrackLinkByDeltaR( dSARefs, recoMuonOuterTrackRefs );
  map<reco::TrackRef, reco::TrackRef> linkBuiltByHPOverlap =
      buildTrackLinkByHitPatternOverlap( dSARefs, recoMuonOuterTrackRefs );

  cout << "[linkByDeltaR        ]\t";
  for ( const auto& link : linkBuiltByDeltaR ) {
    if ( link.second.isNonnull() and
         recoMuonOuterToInnerTrackLink.find( link.second ) !=
             recoMuonOuterToInnerTrackLink.end() )
      continue;

    cout << link.first.key() << "/";
    if ( link.second.isNonnull() ) {
      cout << link.second.key();
    } else {
      cout << "_";
    }
    cout << ", ";
  }
  cout << endl;

  cout << "[linkBuiltByHPOverlap]\t";
  for ( const auto& link : linkBuiltByHPOverlap ) {
    if ( link.second.isNonnull() and
         recoMuonOuterToInnerTrackLink.find( link.second ) !=
             recoMuonOuterToInnerTrackLink.end() )
      continue;

    cout << link.first.key() << "/";
    if ( link.second.isNonnull() ) {
      cout << link.second.key();
    } else {
      cout << "_";
    }
    cout << ", ";
  }
  cout << endl;
}

std::map<reco::TrackRef, reco::TrackRef>
ffTesterNonSkim::buildTrackLinkByDeltaR(
    const std::vector<reco::TrackRef>& src,
    const std::vector<reco::TrackRef>& dest,
    float                              metric ) const {
  using namespace std;

  map<reco::TrackRef, reco::TrackRef> resultLink{};

  vector<unsigned int> matchedDestKeys{};
  for ( const auto& s : src ) {
    reco::TrackRef matched;
    for ( const auto& d : dest ) {
      if ( find( matchedDestKeys.begin(), matchedDestKeys.end(), d.key() ) !=
           matchedDestKeys.end() )
        continue;
      if ( deltaR( *s.get(), *d.get() ) > metric )
        continue;

      matchedDestKeys.push_back( d.key() );
      matched = d;
      break;
    }
    resultLink.emplace( s, matched );
  }

  return resultLink;
}

std::map<reco::TrackRef, reco::TrackRef>
ffTesterNonSkim::buildTrackLinkByHitPatternOverlap(
    const std::vector<reco::TrackRef>& src,
    const std::vector<reco::TrackRef>& dest,
    float                              metric ) const {
  using namespace std;

  map<reco::TrackRef, reco::TrackRef> resultLink{};

  vector<unsigned int> matchedDestKeys{};
  for ( const auto& s : src ) {
    vector<uint16_t> srcHitInfo = getHitInfo( s->hitPattern() );
    reco::TrackRef   matched;

    for ( const auto& d : dest ) {
      if ( find( matchedDestKeys.begin(), matchedDestKeys.end(), d.key() ) !=
           matchedDestKeys.end() )
        continue;

      vector<uint16_t> destHitInfo = getHitInfo( d->hitPattern() );
      if ( ff::calcOverlap<uint16_t>( srcHitInfo, destHitInfo ) < metric )
        continue;

      matchedDestKeys.push_back( d.key() );
      matched = d;
      break;
    }
    resultLink.emplace( s, matched );
  }

  return resultLink;
}

std::vector<uint16_t>
ffTesterNonSkim::getHitInfo( const reco::HitPattern& hp ) const {
  std::vector<uint16_t> res{};
  for ( int i( 0 ); i != hp.numberOfAllHits( reco::HitPattern::TRACK_HITS );
        ++i ) {
    res.push_back( hp.getHitPattern( reco::HitPattern::TRACK_HITS, i ) );
  }
  return res;
}

void
ffTesterNonSkim::dSARecoMuonOuterTrackSize( const edm::Event& e ) {
  using namespace std;

  e.getByToken( fMuonToken, fMuonHdl );
  assert( fMuonHdl.isValid() );
  e.getByToken( fDSAToken, fDSAHdl );
  assert( fDSAHdl.isValid() );
  e.getByToken( fSAToken, fSAHdl );
  assert( fSAHdl.isValid() );
  e.getByToken( fSAVtxToken, fSAVtxHdl );
  assert( fSAVtxHdl.isValid() );

  const auto& recoMuons = *fMuonHdl;

  int recoMuonWithOuterTrackSize =
      count_if( recoMuons.begin(), recoMuons.end(),
                []( const auto& mu ) { return mu.outerTrack().isNonnull(); } );
  cout << "[dSA|recoMuonOuterTrack|SAVtx|SA] " << fDSAHdl->size() << "/"
       << recoMuonWithOuterTrackSize << "/" << fSAVtxHdl->size() << "/"
       << fSAHdl->size() << endl;
}

void
ffTesterNonSkim::cosmicMuonTiming( const edm::Event& e ) {
  using namespace std;

  e.getByToken( fMuonToken, fMuonHdl );
  assert( fMuonHdl.isValid() );
  e.getByToken( fCosmicMuonToken, fCosmicMuonHdl );
  assert( fCosmicMuonHdl.isValid() );
  e.getByToken( fCosmicMuonOneLegToken, fCosmicMuonOneLegHdl );
  assert( fCosmicMuonOneLegHdl.isValid() );

  cout << "[muons               ]\t";
  for ( const auto& mu : *fMuonHdl ) {
    if ( !mu.isTimeValid() )
      continue;
    cout << mu.time().timeAtIpInOut << "/" << mu.time().timeAtIpOutIn << "("
         << mu.time().direction() << ")"
         << ", ";
  }
  cout << endl;

  cout << "[muonsFromCosmics    ]\t";
  for ( const auto& mu : *fCosmicMuonHdl ) {
    if ( !mu.isTimeValid() )
      continue;
    cout << mu.time().timeAtIpInOut << "/" << mu.time().timeAtIpOutIn << "("
         << mu.time().direction() << ")"
         << ", ";
  }
  cout << endl;

  cout << "[muonsFromCosmics1Leg]\t";
  for ( const auto& mu : *fCosmicMuonOneLegHdl ) {
    if ( !mu.isTimeValid() )
      continue;
    cout << mu.time().timeAtIpInOut << "/" << mu.time().timeAtIpOutIn << "("
         << mu.time().direction() << ")"
         << ", ";
  }
  cout << endl;
}

void
ffTesterNonSkim::fillDescriptions(
    edm::ConfigurationDescriptions& descriptions ) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( ffTesterNonSkim );