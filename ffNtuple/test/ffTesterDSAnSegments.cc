#include <cmath>

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/CSCRecHit/interface/CSCSegmentCollection.h"
#include "DataFormats/DTRecHit/interface/DTRecSegment4DCollection.h"
#include "DataFormats/GeometrySurface/interface/Surface.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/Math/interface/deltaPhi.h"
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
#include "Firefighter/recoStuff/interface/RecoHelpers.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "RecoVertex/VertexPrimitives/interface/VertexState.h"
#include "TH2.h"
#include "TTree.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/PatternTools/interface/TransverseImpactPointExtrapolator.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"

/**
 * \class ffTesterDSAnSegments
 * \description
 * Tester for using parallelness between DSA track and DT/CSC
 * segments to identify cosmic muons
 */

class ffTesterDSAnSegments : public edm::one::EDAnalyzer<edm::one::SharedResources> {
 public:
  explicit ffTesterDSAnSegments( const edm::ParameterSet& );
  ~ffTesterDSAnSegments() = default;

  static void fillDescriptions( edm::ConfigurationDescriptions& );

  void checkParallelnessWithSegments( const reco::Track&,
                                      const DTRecSegment4DCollection&,
                                      const CSCSegmentCollection&,
                                      const edm::EventSetup& );
  void checkParallelnessWithSegemntsByPropagation( const reco::Track&,
                                                   const DTRecSegment4DCollection&,
                                                   const CSCSegmentCollection&,
                                                   const edm::EventSetup& );

  void checkParallelnessDSAnDSA( const reco::Track&, const reco::Track& );

  std::pair<bool, Measurement1D> transverseImpactDistance( const reco::Track&,
                                                           const reco::Vertex&,
                                                           const edm::EventSetup& ) const;

  float transverseImpactDistance2( const reco::Track&,
                                   const reco::Vertex&,
                                   const edm::EventSetup& ) const;

 private:
  TTree* fTree;
  TH2D*  fDSAnDSAdist;
  TH2D * fDSAnSegDist, *fDSAnSegDist1st, *fDSAnSegDist2nd;
  TH2D * fPropDSAnSegDist, *fPropDSAnSegDist1st, *fPropDSAnSegDist2nd;

  virtual void beginJob() override;
  virtual void analyze( const edm::Event&, const edm::EventSetup& ) override;
  virtual void endJob() override;

  edm::EDGetTokenT<reco::TrackCollection>    fDSAMuonToken;
  edm::EDGetTokenT<DTRecSegment4DCollection> fDTSegToken;
  edm::EDGetTokenT<CSCSegmentCollection>     fCSCSegToken;
  edm::EDGetTokenT<reco::VertexCollection>   fPVToken;

  edm::Handle<reco::TrackCollection>    fDSAMuonHdl;
  edm::Handle<DTRecSegment4DCollection> fDTSegHdl;
  edm::Handle<CSCSegmentCollection>     fCSCSegHdl;
  edm::Handle<reco::VertexCollection>   fPVHdl;

  // data to save
  bool               fVerbose;
  std::vector<float> fDSAnSegDr;
  std::vector<float> fPDSAnSegDr;
  std::vector<float> fDSAnDSADr;
  std::vector<float> fPropDSAnSegGeomDist;
};

ffTesterDSAnSegments::ffTesterDSAnSegments( const edm::ParameterSet& ps )
    : fDSAMuonToken( consumes<reco::TrackCollection>( edm::InputTag( "displacedStandAloneMuons" ) ) ),
      fDTSegToken( consumes<DTRecSegment4DCollection>( edm::InputTag( "dt4DSegments" ) ) ),
      fCSCSegToken( consumes<CSCSegmentCollection>( edm::InputTag( "cscSegments" ) ) ),
      fPVToken( consumes<reco::VertexCollection>( edm::InputTag( "offlinePrimaryVertices" ) ) ),
      fVerbose( ps.getParameter<bool>( "verbose" ) ) {
  edm::Service<TFileService> fs;
  fTree = fs->make<TTree>( "ffNtuple", "ffNtuple" );
  fTree->Branch( "DSAnSegDr", &fDSAnSegDr );
  fTree->Branch( "PDSAnSegDr", &fPDSAnSegDr );  // propagated DSA w/ segment distance
  fTree->Branch( "DSAnDSADr", &fDSAnDSADr );
  fTree->Branch( "PropDSAnSegGeomDist", &fPropDSAnSegGeomDist );

  fDSAnDSAdist    = fs->make<TH2D>( "DSAnDSAdist", "cosmic#DeltaR(DSA,DSA);|#eta_{DSA0}+#eta_{DSA1}|;#pi-|#Delta#phi_{DSA0,1}|;DSAs", 100, 0, 0.5, 100, 0, 0.5 );
  fDSAnSegDist    = fs->make<TH2D>( "DSAnSegDist", "cosmic#DeltaR(DSA,Seg);|#eta_{DSA}+#eta_{Seg}|;#pi-|#Delta#phi_{DSA,Seg}|;segments", 100, 0, 0.5, 100, 0, 0.5 );
  fDSAnSegDist1st = fs->make<TH2D>( "DSAnSegDist1st", "cosmic#DeltaR(DSA,Seg) 1;|#eta_{DSA}+#eta_{Seg}|;#pi-|#Delta#phi_{DSA,Seg}|;segments", 100, 0, 0.5, 100, 0, 0.5 );
  fDSAnSegDist2nd = fs->make<TH2D>( "DSAnSegDist2nd", "cosmic#DeltaR(DSA,Seg) 2;|#eta_{DSA}+#eta_{Seg}|;#pi-|#Delta#phi_{DSA,Seg}|;segments", 100, 0, 0.5, 100, 0, 0.5 );

  fPropDSAnSegDist    = fs->make<TH2D>( "PropDSAnSegDist", "cosmic#DeltaR(DSA,Seg) after propagation;|#eta_{DSA}+#eta_{Seg}|;#pi-|#Delta#phi_{DSA,Seg}|;segments", 100, 0, 0.5, 100, 0, 0.5 );
  fPropDSAnSegDist1st = fs->make<TH2D>( "PropDSAnSegDist1st", "cosmic#DeltaR(DSA,Seg) 1 after propagation;|#eta_{DSA}+#eta_{Seg}|;#pi-|#Delta#phi_{DSA,Seg}|;segments", 100, 0, 0.5, 100, 0, 0.5 );
  fPropDSAnSegDist2nd = fs->make<TH2D>( "PropDSAnSegDist2nd", "cosmic#DeltaR(DSA,Seg) 2 after propagation;|#eta_{DSA}+#eta_{Seg}|;#pi-|#Delta#phi_{DSA,Seg}|;segments", 100, 0, 0.5, 100, 0, 0.5 );
}

void
ffTesterDSAnSegments::analyze( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  // cout << "~~~~~~~~~~~~  " << e.run() << ":" << e.luminosityBlock() << ":" << e.id().event() << "  ~~~~~~~~~~~~" << endl;

  e.getByToken( fDSAMuonToken, fDSAMuonHdl );
  assert( fDSAMuonHdl.isValid() );
  e.getByToken( fDTSegToken, fDTSegHdl );
  assert( fDTSegHdl.isValid() );
  e.getByToken( fCSCSegToken, fCSCSegHdl );
  assert( fCSCSegHdl.isValid() );
  e.getByToken( fPVToken, fPVHdl );
  assert( fPVHdl.isValid() && fPVHdl->size() > 0 );

  const auto& pv = *( fPVHdl->begin() );

  if ( fVerbose )
    cout << "DSA size: " << fDSAMuonHdl->size() << " DTSeg size: " << fDTSegHdl->size() << " CSCSeg size: " << fCSCSegHdl->size() << endl;

  // branches clear
  fDSAnSegDr.clear();
  fPDSAnSegDr.clear();
  fDSAnDSADr.clear();
  fPropDSAnSegGeomDist.clear();

  // loop DSA
  for ( const auto& dsa : *fDSAMuonHdl ) {
    if ( dsa.pt() < 10 ) continue;
    if ( fabs( dsa.eta() ) > 2.4 ) continue;
    if ( dsa.ptError() / dsa.pt() > 1 ) continue;
    if ( dsa.normalizedChi2() > 4 ) continue;

    // pair<bool, Measurement1D> transImpact = transverseImpactDistance( dsa, pv, es );
    // float transImpact2 = transverseImpactDistance2( dsa, pv, es );
    if ( fVerbose )
      cout << ">>> DSA pt: " << dsa.pt()
           << " eta: " << dsa.eta()
           << " phi: " << dsa.phi()
           << " chi2/ndof: " << dsa.normalizedChi2()
           << " #Valid TK hits: " << dsa.hitPattern().numberOfValidTrackerHits()
           << " dxy: " << fabs( dsa.dxy( pv.position() ) )
           //  << " dxy(propagate): " << ( transImpact.first ? transImpact.second.value() : -1 )
           //  << " dxy(propagate): " << transImpact2
           << endl;

    checkParallelnessWithSegments( dsa, *fDTSegHdl, *fCSCSegHdl, es );
    checkParallelnessWithSegemntsByPropagation( dsa, *fDTSegHdl, *fCSCSegHdl, es );
  }

  // loop DSA pair
  for ( size_t i( 0 ); i != fDSAMuonHdl->size(); i++ ) {
    reco::TrackRef iDSA( fDSAMuonHdl, i );
    if ( iDSA->pt() < 10 ) continue;
    if ( fabs( iDSA->eta() ) > 2.4 ) continue;
    if ( iDSA->ptError() / iDSA->pt() > 1 ) continue;
    if ( iDSA->normalizedChi2() > 4 ) continue;

    for ( size_t j( i + 1 ); j != fDSAMuonHdl->size(); j++ ) {
      reco::TrackRef jDSA( fDSAMuonHdl, j );
      if ( jDSA->pt() < 10 ) continue;
      if ( fabs( jDSA->eta() ) > 2.4 ) continue;
      if ( jDSA->ptError() / jDSA->pt() > 1 ) continue;
      if ( jDSA->normalizedChi2() > 4 ) continue;

      checkParallelnessDSAnDSA( *iDSA, *jDSA );
    }
  }

  fTree->Fill();
  if ( fVerbose ) cout << endl;
}

void
ffTesterDSAnSegments::checkParallelnessWithSegments( const reco::Track&              dsa,
                                                     const DTRecSegment4DCollection& dtsegs,
                                                     const CSCSegmentCollection&     cscsegs,
                                                     const edm::EventSetup&          es ) {
  using namespace std;
  size_t MAX_OUT;

  edm::ESHandle<CSCGeometry> cscG;
  edm::ESHandle<DTGeometry>  dtG;
  edm::ESHandle<RPCGeometry> rpcG;
  es.get<MuonGeometryRecord>().get( cscG );
  es.get<MuonGeometryRecord>().get( dtG );
  es.get<MuonGeometryRecord>().get( rpcG );

  float dsaOuterY = dsa.outerPosition().y();

  // Loop DTSegments first
  vector<pair<double, double>> DTdr{};
  for ( const DTRecSegment4D& dtSeg : dtsegs ) {
    const DTChamber* dtchamber = dtG->chamber( DTChamberId( dtSeg.geographicalId() ) );

    GlobalPoint  gpos = dtchamber->toGlobal( dtSeg.localPosition() );
    GlobalVector gdir = dtchamber->toGlobal( dtSeg.localDirection() );

    if ( dsaOuterY * gpos.y() > 0 ) continue;  // same hemisphere

    double etaSum    = dsa.eta() + gdir.eta();
    double phiPiDiff = M_PI - fabs( deltaPhi( dsa, gdir ) );
    // double cosmicDR  = hypot( etaSum, phiPiDiff );

    fDSAnSegDist->Fill( fabs( etaSum ), fabs( phiPiDiff ) );

    DTdr.emplace_back( fabs( etaSum ), fabs( phiPiDiff ) );
  }
  sort( DTdr.begin(), DTdr.end(), []( const auto& lhs, const auto& rhs ) { return hypot( lhs.first, lhs.second ) < hypot( rhs.first, rhs.second ); } );
  MAX_OUT = DTdr.size() > 4 ? 4 : DTdr.size();

  if ( fVerbose ) {
    cout << "Min cosmicDR[4] with DTSeg: ";
    for ( size_t i( 0 ); i != MAX_OUT; i++ )
      cout << hypot( DTdr[ i ].first, DTdr[ i ].second ) << ", ";
    cout << endl;
  }

  // Loop CSCSegments then
  vector<pair<double, double>> CSCdr{};
  for ( const CSCSegment& cscSeg : cscsegs ) {
    const CSCChamber* cscchamber = cscG->chamber( CSCDetId( cscSeg.cscDetId() ) );

    GlobalPoint  gpos = cscchamber->toGlobal( cscSeg.localPosition() );
    GlobalVector gdir = cscchamber->toGlobal( cscSeg.localDirection() );

    if ( dsaOuterY * gpos.y() > 0 ) continue;  // same hemisphere

    double etaSum    = dsa.eta() + gdir.eta();
    double phiPiDiff = M_PI - fabs( deltaPhi( dsa, gdir ) );
    // double cosmicDR  = hypot( etaSum, phiPiDiff );

    fDSAnSegDist->Fill( fabs( etaSum ), fabs( phiPiDiff ) );

    CSCdr.emplace_back( fabs( etaSum ), fabs( phiPiDiff ) );
  }
  sort( CSCdr.begin(), CSCdr.end(), []( const auto& lhs, const auto& rhs ) { return hypot( lhs.first, lhs.second ) < hypot( rhs.first, rhs.second ); } );
  MAX_OUT = CSCdr.size() > 4 ? 4 : CSCdr.size();

  if ( fVerbose ) {
    cout << "Min cosmicDR[4] with CSCSeg: ";
    for ( size_t i( 0 ); i != MAX_OUT; i++ )
      cout << hypot( CSCdr[ i ].first, CSCdr[ i ].second ) << ", ";
    cout << endl;
  }

  // merge two together
  vector<pair<double, double>> SegDr;
  SegDr.reserve( DTdr.size() + CSCdr.size() );
  copy( DTdr.begin(), DTdr.end(), back_inserter( SegDr ) );
  copy( CSCdr.begin(), CSCdr.end(), back_inserter( SegDr ) );
  sort( SegDr.begin(), SegDr.end(), []( const auto& lhs, const auto& rhs ) { return hypot( lhs.first, lhs.second ) < hypot( rhs.first, rhs.second ); } );
  MAX_OUT = SegDr.size() > 4 ? 4 : SegDr.size();

  if ( fVerbose ) {
    cout << "Min cosmicDR[4] with DT+CSC Seg: ";
    for ( size_t i( 0 ); i != MAX_OUT; i++ )
      cout << hypot( SegDr[ i ].first, SegDr[ i ].second ) << ", ";
    cout << endl;
  }

  for ( size_t i( 0 ); i != MAX_OUT; i++ ) {
    fDSAnSegDr.emplace_back( hypot( SegDr[ i ].first, SegDr[ i ].second ) );
    if ( i == 0 ) fDSAnSegDist1st->Fill( SegDr[ i ].first, SegDr[ i ].second );
    if ( i == 1 ) fDSAnSegDist2nd->Fill( SegDr[ i ].first, SegDr[ i ].second );
  }
}

void
ffTesterDSAnSegments::checkParallelnessWithSegemntsByPropagation( const reco::Track&              dsa,
                                                                  const DTRecSegment4DCollection& dtsegs,
                                                                  const CSCSegmentCollection&     cscsegs,
                                                                  const edm::EventSetup&          es ) {
  using namespace std;
  using namespace edm;
  size_t MAX_OUT;
  float  dsaOuterY = dsa.outerPosition().y();

  ESHandle<CSCGeometry> cscG;
  ESHandle<DTGeometry>  dtG;
  ESHandle<RPCGeometry> rpcG;
  es.get<MuonGeometryRecord>().get( cscG );
  es.get<MuonGeometryRecord>().get( dtG );
  es.get<MuonGeometryRecord>().get( rpcG );

  ESHandle<MagneticField> field_h;
  es.get<IdealMagneticFieldRecord>().get( field_h );
  assert( field_h.isValid() );
  const MagneticField* bField = field_h.product();

  ESHandle<Propagator> propagator_h;
  es.get<TrackingComponentsRecord>().get( "SteppingHelixPropagatorAny", propagator_h );
  assert( propagator_h.isValid() );
  const Propagator* propagator = propagator_h.product();

  FreeTrajectoryState startingState = trajectoryStateTransform::initialFreeState( dsa, bField );

  // TrajectoryStateOnSurface propagationResult = propagator->propagate(startingState, surface);

  // Loop DTSegments first
  vector<pair<double, double>> DTdr{};
  for ( const DTRecSegment4D& dtSeg : dtsegs ) {
    const DTChamber* dtchamber = dtG->chamber( DTChamberId( dtSeg.geographicalId() ) );

    GlobalPoint  gpos = dtchamber->toGlobal( dtSeg.localPosition() );
    GlobalVector gdir = dtchamber->toGlobal( dtSeg.localDirection() );

    if ( dsaOuterY * gpos.y() > 0 ) continue;  // same hemisphere

    TrajectoryStateOnSurface propagationResult = propagator->propagate( startingState, dtchamber->surface() );
    if ( !propagationResult.isValid() ) continue;

    GlobalPoint  ppos = propagationResult.globalPosition();
    GlobalVector pdir = propagationResult.globalDirection();

    double etaSum    = pdir.eta() + gdir.eta();
    double phiPiDiff = M_PI - fabs( deltaPhi( pdir.barePhi(), gdir.barePhi() ) );
    // double cosmicDR  = hypot( etaSum, phiPiDiff );

    fPropDSAnSegDist->Fill( fabs( etaSum ), fabs( phiPiDiff ) );
    fPropDSAnSegGeomDist.emplace_back( ( gpos - ppos ).mag() );

    DTdr.emplace_back( fabs( etaSum ), fabs( phiPiDiff ) );
  }
  sort( DTdr.begin(), DTdr.end(), []( const auto& lhs, const auto& rhs ) { return hypot( lhs.first, lhs.second ) < hypot( rhs.first, rhs.second ); } );
  MAX_OUT = DTdr.size() > 4 ? 4 : DTdr.size();

  if ( fVerbose ) {
    cout << "Min cosmicDR[4] (propagated) with DTSeg: ";
    for ( size_t i( 0 ); i != MAX_OUT; i++ )
      cout << hypot( DTdr[ i ].first, DTdr[ i ].second ) << ", ";
    cout << endl;
  }

  // Loop CSCSegments then
  vector<pair<double, double>> CSCdr{};
  for ( const CSCSegment& cscSeg : cscsegs ) {
    const CSCChamber* cscchamber = cscG->chamber( CSCDetId( cscSeg.cscDetId() ) );

    GlobalPoint  gpos = cscchamber->toGlobal( cscSeg.localPosition() );
    GlobalVector gdir = cscchamber->toGlobal( cscSeg.localDirection() );

    if ( dsaOuterY * gpos.y() > 0 ) continue;  // same hemisphere

    TrajectoryStateOnSurface propagationResult = propagator->propagate( startingState, cscchamber->surface() );
    if ( !propagationResult.isValid() ) continue;

    GlobalPoint  ppos = propagationResult.globalPosition();
    GlobalVector pdir = propagationResult.globalDirection();

    double etaSum    = pdir.eta() + gdir.eta();
    double phiPiDiff = M_PI - fabs( deltaPhi( pdir.barePhi(), gdir.barePhi() ) );
    // double cosmicDR  = hypot( etaSum, phiPiDiff );

    fPropDSAnSegDist->Fill( fabs( etaSum ), fabs( phiPiDiff ) );
    fPropDSAnSegGeomDist.emplace_back( ( gpos - ppos ).mag() );

    CSCdr.emplace_back( fabs( etaSum ), fabs( phiPiDiff ) );
  }
  sort( CSCdr.begin(), CSCdr.end(), []( const auto& lhs, const auto& rhs ) { return hypot( lhs.first, lhs.second ) < hypot( rhs.first, rhs.second ); } );
  MAX_OUT = CSCdr.size() > 4 ? 4 : CSCdr.size();

  if ( fVerbose ) {
    cout << "Min cosmicDR[4] (propagated) with CSCSeg: ";
    for ( size_t i( 0 ); i != MAX_OUT; i++ )
      cout << hypot( CSCdr[ i ].first, CSCdr[ i ].second ) << ", ";
    cout << endl;
  }

  // merge two together
  vector<pair<double, double>> SegDr;
  SegDr.reserve( DTdr.size() + CSCdr.size() );
  copy( DTdr.begin(), DTdr.end(), back_inserter( SegDr ) );
  copy( CSCdr.begin(), CSCdr.end(), back_inserter( SegDr ) );
  sort( SegDr.begin(), SegDr.end(), []( const auto& lhs, const auto& rhs ) { return hypot( lhs.first, lhs.second ) < hypot( rhs.first, rhs.second ); } );
  MAX_OUT = SegDr.size() > 4 ? 4 : SegDr.size();

  if ( fVerbose ) {
    cout << "Min cosmicDR[4] (propagated) with DT+CSC Seg: ";
    for ( size_t i( 0 ); i != MAX_OUT; i++ )
      cout << hypot( SegDr[ i ].first, SegDr[ i ].second ) << ", ";
    cout << endl;
  }

  for ( size_t i( 0 ); i != MAX_OUT; i++ ) {
    fPDSAnSegDr.emplace_back( hypot( SegDr[ i ].first, SegDr[ i ].second ) );
    if ( i == 0 ) fPropDSAnSegDist1st->Fill( SegDr[ i ].first, SegDr[ i ].second );
    if ( i == 1 ) fPropDSAnSegDist2nd->Fill( SegDr[ i ].first, SegDr[ i ].second );
  }
}

void
ffTesterDSAnSegments::checkParallelnessDSAnDSA( const reco::Track& idsa, const reco::Track& jdsa ) {
  using namespace std;

  if ( idsa.outerPosition().y() * jdsa.outerPosition().y() > 0 ) return;

  double etaSum    = idsa.eta() + jdsa.eta();
  double phiPiDiff = M_PI - fabs( deltaPhi( idsa.phi(), jdsa.phi() ) );
  double cosmicDR  = hypot( etaSum, phiPiDiff );

  fDSAnDSAdist->Fill( fabs( etaSum ), fabs( phiPiDiff ) );
  fDSAnDSADr.emplace_back( cosmicDR );
}

std::pair<bool, Measurement1D>
ffTesterDSAnSegments::transverseImpactDistance( const reco::Track& dsa, const reco::Vertex& pv, const edm::EventSetup& es ) const {
  using namespace std;
  pair<bool, Measurement1D> res = make_pair( false, Measurement1D( 0., 0. ) );

  // VertexState of PV
  VertexState pvvs( GlobalPoint( pv.x(), pv.y(), pv.z() ), GlobalError( pv.error4D() ) );
  if ( !pvvs.isValid() ) return res;

  // TransientTrack of DSA
  edm::ESHandle<TransientTrackBuilder> theB;
  es.get<TransientTrackRecord>().get( "TransientTrackBuilder", theB );
  reco::TransientTrack tt = theB->build( dsa );
  if ( !tt.isValid() ) return res;

  // Extrapolation
  res = ff::absoluteTransverseImpactParameter( tt, pvvs );

  return res;
}

float
ffTesterDSAnSegments::transverseImpactDistance2( const reco::Track&     dsa,
                                                 const reco::Vertex&    pv,
                                                 const edm::EventSetup& es ) const {
  using namespace std;
  using namespace edm;
  float res( -1. );

  ESHandle<MagneticField> field_h;
  es.get<IdealMagneticFieldRecord>().get( field_h );
  assert( field_h.isValid() );
  const MagneticField* bField = field_h.product();

  // ESHandle<GlobalTrackingGeometry> geometry_h;
  // es.get<GlobalTrackingGeometryRecord>().get( geometry_h );
  // assert( geometry_h.isValid() );
  // const GlobalTrackingGeometry* tkGeom = geometry_h.product();

  ESHandle<Propagator> propagator_h;
  es.get<TrackingComponentsRecord>().get( "SteppingHelixPropagatorAny", propagator_h );
  assert( propagator_h.isValid() );
  const Propagator* propagator = propagator_h.product();

  GlobalPoint              pvpos( pv.x(), pv.y(), pv.z() );
  TrajectoryStateOnSurface tsos = TransverseImpactPointExtrapolator( bField ).extrapolate(
      trajectoryStateTransform::initialFreeState( dsa, bField ), pvpos, *propagator );
  // trajectoryStateTransform::innerStateOnSurface(dsa, *tkGeom, bField ), pvpos);
  if ( !tsos.isValid() ) return res;

  // res = tsos.localPosition().perp();
  res = ( tsos.globalPosition() - pvpos ).perp();

  return res;
}

void
ffTesterDSAnSegments::beginJob() {}
void
ffTesterDSAnSegments::endJob() {}

void
ffTesterDSAnSegments::fillDescriptions( edm::ConfigurationDescriptions& descriptions ) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( ffTesterDSAnSegments );
