#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TTree.h"
#include "TrackingTools/PatternTools/interface/TransverseImpactPointExtrapolator.h"
#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistance.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"

/**
 * \class ffTesterMuonsFromCosmics1Leg
 * \description
 * Tester for using reReco-ed cosmicMuons1Leg to identify cosmic muons
 */

class ffTesterMuonsFromCosmics1Leg : public edm::one::EDAnalyzer<edm::one::SharedResources> {
 public:
  explicit ffTesterMuonsFromCosmics1Leg( const edm::ParameterSet& );
  ~ffTesterMuonsFromCosmics1Leg() = default;

  static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

 private:
  TTree* fTree;

  virtual void beginJob() override;
  virtual void analyze( const edm::Event&, const edm::EventSetup& ) override;
  virtual void endJob() override;

  edm::EDGetTokenT<reco::TrackCollection>  fCosmicOneLegToken;
  edm::EDGetTokenT<reco::TrackCollection>  fDSAMuonToken;
  edm::EDGetTokenT<reco::VertexCollection> fPVToken;

  edm::Handle<reco::TrackCollection>  fCosmicOneLegHdl;
  edm::Handle<reco::TrackCollection>  fDSAMuonHdl;
  edm::Handle<reco::VertexCollection> fPVHdl;

  std::vector<float> fCosmicHitX;
  std::vector<float> fCosmicHitY;
  std::vector<float> fCosmicImpactDist2d;
  std::vector<float> fCosmicDSAAbsCosAlpha;
  std::vector<float> fCosmicDSAMinDist;
  std::vector<float> fCosmicNormChi2;
  std::vector<float> fCosmicPtErrorOverPt;
};

ffTesterMuonsFromCosmics1Leg::ffTesterMuonsFromCosmics1Leg( const edm::ParameterSet& ps )
    : fCosmicOneLegToken( consumes<reco::TrackCollection>( edm::InputTag( "cosmicMuons1Leg", "", "FFTEST" ) ) ),
      fDSAMuonToken( consumes<reco::TrackCollection>( edm::InputTag( "displacedStandAloneMuons" ) ) ),
      fPVToken( consumes<reco::VertexCollection>( edm::InputTag( "offlinePrimaryVertices" ) ) ) {
  edm::Service<TFileService> fs;
  fTree = fs->make<TTree>( "ffNtuple", "ffNtuple" );
  fTree->Branch( "cosmicHitX", &fCosmicHitX );
  fTree->Branch( "cosmicHitY", &fCosmicHitY );
  fTree->Branch( "cosmicImpactDist2d", &fCosmicImpactDist2d );
  fTree->Branch( "cosmicDSAAbsCosAlpha", &fCosmicDSAAbsCosAlpha );
  fTree->Branch( "cosmicDSAMinDist", &fCosmicDSAMinDist );
  fTree->Branch( "cosmicNormChi2", &fCosmicNormChi2 );
  fTree->Branch( "cosmicPtErrorOverPt", &fCosmicPtErrorOverPt );
}

void
ffTesterMuonsFromCosmics1Leg::analyze( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  // cout << "~~~~~~~~~~~~  " << e.run() << ":" << e.luminosityBlock() << ":" << e.id().event() << "  ~~~~~~~~~~~~" << endl;

  e.getByToken( fCosmicOneLegToken, fCosmicOneLegHdl );
  assert( fCosmicOneLegHdl.isValid() );
  e.getByToken( fDSAMuonToken, fDSAMuonHdl );
  assert( fDSAMuonHdl.isValid() );
  e.getByToken( fPVToken, fPVHdl );
  assert( fPVHdl.isValid() );
  if ( fPVHdl->size() < 1 ) return;
  const auto& pv = *( fPVHdl->begin() );

  cout << "Cosmic1Leg size: " << fCosmicOneLegHdl->size() << " DSA Muon size: " << fDSAMuonHdl->size() << endl;

  ESHandle<MagneticField> field_h;
  es.get<IdealMagneticFieldRecord>().get( field_h );
  assert( field_h.isValid() );
  const MagneticField* bField = field_h.product();

  ESHandle<CSCGeometry> cscG;
  ESHandle<DTGeometry>  dtG;
  ESHandle<RPCGeometry> rpcG;
  es.get<MuonGeometryRecord>().get( cscG );
  es.get<MuonGeometryRecord>().get( dtG );
  es.get<MuonGeometryRecord>().get( rpcG );

  fCosmicHitX.clear();
  fCosmicHitY.clear();
  fCosmicImpactDist2d.clear();
  fCosmicDSAAbsCosAlpha.clear();
  fCosmicDSAMinDist.clear();
  fCosmicNormChi2.clear();
  fCosmicPtErrorOverPt.clear();

  int realCosmics( 0 );  // counting how many cosmics I really believe.
  for ( size_t i( 0 ); i != fCosmicOneLegHdl->size(); i++ ) {
    reco::TrackRef cosmicRef( fCosmicOneLegHdl, i );
    const auto&    cosmic = *cosmicRef;
    cout << "cosmic [" << i << "]\t"
         << "pT: " << cosmic.pt() << " eta: " << cosmic.eta() << " phi: " << cosmic.phi() << " normChi2: " << cosmic.normalizedChi2() << endl;
    cout << "recHit Y: ";

    vector<DetId> chamberId{};
    int           dtT( 0 ), dtB( 0 ), cscT( 0 ), cscB( 0 ), rpcT( 0 ), rpcB( 0 );
    for ( auto ih = cosmic.recHitsBegin(); ih != cosmic.recHitsEnd(); ih++ ) {
      const auto& hit = *( *ih );
      if ( !hit.isValid() ) continue;
      if ( hit.geographicalId().det() != DetId::Muon ) continue;
      if ( hit.geographicalId().subdetId() == MuonSubdetId::DT ) {
        const DTChamber* dtchamber = dtG->chamber( hit.geographicalId() );
        GlobalPoint      gpos      = dtchamber->toGlobal( hit.localPosition() );
        if ( gpos.y() > 0 )
          cout << "+";
        else if ( gpos.y() < 0 )
          cout << "-";
        else
          cout << "/";
        fCosmicHitX.emplace_back( gpos.x() );
        fCosmicHitY.emplace_back( gpos.y() );
        if ( find( chamberId.begin(), chamberId.end(), DetId( dtchamber->id().rawId() ) ) == chamberId.end() ) {
          chamberId.emplace_back( dtchamber->id().rawId() );
          if ( dtchamber->position().y() > 0 ) dtT++;
          if ( dtchamber->position().y() < 0 ) dtB++;
        }
      } else if ( hit.geographicalId().subdetId() == MuonSubdetId::CSC ) {
        const CSCChamber* cscchamber = cscG->chamber( hit.geographicalId() );
        GlobalPoint       gpos       = cscchamber->toGlobal( hit.localPosition() );
        if ( gpos.y() > 0 )
          cout << "+";
        else if ( gpos.y() < 0 )
          cout << "-";
        else
          cout << "/";
        fCosmicHitX.emplace_back( gpos.x() );
        fCosmicHitY.emplace_back( gpos.y() );
        if ( find( chamberId.begin(), chamberId.end(), DetId( cscchamber->id().rawId() ) ) == chamberId.end() ) {
          chamberId.emplace_back( cscchamber->id().rawId() );
          if ( cscchamber->position().y() > 0 ) cscT++;
          if ( cscchamber->position().y() < 0 ) cscB++;
        }
      } else if ( hit.geographicalId().subdetId() == MuonSubdetId::RPC ) {
        const RPCChamber* rpcchamber = rpcG->chamber( hit.geographicalId() );
        GlobalPoint       gpos       = rpcchamber->toGlobal( hit.localPosition() );
        if ( gpos.y() > 0 )
          cout << "+";
        else if ( gpos.y() < 0 )
          cout << "-";
        else
          cout << "/";
        fCosmicHitX.emplace_back( gpos.x() );
        fCosmicHitY.emplace_back( gpos.y() );
        if ( find( chamberId.begin(), chamberId.end(), DetId( rpcchamber->id().rawId() ) ) == chamberId.end() ) {
          chamberId.emplace_back( rpcchamber->id().rawId() );
          if ( rpcchamber->position().y() > 0 ) rpcT++;
          if ( rpcchamber->position().y() < 0 ) rpcB++;
        }
      } else {
        cout << "O";
      }
    }
    cout << endl;
    cout << "chambers: DT " << dtT << "/" << dtB << ", CSC " << cscT << "/" << cscB << ", RPC " << rpcT << "/" << rpcB << endl;

    /// transverse impact distance
    GlobalPoint              pvpos( pv.x(), pv.y(), pv.z() );
    TrajectoryStateOnSurface tsos = TransverseImpactPointExtrapolator( bField ).extrapolate(
        trajectoryStateTransform::initialFreeState( cosmic, bField ), pvpos );
    float impact2d( 999. );
    if ( tsos.isValid() ) impact2d = hypot( tsos.localPosition().x(), tsos.localPosition().y() );
    cout << "impact distance 2d: " << impact2d << endl;
    fCosmicImpactDist2d.emplace_back( impact2d );

    // min abs(cos(alpha))
    float minAbsCosAlpha( 2. );
    // min two track distance
    float minDist( 999. );

    for ( const auto& dsa : *fDSAMuonHdl ) {
      //   if ( dsa.pt() < 10. ) continue;
      float cosalpha = cosmic.momentum().Dot( dsa.momentum() );
      cosalpha /= cosmic.momentum().R() * dsa.momentum().R();
      if ( fabs( cosalpha ) < minAbsCosAlpha ) minAbsCosAlpha = fabs( cosalpha );

      TwoTrackMinimumDistance ttmd;

      bool status = ttmd.calculate( trajectoryStateTransform::initialFreeState( cosmic, bField ),
                                    trajectoryStateTransform::initialFreeState( dsa, bField ) );
      if ( status ) {
        if ( ttmd.distance() < minDist )
          minDist = ttmd.distance();
      }
    }
    fCosmicDSAAbsCosAlpha.push_back( minAbsCosAlpha );
    fCosmicDSAMinDist.push_back( minDist );

    fCosmicPtErrorOverPt.emplace_back( cosmic.ptError() / cosmic.pt() );
    fCosmicNormChi2.emplace_back( cosmic.normalizedChi2() );

    if ( ( dtT + cscT ) > 1 && ( dtB + cscB ) > 1 && cosmic.pt() > 10. && cosmic.normalizedChi2() < 20. && impact2d > 20. ) realCosmics++;
  }

  cout << "Real cosmics in events: " << realCosmics << endl;

  fTree->Fill();

  cout << endl;
}

void
ffTesterMuonsFromCosmics1Leg::beginJob() {}
void
ffTesterMuonsFromCosmics1Leg::endJob() {}

void
ffTesterMuonsFromCosmics1Leg::fillDescriptions( edm::ConfigurationDescriptions& descriptions ) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( ffTesterMuonsFromCosmics1Leg );
