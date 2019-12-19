#include <algorithm>
#include <functional>
#include <map>
#include <numeric>
#include <sstream>

#include "CommonTools/Statistics/interface/ChiSquaredProbability.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"
#include "Firefighter/recoStuff/interface/RecoHelpers.h"
#include "Firefighter/recoStuff/interface/ffLeptonJetMVAEstimator.h"
#include "Firefighter/recoStuff/interface/ffPFJetProcessors.h"
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicVertex.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"

using Point         = math::XYZPointF;
using LorentzVector = math::XYZTLorentzVectorF;

class ffNtuplePfJet : public ffNtupleBaseNoHLT {
 public:
  ffNtuplePfJet( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken pfjet_token_;
  edm::EDGetToken pvs_token_;
  edm::EDGetToken generaltk_token_;
  edm::EDGetToken pfcandNoPU_token_;
  edm::EDGetToken pfcand_token_;
  edm::EDGetToken subjet_lambda_token_;
  edm::EDGetToken subjet_epsilon_token_;
  edm::EDGetToken subjet_ecf1_token_;
  edm::EDGetToken subjet_ecf2_token_;
  edm::EDGetToken subjet_ecf3_token_;

  bool doVertexing_;
  bool doSubstructureVariables_;
  bool doMVA_;

  StringCutObjectSelector<reco::Track>     track_selector_;
  edm::ParameterSet                        kvfParam_;
  std::vector<double>                      isoRadius_;
  std::unique_ptr<ffLeptonJetMVAEstimator> mvaEstimator_;

  int                                 pfjet_n_;
  std::vector<LorentzVector>          pfjet_p4_;
  std::vector<float>                  pfjet_chargedHadronE_;
  std::vector<float>                  pfjet_neutralHadronE_;
  std::vector<float>                  pfjet_chargedEmE_;
  std::vector<float>                  pfjet_neutralEmE_;
  std::vector<float>                  pfjet_photonE_;
  std::vector<float>                  pfjet_electronE_;
  std::vector<float>                  pfjet_muonE_;
  std::vector<float>                  pfjet_hfHadronE_;
  std::vector<float>                  pfjet_hfEmE_;
  std::vector<int>                    pfjet_charged_n_;
  std::vector<int>                    pfjet_neutral_n_;
  std::vector<int>                    pfjet_chargedHadron_n_;
  std::vector<int>                    pfjet_neutralHadron_n_;
  std::vector<int>                    pfjet_photon_n_;
  std::vector<int>                    pfjet_electron_n_;
  std::vector<int>                    pfjet_muon_n_;
  std::vector<float>                  pfjet_area_;
  std::vector<float>                  pfjet_maxDistance_;
  std::map<float, std::vector<float>> pfjet_pfIsolationNoPU_;
  std::map<float, std::vector<float>> pfjet_neuIsolationNoPU_;
  std::map<float, std::vector<float>> pfjet_hadIsolationNoPU_;
  std::map<float, std::vector<float>> pfjet_pfIsolation_;
  std::map<float, std::vector<float>> pfjet_neuIsolation_;
  std::map<float, std::vector<float>> pfjet_hadIsolation_;
  std::map<float, std::vector<float>> pfjet_tkPtSum_;
  std::map<float, std::vector<float>> pfjet_tkIsolation_;
  std::vector<int>                    pfjet_tracks_n_;
  std::vector<float>                  pfjet_ptDistribution_;
  std::vector<float>                  pfjet_dRSpread_;

  std::vector<int>   pfjet_pfcands_n_;
  std::vector<float> pfjet_pfcands_chargedMass_;
  std::vector<int>   pfjet_pfcands_nDsaMu_;
  std::vector<int>   pfjet_pfcands_maxPtType_;
  std::vector<float> pfjet_pfcands_minTwoTkDist_;

  std::vector<std::vector<int>>   pfjet_pfcand_type_;
  std::vector<std::vector<int>>   pfjet_pfcand_charge_;
  std::vector<std::vector<float>> pfjet_pfcand_pt_;
  std::vector<std::vector<float>> pfjet_pfcand_eta_;
  std::vector<std::vector<float>> pfjet_pfcand_phi_;
  std::vector<std::vector<float>> pfjet_pfcand_energy_;
  std::vector<std::vector<float>> pfjet_pfcand_tkD0_;
  std::vector<std::vector<float>> pfjet_pfcand_tkD0Sig_;
  std::vector<std::vector<float>> pfjet_pfcand_tkDz_;
  std::vector<std::vector<float>> pfjet_pfcand_tkDzSig_;
  std::vector<std::vector<float>> pfjet_pfcand_tkNormChi2_;

  std::vector<std::vector<unsigned int>> pfjet_pfcand_electronIdx_;
  std::vector<std::vector<unsigned int>> pfjet_pfcand_photonIdx_;
  std::vector<std::vector<unsigned int>> pfjet_pfcand_pfmuonIdx_;
  std::vector<std::vector<unsigned int>> pfjet_pfcand_dsamuonIdx_;

  std::vector<std::vector<float>> pfjet_pfcand_muonTime_;
  std::vector<std::vector<float>> pfjet_pfcand_muonTimeErr_;
  std::vector<float>              pfjet_pfcand_muonTimeStd_;

  std::vector<float> pfjet_pfcand_tkD0Max_;
  std::vector<float> pfjet_pfcand_tkD0Sub_;
  std::vector<float> pfjet_pfcand_tkD0Min_;
  std::vector<float> pfjet_pfcand_tkD0SigMax_;
  std::vector<float> pfjet_pfcand_tkD0SigSub_;
  std::vector<float> pfjet_pfcand_tkD0SigMin_;

  std::vector<Point> pfjet_medianvtx_;
  std::vector<Point> pfjet_averagevtx_;

  std::vector<Point>              pfjet_klmvtx_;
  std::vector<float>              pfjet_klmvtx_lxy_;
  std::vector<float>              pfjet_klmvtx_l3d_;
  std::vector<float>              pfjet_klmvtx_lxySig_;
  std::vector<float>              pfjet_klmvtx_l3dSig_;
  std::vector<float>              pfjet_klmvtx_normChi2_;
  std::vector<float>              pfjet_klmvtx_prob_;
  std::vector<float>              pfjet_klmvtx_mass_;
  std::vector<float>              pfjet_klmvtx_cosThetaXy_;
  std::vector<float>              pfjet_klmvtx_cosTheta3d_;
  std::vector<float>              pfjet_klmvtx_impactDistXy_;
  std::vector<float>              pfjet_klmvtx_impactDist3d_;
  std::vector<std::vector<float>> pfjet_klmvtx_tkImpactDist2d_;
  std::vector<std::vector<float>> pfjet_klmvtx_tkImpactDist3d_;

  std::vector<Point>              pfjet_kinvtx_;
  std::vector<float>              pfjet_kinvtx_lxy_;
  std::vector<float>              pfjet_kinvtx_l3d_;
  std::vector<float>              pfjet_kinvtx_lxySig_;
  std::vector<float>              pfjet_kinvtx_l3dSig_;
  std::vector<float>              pfjet_kinvtx_normChi2_;
  std::vector<float>              pfjet_kinvtx_prob_;
  std::vector<float>              pfjet_kinvtx_mass_;
  std::vector<float>              pfjet_kinvtx_cosThetaXy_;
  std::vector<float>              pfjet_kinvtx_cosTheta3d_;
  std::vector<float>              pfjet_kinvtx_impactDistXy_;
  std::vector<float>              pfjet_kinvtx_impactDist3d_;
  std::vector<std::vector<float>> pfjet_kinvtx_tkImpactDist2d_;
  std::vector<std::vector<float>> pfjet_kinvtx_tkImpactDist3d_;

  std::vector<float> pfjet_subjet_lambda_;
  std::vector<float> pfjet_subjet_epsilon_;
  std::vector<float> pfjet_subjet_ecf1_;
  std::vector<float> pfjet_subjet_ecf2_;
  std::vector<float> pfjet_subjet_ecf3_;

  std::vector<float> pfjet_mva_;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtuplePfJet, "ffNtuplePfJet" );

ffNtuplePfJet::ffNtuplePfJet( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ),
      doVertexing_( ps.getParameter<bool>( "doVertexing" ) ),
      doSubstructureVariables_( ps.getParameter<bool>( "doSubstructureVariables" ) ),
      doMVA_( ps.getParameter<bool>( "doMVA" ) ),
      track_selector_( ps.getParameter<std::string>( "TrackSelection" ), true ),
      isoRadius_( ps.getParameter<std::vector<double>>( "IsolationRadius" ) ) {
  if ( doVertexing_ ) {
    assert( ps.existsAs<edm::ParameterSet>( "kvfParam" ) );
    kvfParam_ = ps.getParameter<edm::ParameterSet>( "kvfParam" );
  }

  if ( !doSubstructureVariables_ and doMVA_ ) {
    std::cout << "[ffNtuplePfJet]\t doSubstructureVariables need to be set as True if doMVA is set to be true. ";
    std::cout << "I am disabling both!" << std::endl;
    doMVA_ = false;
  }

  if ( doMVA_ ) {
    assert( ps.existsAs<edm::ParameterSet>( "mvaParam" ) );
    mvaEstimator_ = std::make_unique<ffLeptonJetMVAEstimator>( ps.getParameter<edm::ParameterSet>( "mvaParam" ) );
  }
}

void
ffNtuplePfJet::initialize( TTree&                   tree,
                           const edm::ParameterSet& ps,
                           edm::ConsumesCollector&& cc ) {
  pfjet_token_      = cc.consumes<reco::PFJetCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  pvs_token_        = cc.consumes<reco::VertexCollection>( ps.getParameter<edm::InputTag>( "PrimaryVertices" ) );
  generaltk_token_  = cc.consumes<reco::TrackCollection>( ps.getParameter<edm::InputTag>( "GeneralTracks" ) );
  pfcandNoPU_token_ = cc.consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "pfNoPileUpIso" ) );
  pfcand_token_     = cc.consumes<reco::PFCandidateCollection>( edm::InputTag( "particleFlow" ) );

  if ( doSubstructureVariables_ ) {
    subjet_lambda_token_  = cc.consumes<edm::ValueMap<float>>( ps.getParameter<edm::InputTag>( "SubjetMomentumDistribution" ) );
    subjet_epsilon_token_ = cc.consumes<edm::ValueMap<float>>( ps.getParameter<edm::InputTag>( "SubjetEnergyDistributioin" ) );
    subjet_ecf1_token_    = cc.consumes<edm::ValueMap<float>>( ps.getParameter<edm::InputTag>( "SubjetEcf1" ) );
    subjet_ecf2_token_    = cc.consumes<edm::ValueMap<float>>( ps.getParameter<edm::InputTag>( "SubjetEcf2" ) );
    subjet_ecf3_token_    = cc.consumes<edm::ValueMap<float>>( ps.getParameter<edm::InputTag>( "SubjetEcf3" ) );
  }

  tree.Branch( "pfjet_n", &pfjet_n_, "pfjet_n/I" );
  tree.Branch( "pfjet_p4", &pfjet_p4_ );
  tree.Branch( "pfjet_chargedHadronE", &pfjet_chargedHadronE_ );
  tree.Branch( "pfjet_neutralHadronE", &pfjet_neutralHadronE_ );
  tree.Branch( "pfjet_chargedEmE", &pfjet_chargedEmE_ );
  tree.Branch( "pfjet_neutralEmE", &pfjet_neutralEmE_ );
  tree.Branch( "pfjet_photonE", &pfjet_photonE_ );
  tree.Branch( "pfjet_electronE", &pfjet_electronE_ );
  tree.Branch( "pfjet_muonE", &pfjet_muonE_ );
  tree.Branch( "pfjet_hfHadronE", &pfjet_hfHadronE_ );
  tree.Branch( "pfjet_hfEmE", &pfjet_hfEmE_ );
  tree.Branch( "pfjet_charged_n", &pfjet_charged_n_ );
  tree.Branch( "pfjet_neutral_n", &pfjet_neutral_n_ );
  tree.Branch( "pfjet_chargedHadron_n", &pfjet_chargedHadron_n_ );
  tree.Branch( "pfjet_neutralHadron_n", &pfjet_neutralHadron_n_ );
  tree.Branch( "pfjet_photon_n", &pfjet_photon_n_ );
  tree.Branch( "pfjet_electron_n", &pfjet_electron_n_ );
  tree.Branch( "pfjet_muon_n", &pfjet_muon_n_ );
  tree.Branch( "pfjet_area", &pfjet_area_ );
  tree.Branch( "pfjet_maxDistance", &pfjet_maxDistance_ );
  for ( const double& isor : isoRadius_ ) {
    pfjet_pfIsolationNoPU_[ isor ]  = {};
    pfjet_neuIsolationNoPU_[ isor ] = {};
    pfjet_hadIsolationNoPU_[ isor ] = {};
    pfjet_pfIsolation_[ isor ]      = {};
    pfjet_neuIsolation_[ isor ]     = {};
    pfjet_hadIsolation_[ isor ]     = {};
    pfjet_tkPtSum_[ isor ]          = {};
    pfjet_tkIsolation_[ isor ]      = {};
    std::stringstream ss;
    ss << isor;
    std::string suffix = ss.str().replace( 1, 1, "" );
    tree.Branch( ( "pfjet_pfIsolationNoPU" + suffix ).c_str(), &pfjet_pfIsolationNoPU_[ isor ] );
    tree.Branch( ( "pfjet_neuIsolationNoPU" + suffix ).c_str(), &pfjet_neuIsolationNoPU_[ isor ] );
    tree.Branch( ( "pfjet_hadIsolationNoPU" + suffix ).c_str(), &pfjet_hadIsolationNoPU_[ isor ] );
    tree.Branch( ( "pfjet_pfIsolation" + suffix ).c_str(), &pfjet_pfIsolation_[ isor ] );
    tree.Branch( ( "pfjet_neuIsolation" + suffix ).c_str(), &pfjet_neuIsolation_[ isor ] );
    tree.Branch( ( "pfjet_hadIsolation" + suffix ).c_str(), &pfjet_hadIsolation_[ isor ] );
    tree.Branch( ( "pfjet_tkPtSum" + suffix ).c_str(), &pfjet_tkPtSum_[ isor ] );
    tree.Branch( ( "pfjet_tkIsolation" + suffix ).c_str(), &pfjet_tkIsolation_[ isor ] );
  }
  tree.Branch( "pfjet_pfcands_n", &pfjet_pfcands_n_ );
  tree.Branch( "pfjet_tracks_n", &pfjet_tracks_n_ );
  tree.Branch( "pfjet_ptDistribution", &pfjet_ptDistribution_ );
  tree.Branch( "pfjet_dRSpread", &pfjet_dRSpread_ );
  tree.Branch( "pfjet_pfcands_chargedMass", &pfjet_pfcands_chargedMass_ );
  tree.Branch( "pfjet_pfcands_nDsaMu", &pfjet_pfcands_nDsaMu_ );
  tree.Branch( "pfjet_pfcands_maxPtType", &pfjet_pfcands_maxPtType_ );
  tree.Branch( "pfjet_pfcands_minTwoTkDist", &pfjet_pfcands_minTwoTkDist_ );

  tree.Branch( "pfjet_pfcand_type", &pfjet_pfcand_type_ );
  tree.Branch( "pfjet_pfcand_charge", &pfjet_pfcand_charge_ );
  tree.Branch( "pfjet_pfcand_pt", &pfjet_pfcand_pt_ );
  tree.Branch( "pfjet_pfcand_eta", &pfjet_pfcand_eta_ );
  tree.Branch( "pfjet_pfcand_phi", &pfjet_pfcand_phi_ );
  tree.Branch( "pfjet_pfcand_energy", &pfjet_pfcand_energy_ );
  tree.Branch( "pfjet_pfcand_tkD0", &pfjet_pfcand_tkD0_ );
  tree.Branch( "pfjet_pfcand_tkD0Sig", &pfjet_pfcand_tkD0Sig_ );
  tree.Branch( "pfjet_pfcand_tkDz", &pfjet_pfcand_tkDz_ );
  tree.Branch( "pfjet_pfcand_tkDzSig", &pfjet_pfcand_tkDzSig_ );
  tree.Branch( "pfjet_pfcand_tkNormChi2", &pfjet_pfcand_tkNormChi2_ );
  tree.Branch( "pfjet_pfcand_electronIdx", &pfjet_pfcand_electronIdx_ );
  tree.Branch( "pfjet_pfcand_photonIdx", &pfjet_pfcand_photonIdx_ );
  tree.Branch( "pfjet_pfcand_pfmuonIdx", &pfjet_pfcand_pfmuonIdx_ );
  tree.Branch( "pfjet_pfcand_dsamuonIdx", &pfjet_pfcand_dsamuonIdx_ );

  tree.Branch( "pfjet_pfcand_muonTime", &pfjet_pfcand_muonTime_ );
  tree.Branch( "pfjet_pfcand_muonTimeErr", &pfjet_pfcand_muonTimeErr_ );
  tree.Branch( "pfjet_pfcand_muonTimeStd", &pfjet_pfcand_muonTimeStd_ );

  tree.Branch( "pfjet_pfcand_tkD0Max", &pfjet_pfcand_tkD0Max_ );
  tree.Branch( "pfjet_pfcand_tkD0Sub", &pfjet_pfcand_tkD0Sub_ );
  tree.Branch( "pfjet_pfcand_tkD0Min", &pfjet_pfcand_tkD0Min_ );
  tree.Branch( "pfjet_pfcand_tkD0SigMax", &pfjet_pfcand_tkD0SigMax_ );
  tree.Branch( "pfjet_pfcand_tkD0SigSub", &pfjet_pfcand_tkD0SigSub_ );
  tree.Branch( "pfjet_pfcand_tkD0SigMin", &pfjet_pfcand_tkD0SigMin_ );

  if ( doVertexing_ ) {
    tree.Branch( "pfjet_medianvtx", &pfjet_medianvtx_ );
    tree.Branch( "pfjet_averagevtx", &pfjet_averagevtx_ );

    tree.Branch( "pfjet_klmvtx", &pfjet_klmvtx_ );
    tree.Branch( "pfjet_klmvtx_lxy", &pfjet_klmvtx_lxy_ );
    tree.Branch( "pfjet_klmvtx_l3d", &pfjet_klmvtx_l3d_ );
    tree.Branch( "pfjet_klmvtx_lxySig", &pfjet_klmvtx_lxySig_ );
    tree.Branch( "pfjet_klmvtx_l3dSig", &pfjet_klmvtx_l3dSig_ );
    tree.Branch( "pfjet_klmvtx_normChi2", &pfjet_klmvtx_normChi2_ );
    tree.Branch( "pfjet_klmvtx_prob", &pfjet_klmvtx_prob_ );
    tree.Branch( "pfjet_klmvtx_mass", &pfjet_klmvtx_mass_ );
    tree.Branch( "pfjet_klmvtx_cosThetaXy", &pfjet_klmvtx_cosThetaXy_ );
    tree.Branch( "pfjet_klmvtx_cosTheta3d", &pfjet_klmvtx_cosTheta3d_ );
    tree.Branch( "pfjet_klmvtx_impactDistXy", &pfjet_klmvtx_impactDistXy_ );
    tree.Branch( "pfjet_klmvtx_impactDist3d", &pfjet_klmvtx_impactDist3d_ );
    tree.Branch( "pfjet_klmvtx_tkImpactDist2d", &pfjet_klmvtx_tkImpactDist2d_ );
    tree.Branch( "pfjet_klmvtx_tkImpactDist3d", &pfjet_klmvtx_tkImpactDist3d_ );

    tree.Branch( "pfjet_kinvtx", &pfjet_kinvtx_ );
    tree.Branch( "pfjet_kinvtx_lxy", &pfjet_kinvtx_lxy_ );
    tree.Branch( "pfjet_kinvtx_l3d", &pfjet_kinvtx_l3d_ );
    tree.Branch( "pfjet_kinvtx_lxySig", &pfjet_kinvtx_lxySig_ );
    tree.Branch( "pfjet_kinvtx_l3dSig", &pfjet_kinvtx_l3dSig_ );
    tree.Branch( "pfjet_kinvtx_normChi2", &pfjet_kinvtx_normChi2_ );
    tree.Branch( "pfjet_kinvtx_prob", &pfjet_kinvtx_prob_ );
    tree.Branch( "pfjet_kinvtx_mass", &pfjet_kinvtx_mass_ );
    tree.Branch( "pfjet_kinvtx_cosThetaXy", &pfjet_kinvtx_cosThetaXy_ );
    tree.Branch( "pfjet_kinvtx_cosTheta3d", &pfjet_kinvtx_cosTheta3d_ );
    tree.Branch( "pfjet_kinvtx_impactDistXy", &pfjet_kinvtx_impactDistXy_ );
    tree.Branch( "pfjet_kinvtx_impactDist3d", &pfjet_kinvtx_impactDist3d_ );
    tree.Branch( "pfjet_kinvtx_tkImpactDist2d", &pfjet_kinvtx_tkImpactDist2d_ );
    tree.Branch( "pfjet_kinvtx_tkImpactDist3d", &pfjet_kinvtx_tkImpactDist3d_ );
  }
  if ( doSubstructureVariables_ ) {
    tree.Branch( "pfjet_subjet_lambda", &pfjet_subjet_lambda_ );
    tree.Branch( "pfjet_subjet_epsilon", &pfjet_subjet_epsilon_ );
    tree.Branch( "pfjet_subjet_ecf1", &pfjet_subjet_ecf1_ );
    tree.Branch( "pfjet_subjet_ecf2", &pfjet_subjet_ecf2_ );
    tree.Branch( "pfjet_subjet_ecf3", &pfjet_subjet_ecf3_ );
  }

  if ( doMVA_ ) {
    tree.Branch( "pfjet_mva", &pfjet_mva_ );
  }
}

void
ffNtuplePfJet::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;
  using namespace ff;

  Handle<reco::PFJetCollection> pfjet_h;
  e.getByToken( pfjet_token_, pfjet_h );
  assert( pfjet_h.isValid() );
  const reco::PFJetCollection& pfjets = *pfjet_h;

  Handle<reco::VertexCollection> pvs_h;
  e.getByToken( pvs_token_, pvs_h );
  assert( pvs_h.isValid() && pvs_h->size() > 0 );
  const auto& pv = *( pvs_h->begin() );

  Handle<reco::TrackCollection> generalTk_h;
  e.getByToken( generaltk_token_, generalTk_h );
  assert( generalTk_h.isValid() );

  Handle<reco::PFCandidateFwdPtrVector> pfCandNoPU_h;
  e.getByToken( pfcandNoPU_token_, pfCandNoPU_h );
  assert( pfCandNoPU_h.isValid() );

  Handle<reco::PFCandidateCollection> pfCand_h;
  e.getByToken( pfcand_token_, pfCand_h );
  assert( pfCand_h.isValid() );

  Handle<ValueMap<float>> subjet_lambda_h;
  Handle<ValueMap<float>> subjet_epsilon_h;
  Handle<ValueMap<float>> subjet_ecf1_h;
  Handle<ValueMap<float>> subjet_ecf2_h;
  Handle<ValueMap<float>> subjet_ecf3_h;

  if ( doSubstructureVariables_ ) {
    e.getByToken( subjet_lambda_token_, subjet_lambda_h );
    e.getByToken( subjet_epsilon_token_, subjet_epsilon_h );
    e.getByToken( subjet_ecf1_token_, subjet_ecf1_h );
    e.getByToken( subjet_ecf2_token_, subjet_ecf2_h );
    e.getByToken( subjet_ecf3_token_, subjet_ecf3_h );

    assert( subjet_lambda_h.isValid() );
    assert( subjet_epsilon_h.isValid() );
    assert( subjet_ecf1_h.isValid() );
    assert( subjet_ecf2_h.isValid() );
    assert( subjet_ecf3_h.isValid() );
  }

  clear();

  pfjet_n_ = pfjets.size();
  for ( const auto& pfjet : pfjets ) {
    const vector<const reco::Track*>   tracksSelected = getSelectedTracks( pfjet, track_selector_ );
    const vector<reco::PFCandidatePtr> pfCands        = getPFCands( pfjet );

    pfjet_p4_.emplace_back( pfjet.px(), pfjet.py(), pfjet.pz(), pfjet.energy() );
    pfjet_chargedHadronE_.emplace_back( pfjet.chargedHadronEnergy() );
    pfjet_neutralHadronE_.emplace_back( pfjet.neutralHadronEnergy() );
    pfjet_chargedEmE_.emplace_back( pfjet.chargedEmEnergy() );
    pfjet_neutralEmE_.emplace_back( pfjet.neutralEmEnergy() );
    pfjet_photonE_.emplace_back( pfjet.photonEnergy() );
    pfjet_electronE_.emplace_back( pfjet.electronEnergy() );
    pfjet_muonE_.emplace_back( pfjet.muonEnergy() );
    pfjet_hfHadronE_.emplace_back( pfjet.HFHadronEnergy() );
    pfjet_hfEmE_.emplace_back( pfjet.HFEMEnergy() );
    pfjet_charged_n_.emplace_back( pfjet.chargedMultiplicity() );
    pfjet_neutral_n_.emplace_back( pfjet.neutralMultiplicity() );
    pfjet_chargedHadron_n_.emplace_back( pfjet.chargedHadronMultiplicity() );
    pfjet_neutralHadron_n_.emplace_back( pfjet.neutralHadronMultiplicity() );
    pfjet_photon_n_.emplace_back( pfjet.photonMultiplicity() );
    pfjet_electron_n_.emplace_back( pfjet.electronMultiplicity() );
    pfjet_muon_n_.emplace_back( pfjet.muonMultiplicity() );
    pfjet_area_.emplace_back( pfjet.jetArea() );
    pfjet_maxDistance_.emplace_back( pfjet.maxDistance() );

    for ( const double& isor : isoRadius_ ) {
      pfjet_pfIsolationNoPU_[ isor ].emplace_back( getPfIsolation( pfjet, pfCandNoPU_h, isor ) );
      pfjet_neuIsolationNoPU_[ isor ].emplace_back( getNeutralIsolation( pfjet, pfCandNoPU_h, isor ) );
      pfjet_hadIsolationNoPU_[ isor ].emplace_back( getHadronIsolation( pfjet, pfCandNoPU_h, isor ) );

      pfjet_pfIsolation_[ isor ].emplace_back( getPfIsolation( pfjet, pfCand_h, isor ) );
      pfjet_neuIsolation_[ isor ].emplace_back( getNeutralIsolation( pfjet, pfCand_h, isor ) );
      pfjet_hadIsolation_[ isor ].emplace_back( getHadronIsolation( pfjet, pfCand_h, isor ) );

      pfjet_tkPtSum_[ isor ].emplace_back( getTkPtSumInCone( pfjet, generalTk_h, *pvs_h, isor ) );
      pfjet_tkIsolation_[ isor ].emplace_back( getTkIsolation( pfjet, generalTk_h, *pvs_h, isor ) );
    }

    pfjet_pfcands_n_.emplace_back( pfCands.size() );
    pfjet_tracks_n_.emplace_back( tracksSelected.size() );
    pfjet_ptDistribution_.emplace_back( pfjet.constituentPtDistribution() );
    pfjet_dRSpread_.emplace_back( pfjet.constituentEtaPhiSpread() );
    pfjet_pfcands_chargedMass_.emplace_back( chargedMass( pfjet ) );
    pfjet_pfcands_nDsaMu_.emplace_back( getNumberOfDisplacedStandAloneMuons( pfjet, generalTk_h ) );
    pfjet_pfcands_maxPtType_.emplace_back( getCandType( getCandWithMaxPt( pfCands ), generalTk_h ) );
    pfjet_pfcands_minTwoTkDist_.emplace_back( getMinDistAnyTwoTracks( pfjet, es ) );

    // pfcand ------------------------------------------------------------------
    vector<int>   cPFCandType{};
    vector<int>   cPFCandCharge{};
    vector<float> cPFCandPt{}, cPFCandEnergy{};
    vector<float> cPFCandEta{}, cPFCandPhi{};
    vector<float> cPFCandTkD0{}, cPFCandTkD0Sig{};
    vector<float> cPFCandTkDz{}, cPFCandTkDzSig{};
    vector<float> cPFCandTkNormChi2{};
    vector<float> cPFCandMuonTime{}, cPFCandMuonTimeErr{};

    vector<unsigned int> cPFCandElectronIdx{}, cPFCandPhotonIdx{}, cPFCandPFMuonIdx{}, cPFCandDSAMuonIdx{};

    for ( const auto& cand : pfCands ) {
      cPFCandType.emplace_back( getCandType( cand, generalTk_h ) );
      cPFCandCharge.emplace_back( cand->charge() );
      cPFCandPt.emplace_back( cand->pt() );
      cPFCandEta.emplace_back( cand->eta() );
      cPFCandPhi.emplace_back( cand->phi() );
      cPFCandEnergy.emplace_back( cand->energy() );

      const reco::Track* candEmbedTrack = cand->bestTrack();

      cPFCandTkD0.emplace_back( candEmbedTrack != nullptr
                                    ? -candEmbedTrack->dxy( pv.position() )
                                    : NAN );
      cPFCandTkD0Sig.emplace_back(
          candEmbedTrack != nullptr
              ? fabs( -candEmbedTrack->dxy( pv.position() ) /
                      candEmbedTrack->dxyError() )
              : NAN );
      cPFCandTkDz.emplace_back( candEmbedTrack != nullptr
                                    ? candEmbedTrack->dz( pv.position() )
                                    : NAN );
      cPFCandTkDzSig.emplace_back(
          candEmbedTrack != nullptr
              ? fabs( candEmbedTrack->dz( pv.position() ) /
                      candEmbedTrack->dzError() )
              : NAN );
      cPFCandTkNormChi2.emplace_back( candEmbedTrack != nullptr &&
                                              candEmbedTrack->ndof() != 0
                                          ? candEmbedTrack->normalizedChi2()
                                          : NAN );
      if ( cPFCandType.back() == 2 and cand->gsfElectronRef().isNonnull() )
        cPFCandElectronIdx.emplace_back( cand->gsfElectronRef().key() );
      if ( cPFCandType.back() == 3 and cand->muonRef().isNonnull() )
        cPFCandPFMuonIdx.emplace_back( cand->muonRef().key() );
      if ( cPFCandType.back() == 4 and cand->photonRef().isNonnull() )
        cPFCandPhotonIdx.emplace_back( cand->photonRef().key() );
      if ( cPFCandType.back() == 8 and cand->muonRef().isNonnull() )
        cPFCandDSAMuonIdx.emplace_back( cand->muonRef().key() );

      const reco::MuonRef cmuref = cand->muonRef();
      cPFCandMuonTime.emplace_back( cmuref.isNonnull() and cmuref->isTimeValid()
                                        ? cmuref->time().timeAtIpInOut
                                        : NAN );
      cPFCandMuonTimeErr.emplace_back( cmuref.isNonnull() and
                                               cmuref->isTimeValid()
                                           ? cmuref->time().timeAtIpInOutErr
                                           : NAN );
    }

    pfjet_pfcand_type_.push_back( cPFCandType );
    pfjet_pfcand_charge_.push_back( cPFCandCharge );
    pfjet_pfcand_pt_.push_back( cPFCandPt );
    pfjet_pfcand_eta_.push_back( cPFCandEta );
    pfjet_pfcand_phi_.push_back( cPFCandPhi );
    pfjet_pfcand_energy_.push_back( cPFCandEnergy );
    pfjet_pfcand_tkD0_.push_back( cPFCandTkD0 );
    pfjet_pfcand_tkD0Sig_.push_back( cPFCandTkD0Sig );
    pfjet_pfcand_tkDz_.push_back( cPFCandTkDz );
    pfjet_pfcand_tkDzSig_.push_back( cPFCandTkDzSig );
    pfjet_pfcand_tkNormChi2_.push_back( cPFCandTkNormChi2 );
    pfjet_pfcand_electronIdx_.push_back( cPFCandElectronIdx );
    pfjet_pfcand_pfmuonIdx_.push_back( cPFCandPFMuonIdx );
    pfjet_pfcand_photonIdx_.push_back( cPFCandPhotonIdx );
    pfjet_pfcand_dsamuonIdx_.push_back( cPFCandDSAMuonIdx );

    pfjet_pfcand_muonTime_.push_back( cPFCandMuonTime );
    pfjet_pfcand_muonTimeErr_.push_back( cPFCandMuonTimeErr );
    pfjet_pfcand_muonTimeStd_.push_back( ff::calculateStandardDeviation<float>( cPFCandMuonTime ) );

    vector<float> cPFCandTkD0Cleaned{}, cPFCandTkD0SigCleaned{};
    vector<float> cPFCandTkAbsD0;
    transform( cPFCandTkD0.begin(), cPFCandTkD0.end(), back_inserter( cPFCandTkAbsD0 ), []( const float& n ) { return fabs( n ); } );
    copy_if( cPFCandTkAbsD0.begin(), cPFCandTkAbsD0.end(), back_inserter( cPFCandTkD0Cleaned ), []( const float& val ) { return !isnan( val ); } );
    copy_if( cPFCandTkD0Sig.begin(), cPFCandTkD0Sig.end(), back_inserter( cPFCandTkD0SigCleaned ), []( const float& val ) { return !isnan( val ); } );
    sort( cPFCandTkD0Cleaned.begin(), cPFCandTkD0Cleaned.end(), greater<float>() );
    sort( cPFCandTkD0SigCleaned.begin(), cPFCandTkD0SigCleaned.end(), greater<float>() );

    pfjet_pfcand_tkD0Max_.emplace_back( cPFCandTkD0Cleaned.size() > 0 ? cPFCandTkD0Cleaned[ 0 ] : NAN );
    pfjet_pfcand_tkD0Sub_.emplace_back( cPFCandTkD0Cleaned.size() > 1 ? cPFCandTkD0Cleaned[ 1 ] : NAN );
    pfjet_pfcand_tkD0Min_.emplace_back( cPFCandTkD0Cleaned.size() > 0 ? cPFCandTkD0Cleaned[ cPFCandTkD0Cleaned.size() - 1 ] : NAN );
    pfjet_pfcand_tkD0SigMax_.emplace_back( cPFCandTkD0SigCleaned.size() > 0 ? cPFCandTkD0SigCleaned[ 0 ] : NAN );
    pfjet_pfcand_tkD0SigSub_.emplace_back( cPFCandTkD0SigCleaned.size() > 1 ? cPFCandTkD0SigCleaned[ 1 ] : NAN );
    pfjet_pfcand_tkD0SigMin_.emplace_back( cPFCandTkD0SigCleaned.size() > 0 ? cPFCandTkD0SigCleaned[ cPFCandTkD0SigCleaned.size() - 1 ] : NAN );
    // -------------------------------------------------------------------------

    // vertices ----------------------------------------------------------------
    if ( doVertexing_ ) {
      pfjet_medianvtx_.push_back( estimatedVertexFromMedianReferencePoints( tracksSelected ) );
      pfjet_averagevtx_.push_back( estimatedVertexFromAverageReferencePoints( tracksSelected ) );

      vector<reco::TransientTrack> transientTks = transientTracksFromPFJet( pfjet, track_selector_, es );
      Measurement1D                distXY;
      Measurement1D                dist3D;

      GlobalVector pfjetMomentum( pfjet.px(), pfjet.py(), pfjet.pz() );

      const auto             klmVtxInfo  = kalmanVertexFromTransientTracks( transientTks, kvfParam_ );
      const TransientVertex& klmVtx      = klmVtxInfo.first;
      const float&           klmVtxMass  = klmVtxInfo.second;
      bool                   klmVtxValid = klmVtx.isValid();

      distXY = klmVtxValid ? signedDistanceXY( pv, klmVtx.vertexState(), pfjetMomentum ) : Measurement1D();
      dist3D = klmVtxValid ? signedDistance3D( pv, klmVtx.vertexState(), pfjetMomentum ) : Measurement1D();

      pfjet_klmvtx_.emplace_back( klmVtxValid ? Point( klmVtx.position().x(),
                                                       klmVtx.position().y(),
                                                       klmVtx.position().z() )
                                              : Point( NAN, NAN, NAN ) );
      pfjet_klmvtx_lxy_.emplace_back( distXY.significance() ? distXY.value() : NAN );
      pfjet_klmvtx_l3d_.emplace_back( dist3D.significance() ? dist3D.value() : NAN );
      pfjet_klmvtx_lxySig_.emplace_back( distXY.significance() ? distXY.value() / distXY.error() : NAN );
      pfjet_klmvtx_l3dSig_.emplace_back( dist3D.significance() ? dist3D.value() / dist3D.error() : NAN );
      pfjet_klmvtx_normChi2_.emplace_back(
          klmVtxValid && klmVtx.degreesOfFreedom() ? klmVtx.normalisedChiSquared()
                                                   : NAN );
      pfjet_klmvtx_prob_.emplace_back(
          klmVtxValid ? ChiSquaredProbability( klmVtx.totalChiSquared(),
                                               klmVtx.degreesOfFreedom() )
                      : NAN );
      pfjet_klmvtx_mass_.emplace_back( klmVtxValid ? klmVtxMass : NAN );
      pfjet_klmvtx_cosThetaXy_.emplace_back(
          klmVtxValid
              ? cosThetaOfJetPvXY( pv, klmVtx.vertexState(), pfjetMomentum )
              : NAN );
      pfjet_klmvtx_cosTheta3d_.emplace_back(
          klmVtxValid
              ? cosThetaOfJetPv3D( pv, klmVtx.vertexState(), pfjetMomentum )
              : NAN );
      pfjet_klmvtx_impactDistXy_.emplace_back(
          klmVtxValid
              ? impactDistanceXY( pv, klmVtx.vertexState(), pfjetMomentum )
              : NAN );
      pfjet_klmvtx_impactDist3d_.emplace_back(
          klmVtxValid
              ? impactDistance3D( pv, klmVtx.vertexState(), pfjetMomentum )
              : NAN );

      vector<float> trackImpactDist2dKlmVtx{}, trackImpactDist3dKlmVtx{};
      if ( klmVtxValid ) {
        for ( const auto& tt : transientTks ) {
          pair<bool, Measurement1D> impact2dResult =
              ff::absoluteTransverseImpactParameter( tt, klmVtx.vertexState() );
          trackImpactDist2dKlmVtx.emplace_back(
              impact2dResult.first && impact2dResult.second.significance()
                  ? impact2dResult.second.value()
                  : NAN );

          pair<bool, Measurement1D> impact3dResult =
              ff::absoluteImpactParameter3D( tt, klmVtx.vertexState() );
          trackImpactDist3dKlmVtx.emplace_back(
              impact3dResult.first && impact3dResult.second.significance()
                  ? impact3dResult.second.value()
                  : NAN );
        }
      }
      pfjet_klmvtx_tkImpactDist2d_.emplace_back( trackImpactDist2dKlmVtx );
      pfjet_klmvtx_tkImpactDist3d_.emplace_back( trackImpactDist3dKlmVtx );

      const auto             kinVtxInfo  = kinematicVertexFromTransientTracks( transientTks );
      const KinematicVertex& kinVtx      = kinVtxInfo.first;
      const float&           kinVtxMass  = kinVtxInfo.second;
      bool                   kinVtxValid = kinVtx.vertexIsValid();

      distXY = kinVtxValid
                   ? signedDistanceXY( pv, kinVtx.vertexState(), pfjetMomentum )
                   : Measurement1D();
      dist3D = kinVtxValid
                   ? signedDistance3D( pv, kinVtx.vertexState(), pfjetMomentum )
                   : Measurement1D();

      pfjet_kinvtx_.emplace_back( kinVtxValid ? Point( kinVtx.position().x(),
                                                       kinVtx.position().y(),
                                                       kinVtx.position().z() )
                                              : Point( NAN, NAN, NAN ) );
      pfjet_kinvtx_lxy_.emplace_back( distXY.significance() ? distXY.value()
                                                            : NAN );
      pfjet_kinvtx_l3d_.emplace_back( dist3D.significance() ? dist3D.value()
                                                            : NAN );
      pfjet_kinvtx_lxySig_.emplace_back(
          distXY.significance() ? distXY.value() / distXY.error() : NAN );
      pfjet_kinvtx_l3dSig_.emplace_back(
          dist3D.significance() ? dist3D.value() / dist3D.error() : NAN );
      pfjet_kinvtx_normChi2_.emplace_back(
          kinVtxValid && kinVtx.degreesOfFreedom()
              ? kinVtx.chiSquared() / kinVtx.degreesOfFreedom()
              : NAN );
      pfjet_kinvtx_prob_.emplace_back(
          kinVtxValid ? ChiSquaredProbability( kinVtx.chiSquared(),
                                               kinVtx.degreesOfFreedom() )
                      : NAN );
      pfjet_kinvtx_mass_.emplace_back( kinVtxValid ? kinVtxMass : NAN );
      pfjet_kinvtx_cosThetaXy_.emplace_back(
          kinVtxValid
              ? cosThetaOfJetPvXY( pv, kinVtx.vertexState(), pfjetMomentum )
              : NAN );
      pfjet_kinvtx_cosTheta3d_.emplace_back(
          kinVtxValid
              ? cosThetaOfJetPv3D( pv, kinVtx.vertexState(), pfjetMomentum )
              : NAN );
      pfjet_kinvtx_impactDistXy_.emplace_back(
          kinVtxValid
              ? impactDistanceXY( pv, kinVtx.vertexState(), pfjetMomentum )
              : NAN );
      pfjet_kinvtx_impactDist3d_.emplace_back(
          kinVtxValid
              ? impactDistance3D( pv, kinVtx.vertexState(), pfjetMomentum )
              : NAN );

      vector<float> trackImpactDist2dKinVtx{}, trackImpactDist3dKinVtx{};
      if ( kinVtxValid ) {
        for ( const auto& tt : transientTks ) {
          pair<bool, Measurement1D> impact2dResult =
              ff::absoluteTransverseImpactParameter( tt, kinVtx.vertexState() );
          trackImpactDist2dKinVtx.emplace_back(
              impact2dResult.first && impact2dResult.second.significance()
                  ? impact2dResult.second.value()
                  : NAN );

          pair<bool, Measurement1D> impact3dResult =
              ff::absoluteImpactParameter3D( tt, kinVtx.vertexState() );
          trackImpactDist3dKinVtx.emplace_back(
              impact3dResult.first && impact3dResult.second.significance()
                  ? impact3dResult.second.value()
                  : NAN );
        }
      }
      pfjet_kinvtx_tkImpactDist2d_.emplace_back( trackImpactDist2dKinVtx );
      pfjet_kinvtx_tkImpactDist3d_.emplace_back( trackImpactDist3dKinVtx );
    }
    // -------------------------------------------------------------------------

    // subjet ------------------------------------------------------------------
    if ( doSubstructureVariables_ ) {
      size_t           idx( &pfjet - &*pfjets.begin() );
      Ptr<reco::PFJet> pfjetptr( pfjet_h, idx );

      pfjet_subjet_lambda_.emplace_back( ( *subjet_lambda_h )[ pfjetptr ] );
      pfjet_subjet_epsilon_.emplace_back( ( *subjet_epsilon_h )[ pfjetptr ] );
      pfjet_subjet_ecf1_.emplace_back( ( *subjet_ecf1_h )[ pfjetptr ] > 0 ? ( *subjet_ecf1_h )[ pfjetptr ] : NAN );
      pfjet_subjet_ecf2_.emplace_back( ( *subjet_ecf2_h )[ pfjetptr ] > 0 ? ( *subjet_ecf2_h )[ pfjetptr ] : NAN );
      pfjet_subjet_ecf3_.emplace_back( ( *subjet_ecf3_h )[ pfjetptr ] > 0 ? ( *subjet_ecf3_h )[ pfjetptr ] : NAN );
    }
    // -------------------------------------------------------------------------

    // mva ---------------------------------------------------------------------
    if ( doMVA_ ) {
      map<string, float> mvaVariablesMap{};
      mvaVariablesMap.emplace( "pt", pfjet.pt() );
      mvaVariablesMap.emplace( "eta", pfjet.eta() );
      mvaVariablesMap.emplace( "nef", ( pfjet_neutralEmE_.back() + pfjet_neutralHadronE_.back() ) / pfjet.energy() );
      mvaVariablesMap.emplace( "maxd0", isnan( pfjet_pfcand_tkD0Max_.back() ) ? 0. : pfjet_pfcand_tkD0Max_.back() );
      mvaVariablesMap.emplace( "mind0", isnan( pfjet_pfcand_tkD0Min_.back() ) ? 0. : pfjet_pfcand_tkD0Min_.back() );
      mvaVariablesMap.emplace( "maxd0sig", isnan( pfjet_pfcand_tkD0SigMax_.back() ) ? 0. : pfjet_pfcand_tkD0SigMax_.back() );
      mvaVariablesMap.emplace( "mind0sig", isnan( pfjet_pfcand_tkD0SigMin_.back() ) ? 0. : pfjet_pfcand_tkD0SigMin_.back() );
      mvaVariablesMap.emplace( "tkiso05", isnan( pfjet_tkIsolation_[ 0.5 ].back() ) ? 0. : pfjet_tkIsolation_[ 0.5 ].back() );
      mvaVariablesMap.emplace( "pfiso05", pfjet_pfIsolation_[ 0.5 ].back() );
      mvaVariablesMap.emplace( "tkiso06", isnan( pfjet_tkIsolation_[ 0.6 ].back() ) ? 0. : pfjet_tkIsolation_[ 0.6 ].back() );
      mvaVariablesMap.emplace( "pfiso06", pfjet_pfIsolation_[ 0.6 ].back() );
      mvaVariablesMap.emplace( "tkiso07", isnan( pfjet_tkIsolation_[ 0.7 ].back() ) ? 0. : pfjet_tkIsolation_[ 0.7 ].back() );
      mvaVariablesMap.emplace( "pfiso07", pfjet_pfIsolation_[ 0.7 ].back() );
      mvaVariablesMap.emplace( "spreadpt", pfjet_ptDistribution_.back() );
      mvaVariablesMap.emplace( "spreaddr", pfjet_dRSpread_.back() );
      mvaVariablesMap.emplace( "lamb", pfjet_subjet_lambda_.back() );
      mvaVariablesMap.emplace( "epsi", pfjet_subjet_epsilon_.back() );
      mvaVariablesMap.emplace( "ecfe1", pfjet_subjet_ecf1_.back() );
      mvaVariablesMap.emplace( "ecfe2", isnan( pfjet_subjet_ecf2_.back() ) ? 0. : pfjet_subjet_ecf2_.back() );
      mvaVariablesMap.emplace( "ecfe3", isnan( pfjet_subjet_ecf3_.back() ) ? 0. : pfjet_subjet_ecf3_.back() );

      pfjet_mva_.emplace_back( mvaEstimator_->mvaValue( &pfjet, mvaVariablesMap ) );
    }

    // -------------------------------------------------------------------------
  }
}

void
ffNtuplePfJet::clear() {
  pfjet_n_ = 0;
  pfjet_p4_.clear();
  pfjet_chargedHadronE_.clear();
  pfjet_neutralHadronE_.clear();
  pfjet_chargedEmE_.clear();
  pfjet_neutralEmE_.clear();
  pfjet_photonE_.clear();
  pfjet_electronE_.clear();
  pfjet_muonE_.clear();
  pfjet_hfHadronE_.clear();
  pfjet_hfEmE_.clear();
  pfjet_charged_n_.clear();
  pfjet_neutral_n_.clear();
  pfjet_chargedHadron_n_.clear();
  pfjet_neutralHadron_n_.clear();
  pfjet_photon_n_.clear();
  pfjet_electron_n_.clear();
  pfjet_muon_n_.clear();
  pfjet_area_.clear();
  pfjet_maxDistance_.clear();
  for ( const double& isor : isoRadius_ ) {
    pfjet_tkPtSum_[ isor ].clear();
    pfjet_tkIsolation_[ isor ].clear();
    pfjet_pfIsolation_[ isor ].clear();
    pfjet_neuIsolation_[ isor ].clear();
    pfjet_hadIsolation_[ isor ].clear();
    pfjet_pfIsolationNoPU_[ isor ].clear();
    pfjet_neuIsolationNoPU_[ isor ].clear();
    pfjet_hadIsolationNoPU_[ isor ].clear();
  }
  pfjet_pfcands_n_.clear();
  pfjet_tracks_n_.clear();
  pfjet_ptDistribution_.clear();
  pfjet_dRSpread_.clear();
  pfjet_pfcands_chargedMass_.clear();
  pfjet_pfcands_nDsaMu_.clear();
  pfjet_pfcands_maxPtType_.clear();
  pfjet_pfcands_minTwoTkDist_.clear();

  pfjet_pfcand_type_.clear();
  pfjet_pfcand_charge_.clear();
  pfjet_pfcand_pt_.clear();
  pfjet_pfcand_eta_.clear();
  pfjet_pfcand_phi_.clear();
  pfjet_pfcand_energy_.clear();
  pfjet_pfcand_tkD0_.clear();
  pfjet_pfcand_tkD0Sig_.clear();
  pfjet_pfcand_tkDz_.clear();
  pfjet_pfcand_tkDzSig_.clear();
  pfjet_pfcand_tkNormChi2_.clear();
  pfjet_pfcand_electronIdx_.clear();
  pfjet_pfcand_pfmuonIdx_.clear();
  pfjet_pfcand_photonIdx_.clear();
  pfjet_pfcand_dsamuonIdx_.clear();

  pfjet_pfcand_muonTime_.clear();
  pfjet_pfcand_muonTimeErr_.clear();
  pfjet_pfcand_muonTimeStd_.clear();

  pfjet_pfcand_tkD0Max_.clear();
  pfjet_pfcand_tkD0Sub_.clear();
  pfjet_pfcand_tkD0Min_.clear();
  pfjet_pfcand_tkD0SigMax_.clear();
  pfjet_pfcand_tkD0SigSub_.clear();
  pfjet_pfcand_tkD0SigMin_.clear();

  pfjet_medianvtx_.clear();
  pfjet_averagevtx_.clear();

  pfjet_klmvtx_.clear();
  pfjet_klmvtx_lxy_.clear();
  pfjet_klmvtx_l3d_.clear();
  pfjet_klmvtx_lxySig_.clear();
  pfjet_klmvtx_l3dSig_.clear();
  pfjet_klmvtx_normChi2_.clear();
  pfjet_klmvtx_prob_.clear();
  pfjet_klmvtx_mass_.clear();
  pfjet_klmvtx_cosThetaXy_.clear();
  pfjet_klmvtx_cosTheta3d_.clear();
  pfjet_klmvtx_impactDistXy_.clear();
  pfjet_klmvtx_impactDist3d_.clear();
  pfjet_klmvtx_tkImpactDist2d_.clear();
  pfjet_klmvtx_tkImpactDist3d_.clear();

  pfjet_kinvtx_.clear();
  pfjet_kinvtx_lxy_.clear();
  pfjet_kinvtx_l3d_.clear();
  pfjet_kinvtx_lxySig_.clear();
  pfjet_kinvtx_l3dSig_.clear();
  pfjet_kinvtx_normChi2_.clear();
  pfjet_kinvtx_prob_.clear();
  pfjet_kinvtx_mass_.clear();
  pfjet_kinvtx_cosThetaXy_.clear();
  pfjet_kinvtx_cosTheta3d_.clear();
  pfjet_kinvtx_impactDistXy_.clear();
  pfjet_kinvtx_impactDist3d_.clear();
  pfjet_kinvtx_tkImpactDist2d_.clear();
  pfjet_kinvtx_tkImpactDist3d_.clear();

  pfjet_subjet_lambda_.clear();
  pfjet_subjet_epsilon_.clear();
  pfjet_subjet_ecf1_.clear();
  pfjet_subjet_ecf2_.clear();
  pfjet_subjet_ecf3_.clear();

  pfjet_mva_.clear();
}

///**************************************
namespace {
enum ParticleType {
  X = 0,     // undefined
  h,         // charged hadron
  e,         // electron
  mu,        // muon
  gamma,     // photon
  h0,        // neutral hadron
  h_HF,      // HF tower identified as a hadron
  egamma_HF  // HF tower identified as an EM particle
};
}
