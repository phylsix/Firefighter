#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "RecoEgamma/EgammaTools/interface/EffectiveAreas.h"

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

  // RecoEgamma/ElectronIdentification/plugins/cuts/
  float dEtaInSeed( const reco::GsfElectron& ) const;
  float eInvMinusPInv( const reco::GsfElectron& ) const;
  float relIsoWithEA( const reco::GsfElectron& ) const;

  edm::EDGetToken fElectronToken;

  math::XYZTLorentzVectorFCollection fElectronP4;
  std::vector<int>                   fElectronCharge;
  std::vector<float>                 fFull5x5_sigmaIetaIeta;
  std::vector<float>                 fHOverE;
  std::vector<float>                 fAbsdEtaSeed;
  std::vector<float>                 fAbsdPhiIn;
  std::vector<float>                 fRelIsoWithEA;
  std::vector<float>                 fEInvMinusPInv;
  std::vector<int>                   fExpectedMissingInnerHits;
  std::vector<bool>                  fPassConvVeto;
  std::vector<unsigned int>          fIdBit;

  edm::EDGetToken fRhoToken;
  double          fRho;

  EffectiveAreas fEffArea;

  edm::EDGetToken fConvToken;
  edm::EDGetToken fBeamspotToken;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleElectron, "ffNtupleElectron" );

ffNtupleElectron::ffNtupleElectron( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ),
      fEffArea( edm::FileInPath( "RecoEgamma/ElectronIdentification/data/Fall17/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_94X.txt" ).fullPath() ) {}

void
ffNtupleElectron::initialize( TTree&                   tree,
                              const edm::ParameterSet& ps,
                              edm::ConsumesCollector&& cc ) {
  fElectronToken = cc.consumes<reco::GsfElectronCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  fRhoToken      = cc.consumes<double>( edm::InputTag( "fixedGridRhoFastjetAll" ) );
  fConvToken     = cc.consumes<reco::ConversionCollection>( edm::InputTag( "allConversions" ) );
  fBeamspotToken = cc.consumes<reco::BeamSpot>( edm::InputTag( "offlineBeamSpot" ) );

  // Electron
  tree.Branch( "electron_p4", &fElectronP4 );
  tree.Branch( "electron_charge", &fElectronCharge );
  tree.Branch( "electron_full5x5SigmaIetaIeta", &fFull5x5_sigmaIetaIeta );
  tree.Branch( "electron_hOverE", &fHOverE );
  tree.Branch( "electron_absdEtaSeed", &fAbsdEtaSeed );
  tree.Branch( "electron_absdPhiIn", &fAbsdPhiIn );
  tree.Branch( "electron_relIsoWithEA", &fRelIsoWithEA );
  tree.Branch( "electron_absEInvMinusPInv", &fEInvMinusPInv );
  tree.Branch( "electron_expectedMissingInnerHits", &fExpectedMissingInnerHits );
  tree.Branch( "electron_passConvVeto", &fPassConvVeto );
  tree.Branch( "electron_idbit", &fIdBit );
}

void
ffNtupleElectron::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::GsfElectronCollection> electronHdl;
  e.getByToken( fElectronToken, electronHdl );
  assert( electronHdl.isValid() );

  Handle<double> rhoHdl;
  e.getByToken( fRhoToken, rhoHdl );
  assert( rhoHdl.isValid() );
  fRho = *rhoHdl;

  Handle<reco::ConversionCollection> conversionsHdl;
  e.getByToken( fConvToken, conversionsHdl );
  assert( conversionsHdl.isValid() );

  Handle<reco::BeamSpot> beamspotHdl;
  e.getByToken( fBeamspotToken, beamspotHdl );
  assert( beamspotHdl.isValid() );

  clear();

  for ( const auto& electron : *electronHdl ) {
    fElectronP4.emplace_back( electron.px(), electron.py(), electron.pz(), electron.energy() );
    fElectronCharge.emplace_back( electron.charge() );
    fFull5x5_sigmaIetaIeta.emplace_back( electron.full5x5_sigmaIetaIeta() );
    fHOverE.emplace_back( electron.hadronicOverEm() );
    fAbsdEtaSeed.emplace_back( fabs( dEtaInSeed( electron ) ) );
    fAbsdPhiIn.emplace_back( fabs( electron.deltaPhiSuperClusterTrackAtVtx() ) );

    float abseta = fabs( electron.superCluster()->eta() );
    fRelIsoWithEA.emplace_back( relIsoWithEA( electron ) );
    fEInvMinusPInv.emplace_back( eInvMinusPInv( electron ) );
    fExpectedMissingInnerHits.emplace_back( electron.gsfTrack()->hitPattern().numberOfLostHits( reco::HitPattern::MISSING_INNER_HITS ) );
    fPassConvVeto.emplace_back( !ConversionTools::hasMatchedConversion( electron, conversionsHdl, beamspotHdl->position() ) );

    // Loose ID criteria
    // https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Offline_selection_criteria_for_V
    unsigned int idbit = 0;
    if ( abseta <= 1.479 ) {  // barrel
      if ( fFull5x5_sigmaIetaIeta.back() < 0.0112 )
        idbit |= 1 << 0;
      if ( fAbsdEtaSeed.back() < 0.00377 )
        idbit |= 1 << 1;
      if ( fAbsdPhiIn.back() < 0.0884 )
        idbit |= 1 << 2;
      if ( fHOverE.back() < 0.05 + 1.16 / electron.superCluster()->energy() + 0.0324 * fRho / electron.superCluster()->energy() )
        idbit |= 1 << 3;
      if ( fRelIsoWithEA.back() < 0.112 + 0.506 / electron.pt() )
        idbit |= 1 << 4;
      if ( fEInvMinusPInv.back() < 0.193 )
        idbit |= 1 << 5;
      if ( fExpectedMissingInnerHits.back() <= 1 )
        idbit |= 1 << 6;
      if ( fPassConvVeto.back() )
        idbit |= 1 << 7;
    } else {  // endcap
      if ( fFull5x5_sigmaIetaIeta.back() < 0.0425 )
        idbit |= 1 << 0;
      if ( fAbsdEtaSeed.back() < 0.00674 )
        idbit |= 1 << 1;
      if ( fAbsdPhiIn.back() < 0.169 )
        idbit |= 1 << 2;
      if ( fHOverE.back() < 0.0441 + 2.54 / electron.superCluster()->energy() + 0.183 * fRho / electron.superCluster()->energy() )
        idbit |= 1 << 3;
      if ( fRelIsoWithEA.back() < 0.108 + 0.963 / electron.pt() )
        idbit |= 1 << 4;
      if ( fEInvMinusPInv.back() < 0.111 )
        idbit |= 1 << 5;
      if ( fExpectedMissingInnerHits.back() <= 1 )
        idbit |= 1 << 6;
      if ( fPassConvVeto.back() )
        idbit |= 1 << 7;
    }
    fIdBit.emplace_back( idbit );
  }
}

void
ffNtupleElectron::clear() {
  fElectronP4.clear();
  fElectronCharge.clear();
  fFull5x5_sigmaIetaIeta.clear();
  fHOverE.clear();
  fAbsdEtaSeed.clear();
  fAbsdPhiIn.clear();
  fRelIsoWithEA.clear();
  fEInvMinusPInv.clear();
  fExpectedMissingInnerHits.clear();
  fPassConvVeto.clear();
  fIdBit.clear();
}

float
ffNtupleElectron::dEtaInSeed( const reco::GsfElectron& electron ) const {
  return electron.superCluster().isNonnull() && electron.superCluster()->seed().isNonnull() ? electron.deltaEtaSuperClusterTrackAtVtx() - electron.superCluster()->eta() + electron.superCluster()->seed()->eta() : std::numeric_limits<float>::max();
}

float
ffNtupleElectron::eInvMinusPInv( const reco::GsfElectron& electron ) const {
  const float ecal_energy_inverse = 1.0 / electron.ecalEnergy();
  const float eSCoverP            = electron.eSuperClusterOverP();
  return std::abs( 1.0 - eSCoverP ) * ecal_energy_inverse;
}

float
ffNtupleElectron::relIsoWithEA( const reco::GsfElectron& electron ) const {
  float       absEta = fabs( electron.superCluster()->eta() );
  auto        pfIso  = electron.pfIsolationVariables();
  const float chad   = pfIso.sumChargedHadronPt;
  const float nhad   = pfIso.sumNeutralHadronEt;
  const float pho    = pfIso.sumPhotonEt;
  const float eA     = fEffArea.getEffectiveArea( absEta );
  float       iso    = chad + std::max( 0., nhad + pho - fRho * eA );
  iso /= electron.pt();
  return iso;
}