#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "RecoEgamma/EgammaTools/interface/EffectiveAreas.h"

using LorentzVector = math::XYZTLorentzVectorF;

class ffNtuplePhoton : public ffNtupleBase {
 public:
  ffNtuplePhoton( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

 private:
  void clear() final;

  edm::EDGetToken                    fPhotonToken;
  math::XYZTLorentzVectorFCollection fPhotonP4;
  std::vector<float>                 fFull5x5_sigmaIetaIeta;
  std::vector<float>                 fHOverE;
  std::vector<bool>                  fHasPixelSeed;
  std::vector<float>                 fIsoChargedHadron;
  std::vector<float>                 fIsoNeutralHadron;
  std::vector<float>                 fIsoPhoton;
  std::vector<float>                 fIsoChargedHadronWithEA;
  std::vector<float>                 fIsoNeutralHadronWithEA;
  std::vector<float>                 fIsoPhotonWithEA;
  std::vector<unsigned int>          fIdBit;

  edm::EDGetToken fRhoToken;
  double          fRho;

  EffectiveAreas fEffAreaChHadron;
  EffectiveAreas fEffAreaNeuHadron;
  EffectiveAreas fEffAreaPhoton;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtuplePhoton, "ffNtuplePhoton" );

ffNtuplePhoton::ffNtuplePhoton( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ),
      fEffAreaChHadron( edm::FileInPath( "RecoEgamma/PhotonIdentification/data/Fall17/effAreaPhotons_cone03_pfChargedHadrons_90percentBased_V2.txt" ).fullPath() ),
      fEffAreaNeuHadron( edm::FileInPath( "RecoEgamma/PhotonIdentification/data/Fall17/effAreaPhotons_cone03_pfNeutralHadrons_90percentBased_V2.txt" ).fullPath() ),
      fEffAreaPhoton( edm::FileInPath( "RecoEgamma/PhotonIdentification/data/Fall17/effAreaPhotons_cone03_pfPhotons_90percentBased_V2.txt" ).fullPath() ) {}

void
ffNtuplePhoton::initialize( TTree&                   tree,
                            const edm::ParameterSet& ps,
                            edm::ConsumesCollector&& cc ) {
  fPhotonToken = cc.consumes<reco::PhotonCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  fRhoToken    = cc.consumes<double>( edm::InputTag( "fixedGridRhoFastjetAll" ) );

  tree.Branch( "photon_p4", &fPhotonP4 );
  tree.Branch( "photon_full5x5SigmaIetaIeta", &fFull5x5_sigmaIetaIeta );
  tree.Branch( "photon_hOverE", &fHOverE );
  tree.Branch( "photon_hasPixelSeed", &fHasPixelSeed );
  tree.Branch( "photon_isoChargedHadron", &fIsoChargedHadron );
  tree.Branch( "photon_isoNeutralHadron", &fIsoNeutralHadron );
  tree.Branch( "photon_isoPhoton", &fIsoPhoton );
  tree.Branch( "photon_isoChargedHadronWithEA", &fIsoChargedHadronWithEA );
  tree.Branch( "photon_isoNeutralHadronWithEA", &fIsoNeutralHadronWithEA );
  tree.Branch( "photon_isoPhotonWithEA", &fIsoPhotonWithEA );
  tree.Branch( "photon_idBit", &fIdBit );
}

void
ffNtuplePhoton::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::PhotonCollection> photonHdl;
  e.getByToken( fPhotonToken, photonHdl );
  assert( photonHdl.isValid() );
  Handle<double> rhoHdl;
  e.getByToken( fRhoToken, rhoHdl );
  assert( rhoHdl.isValid() );
  fRho = *rhoHdl;

  clear();
  for ( const auto& photon : *photonHdl ) {
    fPhotonP4.emplace_back( photon.px(), photon.py(), photon.pz(), photon.energy() );
    fFull5x5_sigmaIetaIeta.emplace_back( photon.full5x5_sigmaIetaIeta() );
    fHOverE.emplace_back( photon.hadTowOverEm() );
    fHasPixelSeed.emplace_back( photon.hasPixelSeed() );
    fIsoChargedHadron.emplace_back( photon.chargedHadronIso() );
    fIsoNeutralHadron.emplace_back( photon.neutralHadronIso() );
    fIsoPhoton.emplace_back( photon.photonIso() );

    float abseta = fabs( photon.superCluster()->eta() );
    fIsoChargedHadronWithEA.emplace_back( max( 0., photon.chargedHadronIso() - fRho * fEffAreaChHadron.getEffectiveArea( abseta ) ) );
    fIsoNeutralHadronWithEA.emplace_back( max( 0., photon.neutralHadronIso() - fRho * fEffAreaNeuHadron.getEffectiveArea( abseta ) ) );
    fIsoPhotonWithEA.emplace_back( max( 0., photon.photonIso() - fRho * fEffAreaPhoton.getEffectiveArea( abseta ) ) );

    // Loose id criteria
    // https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonIdentificationRun2#Offline_selection_criteria_AN2
    unsigned int idbit = 0;
    if ( abseta <= 1.479 ) {  // barrel
      if ( fHOverE.back() < 0.04596 )
        idbit |= 1 << 0;
      if ( fFull5x5_sigmaIetaIeta.back() < 0.0106 )
        idbit |= 1 << 1;
      if ( fIsoChargedHadronWithEA.back() < 1.694 )
        idbit |= 1 << 2;
      if ( fIsoNeutralHadronWithEA.back() < ( 24.032 + 0.01512 * photon.pt() + 2.259e-05 * photon.pt() * photon.pt() ) )
        idbit |= 1 << 3;
      if ( fIsoPhotonWithEA.back() < ( 2.876 + 0.004017 * photon.pt() ) )
        idbit |= 1 << 4;
    } else {  // endcap
      if ( fHOverE.back() < 0.0590 )
        idbit |= 1 << 0;
      if ( fFull5x5_sigmaIetaIeta.back() < 0.0272 )
        idbit |= 1 << 1;
      if ( fIsoChargedHadronWithEA.back() < 2.089 )
        idbit |= 1 << 2;
      if ( fIsoNeutralHadronWithEA.back() < ( 19.722 + 0.0117 * photon.pt() + 2.3e-05 * photon.pt() * photon.pt() ) )
        idbit |= 1 << 3;
      if ( fIsoPhotonWithEA.back() < ( 4.162 + 0.0037 * photon.pt() ) )
        idbit |= 1 << 4;
    }
    fIdBit.emplace_back( idbit );
  }
}

void
ffNtuplePhoton::clear() {
  fPhotonP4.clear();
  fFull5x5_sigmaIetaIeta.clear();
  fHOverE.clear();
  fHasPixelSeed.clear();
  fIsoChargedHadron.clear();
  fIsoNeutralHadron.clear();
  fIsoPhoton.clear();
  fIsoChargedHadronWithEA.clear();
  fIsoNeutralHadronWithEA.clear();
  fIsoPhotonWithEA.clear();
  fIdBit.clear();
}