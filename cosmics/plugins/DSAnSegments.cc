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

class DSAnSegments : public edm::one::EDAnalyzer<edm::one::SharedResources> {
 public:
  explicit DSAnSegments( const edm::ParameterSet& );
  ~DSAnSegments() = default;

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
  TH1D* fDSAnDSA_mindist;
  TH2D* fDSAnDSA_etasumphidiff;
  TH1D* fDSAnDSA_cosmicdr;

  TH1D* fDSAnSeg_mindist1st;
  TH2D* fDSAnSeg_etasumphidiff1st;
  TH1D* fDSAnSeg_cosmicdr1st;
  TH1D* fDSAnSeg_mindist2nd;
  TH2D* fDSAnSeg_etasumphidiff2nd;
  TH1D* fDSAnSeg_cosmicdr2nd;

  TH2D* fDSAetaphi_cuttk;
  TH2D* fSegetaphi_cuttk;

  TH1D* fDSAnSeg_cuttk_mindist1st;
  TH2D* fDSAnSeg_cuttk_etasumphidiff1st;
  TH1D* fDSAnSeg_cuttk_cosmicdr1st;
  TH1D* fDSAnSeg_cuttk_mindist2nd;
  TH2D* fDSAnSeg_cuttk_etasumphidiff2nd;
  TH1D* fDSAnSeg_cuttk_cosmicdr2nd;

  TH2D* fDSAetaphi_passseg;
  TH2D* fDSAouterRZ;
  TH2D* fDSAouterXY;
};

DSAnSegments::DSAnSegments( const edm::ParameterSet& ps )
    : fDSAMuonToken( consumes<reco::TrackCollection>( edm::InputTag( "displacedStandAloneMuons" ) ) ),
      fDTSegToken( consumes<DTRecSegment4DCollection>( edm::InputTag( "dt4DSegments" ) ) ),
      fCSCSegToken( consumes<CSCSegmentCollection>( edm::InputTag( "cscSegments" ) ) ),
      fVerbose( ps.existsAs<bool>( "verbose" ) ? ps.getParameter<bool>( "verbose" ) : false ) {
  edm::Service<TFileService> fs;

  // TH..
  fDSAetaphi_preselected = fs->make<TH2D>( "preselected DSA", "Preselected DSA (#eta,#phi);#eta;#phi;Counts", 100, -2.4, 2.4, 100, -M_PI, M_PI );
  fDSAnDSA_mindist       = fs->make<TH1D>( "min DSA-DSA tkdist", "Min Track Distance (DSA, DSA);distance [cm];Counts", 100, 0, 100 );
  fDSAnDSA_etasumphidiff = fs->make<TH2D>( "etasum phidiff, DSA-DSA", "DSA,DSA (etasum, phidiff);|#eta_{DSA0}+#eta_{DSA1}|;#pi-|#Delta#phi_{DSA0,1}|;Counts", 100, 0, 0.5, 100, 0, 0.5 );
  fDSAnDSA_cosmicdr      = fs->make<TH1D>( "cosmicdr, DSA-DSA", "DSA,DSA cosmic#DeltaR;#DeltaR;Counts", 100, 0, 1 );

  fDSAnSeg_mindist1st       = fs->make<TH1D>( "min DSA-Seg geomDist", "Min Global Distance (DSA, Seg);distance [cm];Counts", 100, 0, 100 );
  fDSAnSeg_etasumphidiff1st = fs->make<TH2D>( "etasum phidiff, DSA-Seg0", "DSA,Seg0 (etasum, phidiff);|#eta_{DSA}+#eta_{Seg0}|;#pi-|#Delta#phi_{DSA,Seg0}|;Counts", 100, 0, 0.5, 100, 0, 0.5 );
  fDSAnSeg_cosmicdr1st      = fs->make<TH1D>( "cosmicdr, DSA-Seg0", "DSA,Seg0 cosmic#DeltaR;#DeltaR;Counts", 100, 0, 1 );
  fDSAnSeg_mindist2nd       = fs->make<TH1D>( "min DSA-Seg geomDist", "Min Global Distance (DSA, Seg);distance [cm];Counts", 100, 0, 100 );
  fDSAnSeg_etasumphidiff2nd = fs->make<TH2D>( "etasum phidiff, DSA-Seg1", "DSA,Seg1 (etasum, phidiff);|#eta_{DSA}+#eta_{Seg1}|;#pi-|#Delta#phi_{DSA,Seg1}|;Counts", 100, 0, 0.5, 100, 0, 0.5 );
  fDSAnSeg_cosmicdr2nd      = fs->make<TH1D>( "cosmicdr, DSA-Seg1", "DSA,Seg1 cosmic#DeltaR;#DeltaR;Counts", 100, 0, 1 );

  fDSAetaphi_cuttk                = fs->make<TH2D>( "cuttk DSA", "DSA (#eta,#phi) pass track distance;#eta;#phi;Counts", 100, -2.4, 2.4, 100, -M_PI, M_PI );
  fSegetaphi_cuttk                = fs->make<TH2D>( "cuttk Seg", "Seg (#eta,#phi) pass track distance;#eta;#phi;Counts", 100, -2.4, 2.4, 100, -M_PI, M_PI );
  fDSAnSeg_cuttk_mindist1st       = fs->make<TH1D>( "cuttk min DSA-Seg geomDist", "Min Global Distance (DSA, Seg);distance [cm];Counts", 100, 0, 100 );
  fDSAnSeg_cuttk_etasumphidiff1st = fs->make<TH2D>( "cuttk etasum phidiff, DSA-Seg0", "DSA,Seg0 (etasum, phidiff);|#eta_{DSA}+#eta_{Seg0}|;#pi-|#Delta#phi_{DSA,Seg0}|;Counts", 100, 0, 0.5, 100, 0, 0.5 );
  fDSAnSeg_cuttk_cosmicdr1st      = fs->make<TH1D>( "cuttk cosmicdr, DSA-Seg0", "DSA,Seg0 cosmic#DeltaR;#DeltaR;Counts", 100, 0, 1 );
  fDSAnSeg_cuttk_mindist2nd       = fs->make<TH1D>( "cuttk min DSA-Seg geomDist", "Min Global Distance (DSA, Seg);distance [cm];Counts", 100, 0, 100 );
  fDSAnSeg_cuttk_etasumphidiff2nd = fs->make<TH2D>( "cuttk etasum phidiff, DSA-Seg1", "DSA,Seg1 (etasum, phidiff);|#eta_{DSA}+#eta_{Seg1}|;#pi-|#Delta#phi_{DSA,Seg1}|;Counts", 100, 0, 0.5, 100, 0, 0.5 );
  fDSAnSeg_cuttk_cosmicdr2nd      = fs->make<TH1D>( "cuttk cosmicdr, DSA-Seg1", "DSA,Seg1 cosmic#DeltaR;#DeltaR;Counts", 100, 0, 1 );

  fDSAetaphi_passseg = fs->make<TH2D>( "pass segdist DSA", "DSA (#eta,#phi) pass segments distance;#eta;#phi;Counts", 100, -2.4, 2.4, 100, -M_PI, M_PI );
  fDSAouterRZ        = fs->make<TH2D>( "DSAouterRZ", "DSA r,z;z[cm];r[cm];counts", 240, -600, 600, 250, 0, 750 );
  fDSAouterXY        = fs->make<TH2D>( "DSAouterXY", "DSA x,y;x[cm];y[cm];counts", 500, -750, 750, 500, -750, 750 );
}

void
DSAnSegments::analyze( const edm::Event& e, const edm::EventSetup& es ) {
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

    // -- the DSA with minimum two-track-distance
    reco::TrackRef minDistDSA;
    double         minDist( 999. );

    // looping over all DSAs
    for ( size_t j( 0 ); j != fDSAMuonHdl->size(); j++ ) {
      const auto& jDSA = ( *fDSAMuonHdl )[ j ];

      // not from same hemisphere
      if ( !CosmicHelper::oppositeHemisphere( iDSA, jDSA ) ) continue;

      pair<bool, double> mindistres = CosmicHelper::twoTrackMinDistance( iDSA, jDSA, es );
      if ( mindistres.first && mindistres.second < minDist ) {
        minDist    = mindistres.second;
        minDistDSA = reco::TrackRef( fDSAMuonHdl, j );
      }
    }

    fDSAnDSA_mindist->Fill( minDist );

    // calculate cosmic dr
    double i_etaSum( 999 ), i_phiPiDiff( 999 );
    if ( minDistDSA.isNonnull() ) {
      const auto& iiDSA = *minDistDSA;
      i_etaSum          = fabs( iDSA.eta() + iiDSA.eta() );
      i_phiPiDiff       = M_PI - fabs( deltaPhi( iDSA.phi(), iiDSA.phi() ) );
      double i_cosmicDR = hypot( i_etaSum, i_phiPiDiff );
      fDSAnDSA_etasumphidiff->Fill( i_etaSum, i_phiPiDiff );
      fDSAnDSA_cosmicdr->Fill( i_cosmicDR );
    }

    // segemnts
    vector<tuple<double, double, double, double, double>> segdist{};  // <geomDist, etasum, phidiff, segeta, segphi>
    for ( const DTRecSegment4D& dtSeg : *fDTSegHdl ) {
      // not from same hemisphere
      if ( !CosmicHelper::oppositeHemisphere( iDSA, dtSeg, es ) ) continue;

      pair<bool, tuple<double, GlobalVector, GlobalVector>> propagationResult = CosmicHelper::propagateDSAtoDT( iDSA, dtSeg, es );
      if ( !propagationResult.first ) continue;
      const auto& dsaGlbDir   = get<1>( propagationResult.second );
      const auto& dtsegGlbDir = get<2>( propagationResult.second );

      double etaSum    = fabs( dsaGlbDir.eta() + dtsegGlbDir.eta() );
      double phiPiDiff = M_PI - fabs( deltaPhi( dsaGlbDir.barePhi(), dtsegGlbDir.barePhi() ) );

      segdist.emplace_back( get<0>( propagationResult.second ), etaSum, phiPiDiff, dtsegGlbDir.eta(), dtsegGlbDir.barePhi() );
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

      segdist.emplace_back( get<0>( propagationResult.second ), etaSum, phiPiDiff, cscsegGlbDir.eta(), cscsegGlbDir.barePhi() );
    }
    // sort by geomDist, small->large
    sort( segdist.begin(), segdist.end(), []( const auto& lhs, const auto& rhs ) {
      return get<0>( lhs ) < get<0>( rhs );
    } );

    // 1st, (TH1D)geomDist, (TH2D)etaphi, (TH1D)cosmicdr
    if ( segdist.size() > 0 ) {
      double geomdist, etasum, phidiff, segeta, segphi;
      tie( geomdist, etasum, phidiff, segeta, segphi ) = segdist[ 0 ];
      fDSAnSeg_mindist1st->Fill( geomdist );
      fDSAnSeg_etasumphidiff1st->Fill( etasum, phidiff );
      fDSAnSeg_cosmicdr1st->Fill( hypot( etasum, phidiff ) );
    }
    // 2nd, (TH1D)geomDist, (TH2D)etaphi, (TH1D)cosmicdr
    if ( segdist.size() > 1 ) {
      double geomdist, etasum, phidiff, segeta, segphi;
      tie( geomdist, etasum, phidiff, segeta, segphi ) = segdist[ 1 ];
      fDSAnSeg_mindist2nd->Fill( geomdist );
      fDSAnSeg_etasumphidiff2nd->Fill( etasum, phidiff );
      fDSAnSeg_cosmicdr2nd->Fill( hypot( etasum, phidiff ) );
    }

    // cuts on previous DSA dist - (eta,phi), (eta', phi'), cosmicdr
    if ( ( i_etaSum > 0.05 ) || ( i_phiPiDiff > 0.20 ) ) {
      fDSAetaphi_cuttk->Fill( iDSA.eta(), iDSA.phi() );

      // 1st, (TH1D)geomDist, (TH2D)etaphi, (TH1D)cosmicdr
      if ( segdist.size() > 0 ) {
        double geomdist, etasum, phidiff, segeta, segphi;
        tie( geomdist, etasum, phidiff, segeta, segphi ) = segdist[ 0 ];
        fDSAnSeg_cuttk_mindist1st->Fill( geomdist );
        fDSAnSeg_cuttk_etasumphidiff1st->Fill( etasum, phidiff );
        fDSAnSeg_cuttk_cosmicdr1st->Fill( hypot( etasum, phidiff ) );
        fSegetaphi_cuttk->Fill(segeta, segphi);

        // cuts on 2nd seg dist - (eta,phi)
        if ( (etasum>0.05) || (phidiff>0.20) ) {
          fDSAetaphi_passseg->Fill( iDSA.eta(), iDSA.phi() );
          fDSAouterRZ->Fill( iDSA.outerZ(), hypot( iDSA.outerX(), iDSA.outerY() ) );
          fDSAouterXY->Fill( iDSA.outerX(), iDSA.outerY() );
        }

      }
      // 2nd, (TH1D)geomDist, (TH2D)etaphi, (TH1D)cosmicdr
      if ( segdist.size() > 1 ) {
        double geomdist, etasum, phidiff, segeta, segphi;
        tie( geomdist, etasum, phidiff, segeta, segphi ) = segdist[ 1 ];
        fDSAnSeg_cuttk_mindist2nd->Fill( geomdist );
        fDSAnSeg_cuttk_etasumphidiff2nd->Fill( etasum, phidiff );
        fDSAnSeg_cuttk_cosmicdr2nd->Fill( hypot( etasum, phidiff ) );
      }
    }
  }
}

void
DSAnSegments::beginJob() {}

void
DSAnSegments::endJob() {}

void
DSAnSegments::fillDescriptions( edm::ConfigurationDescriptions& descriptions ) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( DSAnSegments );
