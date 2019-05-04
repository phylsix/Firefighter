#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"

#include <algorithm>
#include <iterator>

/**
 * ffNtupler for singleMuon gun sample.
 * Event should contain exactly one muon, used to study
 * matching betwen dSA and recoMuon. Saving
 * p4, charge, time, CSC segment keys, DT segment keys
 */

class ffNtupleSingleMu : public ffNtupleBase {
 public:
  ffNtupleSingleMu( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

 private:
  void clear() final;

  edm::EDGetToken fRecoMuonToken;
  edm::EDGetToken fMuonsFromdSAToken;

  float fDeltaRMax;
  float fOverlapRatioMin;

  math::XYZTLorentzVectorFCollection fP4;
  std::vector<int>                   fCharge;
  std::vector<float>                 fTimeAtIpInOut;
  std::vector<int>                   fNumCSCSegs;
  std::vector<int>                   fNumDTSegs;
  int                                fNumCSCSegsShared;
  int                                fNumDTSegsShared;

  bool fFlagdR;
  bool fFlagWeightedOverlapRatio;
  bool fFlagWeightedOverlapRatioPlusdR;
  bool fFlagWeightedOverlapRatioPlusdRBX;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleSingleMu, "ffNtupleSingleMu" );

ffNtupleSingleMu::ffNtupleSingleMu( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ),
      fDeltaRMax( ps.getParameter<double>( "deltaR" ) ),
      fOverlapRatioMin( ps.getParameter<double>( "overlapRatio" ) ) {}

void
ffNtupleSingleMu::initialize( TTree&                   tree,
                              const edm::ParameterSet& ps,
                              edm::ConsumesCollector&& cc ) {
  fRecoMuonToken = cc.consumes<reco::MuonCollection>(
      ps.getParameter<edm::InputTag>( "recoMuonSrc" ) );
  fMuonsFromdSAToken = cc.consumes<reco::MuonCollection>(
      ps.getParameter<edm::InputTag>( "dSAMuonSrc" ) );

  tree.Branch( "singlemu_p4", &fP4 );
  tree.Branch( "singlemu_charge", &fCharge );
  tree.Branch( "singlemu_time", &fTimeAtIpInOut );
  tree.Branch( "singlemu_nCSCSegs", &fNumCSCSegs );
  tree.Branch( "singlemu_nDTSegs", &fNumDTSegs );
  tree.Branch( "singlemu_nCSCSegsShared", &fNumCSCSegsShared,
               "singlemu_nCSCSegsShared/I" );
  tree.Branch( "singlemu_nDTSegsShared", &fNumDTSegsShared,
               "singlemu_nDTSegsShared/I" );
  tree.Branch( "singlemu_flag_dr", &fFlagdR, "singlemu_flag_dr/O" );
  tree.Branch( "singlemu_flag_weightedOverlapRatio", &fFlagWeightedOverlapRatio,
               "singlemu_flag_weightedOverlapRatio/O" );
  tree.Branch( "singlemu_flag_weightedOverlapRatioPlusdR",
               &fFlagWeightedOverlapRatioPlusdR,
               "singlemu_flag_weightedOverlapRatioPlusdR/O" );
  tree.Branch( "singlemu_flat_weightedOverlapRatioPlusdRBX",
               &fFlagWeightedOverlapRatioPlusdRBX,
               "singlemu_flat_weightedOverlapRatioPlusdRBX/O" );
}

void
ffNtupleSingleMu::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::MuonCollection> fRecoMuonHdl;
  e.getByToken( fRecoMuonToken, fRecoMuonHdl );
  assert( fRecoMuonHdl.isValid() );

  Handle<reco::MuonCollection> fMuonsFromdSAHdl;
  e.getByToken( fMuonsFromdSAToken, fMuonsFromdSAHdl );
  assert( fMuonsFromdSAHdl.isValid() );

  clear();

  vector<reco::MuonRef> recoMuonRef{}, muonsFromdSARef{};
  for ( size_t i( 0 ); i != fRecoMuonHdl->size(); ++i )
    recoMuonRef.emplace_back( fRecoMuonHdl, i );
  for ( size_t i( 0 ); i != fMuonsFromdSAHdl->size(); ++i )
    muonsFromdSARef.emplace_back( fMuonsFromdSAHdl, i );

  if ( !( recoMuonRef.size() > 0 and muonsFromdSARef.size() > 0 ) )
    return;

  sort( recoMuonRef.begin(), recoMuonRef.end(),
        []( const auto& lhs, const auto& rhs ) {
          return lhs->pt() > rhs->pt();
        } );
  sort( muonsFromdSARef.begin(), muonsFromdSARef.end(),
        []( const auto& lhs, const auto& rhs ) {
          return lhs->pt() > rhs->pt();
        } );

  vector<reco::MuonRef> recodSApair{recoMuonRef[ 0 ], muonsFromdSARef[ 0 ]};
  vector<vector<int>>   recodsa_CSC{}, recodsa_DT{};
  for ( const auto& mu : recodSApair ) {
    fP4.emplace_back( mu->px(), mu->py(), mu->pz(), mu->energy() );
    fCharge.emplace_back( mu->charge() );
    fTimeAtIpInOut.emplace_back( mu->isTimeValid() ? mu->time().timeAtIpInOut
                                                   : NAN );
    // CSC, DT segs
    vector<int> cscSegs{}, dtSegs{};
    for ( const auto& mm : mu->matches() ) {
      for ( const auto& seg : mm.segmentMatches ) {
        if ( seg.cscSegmentRef.isNonnull() )
          cscSegs.emplace_back( seg.cscSegmentRef.key() );
        if ( seg.dtSegmentRef.isNonnull() )
          dtSegs.emplace_back( seg.dtSegmentRef.key() );
      }
    }
    sort( cscSegs.begin(), cscSegs.end() );
    sort( dtSegs.begin(), dtSegs.end() );

    recodsa_CSC.push_back( cscSegs );
    recodsa_DT.push_back( dtSegs );

    fNumCSCSegs.emplace_back( cscSegs.size() );
    fNumDTSegs.emplace_back( dtSegs.size() );
  }

  fFlagdR =
      deltaR( *recodSApair[ 0 ].get(), *recodSApair[ 1 ].get() ) < fDeltaRMax;

  vector<int> inter_CSC{}, inter_DT{};
  set_intersection( recodsa_CSC[ 0 ].begin(), recodsa_CSC[ 0 ].end(),
                    recodsa_CSC[ 1 ].begin(), recodsa_CSC[ 1 ].end(),
                    back_inserter( inter_CSC ) );
  set_intersection( recodsa_DT[ 0 ].begin(), recodsa_DT[ 0 ].end(),
                    recodsa_DT[ 1 ].begin(), recodsa_DT[ 1 ].end(),
                    back_inserter( inter_DT ) );
  fNumCSCSegsShared = inter_CSC.size();
  fNumDTSegsShared  = inter_DT.size();

  float cscRatio = recodsa_CSC[ 0 ].empty()
                       ? 0.
                       : fNumCSCSegsShared / recodsa_CSC[ 0 ].size();
  float dtRatio =
      recodsa_DT[ 0 ].empty() ? 0. : fNumDTSegsShared / recodsa_DT[ 0 ].size();

  float weightedOverlapRatio =
      ( recodsa_CSC[ 0 ].size() + recodsa_DT[ 0 ].size() )
          ? cscRatio *
                    ( recodsa_CSC[ 0 ].size() /
                      ( recodsa_CSC[ 0 ].size() + recodsa_DT[ 0 ].size() ) ) +
                dtRatio *
                    ( recodsa_DT[ 0 ].size() /
                      ( recodsa_CSC[ 0 ].size() + recodsa_DT[ 0 ].size() ) )
          : 0.;

  fFlagWeightedOverlapRatio = weightedOverlapRatio >= fOverlapRatioMin;

  fFlagWeightedOverlapRatioPlusdR = fFlagWeightedOverlapRatio or fFlagdR;

  fFlagWeightedOverlapRatioPlusdRBX =
      fFlagWeightedOverlapRatioPlusdR and
      fabs( fTimeAtIpInOut[ 0 ] - fTimeAtIpInOut[ 1 ] ) < 20.;

  // debug
  // stringstream ss;
  // ss << "<CSC> [";
  // for ( const auto i : recodsa_CSC[ 0 ] )
  //   ss << i << ", ";
  // ss << "], [";
  // for ( const auto i : recodsa_CSC[ 1 ] )
  //   ss << i << ", ";
  // ss << "] -- " << fNumCSCSegsShared;
  // cout << ss.str() << endl;

  // ss.str( "" );
  // ss << "<DT> [";
  // for ( const auto i : recodsa_DT[ 0 ] )
  //   ss << i << ", ";
  // ss << "], [";
  // for ( const auto i : recodsa_DT[ 1 ] )
  //   ss << i << ", ";
  // ss << "] -- " << fNumDTSegsShared;
  // cout << ss.str() << endl;

  // cout << "-----------------------------------\n" << endl;
}

void
ffNtupleSingleMu::clear() {
  fP4.clear();
  fCharge.clear();
  fTimeAtIpInOut.clear();
  fNumCSCSegs.clear();
  fNumDTSegs.clear();
  fNumCSCSegsShared                 = 0;
  fNumDTSegsShared                  = 0;
  fFlagdR                           = false;
  fFlagWeightedOverlapRatio         = false;
  fFlagWeightedOverlapRatioPlusdR   = false;
  fFlagWeightedOverlapRatioPlusdRBX = false;
}
