#include <cmath>

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/CSCRecHit/interface/CSCSegmentCollection.h"
#include "DataFormats/DTRecHit/interface/DTRecSegment4DCollection.h"
#include "DataFormats/GeometrySurface/interface/Surface.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "Firefighter/cosmics/interface/CosmicHelper.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TH1.h"
#include "TH2.h"

class DSAnSegmentsAny : public edm::one::EDAnalyzer<edm::one::SharedResources> {
 public:
  explicit DSAnSegmentsAny( const edm::ParameterSet& );
  ~DSAnSegmentsAny() = default;

  static void fillDescriptions( edm::ConfigurationDescriptions& );

 private:
  virtual void beginJob() override;
  virtual void analyze( const edm::Event&, const edm::EventSetup& ) override;
  virtual void endJob() override;

  edm::EDGetTokenT<reco::TrackCollection>    fDSAMuonToken;
  edm::EDGetTokenT<DTRecSegment4DCollection> fDTSegToken;
  edm::EDGetTokenT<CSCSegmentCollection>     fCSCSegToken;

  edm::Handle<reco::TrackCollection>    fDSAMuonHdl;
  edm::Handle<DTRecSegment4DCollection> fDTSegHdl;
  edm::Handle<CSCSegmentCollection>     fCSCSegHdl;

  // data to preserve
  bool  fVerbose;
  TH2D* fDSAetaphi_preselected;
  TH1D* fDSAnDSA_mincosmicdr;
  TH2D* fDSAnDSA_etasumphidiff;
  TH1D* fDSAnSeg_mincosmicdr;
  TH2D* fDSAnSeg_etasumphidiff;
  TH1D* fDSAnSeg_cutdsa_mincosmicdr;
  TH2D* fDSAnSeg_cutdsa_etasumphidiff;
  TH2D* fDSA_cuttkseg_propRZ;
  TH2D* fDSA_cuttkseg_outerRZ;
};

DSAnSegmentsAny::DSAnSegmentsAny( const edm::ParameterSet& ps )
    : fDSAMuonToken( consumes<reco::TrackCollection>( edm::InputTag( "displacedStandAloneMuons" ) ) ),
      fDTSegToken( consumes<DTRecSegment4DCollection>( edm::InputTag( "dt4DSegments" ) ) ),
      fCSCSegToken( consumes<CSCSegmentCollection>( edm::InputTag( "cscSegments" ) ) ),
      fVerbose( ps.existsAs<bool>( "verbose" ) ? ps.getParameter<bool>( "verbose" ) : false ) {
  edm::Service<TFileService> fs;

  // TH..
  fDSAetaphi_preselected        = fs->make<TH2D>( "DSAetaphi_preselected", "Preselected DSA (#eta,#phi);#eta;#phi;Counts", 100, -2.4, 2.4, 100, -M_PI, M_PI );
  fDSAnDSA_mincosmicdr          = fs->make<TH1D>( "DSAnDSA_mincosmicdr", "Min #DeltaR_{cosmic} (DSA, DSA);#DeltaR_{cosmic};Counts", 100, 0, 1 );
  fDSAnDSA_etasumphidiff        = fs->make<TH2D>( "DSAnDSA_etasumphidiff", "DSA,DSA (etasum, phidiff);|#eta_{DSA0}+#eta_{DSA1}|;#pi-|#Delta#phi_{DSA0,1}|;Counts", 100, 0, 0.5, 100, 0, 0.5 );
  fDSAnSeg_mincosmicdr          = fs->make<TH1D>( "DSAnSeg_mincosmicdr", "Min #DeltaR_{cosmic} (DSA, Seg);#DeltaR_{cosmic};Counts", 100, 0, 1 );
  fDSAnSeg_etasumphidiff        = fs->make<TH2D>( "DSAnSeg_etasumphidiff", "DSA,Seg (etasum, phidiff);|#eta_{DSA0}+#eta_{DSA1}|;#pi-|#Delta#phi_{DSA0,1}|;Counts", 100, 0, 0.5, 100, 0, 0.5 );
  fDSAnSeg_cutdsa_mincosmicdr   = fs->make<TH1D>( "DSAnSeg_cutdsa_mincosmicdr", "Min #DeltaR_{cosmic} (DSA, Seg);#DeltaR_{cosmic};Counts", 100, 0, 1 );
  fDSAnSeg_cutdsa_etasumphidiff = fs->make<TH2D>( "DSAnSeg_cutdsa_etasumphidiff", "DSA,Seg (etasum, phidiff);|#eta_{DSA0}+#eta_{DSA1}|;#pi-|#Delta#phi_{DSA0,1}|;Counts", 100, 0, 0.5, 100, 0, 0.5 );
  fDSA_cuttkseg_propRZ          = fs->make<TH2D>( "DSA_cuttkseg_propRZ", "DSA oppposite propagated r,z;z[cm];r[cm];counts", 400, -1000, 1000, 250, 0, 750 );
  fDSA_cuttkseg_outerRZ         = fs->make<TH2D>( "DSA_cuttkseg_outerRZ", "DSA outer r,z;z[cm];r[cm];counts", 400, -1000, 1000, 250, 0, 750 );
}

void
DSAnSegmentsAny::analyze( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  e.getByToken( fDSAMuonToken, fDSAMuonHdl );
  assert( fDSAMuonHdl.isValid() );
  e.getByToken( fDTSegToken, fDTSegHdl );
  assert( fDTSegHdl.isValid() );
  e.getByToken( fCSCSegToken, fCSCSegHdl );
  assert( fCSCSegHdl.isValid() );

  // clear branch if any defined

  // -- preselected DSA
  vector<reco::TrackRef> preselectedDSAs{};
  for ( size_t i( 0 ); i != fDSAMuonHdl->size(); i++ ) {
    reco::TrackRef dsa( fDSAMuonHdl, i );
    if ( !CosmicHelper::passDSApreselection( *dsa ) ) continue;
    fDSAetaphi_preselected->Fill( dsa->eta(), dsa->phi() );
    preselectedDSAs.push_back( dsa );
  }

  for ( size_t i( 0 ); i != preselectedDSAs.size(); i++ ) {
    const auto& iDSA = *( preselectedDSAs[ i ] );

    // -- the DSA with minimum cosmidDr
    reco::TrackRef minCosmicDrDSA;
    double         minCosmicDr( 999. );

    // looping over all DSAs
    for ( size_t j( 0 ); j != fDSAMuonHdl->size(); j++ ) {
      const auto& jDSA = ( *fDSAMuonHdl )[ j ];

      // not from same hemisphere
      if ( !CosmicHelper::oppositeHemisphere( iDSA, jDSA ) ) continue;

      double cosmicDr_ = CosmicHelper::cosmicDeltaR( iDSA, jDSA );
      if ( cosmicDr_ < minCosmicDr ) {
        minCosmicDr    = cosmicDr_;
        minCosmicDrDSA = reco::TrackRef( fDSAMuonHdl, j );
      }
    }

    // calculate cosmic dr
    double i_etaSum( 999 ), i_phiPiDiff( 999 );
    fDSAnDSA_mincosmicdr->Fill( minCosmicDr );
    if ( minCosmicDrDSA.isNonnull() ) {
      const auto& iiDSA = *minCosmicDrDSA;
      i_etaSum          = fabs( iDSA.eta() + iiDSA.eta() );
      i_phiPiDiff       = M_PI - fabs( deltaPhi( iDSA.phi(), iiDSA.phi() ) );
      fDSAnDSA_etasumphidiff->Fill( i_etaSum, i_phiPiDiff );
    }

    // segments
    vector<pair<double, double>> segdist{};  // <etasum, phidiff>
    for ( const DTRecSegment4D& dtSeg : *fDTSegHdl ) {
      // not from same hemisphere
      if ( !CosmicHelper::oppositeHemisphere( iDSA, dtSeg, es ) ) continue;

      pair<bool, tuple<double, GlobalVector, GlobalVector>> propagationResult = CosmicHelper::propagateDSAtoDT( iDSA, dtSeg, es );
      if ( !propagationResult.first ) continue;
      const auto& dsaGlbDir   = get<1>( propagationResult.second );
      const auto& dtsegGlbDir = get<2>( propagationResult.second );

      double etaSum    = fabs( dsaGlbDir.eta() + dtsegGlbDir.eta() );
      double phiPiDiff = M_PI - fabs( deltaPhi( dsaGlbDir.barePhi(), dtsegGlbDir.barePhi() ) );

      segdist.emplace_back( etaSum, phiPiDiff );
    }
    for ( const CSCSegment& cscSeg : *fCSCSegHdl ) {
      // not from same hemisphere
      if ( !CosmicHelper::oppositeHemisphere( iDSA, cscSeg, es ) ) continue;

      pair<bool, tuple<double, GlobalVector, GlobalVector>> propagationResult = CosmicHelper::propagateDSAtoCSC( iDSA, cscSeg, es );

      if ( !propagationResult.first ) continue;
      const auto& dsaGlbDir    = get<1>( propagationResult.second );
      const auto& cscsegGlbDir = get<2>( propagationResult.second );

      double etaSum    = fabs( dsaGlbDir.eta() + cscsegGlbDir.eta() );
      double phiPiDiff = M_PI - fabs( deltaPhi( dsaGlbDir.barePhi(), cscsegGlbDir.barePhi() ) );

      segdist.emplace_back( etaSum, phiPiDiff );
    }
    // sort by cosmicDr, small->large
    sort( segdist.begin(), segdist.end(), []( const auto& lhs, const auto& rhs ) {
      return hypot( lhs.first, lhs.second ) < hypot( rhs.first, rhs.second );
    } );

    // minimum DSA-Seg cosmicDr, (TH1D, TH2D)
    double s_etaSum( 999. ), s_phiPiDiff( 999. );
    if ( segdist.size() > 0 ) {
      s_etaSum    = segdist[ 0 ].first;
      s_phiPiDiff = segdist[ 0 ].second;
      fDSAnSeg_mincosmicdr->Fill( hypot( s_etaSum, s_phiPiDiff ) );
      fDSAnSeg_etasumphidiff->Fill( s_etaSum, s_phiPiDiff );
    }

    // place some cuts...
    if ( ( i_etaSum > 0.05 ) || ( i_phiPiDiff > 0.15 ) ) {
      fDSAnSeg_cutdsa_mincosmicdr->Fill( hypot( s_etaSum, s_phiPiDiff ) );
      fDSAnSeg_cutdsa_etasumphidiff->Fill( s_etaSum, s_phiPiDiff );

      // cuts on seg
      if ( ( s_etaSum > 0.05 ) || ( s_phiPiDiff > 0.20 ) ) {
        TrajectoryStateOnSurface MB1tsos = CosmicHelper::propagateTrackToCylinderSurface( iDSA, es, 402.0, -1 );
        TrajectoryStateOnSurface MB2tsos = CosmicHelper::propagateTrackToCylinderSurface( iDSA, es, 488.5, -1 );
        TrajectoryStateOnSurface MB3tsos = CosmicHelper::propagateTrackToCylinderSurface( iDSA, es, 597.5, -1 );
        TrajectoryStateOnSurface MB4tsos = CosmicHelper::propagateTrackToCylinderSurface( iDSA, es, 710.0, -1 );

        if ( MB1tsos.isValid() ) fDSA_cuttkseg_propRZ->Fill( MB1tsos.globalPosition().z(), MB1tsos.globalPosition().perp() );
        if ( MB2tsos.isValid() ) fDSA_cuttkseg_propRZ->Fill( MB2tsos.globalPosition().z(), MB2tsos.globalPosition().perp() );
        if ( MB3tsos.isValid() ) fDSA_cuttkseg_propRZ->Fill( MB3tsos.globalPosition().z(), MB3tsos.globalPosition().perp() );
        if ( MB4tsos.isValid() ) fDSA_cuttkseg_propRZ->Fill( MB4tsos.globalPosition().z(), MB4tsos.globalPosition().perp() );

        fDSA_cuttkseg_outerRZ->Fill( iDSA.outerZ(), hypot( iDSA.outerX(), iDSA.outerY() ) );
      }
    }
  }
}

void
DSAnSegmentsAny::beginJob() {}

void
DSAnSegmentsAny::endJob() {}

void
DSAnSegmentsAny::fillDescriptions( edm::ConfigurationDescriptions& descriptions ) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( DSAnSegmentsAny );
