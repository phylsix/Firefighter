#include <algorithm>
#include <tuple>

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonTimeExtra.h"
#include "DataFormats/MuonReco/interface/MuonTimeExtraFwd.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackExtraFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"
#include "Firefighter/recoStuff/interface/RecoHelpers.h"
#include "Firefighter/recoStuff/interface/ffPFJetProcessors.h"

class ffNtupleLeptonJetMisc : public ffNtupleBaseNoHLT {
 public:
  ffNtupleLeptonJetMisc( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken fDsaMuonToken;
  edm::EDGetToken fRecoMuonToken;
  edm::EDGetToken fCosmicMuonToken;
  edm::EDGetToken fLeptonJetToken;
  edm::EDGetToken fGeneralTkToken;
  edm::EDGetToken fDsaAsRecoMuonToken;

  std::map<std::string, edm::EDGetToken> fDsaAsRecoMuonTimeTokens;
  std::vector<std::string>               fMuonTimeInstances;

  unsigned int fNumParallelDsaMuon;
  unsigned int fNumParallelCosmicMuon;

  std::vector<std::vector<float>> fDsaInLjPt;
  std::vector<std::vector<float>> fDsaInLjPtError;
  std::vector<std::vector<int>>   fDsaInLjNumValidDTHits;
  std::vector<std::vector<int>>   fDsaInLjNumValidCSCHits;
  std::vector<std::vector<int>>   fDsaInLjNumValidRPCHits;
  std::vector<std::vector<int>>   fDsaInLjNumValidMuonStations;
  std::vector<std::vector<int>>   fDsaInLjNumValidDTChambers;
  std::vector<std::vector<int>>   fDsaInLjHemisphere;
  std::vector<std::vector<float>> fDsaInLjDTTOF;
  std::vector<std::vector<int>>   fDsaInLjDTTOFnDOF;
  std::vector<std::vector<float>> fDsaInLjDTTOFError;
  std::vector<std::vector<float>> fDsaInLjRPCTOF;
  std::vector<std::vector<bool>>  fDsaInLjIsSubsetRecoMuonWithInnerTrack;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleLeptonJetMisc, "ffNtupleLeptonJetMisc" );

ffNtupleLeptonJetMisc::ffNtupleLeptonJetMisc( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtupleLeptonJetMisc::initialize( TTree&                   tree,
                                   const edm::ParameterSet& ps,
                                   edm::ConsumesCollector&& cc ) {
  fDsaMuonToken    = cc.consumes<reco::TrackCollection>( edm::InputTag( "displacedStandAloneMuons" ) );
  fRecoMuonToken   = cc.consumes<reco::MuonCollection>( edm::InputTag( "muons" ) );
  fCosmicMuonToken = cc.consumes<reco::TrackCollection>( edm::InputTag( "cosmicMuons" ) );
  fLeptonJetToken  = cc.consumes<reco::PFJetCollection>( edm::InputTag( "filteredLeptonJet" ) );
  fGeneralTkToken  = cc.consumes<reco::TrackCollection>( edm::InputTag( "generalTracks" ) );

  fDsaAsRecoMuonToken = cc.consumes<reco::MuonCollection>( edm::InputTag( "muonsFromdSA" ) );
  fMuonTimeInstances  = {"combined", "dt", "csc"};
  for ( const std::string& ins : fMuonTimeInstances ) {
    fDsaAsRecoMuonTimeTokens.emplace( ins, cc.consumes<reco::MuonTimeExtraMap>( edm::InputTag( "muonsFromdSA", ins ) ) );
  }

  tree.Branch( "ljmisc_numParallelDsa", &fNumParallelDsaMuon );
  tree.Branch( "ljmisc_numParallelCosmic", &fNumParallelCosmicMuon );
  tree.Branch( "ljmisc_dsaPt", &fDsaInLjPt );
  tree.Branch( "ljmisc_dsaPtError", &fDsaInLjPtError );
  tree.Branch( "ljmisc_dsaNumValidDTHits", &fDsaInLjNumValidDTHits );
  tree.Branch( "ljmisc_dsaNumValidCSCHits", &fDsaInLjNumValidCSCHits );
  tree.Branch( "ljmisc_dsaNumValidRPCHits", &fDsaInLjNumValidRPCHits );
  tree.Branch( "ljmisc_dsaNumValidMuonStations", &fDsaInLjNumValidMuonStations );
  tree.Branch( "ljmisc_dsaNumValidDTChambers", &fDsaInLjNumValidDTChambers );
  tree.Branch( "ljmisc_dsaHemisphere", &fDsaInLjHemisphere );
  tree.Branch( "ljmisc_dsaDTTOF", &fDsaInLjDTTOF );
  tree.Branch( "ljmisc_dsaDTTOFnDOF", &fDsaInLjDTTOFnDOF );
  tree.Branch( "ljmisc_dsaDTTOFError", &fDsaInLjDTTOFError );
  tree.Branch( "ljmisc_dsaRPCTOF", &fDsaInLjRPCTOF );
  tree.Branch( "ljmisc_dsaIsSubsetRecoMuonWithInnerTrack", &fDsaInLjIsSubsetRecoMuonWithInnerTrack );
}

void
ffNtupleLeptonJetMisc::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;
  using namespace ff;

  Handle<reco::TrackCollection> dsaMuonHdl;
  e.getByToken( fDsaMuonToken, dsaMuonHdl );
  assert( dsaMuonHdl.isValid() );
  const reco::TrackCollection& dsamuons = *dsaMuonHdl;

  Handle<reco::MuonCollection> recoMuonHdl;
  e.getByToken( fRecoMuonToken, recoMuonHdl );
  assert( recoMuonHdl.isValid() );
  const reco::MuonCollection& recomuons = *recoMuonHdl;

  Handle<reco::TrackCollection> cosmicMuonHdl;
  e.getByToken( fCosmicMuonToken, cosmicMuonHdl );
  assert( cosmicMuonHdl.isValid() );
  const reco::TrackCollection& cosmicmuons = *cosmicMuonHdl;

  Handle<reco::PFJetCollection> leptonJetHdl;
  e.getByToken( fLeptonJetToken, leptonJetHdl );
  assert( leptonJetHdl.isValid() );
  const reco::PFJetCollection& leptonjets = *leptonJetHdl;

  Handle<reco::TrackCollection> generalTkHdl;
  e.getByToken( fGeneralTkToken, generalTkHdl );
  assert( generalTkHdl.isValid() );

  Handle<reco::MuonCollection> dsaAsRecoMuonHdl;
  e.getByToken( fDsaAsRecoMuonToken, dsaAsRecoMuonHdl );
  assert( dsaAsRecoMuonHdl.isValid() );

  map<string, Handle<reco::MuonTimeExtraMap>> dsaAsRecoMuonTimeHdls;
  for ( const auto& ins : fMuonTimeInstances ) {
    e.getByToken( fDsaAsRecoMuonTimeTokens[ ins ], dsaAsRecoMuonTimeHdls[ ins ] );
    assert( dsaAsRecoMuonTimeHdls[ ins ].isValid() );
  }

  clear();

  for ( size_t i( 0 ); i != dsamuons.size(); i++ ) {
    for ( size_t j( i + 1 ); j != dsamuons.size(); j++ ) {
      float cosalpha = dsamuons[ i ].momentum().Dot( dsamuons[ j ].momentum() );
      cosalpha /= dsamuons[ i ].momentum().R() * dsamuons[ j ].momentum().R();
      if ( fabs( cosalpha ) > 0.99 )
        fNumParallelDsaMuon++;
    }
  }

  for ( size_t i( 0 ); i != cosmicmuons.size(); i++ ) {
    for ( size_t j( i + 1 ); j != cosmicmuons.size(); j++ ) {
      float cosalpha = cosmicmuons[ i ].momentum().Dot( cosmicmuons[ j ].momentum() );
      cosalpha /= cosmicmuons[ i ].momentum().R() * cosmicmuons[ j ].momentum().R();
      if ( fabs( cosalpha ) > 0.99 )
        fNumParallelCosmicMuon++;
    }
  }

  for ( const auto& leptonjet : leptonjets ) {
    vector<float> dsaInLjPt{};
    vector<float> dsaInLjPtError{};
    vector<int>   dsaInLjNumValidDTHits{};
    vector<int>   dsaInLjNumValidCSCHits{};
    vector<int>   dsaInLjNumValidRPCHits{};
    vector<int>   dsaInLjNumValidMuonStations{};
    vector<int>   dsaInLjNumValidDTChambers{};
    vector<int>   dsaInLjHemisphere{};
    vector<int>   dsaInLjDTTOFnDOF{};
    vector<float> dsaInLjDTTOF{};
    vector<float> dsaInLjDTTOFError{};
    vector<float> dsaInLjRPCTOF{};

    vector<bool> dsaInLjIsSubsetRecoMuonWithInnerTrack{};

    const vector<reco::PFCandidatePtr> pfCands = getPFCands( leptonjet );
    for ( const auto& cand : pfCands ) {
      if ( getCandType( cand, generalTkHdl ) != 8 )
        continue;
      const auto& dsatrack = cand->trackRef();
      if ( dsatrack.isNull() ) {
        cout << "found a PFCandidate taking a null trackRef pointing to dSA collection!" << endl;
        continue;
      }

      dsaInLjPt.emplace_back( dsatrack->pt() );
      dsaInLjPtError.emplace_back( dsatrack->ptError() );
      const auto& dsahitpattern = dsatrack->hitPattern();
      dsaInLjNumValidDTHits.emplace_back( dsahitpattern.numberOfValidMuonDTHits() );
      dsaInLjNumValidCSCHits.emplace_back( dsahitpattern.numberOfValidMuonCSCHits() );
      dsaInLjNumValidRPCHits.emplace_back( dsahitpattern.numberOfValidMuonRPCHits() );
      dsaInLjNumValidMuonStations.emplace_back( dsahitpattern.muonStationsWithValidHits() );
      dsaInLjNumValidDTChambers.emplace_back( dsahitpattern.dtStationsWithValidHits() );
      dsaInLjHemisphere.emplace_back( dsatrack->outerY() > 0 ? 1 : -1 );

      /// muon timing info
      reco::MuonRef dresseddsamuon;
      for ( size_t i( 0 ); i != dsaAsRecoMuonHdl->size(); ++i ) {
        reco::MuonRef dsaasrecomuon( dsaAsRecoMuonHdl, i );
        if ( dsaasrecomuon->outerTrack() != dsatrack )
          continue;
        dresseddsamuon = dsaasrecomuon;
        break;
      }
      if ( dresseddsamuon.isNonnull() ) {
        // fill time info;
        const reco::MuonTimeExtra& dtTimeInfo = ( *dsaAsRecoMuonTimeHdls[ "dt" ] )[ dresseddsamuon ];
        dsaInLjDTTOFnDOF.emplace_back( dtTimeInfo.nDof() );
        dsaInLjDTTOF.emplace_back( dtTimeInfo.timeAtIpInOut() );
        dsaInLjDTTOFError.emplace_back( dtTimeInfo.timeAtIpInOutErr() );

        dsaInLjRPCTOF.emplace_back( dresseddsamuon->rpcTime().timeAtIpInOut );
      } else {
        // fill nans;
        dsaInLjDTTOFnDOF.emplace_back( 0 );
        dsaInLjDTTOF.emplace_back( NAN );
        dsaInLjDTTOFError.emplace_back( NAN );
        dsaInLjRPCTOF.emplace_back( NAN );
        cout << "[ljmisc] a dSA track cannot find a cast reco::Muon!" << endl;
      }

      bool isSubsetRecoMuonWithInnerTrack = false;
      // construct a set of dSA's Station/Detector tuple
      vector<tuple<int, int>> dsaStationDets{};
      for ( int i( 0 ); i != dsahitpattern.numberOfValidMuonHits(); i++ ) {
        auto hit = dsahitpattern.getHitPattern( reco::HitPattern::TRACK_HITS, i );
        if ( !dsahitpattern.muonHitFilter( hit ) )
          continue;
        if ( !dsahitpattern.validHitFilter( hit ) )
          continue;
        auto stadet = make_tuple( dsahitpattern.getMuonStation( hit ),
                                  dsahitpattern.getSubStructure( hit ) );
        if ( find( dsaStationDets.cbegin(), dsaStationDets.cend(), stadet ) == dsaStationDets.cend() )
          dsaStationDets.push_back( stadet );
      }
      sort( dsaStationDets.begin(), dsaStationDets.end() );

      // loop over recoMuons
      for ( const auto& recomu : recomuons ) {
        if ( recomu.innerTrack().isNull() )
          continue;

        vector<tuple<int, int>> recomuStationDets{};
        if ( recomu.outerTrack().isNonnull() ) {
          const auto& recomuhitpattern = recomu.outerTrack()->hitPattern();
          for ( int i( 0 ); i != recomuhitpattern.numberOfValidMuonHits(); i++ ) {
            auto hit = recomuhitpattern.getHitPattern( reco::HitPattern::TRACK_HITS, i );
            if ( !recomuhitpattern.muonHitFilter( hit ) )
              continue;
            if ( !recomuhitpattern.validHitFilter( hit ) )
              continue;
            auto stadet = make_tuple( recomuhitpattern.getMuonStation( hit ),
                                      recomuhitpattern.getSubStructure( hit ) );
            if ( find( recomuStationDets.cbegin(), recomuStationDets.cend(), stadet ) == recomuStationDets.cend() )
              recomuStationDets.push_back( stadet );
          }
        }

        if ( recomu.outerTrack().isNull() or recomuStationDets.empty() ) {
          for ( const auto& mm : recomu.matches() ) {
            auto stadet = make_tuple( mm.station(), mm.detector() );
            if ( find( recomuStationDets.cbegin(), recomuStationDets.cend(), stadet ) == recomuStationDets.cend() )
              recomuStationDets.push_back( stadet );
          }
        }
        sort( recomuStationDets.begin(), recomuStationDets.end() );

        if ( !includes( recomuStationDets.begin(), recomuStationDets.end(),
                        dsaStationDets.begin(), dsaStationDets.end() ) )
          continue;

        isSubsetRecoMuonWithInnerTrack = true;
        break;
      }
      dsaInLjIsSubsetRecoMuonWithInnerTrack.emplace_back( isSubsetRecoMuonWithInnerTrack );
    }

    fDsaInLjPt.push_back( dsaInLjPt );
    fDsaInLjPtError.push_back( dsaInLjPtError );
    fDsaInLjNumValidDTHits.push_back( dsaInLjNumValidDTHits );
    fDsaInLjNumValidCSCHits.push_back( dsaInLjNumValidCSCHits );
    fDsaInLjNumValidRPCHits.push_back( dsaInLjNumValidRPCHits );
    fDsaInLjNumValidMuonStations.push_back( dsaInLjNumValidMuonStations );
    fDsaInLjNumValidDTChambers.push_back( dsaInLjNumValidDTChambers );
    fDsaInLjHemisphere.push_back( dsaInLjHemisphere );
    fDsaInLjDTTOFnDOF.push_back( dsaInLjDTTOFnDOF );
    fDsaInLjDTTOF.push_back( dsaInLjDTTOF );
    fDsaInLjDTTOFError.push_back( dsaInLjDTTOFError );
    fDsaInLjRPCTOF.push_back( dsaInLjRPCTOF );
    fDsaInLjIsSubsetRecoMuonWithInnerTrack.push_back( dsaInLjIsSubsetRecoMuonWithInnerTrack );
  }
}

void
ffNtupleLeptonJetMisc::clear() {
  fNumParallelDsaMuon    = 0;
  fNumParallelCosmicMuon = 0;
  fDsaInLjPt.clear();
  fDsaInLjPtError.clear();
  fDsaInLjNumValidDTHits.clear();
  fDsaInLjNumValidCSCHits.clear();
  fDsaInLjNumValidRPCHits.clear();
  fDsaInLjNumValidMuonStations.clear();
  fDsaInLjNumValidDTChambers.clear();
  fDsaInLjHemisphere.clear();
  fDsaInLjDTTOFnDOF.clear();
  fDsaInLjDTTOF.clear();
  fDsaInLjDTTOFError.clear();
  fDsaInLjRPCTOF.clear();

  fDsaInLjIsSubsetRecoMuonWithInnerTrack.clear();
}