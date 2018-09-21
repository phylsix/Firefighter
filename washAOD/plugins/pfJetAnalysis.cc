#include "Firefighter/washAOD/interface/pfJetAnalysis.h"
#include "Firefighter/recoStuff/interface/KalmanVertexFitter.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/Point3D.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/Candidate/interface/CompositeCandidate.h"
#include "DataFormats/Candidate/interface/CompositeCandidateFwd.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"
// #include "DataFormats/HLTReco/interface/TriggerObject.h"
#include <cmath>

pfJetAnalysis::pfJetAnalysis(const edm::ParameterSet& ps) :
  jetTag_(ps.getParameter<edm::InputTag>("src")),
  jetToken_(consumes<reco::PFJetCollection>(jetTag_)),
  assignTypeAnyDsaMu_(ps.getParameter<bool>("anydSAForJetType")),
  dSAMuTag_(ps.getParameter<edm::InputTag>("dsa")),
  dSAMuToken_(consumes<reco::TrackCollection>(dSAMuTag_)),
  kvfParam_(ps.getParameter<edm::ParameterSet>("kvfParam")),
  trigResultsTag_(ps.getParameter<edm::InputTag>("trigResult")),
  trigEventTag_(ps.getParameter<edm::InputTag>("trigEvent")),
  trigPathNoVer_(ps.getParameter<std::string>("trigPath")),
  processName_(ps.getParameter<std::string>("processName")),
  trigResultsToken_(consumes<edm::TriggerResults>(trigResultsTag_)),
  trigEventToken_(consumes<trigger::TriggerEvent>(trigEventTag_)),
  pvsTag_(ps.getParameter<edm::InputTag>("pvs")),
  pvsToken_(consumes<reco::VertexCollection>(pvsTag_))
{
  using namespace std;
  using namespace edm;

  genParticleToken_  = consumes<reco::GenParticleCollection>(InputTag("genParticles"));
  generalTrackToken_ = consumes<reco::TrackCollection>(InputTag("generalTracks"));

  usesResource("TFileService");
  LogVerbatim("pfJetAnalysis") << " pfJetAnalysis configuration: " << endl
    << " jetTag = " << jetTag_.encode() << endl;
}

pfJetAnalysis::~pfJetAnalysis() = default;

void
pfJetAnalysis::fillDescriptions(edm::ConfigurationDescriptions& ds)
{
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src", edm::InputTag("ak4PFJets"));
  desc.add<edm::InputTag>("dsa", edm::InputTag("selectedDsaMuons"));
  desc.add<bool>("anydSAForJetType", true);

  edm::ParameterSetDescription _kvfp;
  _kvfp.add<double>("maxDistance", 0.01); // It is said this is mm.
  _kvfp.add<int>("maxNbrOfIterations", 10);
  _kvfp.add<bool>("doSmoothing", true);
  desc.add<edm::ParameterSetDescription>("kvfParam", _kvfp);

  desc.add<edm::InputTag>("trigResult", edm::InputTag("TriggerResults","","HLT"));
  desc.add<edm::InputTag>("trigEvent", edm::InputTag("hltTriggerSummaryAOD","","HLT"));
  desc.add<std::string>("trigPath", "HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx");
  desc.add<std::string>("processName", "HLT");
  desc.add<edm::InputTag>("pvs", edm::InputTag("offlinePrimaryVertices"));

  ds.add("pfJetAnalysis", desc);
}

void
pfJetAnalysis::beginJob()
{
  jetT_ = fs->make<TTree>("Jet", "");

  jetT_->Branch("nJet", &nJet_, "nJet/i");
  jetT_->Branch("triggered", &triggered_, "triggered/O");
  
  jetT_->Branch("genDarkphotonEnergy",    &genDarkphotonEnergy_);
  jetT_->Branch("genDarkphotonPt",        &genDarkphotonPt_);
  jetT_->Branch("genDarkphotonPz",        &genDarkphotonPz_);
  jetT_->Branch("genDarkphotonEta",       &genDarkphotonEta_);
  jetT_->Branch("genDarkphotonPhi",       &genDarkphotonPhi_);
  jetT_->Branch("genDarkphotonLxy",       &genDarkphotonLxy_);
  jetT_->Branch("genDarkphotonL3D",       &genDarkphotonL3D_);

  jetT_->Branch("jetSeedType",            &jetSeedType_);
  jetT_->Branch("jetEnergy",              &jetEnergy_);
  jetT_->Branch("jetMass",                &jetMass_);
  jetT_->Branch("jetPt",                  &jetPt_);
  jetT_->Branch("jetPz",                  &jetPz_);
  jetT_->Branch("jetEta",                 &jetEta_);
  jetT_->Branch("jetPhi",                 &jetPhi_);
  jetT_->Branch("jetMatchDist",           &jetMatchDist_);
  jetT_->Branch("jetChargedEmEnergyFrac", &jetChargedEmEnergyFrac_);
  jetT_->Branch("jetChargedHadEnergyFrac",&jetChargedHadEnergyFrac_);
  jetT_->Branch("jetNeutralEmEnergyFrac", &jetNeutralEmEnergyFrac_);
  jetT_->Branch("jetNeutralHadEnergyFrac",&jetNeutralHadEnergyFrac_);
  jetT_->Branch("jetChargedMultiplicity", &jetChargedMultiplicity_);
  jetT_->Branch("jetMuonMultiplicity",    &jetMuonMultiplicity_);
  jetT_->Branch("jetNConstituents",       &jetNConstituents_);
  jetT_->Branch("jetNTracks",             &jetNTracks_);
  jetT_->Branch("jetTrackPt",             &jetTrackPt_);
  jetT_->Branch("jetTrackEta",            &jetTrackEta_);
  jetT_->Branch("jetTrackD0Sig",          &jetTrackD0Sig_);
  jetT_->Branch("jetTrackNormChi2",       &jetTrackNormChi2_);
  jetT_->Branch("jetVtxLxy",              &jetVtxLxy_);
  jetT_->Branch("jetVtxL3D",              &jetVtxL3D_);
  jetT_->Branch("jetVtxLxySig",           &jetVtxLxySig_);
  jetT_->Branch("jetVtxL3DSig",           &jetVtxL3DSig_);
  jetT_->Branch("jetVtxMatchDist",        &jetVtxMatchDist_);
  jetT_->Branch("jetVtxMatchDistT",       &jetVtxMatchDistT_);
  jetT_->Branch("jetVtxNormChi2",         &jetVtxNormChi2_);

  // ****************************************

  dSAT_ = fs->make<TTree>("dSA", "");

  dSAT_->Branch("ndSA", &ndSA_, "ndSA/i");
  dSAT_->Branch("triggered", &triggered_, "triggered/O");

  dSAT_->Branch("genMuonPt",  &genMuonPt_);
  dSAT_->Branch("genMuonPz",  &genMuonPz_);
  dSAT_->Branch("genMuonEta", &genMuonEta_);
  dSAT_->Branch("genMuonPhi", &genMuonPhi_);
  dSAT_->Branch("dSAPt",  &dSAPt_);
  dSAT_->Branch("dSAPz",  &dSAPz_);
  dSAT_->Branch("dSAEta", &dSAEta_);
  dSAT_->Branch("dSAPhi", &dSAPhi_);
  dSAT_->Branch("dSAMatchDist", &dSAMatchDist_);
  dSAT_->Branch("dSATrackQual", &dSATrackQual_);
}

void
pfJetAnalysis::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
  using namespace std;
  using namespace edm;

  bool changed(true);
  if (hltConfig_.init(iRun,iSetup,processName_,changed)) {
    if (changed) {
      LogInfo("pfJetAnalysis")<<"trigEffiForMuTrack::beginRun: "<<"hltConfig init for Run"<<iRun.run();
      hltConfig_.dump("ProcessName");
      hltConfig_.dump("GlobalTag");
      hltConfig_.dump("TableName");
    }
  } else {
    LogError("pfJetAnalysis")<<"trigEffiForMuTrack::beginRun: config extraction failure with processName -> "
      <<processName_;
  }

}

void
pfJetAnalysis::analyze(const edm::Event& iEvent,
                       const edm::EventSetup& iSetup)
{
  using namespace std;
  using namespace edm;

  //*****************************************************

  iEvent.getByToken(genParticleToken_, genParticleHandle_);
  assert(genParticleHandle_.isValid());

  // gen particles
  int nAccpted = count_if((*genParticleHandle_).begin(), (*genParticleHandle_).end(),
      [](const reco::GenParticle& g){
        return abs(g.pdgId())==13
           and g.isHardProcess()
           and abs(g.eta())<2.4
           and abs(g.vertex().rho())<740  // decay inside CMS
           and abs(g.vz())<960;
        });
  if (nAccpted<4) return;

  vector<reco::GenParticleRef> darkphotons{};
  for (size_t i(0); i!=genParticleHandle_->size(); ++i)
  {
    if ( (*genParticleHandle_)[i].pdgId()!=32 ) {continue;}
    darkphotons.emplace_back(genParticleHandle_, i);
  }
  assert( darkphotons.size()==2 );
  
  //*****************************************************

  iEvent.getByToken(trigResultsToken_, trigResultsHandle_);
  assert( trigResultsHandle_.isValid() );
  iEvent.getByToken(trigEventToken_, trigEventHandle_);
  assert( trigEventHandle_.isValid() );

  const vector<string>& pathNames = hltConfig_.triggerNames();
  const vector<string> matchedPaths(hltConfig_.restoreVersion(pathNames, trigPathNoVer_));
  if (matchedPaths.size() == 0) { return; }
  trigPath_ = matchedPaths[0];
  if (hltConfig_.triggerIndex(trigPath_) >= hltConfig_.size()) { return; }
  triggered_ = trigResultsHandle_->accept(hltConfig_.triggerIndex(trigPath_));

  //*****************************************************
  
  iEvent.getByToken(generalTrackToken_, generalTrackHandle_);
  iEvent.getByToken(dSAMuToken_, dSAMuHandle_);
  assert(generalTrackHandle_.isValid());
  assert(dSAMuHandle_.isValid());

  genMuonPt_ .clear();
  genMuonPz_ .clear();
  genMuonEta_.clear();
  genMuonPhi_.clear();
  dSAPt_     .clear();
  dSAPz_     .clear();
  dSAEta_    .clear();
  dSAPhi_    .clear();
  dSAMatchDist_.clear();
  dSATrackQual_.clear();
  
  genMuonPt_ .reserve(4);
  genMuonPz_ .reserve(4);
  genMuonEta_.reserve(4);
  genMuonPhi_.reserve(4);
  dSAPt_     .reserve(4);
  dSAPz_     .reserve(4);
  dSAEta_    .reserve(4);
  dSAPhi_    .reserve(4);
  dSAMatchDist_.reserve(4);
  dSATrackQual_.reserve(4);

  vector<reco::GenParticleRef> genMuons{};
  for (const auto& dp : darkphotons)
  {
    size_t nDau = dp->numberOfDaughters();
    assert( nDau == 2 );
    for (size_t id(0); id!=nDau; ++id)
    {
      genMuons.emplace_back( dp->daughterRef(id) );
    }
  }

  vector<reco::TrackRef> dSAMuons{};
  for (size_t i_mu(0); i_mu!=dSAMuHandle_->size(); ++i_mu)
  {
    dSAMuons.emplace_back(dSAMuHandle_, i_mu);
  }
  ndSA_ = dSAMuons.size();

  map<reco::TrackRef, reco::GenParticleRef> dsaGenMuonMap{};

  for (const auto& muGen : genMuons)
  {
    float minGenDsamu_dR(999.);
    int bestMatchDsa_idx(-1);
    for (const auto& muTk : dSAMuons)
    {
      if ( dsaGenMuonMap.count(muTk)>0 ) { continue; }
      float _dR = deltaR( *(muGen.get()), *(muTk.get()) );
      if ( _dR >= minGenDsamu_dR ) { continue; }
      minGenDsamu_dR = _dR;
      bestMatchDsa_idx = muTk.key();
    }
    if ( bestMatchDsa_idx == -1 or
         bestMatchDsa_idx >= static_cast<int>(dSAMuHandle_->size()))
    { continue; }
    if ( minGenDsamu_dR > 0.3 ) { continue; }
    dsaGenMuonMap.emplace( reco::TrackRef(dSAMuHandle_, bestMatchDsa_idx), muGen);
  }

  for (const auto& dsa_gen : dsaGenMuonMap)
  {
    const auto& dsa = *(dsa_gen.first.get());
    const auto& gen = *(dsa_gen.second.get());

    dSAPt_.emplace_back(dsa.pt());
    dSAPz_.emplace_back(dsa.pz());
    dSAEta_.emplace_back(dsa.eta());
    dSAPhi_.emplace_back(dsa.phi());
    dSAMatchDist_.emplace_back( deltaR(dsa, gen) );
    // http://cmsdoxygen.web.cern.ch/cmsdoxygen/CMSSW_10_0_3/doc/html/d8/df2/classreco_1_1TrackBase.html#aeee12ec6a3ea0d65caa2695c84ab25d5
    dSATrackQual_.emplace_back( dsa.qualityMask() );

    genMuonPt_.emplace_back(gen.pt());
    genMuonPz_.emplace_back(gen.pz());
    genMuonEta_.emplace_back(gen.eta());
    genMuonPhi_.emplace_back(gen.phi());
  }

  dSAT_->Fill();

  //********************************************************
  
  iEvent.getByToken(pvsToken_, pvsHandle_);
  assert(pvsHandle_.isValid());

  if ( pvsHandle_->size()==0 ) { return; }
  const auto& pv = *(pvsHandle_->begin());

  //********************************************************

  iEvent.getByToken(jetToken_, jetHandle_);
  assert(jetHandle_.isValid());

  nJet_ = (*jetHandle_).size();
  jetSeedType_.clear();
  jetSeedType_.reserve(2);
  jetEnergy_  .clear();
  jetEnergy_  .reserve(2);
  jetMass_    .clear();
  jetMass_    .reserve(2);
  jetPt_      .clear();
  jetPt_      .reserve(2);
  jetPz_      .clear();
  jetPz_      .reserve(2);
  jetEta_     .clear();
  jetEta_     .reserve(2);
  jetPhi_     .clear();
  jetPhi_     .reserve(2);
  jetVtxLxy_  .clear();
  jetVtxLxy_  .reserve(2);
  jetVtxL3D_  .clear();
  jetVtxL3D_  .reserve(2);
  jetVtxLxySig_.clear();
  jetVtxLxySig_.reserve(2);
  jetVtxL3DSig_.clear();
  jetVtxL3DSig_.reserve(2);
  jetMatchDist_.clear();
  jetMatchDist_.reserve(2);
  jetVtxMatchDist_.clear();
  jetVtxMatchDist_.reserve(2);
  jetVtxMatchDistT_.clear();
  jetVtxMatchDistT_.reserve(2);
  jetChargedEmEnergyFrac_.clear();
  jetChargedEmEnergyFrac_.reserve(2);
  jetChargedHadEnergyFrac_.clear();
  jetChargedHadEnergyFrac_.reserve(2);
  jetNeutralEmEnergyFrac_.clear();
  jetNeutralEmEnergyFrac_.reserve(2);
  jetNeutralHadEnergyFrac_.clear();
  jetNeutralHadEnergyFrac_.reserve(2);
  jetTrackPt_.clear();
  jetTrackPt_.reserve(4);
  jetTrackEta_.clear();
  jetTrackEta_.reserve(4);
  jetTrackD0Sig_.clear();
  jetTrackD0Sig_.reserve(4);
  jetTrackNormChi2_.clear();
  jetTrackNormChi2_.reserve(4);
  jetVtxNormChi2_.clear();
  jetVtxNormChi2_.reserve(2);
  jetChargedMultiplicity_.clear();
  jetChargedMultiplicity_.reserve(2);
  jetMuonMultiplicity_.clear();
  jetMuonMultiplicity_.reserve(2);
  jetNConstituents_.clear();
  jetNConstituents_.reserve(2);
  jetNTracks_.clear();
  jetNTracks_.reserve(2);

  genDarkphotonEnergy_.clear();
  genDarkphotonEnergy_.reserve(2);
  genDarkphotonPt_    .clear();
  genDarkphotonPt_    .reserve(2);
  genDarkphotonPz_    .clear();
  genDarkphotonEta_   .clear();
  genDarkphotonEta_   .reserve(2);
  genDarkphotonPhi_   .clear();
  genDarkphotonPhi_   .reserve(2);
  genDarkphotonLxy_   .clear();
  genDarkphotonLxy_   .reserve(2);
  genDarkphotonL3D_   .clear();
  genDarkphotonL3D_   .reserve(2);

  map<reco::PFJetRef, reco::GenParticleRef> jetDarkphotonMap{};
  
  // Filtering out decent good jets.
  vector<reco::PFJetRef> goodJets{};
  for (size_t i(0); i!=jetHandle_->size(); ++i)
  {
    reco::PFJetRef jRef(jetHandle_, i);
    if ( fabs(jRef->eta()) > 2.4 or
         jRef->numberOfDaughters() < 2 or
         jRef->neutralHadronEnergyFraction() > 0.99 or
         jRef->neutralEmEnergyFraction() > 0.99 )
    {
      continue;
    }

    int _ntracks(0);
    for (int ic(0); ic!=jRef->nConstituents(); ++ic)
    {
      auto&& iConst(jRef->getPFConstituent(ic));
      if ( iConst->trackRef().isNull() ) { continue; }
      auto&& iConstTk(iConst->trackRef());
      if ( iConstTk->pt() < 0.5 or
           iConstTk->normalizedChi2() > 10.)
      {
        continue;
      }
      ++_ntracks;
    }
    if ( _ntracks<2 ) { continue; }
    
    goodJets.push_back(jRef);
  }
  if ( goodJets.size()==0 ) { return; }


  for (const auto& dp : darkphotons)
  {
    float darkphotonJet_mindR(999.);
    int   darkphotonJet_bestIdx(-1);
    for (const auto& jet : goodJets)
    {
      if ( jetDarkphotonMap.count(jet)>0 ) { continue; }
      if ( deltaR(*(dp.get()), *(jet.get()))<darkphotonJet_mindR )
      {
        darkphotonJet_mindR = deltaR(*(dp.get()), *(jet.get()));
        darkphotonJet_bestIdx = jet.key();
      }
    }

    if (darkphotonJet_bestIdx == -1 or
        darkphotonJet_bestIdx >= static_cast<int>( jetHandle_->size() )) { continue; };
    if ( darkphotonJet_mindR > 0.3 ) {continue;}
    jetDarkphotonMap.emplace(reco::PFJetRef(jetHandle_, darkphotonJet_bestIdx), dp);
  }


  ESHandle<TransientTrackBuilder> theB;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);

  VertexDistanceXY vdistXY;
  VertexDistance3D vdist3D;

  for (const auto& j_dp : jetDarkphotonMap)
  {
    const auto& j  = *(j_dp.first.get());
    const auto& dp = *(j_dp.second.get());

    //***********************************
    
    vector<reco::TrackRef> tks{};
    tks.reserve(j.nConstituents());
    
    int seedIndex(-1);
    float seedPt(-999.);
    bool hasDsaMu(false);
    int nMu(0); // number of mu type PFCandidates
    float chargedEmEnergy(0.);
    for (int i(0); i!=j.nConstituents(); ++i)
    {
      auto&& iConst(j.getPFConstituent(i));
      
      if (iConst->pt() > seedPt)
      {
        seedIndex = i;
        seedPt = iConst->pt();
      }
      
      if (iConst->trackRef().isNonnull())
      {
        tks.emplace_back(iConst->trackRef());
        if (!hasDsaMu and iConst->trackRef().id() == dSAMuHandle_.id())
        {
          hasDsaMu = true;
        }
      }

      if (iConst->particleId() == reco::PFCandidate::ParticleType::mu) {++nMu;}
      if (iConst->particleId() == reco::PFCandidate::ParticleType::mu or
          iConst->particleId() == reco::PFCandidate::ParticleType::e )
      {
        chargedEmEnergy += iConst->energy();
      }
    }

    jetEnergy_.emplace_back(j.energy());
    jetMass_  .emplace_back(j.mass());
    jetPt_    .emplace_back(j.pt());
    jetPz_    .emplace_back(j.pz());
    jetEta_   .emplace_back(j.eta());
    jetPhi_   .emplace_back(j.phi());
    jetChargedMultiplicity_.emplace_back(j.chargedMultiplicity());
    jetNConstituents_      .emplace_back(j.nConstituents());
    jetChargedEmEnergyFrac_.emplace_back( chargedEmEnergy/j.energy() );
    jetChargedHadEnergyFrac_.emplace_back(j.chargedHadronEnergyFraction());
    jetNeutralEmEnergyFrac_ .emplace_back(j.neutralEmEnergyFraction());
    jetNeutralHadEnergyFrac_.emplace_back(j.neutralHadronEnergyFraction());
    jetNTracks_         .emplace_back(tks.size());
    jetMuonMultiplicity_.emplace_back(nMu);

    //***********************************

    /// finding the seed (own max pT) type
    reco::PFCandidatePtr&& seed(j.getPFConstituent(seedIndex));
    assert(seed.isNonnull());
    int _seedtype = static_cast<int>( seed->particleId() ); // http://cmsdoxygen.web.cern.ch/cmsdoxygen/CMSSW_9_4_8/doc/html/dc/d55/classreco_1_1PFCandidate.html#af39a4e9ae718041649773fa7ca0919bc
    if (seed->particleId() == reco::PFCandidate::ParticleType::mu)
    {
        if (seed->trackRef().isNonnull() and
            seed->trackRef().id() == dSAMuHandle_.id())
        { _seedtype = 8; }
    }
    if (assignTypeAnyDsaMu_ and hasDsaMu) { _seedtype = 8; }
    jetSeedType_.emplace_back(_seedtype);
    
    //***********************************

    bool _foundGoodVertex = false;
    vector<reco::TransientTrack> t_tks{};
    for (const auto& tk : tks)
    {
      jetTrackPt_ .emplace_back( tk->pt() );
      jetTrackEta_.emplace_back( tk->eta() );
      jetTrackD0Sig_   .emplace_back( fabs(tk->d0())/tk->d0Error() );
      jetTrackNormChi2_.emplace_back( tk->normalizedChi2() );

      // if ( fabs(tk->d0())/tk->d0Error() < 2.) { continue; }
      if ( tk->normalizedChi2()>5 ) { continue; }
      if ( tk->pt() < 0.5 ) { continue; }
      t_tks.push_back( theB->build(tk.get()) );
    }
    
    if ( t_tks.size()>= 2 )
    {
      unique_ptr<ff::KalmanVertexFitter> kvf(new ff::KalmanVertexFitter(kvfParam_,
                                            kvfParam_.getParameter<bool>("doSmoothing")));
      TransientVertex tv = kvf->vertex(t_tks);
      if ( tv.isValid() /*and tv.normalisedChiSquared()<5.*/ )
      {
        _foundGoodVertex = true;
        
        Measurement1D distXY = vdistXY.distance(tv.vertexState(), pv);
        Measurement1D dist3D = vdist3D.distance(tv.vertexState(), pv);
        jetVtxLxy_.emplace_back( distXY.value() );
        jetVtxL3D_.emplace_back( dist3D.value() );
        jetVtxLxySig_.emplace_back( distXY.value()/distXY.error() );
        jetVtxL3DSig_.emplace_back( dist3D.value()/dist3D.error() );
        jetVtxNormChi2_.emplace_back( tv.normalisedChiSquared() );

        math::XYZPoint vtxDiff(reco::Vertex(tv).position() - dp.daughterRef(0)->vertex());
        jetVtxMatchDistT_.emplace_back( vtxDiff.rho() );
        jetVtxMatchDist_ .emplace_back( sqrt(vtxDiff.mag2()) );
      }
    }

    if ( _foundGoodVertex==false )
    {
      jetVtxLxy_   .emplace_back( NAN );
      jetVtxL3D_   .emplace_back( NAN );
      jetVtxLxySig_.emplace_back( NAN );
      jetVtxL3DSig_.emplace_back( NAN );
      jetVtxNormChi2_  .emplace_back( NAN );
      jetVtxMatchDistT_.emplace_back( NAN );
      jetVtxMatchDist_ .emplace_back( NAN );
    }

    //***********************************

    /// Filling MC info
    genDarkphotonEnergy_.emplace_back(dp.energy());
    genDarkphotonPt_    .emplace_back(dp.pt());
    genDarkphotonPz_    .emplace_back(dp.pz());
    genDarkphotonEta_   .emplace_back(dp.eta());
    genDarkphotonPhi_   .emplace_back(dp.phi());
    genDarkphotonLxy_   .emplace_back((dp.daughterRef(0)->vertex()).rho());
    genDarkphotonL3D_   .emplace_back( sqrt((dp.daughterRef(0)->vertex()).mag2()) );
    
    jetMatchDist_       .emplace_back( deltaR(j, dp) );

  }

  jetT_->Fill();

  //*****************************************************


  return;
}

void
pfJetAnalysis::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void
pfJetAnalysis::endJob() {}