#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"
#include "Firefighter/recoStuff/interface/KalmanVertexFitter.h"
#include "Firefighter/recoStuff/interface/KinematicParticleVertexFitter.h"
#include "Firefighter/recoStuff/interface/RecoHelpers.h"

#include "CommonTools/Statistics/interface/ChiSquaredProbability.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/GeometrySurface/interface/Line.h"
#include "DataFormats/GeometryVector/interface/GlobalTag.h"
#include "DataFormats/GeometryVector/interface/Vector2DBase.h"
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
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicParticle.h"
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicParticleFactoryFromTransientTrack.h"
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicVertex.h"
#include "RecoVertex/KinematicFitPrimitives/interface/RefCountedKinematicParticle.h"
#include "RecoVertex/KinematicFitPrimitives/interface/RefCountedKinematicVertex.h"
#include "RecoVertex/KinematicFitPrimitives/interface/TransientTrackKinematicParticle.h"
#include "RecoVertex/KinematicFitPrimitives/interface/TransientTrackKinematicStateBuilder.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"

#include <algorithm>
#include <numeric>

using Point          = math::XYZPointF;
using LorentzVector  = math::XYZTLorentzVectorF;
using Global2DVector = Vector2DBase<float, GlobalTag>;

class ffNtuplePfJet : public ffNtupleBase {
 public:
  ffNtuplePfJet( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

  std::vector<reco::PFCandidatePtr> getPFCands( const reco::PFJet& ) const;
  std::vector<reco::PFCandidatePtr> getChargedPFCands(
      const reco::PFJet& ) const;
  std::vector<reco::PFCandidatePtr> getTrackEmbededPFCands(
      const reco::PFJet& ) const;
  std::vector<const reco::Track*> getSelectedTracks(
      const reco::PFJet&,
      const StringCutObjectSelector<reco::Track>& ) const;

  LorentzVector        sumP4( const std::vector<reco::PFCandidatePtr>& ) const;
  reco::PFCandidatePtr getCandWithMaxPt(
      const std::vector<reco::PFCandidatePtr>& ) const;
  float chargedMass( const reco::PFJet& ) const;
  bool  hasDisplacedStandAloneMuon(
       const reco::PFJet&,
       const edm::Handle<reco::TrackCollection>& ) const;

  /**
   * @brief genralTracks Isolation
   *
   * pT of tracks in cone that NOT associated with the jet
   * ---------------------- over ----------------------
   * pT of tracks in cone that associated with the jet + above
   * --> The lower the value, the more isolated
   *
   * @return float
   */
  float getTkIsolation( const reco::PFJet&,
                        const edm::Handle<reco::TrackCollection>&,
                        const float& ) const;

  /**
   * @brief PFCandidate Isolation
   *
   * energy of candidates in cone that NOT associated with the jet
   * ---------------------- over ----------------------
   * energy of candidates in cone that associated with the jet + above
   * --> The lower the value, the more isolated
   *
   * @return float
   */
  float getPfIsolation( const reco::PFJet&,
                        const edm::Handle<reco::PFCandidateCollection>&,
                        const float& ) const;

  /**
   * @brief Neutral isolation
   *
   * energy of neutral candidates in clone (NOT associated with the jet)
   * ---------------------- over ----------------------
   * energy of candidates in cone associated with the jet + above
   * --> The lower value, the more isolated
   *
   * @return float
   */
  float getNeutralIsolation( const reco::PFJet&,
                             const edm::Handle<reco::PFCandidateCollection>&,
                             const float& ) const;

  int getCandType( const reco::PFCandidatePtr&,
                   const edm::Handle<reco::TrackCollection>& ) const;

  std::vector<reco::TransientTrack> transientTracksFromPFJet(
      const reco::PFJet&,
      const StringCutObjectSelector<reco::Track>&,
      const edm::EventSetup& ) const;

  /**
   * @brief Estimate vertex as the median value of reference points of tracks of
   * the jet.
   *
   * @param pTks vector of tracks.
   * @return Point
   */
  Point estimatedVertexFromMedianReferencePoints(
      const std::vector<const reco::Track*>& pTks ) const;

  /**
   * @brief Estimate vertex as the average value of reference points of tracks
   * of the jet.
   *
   * @param pTks vector of tracks.
   * @return Point
   */
  Point estimatedVertexFromAverageReferencePoints(
      const std::vector<const reco::Track*>& pTks ) const;

  std::pair<TransientVertex, float> kalmanVertexFromTransientTracks(
      const std::vector<reco::TransientTrack>& ) const;
  std::pair<KinematicVertex, float> kinematicVertexFromTransientTracks(
      const std::vector<reco::TransientTrack>& ) const;

  Measurement1D signedDistanceXY( const reco::Vertex&,
                                  const VertexState&,
                                  const GlobalVector& ) const;
  Measurement1D signedDistance3D( const reco::Vertex&,
                                  const VertexState&,
                                  const GlobalVector& ) const;

  float cosThetaOfJetPvXY( const reco::Vertex&,
                           const VertexState&,
                           const GlobalVector& ) const;

  float cosThetaOfJetPv3D( const reco::Vertex&,
                           const VertexState&,
                           const GlobalVector& ) const;

  float impactDistanceXY( const reco::Vertex&,
                          const VertexState&,
                          const GlobalVector& ) const;

  float impactDistance3D( const reco::Vertex&,
                          const VertexState&,
                          const GlobalVector& ) const;

 private:
  void clear() final;

  edm::EDGetToken pfjet_token_;
  edm::EDGetToken pvs_token_;
  edm::EDGetToken generaltk_token_;
  edm::EDGetToken pfcand_token_;
  edm::EDGetToken subjet_lambda_token_;
  edm::EDGetToken subjet_epsilon_token_;
  edm::EDGetToken subjet_ecf1_token_;
  edm::EDGetToken subjet_ecf2_token_;
  edm::EDGetToken subjet_ecf3_token_;

  StringCutObjectSelector<reco::PFJet> pfjet_selector_;
  StringCutObjectSelector<reco::Track> track_selector_;
  edm::ParameterSet                    kvfParam_;
  float                                isoRadius_;
  float                                minChargedMass_;

  int                        pfjet_n_;
  std::vector<LorentzVector> pfjet_p4_;
  std::vector<float>         pfjet_chargedHadronE_;
  std::vector<float>         pfjet_neutralHadronE_;
  std::vector<float>         pfjet_chargedEmE_;
  std::vector<float>         pfjet_neutralEmE_;
  std::vector<float>         pfjet_photonE_;
  std::vector<float>         pfjet_electronE_;
  std::vector<float>         pfjet_muonE_;
  std::vector<float>         pfjet_hfHadronE_;
  std::vector<float>         pfjet_hfEmE_;
  std::vector<int>           pfjet_charged_n_;
  std::vector<int>           pfjet_neutral_n_;
  std::vector<int>           pfjet_chargedHadron_n_;
  std::vector<int>           pfjet_neutralHadron_n_;
  std::vector<int>           pfjet_photon_n_;
  std::vector<int>           pfjet_electron_n_;
  std::vector<int>           pfjet_muon_n_;
  std::vector<float>         pfjet_area_;
  std::vector<float>         pfjet_maxDistance_;
  std::vector<float>         pfjet_tkIsolation_;
  std::vector<float>         pfjet_pfIsolation_;
  std::vector<float>         pfjet_neuIsolation_;
  std::vector<int>           pfjet_tracks_n_;
  std::vector<float>         pfjet_ptDistribution_;
  std::vector<float>         pfjet_dRSpread_;

  std::vector<int>   pfjet_pfcands_n_;
  std::vector<float> pfjet_pfcands_chargedMass_;
  std::vector<bool>  pfjet_pfcands_hasDsaMu_;
  std::vector<int>   pfjet_pfcands_maxPtType_;

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
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtuplePfJet, "ffNtuplePfJet" );

ffNtuplePfJet::ffNtuplePfJet( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ),
      pfjet_selector_( ps.getParameter<std::string>( "PFJetSelection" ) ),
      track_selector_( ps.getParameter<std::string>( "TrackSelection" ) ),
      kvfParam_( ps.getParameter<edm::ParameterSet>( "kvfParam" ) ),
      isoRadius_( ps.getParameter<double>( "IsolationRadius" ) ),
      minChargedMass_( ps.getParameter<double>( "MinChargedMass" ) ) {}

void
ffNtuplePfJet::initialize( TTree&                   tree,
                           const edm::ParameterSet& ps,
                           edm::ConsumesCollector&& cc ) {
  pfjet_token_ = cc.consumes<reco::PFJetCollection>(
      ps.getParameter<edm::InputTag>( "src" ) );
  pvs_token_ = cc.consumes<reco::VertexCollection>(
      ps.getParameter<edm::InputTag>( "PrimaryVertices" ) );
  generaltk_token_ = cc.consumes<reco::TrackCollection>(
      ps.getParameter<edm::InputTag>( "GeneralTracks" ) );
  pfcand_token_ = cc.consumes<reco::PFCandidateCollection>(
      ps.getParameter<edm::InputTag>( "ParticleFlowCands" ) );
  subjet_lambda_token_ = cc.consumes<edm::ValueMap<float>>(
      ps.getParameter<edm::InputTag>( "SubjetMomentumDistribution" ) );
  subjet_epsilon_token_ = cc.consumes<edm::ValueMap<float>>(
      ps.getParameter<edm::InputTag>( "SubjetEnergyDistributioin" ) );
  subjet_ecf1_token_ = cc.consumes<edm::ValueMap<float>>(
      ps.getParameter<edm::InputTag>( "SubjetEcf1" ) );
  subjet_ecf2_token_ = cc.consumes<edm::ValueMap<float>>(
      ps.getParameter<edm::InputTag>( "SubjetEcf2" ) );
  subjet_ecf3_token_ = cc.consumes<edm::ValueMap<float>>(
      ps.getParameter<edm::InputTag>( "SubjetEcf3" ) );

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
  tree.Branch( "pfjet_tkIsolation", &pfjet_tkIsolation_ );
  tree.Branch( "pfjet_pfIsolation", &pfjet_pfIsolation_ );
  tree.Branch( "pfjet_neuIsolation", &pfjet_neuIsolation_ );
  tree.Branch( "pfjet_pfcands_n", &pfjet_pfcands_n_ );
  tree.Branch( "pfjet_tracks_n", &pfjet_tracks_n_ );
  tree.Branch( "pfjet_ptDistribution", &pfjet_ptDistribution_ );
  tree.Branch( "pfjet_dRSpread", &pfjet_dRSpread_ );
  tree.Branch( "pfjet_pfcands_chargedMass", &pfjet_pfcands_chargedMass_ );
  tree.Branch( "pfjet_pfcands_hasDsaMu", &pfjet_pfcands_hasDsaMu_ );
  tree.Branch( "pfjet_pfcands_maxPtType", &pfjet_pfcands_maxPtType_ );

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

  tree.Branch( "pfjet_subjet_lambda", &pfjet_subjet_lambda_ );
  tree.Branch( "pfjet_subjet_epsilon", &pfjet_subjet_epsilon_ );
  tree.Branch( "pfjet_subjet_ecf1", &pfjet_subjet_ecf1_ );
  tree.Branch( "pfjet_subjet_ecf2", &pfjet_subjet_ecf2_ );
  tree.Branch( "pfjet_subjet_ecf3", &pfjet_subjet_ecf3_ );
}

void
ffNtuplePfJet::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

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

  Handle<reco::PFCandidateCollection> pfCand_h;
  e.getByToken( pfcand_token_, pfCand_h );
  assert( pfCand_h.isValid() );

  Handle<ValueMap<float>> subjet_lambda_h;
  e.getByToken( subjet_lambda_token_, subjet_lambda_h );
  assert( subjet_lambda_h.isValid() );
  const auto& subjetLambdaVM = *subjet_lambda_h;

  Handle<ValueMap<float>> subjet_epsilon_h;
  e.getByToken( subjet_epsilon_token_, subjet_epsilon_h );
  assert( subjet_epsilon_h.isValid() );
  const auto& subjetEpsilonVM = *subjet_epsilon_h;

  Handle<ValueMap<float>> subjet_ecf1_h, subjet_ecf2_h, subjet_ecf3_h;
  e.getByToken( subjet_ecf1_token_, subjet_ecf1_h );
  e.getByToken( subjet_ecf2_token_, subjet_ecf2_h );
  e.getByToken( subjet_ecf3_token_, subjet_ecf3_h );
  assert( subjet_ecf1_h.isValid() && subjet_ecf2_h.isValid() &&
          subjet_ecf3_h.isValid() );
  const auto& subjetecf1VM = *subjet_ecf1_h;
  const auto& subjetecf2VM = *subjet_ecf2_h;
  const auto& subjetecf3VM = *subjet_ecf3_h;

  clear();

  pfjet_n_ = pfjets.size();
  for ( const auto& pfjet : pfjets ) {
    if ( !pfjet_selector_( pfjet ) )
      continue;
    if ( chargedMass( pfjet ) < minChargedMass_ )
      continue;

    const vector<const reco::Track*> tracksSelected =
        getSelectedTracks( pfjet, track_selector_ );
    const vector<reco::PFCandidatePtr> pfCands = getPFCands( pfjet );

    pfjet_p4_.push_back(
        LorentzVector( pfjet.px(), pfjet.py(), pfjet.pz(), pfjet.energy() ) );
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
    pfjet_tkIsolation_.emplace_back(
        getTkIsolation( pfjet, generalTk_h, isoRadius_ ) );
    pfjet_pfIsolation_.emplace_back(
        getPfIsolation( pfjet, pfCand_h, isoRadius_ ) );
    pfjet_neuIsolation_.emplace_back(
        getNeutralIsolation( pfjet, pfCand_h, isoRadius_ ) );
    pfjet_pfcands_n_.emplace_back( pfCands.size() );
    pfjet_tracks_n_.emplace_back( tracksSelected.size() );
    pfjet_ptDistribution_.emplace_back( pfjet.constituentPtDistribution() );
    pfjet_dRSpread_.emplace_back( pfjet.constituentEtaPhiSpread() );
    pfjet_pfcands_chargedMass_.emplace_back( chargedMass( pfjet ) );
    pfjet_pfcands_hasDsaMu_.emplace_back(
        hasDisplacedStandAloneMuon( pfjet, generalTk_h ) );
    pfjet_pfcands_maxPtType_.emplace_back(
        getCandType( getCandWithMaxPt( pfCands ), generalTk_h ) );

    // pfcand -------------------------------------------------------------
    vector<int>   cPFCandType{};
    vector<int>   cPFCandCharge{};
    vector<float> cPFCandPt{}, cPFCandEnergy{};
    vector<float> cPFCandEta{}, cPFCandPhi{};
    vector<float> cPFCandTkD0{}, cPFCandTkD0Sig{};
    vector<float> cPFCandTkDz{}, cPFCandTkDzSig{};
    vector<float> cPFCandTkNormChi2{};

    for ( const auto& cand : pfCands ) {
      cPFCandType.emplace_back( cand->particleId() );
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
    // --------------------------------------------------------------------

    // vertices -----------------------------------------------------------
    pfjet_medianvtx_.push_back(
        estimatedVertexFromMedianReferencePoints( tracksSelected ) );
    pfjet_averagevtx_.push_back(
        estimatedVertexFromAverageReferencePoints( tracksSelected ) );

    vector<reco::TransientTrack> transientTks =
        transientTracksFromPFJet( pfjet, track_selector_, es );
    Measurement1D distXY;
    Measurement1D dist3D;

    GlobalVector pfjetMomentum( pfjet.px(), pfjet.py(), pfjet.pz() );

    const auto klmVtxInfo = kalmanVertexFromTransientTracks( transientTks );
    const TransientVertex& klmVtx      = klmVtxInfo.first;
    const float&           klmVtxMass  = klmVtxInfo.second;
    bool                   klmVtxValid = klmVtx.isValid();

    distXY = klmVtxValid
                 ? signedDistanceXY( pv, klmVtx.vertexState(), pfjetMomentum )
                 : Measurement1D();
    dist3D = klmVtxValid
                 ? signedDistance3D( pv, klmVtx.vertexState(), pfjetMomentum )
                 : Measurement1D();

    pfjet_klmvtx_.emplace_back( klmVtxValid ? Point( klmVtx.position().x(),
                                                     klmVtx.position().y(),
                                                     klmVtx.position().z() )
                                            : Point( NAN, NAN, NAN ) );
    pfjet_klmvtx_lxy_.emplace_back( distXY.significance() ? distXY.value()
                                                          : NAN );
    pfjet_klmvtx_l3d_.emplace_back( dist3D.significance() ? dist3D.value()
                                                          : NAN );
    pfjet_klmvtx_lxySig_.emplace_back(
        distXY.significance() ? distXY.value() / distXY.error() : NAN );
    pfjet_klmvtx_l3dSig_.emplace_back(
        dist3D.significance() ? dist3D.value() / dist3D.error() : NAN );
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

    const auto kinVtxInfo = kinematicVertexFromTransientTracks( transientTks );
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
    // --------------------------------------------------------------------

    // subjet -----------------------------------------------------------
    size_t           idx( &pfjet - &*pfjets.begin() );
    Ptr<reco::PFJet> pfjetptr( pfjet_h, idx );

    pfjet_subjet_lambda_.emplace_back( subjetLambdaVM[ pfjetptr ] );
    pfjet_subjet_epsilon_.emplace_back( subjetEpsilonVM[ pfjetptr ] );
    pfjet_subjet_ecf1_.emplace_back(
        subjetecf1VM[ pfjetptr ] > 0 ? subjetecf1VM[ pfjetptr ] : NAN );
    pfjet_subjet_ecf2_.emplace_back(
        subjetecf2VM[ pfjetptr ] > 0 ? subjetecf2VM[ pfjetptr ] : NAN );
    pfjet_subjet_ecf3_.emplace_back(
        subjetecf3VM[ pfjetptr ] > 0 ? subjetecf3VM[ pfjetptr ] : NAN );
    // --------------------------------------------------------------------
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
  pfjet_tkIsolation_.clear();
  pfjet_pfIsolation_.clear();
  pfjet_neuIsolation_.clear();
  pfjet_pfcands_n_.clear();
  pfjet_tracks_n_.clear();
  pfjet_ptDistribution_.clear();
  pfjet_dRSpread_.clear();
  pfjet_pfcands_chargedMass_.clear();
  pfjet_pfcands_hasDsaMu_.clear();
  pfjet_pfcands_maxPtType_.clear();

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
}

std::vector<reco::PFCandidatePtr>
ffNtuplePfJet::getPFCands( const reco::PFJet& jet ) const {
  std::vector<reco::PFCandidatePtr> result = jet.getPFConstituents();
  result.erase(
      std::remove_if( result.begin(), result.end(),
                      []( const auto& cand ) { return cand.isNull(); } ),
      result.end() );

  return result;
}

std::vector<reco::PFCandidatePtr>
ffNtuplePfJet::getChargedPFCands( const reco::PFJet& jet ) const {
  std::vector<reco::PFCandidatePtr> result = getPFCands( jet );
  result.erase(
      std::remove_if( result.begin(), result.end(),
                      []( const auto& cand ) { return cand->charge() == 0; } ),
      result.end() );

  return result;
}

std::vector<reco::PFCandidatePtr>
ffNtuplePfJet::getTrackEmbededPFCands( const reco::PFJet& jet ) const {
  std::vector<reco::PFCandidatePtr> result = getChargedPFCands( jet );
  result.erase( std::remove_if( result.begin(), result.end(),
                                []( const auto& cand ) {
                                  return cand->bestTrack() == nullptr;
                                } ),
                result.end() );

  return result;
}

std::vector<const reco::Track*>
ffNtuplePfJet::getSelectedTracks(
    const reco::PFJet&                          jet,
    const StringCutObjectSelector<reco::Track>& tkSelector ) const {
  std::vector<const reco::Track*> result{};
  result.reserve( jet.chargedMultiplicity() );
  for ( const auto& cand : getTrackEmbededPFCands( jet ) ) {
    const reco::Track* tk = cand->bestTrack();
    if ( tkSelector( *tk ) ) {
      result.push_back( tk );
    }
  }

  return result;
}

LorentzVector
ffNtuplePfJet::sumP4( const std::vector<reco::PFCandidatePtr>& cands ) const {
  LorentzVector result = LorentzVector();
  for ( const auto& cand : cands )
    result +=
        LorentzVector( cand->px(), cand->py(), cand->pz(), cand->energy() );

  return result;
}

float
ffNtuplePfJet::chargedMass( const reco::PFJet& jet ) const {
  return sumP4( getChargedPFCands( jet ) ).M();
}

bool
ffNtuplePfJet::hasDisplacedStandAloneMuon(
    const reco::PFJet&                        jet,
    const edm::Handle<reco::TrackCollection>& generalTkH ) const {
  std::vector<reco::PFCandidatePtr> candsWithTk = getTrackEmbededPFCands( jet );

  for ( const auto& cand : candsWithTk ) {
    if ( cand->trackRef().isNonnull() and
         ( cand->trackRef().id() != generalTkH.id() ) )
      return true;
  }

  return false;
}

float
ffNtuplePfJet::getTkIsolation( const reco::PFJet&                        jet,
                               const edm::Handle<reco::TrackCollection>& tkH,
                               const float& isoRadius ) const {
  std::vector<reco::TrackRef> generalTkRefs{};
  for ( size_t i( 0 ); i != tkH->size(); ++i )
    generalTkRefs.emplace_back( tkH, i );

  std::vector<reco::PFCandidatePtr> cands = getTrackEmbededPFCands( jet );

  float notOfCands( 0. );
  for ( const auto& tkRef : generalTkRefs ) {
    if ( deltaR( jet, *tkRef ) > isoRadius )
      continue;  // outside radius

    if ( std::find_if( cands.begin(), cands.end(), [&tkRef]( const auto& c ) {
           return c->trackRef() == tkRef;
         } ) != cands.end() )
      continue;  // associated with the jet

    notOfCands += tkRef->pt();
  }

  float ofCands =
      std::accumulate( cands.begin(), cands.end(), 0.,
                       []( float ptsum, const reco::PFCandidatePtr& jc ) {
                         return ptsum + jc->bestTrack()->pt();
                       } );

  return ( ofCands + notOfCands ) == 0 ? NAN
                                       : notOfCands / ( ofCands + notOfCands );
}

float
ffNtuplePfJet::getPfIsolation(
    const reco::PFJet&                              jet,
    const edm::Handle<reco::PFCandidateCollection>& pfH,
    const float&                                    isoRadius ) const {
  std::vector<reco::PFCandidatePtr> pfCandPtrs{};
  for ( size_t i( 0 ); i != pfH->size(); ++i )
    pfCandPtrs.emplace_back( pfH, i );

  std::vector<reco::PFCandidatePtr> jetcands = getPFCands( jet );

  float notOfCands( 0. );
  for ( const auto& cand : pfCandPtrs ) {
    if ( deltaR( jet, *cand ) > isoRadius )
      continue;  // outside radius

    if ( std::find_if( jetcands.begin(), jetcands.end(),
                       [&cand]( const auto& jc ) { return jc == cand; } ) !=
         jetcands.end() )
      continue;  // associated with the jet

    notOfCands += cand->energy();
  }

  float ofCands =
      std::accumulate( jetcands.begin(), jetcands.end(), 0.,
                       []( float esum, const reco::PFCandidatePtr& jc ) {
                         return esum + jc->energy();
                       } );

  return ( ofCands + notOfCands ) == 0 ? NAN
                                       : notOfCands / ( ofCands + notOfCands );
}

float
ffNtuplePfJet::getNeutralIsolation(
    const reco::PFJet&                              jet,
    const edm::Handle<reco::PFCandidateCollection>& pfH,
    const float&                                    isoRadius ) const {
  std::vector<reco::PFCandidatePtr> pfCandPtrs{};
  for ( size_t i( 0 ); i != pfH->size(); ++i )
    pfCandPtrs.emplace_back( pfH, i );

  std::vector<reco::PFCandidatePtr> jetcands = getPFCands( jet );

  float notOfCands( 0. );
  for ( const auto& cand : pfCandPtrs ) {
    if ( cand->charge() != 0 )
      continue;  // charged
    if ( deltaR( jet, *cand ) > isoRadius )
      continue;  // outside radius

    if ( std::find_if( jetcands.begin(), jetcands.end(),
                       [&cand]( const auto& jc ) { return jc == cand; } ) !=
         jetcands.end() )
      continue;  // associated with the jet

    notOfCands += cand->energy();
  }

  float ofCands =
      std::accumulate( jetcands.begin(), jetcands.end(), 0.,
                       []( float esum, const reco::PFCandidatePtr& jc ) {
                         return esum + jc->energy();
                       } );

  return ( ofCands + notOfCands ) == 0 ? NAN
                                       : notOfCands / ( ofCands + notOfCands );
}

reco::PFCandidatePtr
ffNtuplePfJet::getCandWithMaxPt(
    const std::vector<reco::PFCandidatePtr>& cands ) const {
  if ( cands.size() == 0 )
    return reco::PFCandidatePtr();

  float  _ptmax( 0. );
  size_t _idxmax( cands.size() );
  for ( size_t id( 0 ); id != cands.size(); ++id ) {
    if ( cands[ id ]->pt() > _ptmax ) {
      _ptmax  = cands[ id ]->pt();
      _idxmax = id;
    }
  }

  return _idxmax == cands.size() ? reco::PFCandidatePtr() : cands[ _idxmax ];
}

int
ffNtuplePfJet::getCandType(
    const reco::PFCandidatePtr&               cand,
    const edm::Handle<reco::TrackCollection>& generalTkH ) const {
  if ( cand.isNull() )
    return 0;

  if ( cand->trackRef().isNonnull() &&
       cand->trackRef().id() != generalTkH.id() )
    return 8;  // This is coming from a displacedStandAloneMuon

  return cand->particleId();
}

std::vector<reco::TransientTrack>
ffNtuplePfJet::transientTracksFromPFJet(
    const reco::PFJet&                          jet,
    const StringCutObjectSelector<reco::Track>& tkSelector,
    const edm::EventSetup&                      es ) const {
  std::vector<reco::PFCandidatePtr> cands = getTrackEmbededPFCands( jet );

  edm::ESHandle<TransientTrackBuilder> theB;
  es.get<TransientTrackRecord>().get( "TransientTrackBuilder", theB );

  std::vector<reco::TransientTrack> t_tks{};
  for ( const auto& c : cands ) {
    if ( !tkSelector( *( c->bestTrack() ) ) )
      continue;
    reco::TransientTrack tt = theB->build( c );
    if ( !tt.isValid() )
      continue;
    t_tks.push_back( tt );
  }

  return t_tks;
}

Point
ffNtuplePfJet::estimatedVertexFromMedianReferencePoints(
    const std::vector<const reco::Track*>& pTks ) const {
  std::vector<float> cXinnerPos, cYinnerPos, cZinnerPos;
  for ( const auto& cTk : pTks ) {
    cXinnerPos.push_back( cTk->referencePoint().X() );
    cYinnerPos.push_back( cTk->referencePoint().Y() );
    cZinnerPos.push_back( cTk->referencePoint().Z() );
  }

  return Point( ff::medianValue<float>( cXinnerPos ),
                ff::medianValue<float>( cYinnerPos ),
                ff::medianValue<float>( cZinnerPos ) );
}

Point
ffNtuplePfJet::estimatedVertexFromAverageReferencePoints(
    const std::vector<const reco::Track*>& pTks ) const {
  std::vector<float> cXinnerPos, cYinnerPos, cZinnerPos;
  for ( const auto& cTk : pTks ) {
    cXinnerPos.push_back( cTk->referencePoint().X() );
    cYinnerPos.push_back( cTk->referencePoint().Y() );
    cZinnerPos.push_back( cTk->referencePoint().Z() );
  }

  return Point( ff::averageValue<float>( cXinnerPos ),
                ff::averageValue<float>( cYinnerPos ),
                ff::averageValue<float>( cZinnerPos ) );
}

std::pair<TransientVertex, float>
ffNtuplePfJet::kalmanVertexFromTransientTracks(
    const std::vector<reco::TransientTrack>& t_tks ) const {
  /// vertexing
  if ( t_tks.size() < 2 )
    return std::make_pair( TransientVertex(), NAN );

  std::unique_ptr<ff::KalmanVertexFitter> kvf( new ff::KalmanVertexFitter(
      kvfParam_, kvfParam_.getParameter<bool>( "doSmoothing" ) ) );

  TransientVertex tv;
  try {
    tv = kvf->vertex( t_tks );
  } catch ( ... ) {
    std::cout
        << "Exception from ffNtuplePfJet::kalmanVertexFromTransientTracks !"
        << std::endl;
    return std::make_pair( TransientVertex(), NAN );
  }

  if ( !tv.isValid() )
    return std::make_pair( TransientVertex(), NAN );

  /// mass
  LorentzVector vtxp4;
  for ( const auto& refitTks : tv.refittedTracks() ) {
    const reco::Track& tk   = refitTks.track();
    reco::CandidatePtr cand = refitTks.basicTransientTrack()->candidate();
    float              mass = cand.isNonnull() ? cand->mass() : 0.;

    vtxp4 +=
        LorentzVector( tk.px(), tk.py(), tk.pz(), std::hypot( tk.p(), mass ) );
  }

  return std::make_pair( tv, vtxp4.M() );
}

std::pair<KinematicVertex, float>
ffNtuplePfJet::kinematicVertexFromTransientTracks(
    const std::vector<reco::TransientTrack>& t_tks ) const {
  /// vertexing
  if ( t_tks.size() < 2 )
    return std::make_pair( KinematicVertex(), NAN );

  KinematicParticleFactoryFromTransientTrack pFactory;

  std::vector<RefCountedKinematicParticle> allParticles;
  TransientTrackKinematicStateBuilder      ttkStateBuilder;
  // sigma to avoid singularities in the covariance matrix.
  float pSigma = 0.0000001;
  // initial chi2 and ndf before kinematic fits.
  // The chi2 of the reconstruction is not considered
  float chi = 0.;
  float ndf = 0.;

  for ( const auto& ttk : t_tks ) {
    reco::CandidatePtr cand  = ttk.basicTransientTrack()->candidate();
    double             pMass = cand.isNonnull() ? cand->mass() : 0.;
    allParticles.push_back( pFactory.particle( ttk, pMass, chi, ndf, pSigma ) );
  }

  std::unique_ptr<ff::KinematicParticleVertexFitter> kinFitter(
      new ff::KinematicParticleVertexFitter() );
  RefCountedKinematicTree kinTree;
  try {
    kinTree = kinFitter->fit( allParticles );
  } catch ( ... ) {
    std::cout
        << "Exception from ffNtuplePfJet::kinematicVertexFromTransientTracks !"
        << std::endl;
    return std::make_pair( KinematicVertex(), NAN );
  }

  if ( !kinTree->isValid() )
    return std::make_pair( KinematicVertex(), NAN );

  kinTree->movePointerToTheTop();
  RefCountedKinematicVertex kinV = kinTree->currentDecayVertex();

  if ( !kinV->vertexIsValid() or kinV->correspondingTree() == nullptr )
    return std::make_pair( KinematicVertex(), NAN );

  /// mass
  LorentzVector vtxp4;
  for ( const auto& dau : kinTree->daughterParticles() ) {
    const TransientTrackKinematicParticle* ttkp =
        dynamic_cast<const TransientTrackKinematicParticle*>( dau.get() );
    if ( ttkp == nullptr ) {
      continue;
    }

    reco::Track dauTk   = ttkp->refittedTransientTrack().track();
    double      dauMass = dau->initialState().mass();

    vtxp4 += LorentzVector( dauTk.px(), dauTk.py(), dauTk.pz(),
                            std::hypot( dauTk.p(), dauMass ) );
  }

  return std::make_pair( *kinV, vtxp4.M() );
}

Measurement1D
ffNtuplePfJet::signedDistance3D( const reco::Vertex& vtx1,
                                 const VertexState&  vs2,
                                 const GlobalVector& ref ) const {
  VertexDistance3D vdist3D;

  Measurement1D unsignedDistance = vdist3D.distance( vtx1, vs2 );
  GlobalVector  diff =
      GlobalPoint( Basic3DVector<float>( vtx1.position() ) ) - vs2.position();

  if ( ( ref.x() * diff.x() + ref.y() * diff.y() + ref.z() * diff.z() ) < 0 )
    return Measurement1D( -1.0 * unsignedDistance.value(),
                          unsignedDistance.error() );

  return unsignedDistance;
}

Measurement1D
ffNtuplePfJet::signedDistanceXY( const reco::Vertex& vtx1,
                                 const VertexState&  vs2,
                                 const GlobalVector& ref ) const {
  VertexDistanceXY vdistXY;

  Measurement1D unsignedDistance = vdistXY.distance( vtx1, vs2 );
  GlobalVector  diff =
      GlobalPoint( Basic3DVector<float>( vtx1.position() ) ) - vs2.position();

  if ( ( ref.x() * diff.x() + ref.y() * diff.y() ) < 0 )
    return Measurement1D( -1.0 * unsignedDistance.value(),
                          unsignedDistance.error() );

  return unsignedDistance;
}

float
ffNtuplePfJet::cosThetaOfJetPvXY( const reco::Vertex& vtx1,
                                  const VertexState&  vs2,
                                  const GlobalVector& ref ) const {
  GlobalVector diff =
      GlobalPoint( Basic3DVector<float>( vtx1.position() ) ) - vs2.position();
  Global2DVector diff2D = Global2DVector( diff.x(), diff.y() );
  Global2DVector ref2D  = Global2DVector( ref.x(), ref.y() );
  return diff2D.unit().dot( ref2D.unit() );
}

float
ffNtuplePfJet::cosThetaOfJetPv3D( const reco::Vertex& vtx1,
                                  const VertexState&  vs2,
                                  const GlobalVector& ref ) const {
  GlobalVector diff =
      GlobalPoint( Basic3DVector<float>( vtx1.position() ) ) - vs2.position();
  return diff.unit().dot( ref.unit() );
}

float
ffNtuplePfJet::impactDistanceXY( const reco::Vertex& vtx1,
                                 const VertexState&  vs2,
                                 const GlobalVector& ref ) const {
  Line::PositionType pos(
      GlobalPoint( vs2.position().x(), vs2.position().y(), 0 ) );
  Line::DirectionType dir( GlobalVector( ref.x(), ref.y(), 0 ).unit() );
  Line                jetDirectionLineXY( pos, dir );

  GlobalPoint pv( vtx1.position().x(), vtx1.position().y(), 0 );

  return jetDirectionLineXY.distance( pv ).mag();
}

float
ffNtuplePfJet::impactDistance3D( const reco::Vertex& vtx1,
                                 const VertexState&  vs2,
                                 const GlobalVector& ref ) const {
  Line::PositionType  pos( vs2.position() );
  Line::DirectionType dir( ref.unit() );
  Line                jetDirectionLine( pos, dir );

  GlobalPoint pv( vtx1.position().x(), vtx1.position().y(),
                  vtx1.position().z() );

  return jetDirectionLine.distance( pv ).mag();
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
