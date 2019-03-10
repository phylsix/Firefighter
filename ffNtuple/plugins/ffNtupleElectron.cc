#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"

#define M_Electron 0.510999e-3
using Point         = math::XYZPointF;
using LorentzVector = math::XYZTLorentzVectorF;

class ffNtupleElectron : public ffNtupleBase {
 public:
  ffNtupleElectron( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

 private:
  void clear() final;

  edm::EDGetToken electron_token_;
  int             electron_n_;

  // http://cmsdoxygen.web.cern.ch/cmsdoxygen/CMSSW_10_2_12/doc/html/d8/dac/GsfElectron_8h_source.html
  // Electron
  math::XYZTLorentzVectorFCollection electron_p4_;
  std::vector<int>                   electron_charge_;

  // track extrapolation
  math::XYZTLorentzVectorFCollection electron_trackMomentumAtVtx_;
  math::XYZTLorentzVectorFCollection electron_trackMomentumAtCalo_;
  std::vector<Point>                 electron_trackPositionAtVtx_;
  std::vector<Point>                 electron_trackPositionAtCalo_;

  // supercluster
  std::vector<Point> electron_superClusterPosition_;
  std::vector<int>   electron_basicClusterSize_;

  // shower shape variables
  std::vector<float> electron_sigmaEtaEta_;
  std::vector<float> electron_sigmaIetaIeta_;
  std::vector<float> electron_sigmaIphiIphi_;
  std::vector<float> electron_r9_;
  std::vector<float> electron_hcalOverEcal_;
  std::vector<float> electron_hcalOverEcalBc_;

  // saturation info
  std::vector<int>  electron_nSaturatedXtals_;
  std::vector<bool> electron_isSeedSaturated_;

  // isolation variables
  std::vector<float> electron_dr03TkSumPt_;
  std::vector<float> electron_dr03EcalSumEt_;
  std::vector<float> electron_dr03HcalSumEt_;
  std::vector<float> electron_dr03HcalSumEtBc_;
  std::vector<float> electron_dr04TkSumPt_;
  std::vector<float> electron_dr04EcalSumEt_;
  std::vector<float> electron_dr04HcalSumEt_;
  std::vector<float> electron_dr04HcalSumEtBc_;

  // conversion rejection information
  std::vector<float> electron_convDist_;
  std::vector<float> electron_convDcot_;
  std::vector<float> electron_convRadius_;

  // plfow information
  std::vector<float> electron_ecalPFClusterIso_;
  std::vector<float> electron_hcalPFClusterIso_;
  std::vector<float> electron_mva_isolated_;
  std::vector<float> electron_mva_ePi_;

  // brem fractions and classification
  std::vector<float> electron_trackFbrem_;
  std::vector<float> electron_superClusterFbrem_;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleElectron, "ffNtupleElectron" );

ffNtupleElectron::ffNtupleElectron( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleElectron::initialize( TTree&                   tree,
                              const edm::ParameterSet& ps,
                              edm::ConsumesCollector&& cc ) {
  electron_token_ = cc.consumes<reco::GsfElectronCollection>(
      ps.getParameter<edm::InputTag>( "src" ) );

  tree.Branch( "electron_n", &electron_n_, "electron_n/I" );
  // Electron
  tree.Branch( "electron_p4", &electron_p4_ );
  tree.Branch( "electron_charge", &electron_charge_ );

  // track extrapolation
  tree.Branch( "electron_trackMomentumAtVtx", &electron_trackMomentumAtVtx_ );
  tree.Branch( "electron_trackMomentumAtCalo", &electron_trackMomentumAtCalo_ );
  tree.Branch( "electron_trackPositionAtVtx", &electron_trackPositionAtVtx_ );
  tree.Branch( "electron_trackPositionAtCalo", &electron_trackPositionAtCalo_ );

  // supercluster
  tree.Branch( "electron_superClusterPosition",
               &electron_superClusterPosition_ );
  tree.Branch( "electron_basicClusterSize", &electron_basicClusterSize_ );

  // shower shape variables
  tree.Branch( "electron_sigmaEtaEta", &electron_sigmaEtaEta_ );
  tree.Branch( "electron_sigmaIetaIeta", &electron_sigmaIetaIeta_ );
  tree.Branch( "electron_sigmaIphiIphi", &electron_sigmaIphiIphi_ );
  tree.Branch( "electron_r9", &electron_r9_ );
  tree.Branch( "electron_hcalOverEcal", &electron_hcalOverEcal_ );
  tree.Branch( "electron_hcalOverEcalBc", &electron_hcalOverEcalBc_ );

  // saturation info
  tree.Branch( "electron_nSaturatedXtals", &electron_nSaturatedXtals_ );
  tree.Branch( "electron_isSeedSaturated", &electron_isSeedSaturated_ );

  // isolation variables
  tree.Branch( "electron_dr03TkSumPt", &electron_dr03TkSumPt_ );
  tree.Branch( "electron_dr03EcalSumEt", &electron_dr03EcalSumEt_ );
  tree.Branch( "electron_dr03HcalSumEt", &electron_dr03HcalSumEt_ );
  tree.Branch( "electron_dr03HcalSumEtBc", &electron_dr03HcalSumEtBc_ );
  tree.Branch( "electron_dr04TkSumPt", &electron_dr04TkSumPt_ );
  tree.Branch( "electron_dr04EcalSumEt", &electron_dr04EcalSumEt_ );
  tree.Branch( "electron_dr04HcalSumEt", &electron_dr04HcalSumEt_ );
  tree.Branch( "electron_dr04HcalSumEtBc", &electron_dr04HcalSumEtBc_ );

  // conversion rejection information
  tree.Branch( "electron_convDist", &electron_convDist_ );
  tree.Branch( "electron_convDcot", &electron_convDcot_ );
  tree.Branch( "electron_convRadius", &electron_convRadius_ );

  // plfow information
  tree.Branch( "electron_ecalPFClusterIso", &electron_ecalPFClusterIso_ );
  tree.Branch( "electron_hcalPFClusterIso", &electron_hcalPFClusterIso_ );
  tree.Branch( "electron_mva_isolated", &electron_mva_isolated_ );
  tree.Branch( "electron_mva_ePi", &electron_mva_ePi_ );

  // brem fractions and classification
  tree.Branch( "electron_trackFbrem", &electron_trackFbrem_ );
  tree.Branch( "electron_superClusterFbrem", &electron_superClusterFbrem_ );
}

void
ffNtupleElectron::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::GsfElectronCollection> electron_h;
  e.getByToken( electron_token_, electron_h );
  assert( electron_h.isValid() );
  const reco::GsfElectronCollection& electrons = *electron_h;

  clear();

  electron_n_ = electrons.size();
  for ( const auto& electron : electrons ) {
    // Electron
    electron_p4_.push_back( LorentzVector( electron.px(), electron.py(),
                                           electron.pz(), electron.energy() ) );
    electron_charge_.emplace_back( electron.charge() );

    // track extrapolation
    electron_trackMomentumAtVtx_.push_back( LorentzVector(
        electron.trackMomentumAtVtx().X(), electron.trackMomentumAtVtx().Y(),
        electron.trackMomentumAtVtx().Z(),
        hypot( electron.trackMomentumAtVtx().R(), M_Electron ) ) );
    electron_trackMomentumAtCalo_.push_back( LorentzVector(
        electron.trackMomentumAtCalo().X(), electron.trackMomentumAtCalo().Y(),
        electron.trackMomentumAtCalo().Z(),
        hypot( electron.trackMomentumAtCalo().R(), M_Electron ) ) );
    electron_trackPositionAtVtx_.emplace_back( electron.trackPositionAtVtx() );
    electron_trackPositionAtCalo_.emplace_back(
        electron.trackPositionAtCalo() );

    // supercluster
    electron_superClusterPosition_.emplace_back(
        electron.superClusterPosition() );
    electron_basicClusterSize_.emplace_back( electron.basicClustersSize() );

    // shower shape variables
    electron_sigmaEtaEta_.emplace_back( electron.sigmaEtaEta() );
    electron_sigmaIetaIeta_.emplace_back( electron.sigmaIetaIeta() );
    electron_sigmaIphiIphi_.emplace_back( electron.sigmaIphiIphi() );
    electron_r9_.emplace_back( electron.r9() );
    electron_hcalOverEcal_.emplace_back( electron.hcalOverEcal() );
    electron_hcalOverEcalBc_.emplace_back( electron.hcalOverEcalBc() );

    // saturation info
    electron_nSaturatedXtals_.emplace_back(
        electron.saturationInfo().nSaturatedXtals );
    electron_isSeedSaturated_.emplace_back(
        electron.saturationInfo().isSeedSaturated );

    // isolation variables
    electron_dr03TkSumPt_.emplace_back( electron.dr03TkSumPt() );
    electron_dr03EcalSumEt_.emplace_back( electron.dr03EcalRecHitSumEt() );
    electron_dr03HcalSumEt_.emplace_back( electron.dr03HcalTowerSumEt() );
    electron_dr03HcalSumEtBc_.emplace_back( electron.dr03HcalTowerSumEtBc() );
    electron_dr04TkSumPt_.emplace_back( electron.dr04TkSumPt() );
    electron_dr04EcalSumEt_.emplace_back( electron.dr04EcalRecHitSumEt() );
    electron_dr04HcalSumEt_.emplace_back( electron.dr04HcalTowerSumEt() );
    electron_dr04HcalSumEtBc_.emplace_back( electron.dr04HcalTowerSumEtBc() );

    // conversion rejection information
    electron_convDist_.emplace_back( electron.convDist() );
    electron_convDcot_.emplace_back( electron.convDcot() );
    electron_convRadius_.emplace_back( electron.convRadius() );

    // plfow information
    electron_ecalPFClusterIso_.emplace_back( electron.ecalPFClusterIso() );
    electron_hcalPFClusterIso_.emplace_back( electron.hcalPFClusterIso() );
    electron_mva_isolated_.emplace_back( electron.mva_Isolated() );
    electron_mva_ePi_.emplace_back( electron.mva_e_pi() );

    // brem fractions and classification
    electron_trackFbrem_.emplace_back( electron.trackFbrem() );
    electron_superClusterFbrem_.emplace_back( electron.superClusterFbrem() );
  }
}

void
ffNtupleElectron::clear() {
  electron_n_ = 0;

  // Electron
  electron_p4_.clear();
  electron_charge_.clear();

  // track extrapolation
  electron_trackMomentumAtVtx_.clear();
  electron_trackMomentumAtCalo_.clear();
  electron_trackPositionAtVtx_.clear();
  electron_trackPositionAtCalo_.clear();

  // supercluster
  electron_superClusterPosition_.clear();
  electron_basicClusterSize_.clear();

  // shower shape variables
  electron_sigmaEtaEta_.clear();
  electron_sigmaIetaIeta_.clear();
  electron_sigmaIphiIphi_.clear();
  electron_r9_.clear();
  electron_hcalOverEcal_.clear();
  electron_hcalOverEcalBc_.clear();

  // saturation info
  electron_nSaturatedXtals_.clear();
  electron_isSeedSaturated_.clear();

  // isolation variables
  electron_dr03TkSumPt_.clear();
  electron_dr03EcalSumEt_.clear();
  electron_dr03HcalSumEt_.clear();
  electron_dr03HcalSumEtBc_.clear();
  electron_dr04TkSumPt_.clear();
  electron_dr04EcalSumEt_.clear();
  electron_dr04HcalSumEt_.clear();
  electron_dr04HcalSumEtBc_.clear();

  // conversion rejection information
  electron_convDist_.clear();
  electron_convDcot_.clear();
  electron_convRadius_.clear();

  // plfow information
  electron_ecalPFClusterIso_.clear();
  electron_hcalPFClusterIso_.clear();
  electron_mva_isolated_.clear();
  electron_mva_ePi_.clear();

  // brem fractions and classification
  electron_trackFbrem_.clear();
  electron_superClusterFbrem_.clear();
}