#include "Firefighter/washAOD/interface/pfJetAnalysis.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/Candidate/interface/CompositeCandidate.h"
#include "DataFormats/Candidate/interface/CompositeCandidateFwd.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

pfJetAnalysis::pfJetAnalysis(const edm::ParameterSet& ps) :
  jetTag_(ps.getParameter<edm::InputTag>("src")),
  jetToken_(consumes<reco::PFJetCollection>(jetTag_)),
  assignTypeAnyDsaMu_(ps.getParameter<bool>("anydSAForJetType"))
{
  using namespace std;
  using namespace edm;

  genParticleToken_  = consumes<reco::GenParticleCollection>(InputTag("genParticles"));
  generalTrackToken_ = consumes<reco::TrackCollection>(InputTag("generalTracks"));
  dSAMuToken_        = consumes<reco::TrackCollection>(InputTag("displacedStandAloneMuons"));

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
  desc.add<bool>("anydSAForJetType", true);
  ds.add("pfJetAnalysis", desc);
}

void
pfJetAnalysis::beginJob()
{
  jetT_ = fs->make<TTree>("Jet", "");

  jetT_->Branch("nJet", &nJet_, "nJet/i");
  
  jetT_->Branch("genDarkphotonEnergy",    &genDarkphotonEnergy_);
  jetT_->Branch("genDarkphotonPt",        &genDarkphotonPt_);
  jetT_->Branch("genDarkphotonPz",        &genDarkphotonPz_);
  jetT_->Branch("genDarkphotonEta",       &genDarkphotonEta_);
  jetT_->Branch("genDarkphotonPhi",       &genDarkphotonPhi_);
  jetT_->Branch("genDarkphotonLxy",       &genDarkphotonLxy_);
  jetT_->Branch("genDarkphotonLz",        &genDarkphotonLz_);

  jetT_->Branch("jetSeedType",            &jetSeedType_);
  jetT_->Branch("jetEnergy",              &jetEnergy_);
  jetT_->Branch("jetMass",                &jetMass_);
  jetT_->Branch("jetPt",                  &jetPt_);
  jetT_->Branch("jetPz",                  &jetPz_);
  jetT_->Branch("jetEta",                 &jetEta_);
  jetT_->Branch("jetPhi",                 &jetPhi_);
  jetT_->Branch("jetLxy",                 &jetLxy_);
  jetT_->Branch("jetLz",                  &jetLz_);
  jetT_->Branch("jetChargedMultiplicity", &jetChargedMultiplicity_);
  jetT_->Branch("jetMuonMultiplicity",    &jetMuonMultiplicity_);
  jetT_->Branch("jetNConstituents",       &jetNConstituents_);
  jetT_->Branch("jetNTracks",             &jetNTracks_);
}

void
pfJetAnalysis::analyze(const edm::Event& iEvent,
                       const edm::EventSetup& iSetup)
{
  using namespace std;
  using namespace edm;

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

  iEvent.getByToken(jetToken_, jetHandle_);
  assert(jetHandle_.isValid());

  iEvent.getByToken(generalTrackToken_, generalTrackHandle_);
  iEvent.getByToken(dSAMuToken_, dSAMuHandle_);
  assert(generalTrackHandle_.isValid());
  assert(dSAMuHandle_.isValid());

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
  jetLxy_     .clear();
  jetLxy_     .reserve(2);
  jetLz_      .clear();
  jetLz_      .reserve(2);
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
  genDarkphotonLz_    .clear();
  genDarkphotonLz_    .reserve(2);

  map<reco::PFJetRef, reco::GenParticleRef> jetDarkphotonMap{};
  
  // Filtering out decent good jets.
  vector<reco::PFJetRef> goodJets{};
  for (size_t i(0); i!=jetHandle_->size(); ++i)
  {
    reco::PFJetRef jRef(jetHandle_, i);
    if ( fabs(jRef->eta()) < 2.4 and
         jRef->numberOfDaughters() > 1 and
         jRef->neutralHadronEnergyFraction() < 0.99 and
         jRef->neutralEmEnergyFraction() < 0.99 )
    {
      goodJets.push_back(jRef);
    }
  }
  if ( goodJets.size()<1 ) { return; }


  for (const auto& dp : darkphotons)
  {
    float darkphotonJet_dR(999.);
    int darkphotonJet_idx(-1);
    for (const auto& jet : goodJets)
    {
      if ( jetDarkphotonMap.count(jet)>0 ) {continue;}
      if ( deltaR(*(dp.get()), *(jet.get()))<darkphotonJet_dR )
      {
        darkphotonJet_dR = deltaR(*(dp.get()), *(jet.get()));
        darkphotonJet_idx = jet.key();
      }
    }
    // cout<<darkphotonJet_idx<<" "<<jetHandle_->size()<<endl;
    if (darkphotonJet_idx == -1 or
        darkphotonJet_idx >= static_cast<int>( jetHandle_->size() )) { continue; };
    if ( darkphotonJet_dR > 0.3 ) {continue;}
    jetDarkphotonMap.emplace(reco::PFJetRef(jetHandle_, darkphotonJet_idx), dp);
  }


  ESHandle<TransientTrackBuilder> theB;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);

  for (const auto& j_dp : jetDarkphotonMap)
  {
    const auto& j = *(j_dp.first.get());
    jetEnergy_.emplace_back(j.energy());
    jetMass_  .emplace_back(j.mass());
    jetPt_    .emplace_back(j.pt());
    jetPz_    .emplace_back(j.pz());
    jetEta_   .emplace_back(j.eta());
    jetPhi_   .emplace_back(j.phi());
    jetChargedMultiplicity_.emplace_back(j.chargedMultiplicity());
    jetNConstituents_      .emplace_back(j.nConstituents());

    vector<reco::TrackRef> tks{};
    tks.reserve(jetNConstituents_.back());
    int seedIndex(-1);
    float seedPt(-999.);
    bool hasDsaMu(false);
    int nMu(0); // number of mu type PFCandidates
    for (int i(0); i!=jetNConstituents_.back(); ++i)
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
    }
    jetNTracks_.emplace_back(tks.size());
    jetMuonMultiplicity_.emplace_back(nMu);

    float _lxy(0.0);
    float _lz(0.0);
    if ( tks.size() < 2 ) { return; } // require at least 2 tracks to perform vertex

    vector<reco::TransientTrack> t_tks{};
    for (const auto& tk : tks)
    {
      if ( not tk->quality(reco::Track::tight) ) { continue; }
      if ( tk->pt()<0.5 ) { continue; }
      t_tks.push_back( theB->build(tk.get()) );
    }
    if ( t_tks.size() < 2 ) { return; } // vertexing with >=2 good tracks

    unique_ptr<KalmanVertexFitter> kvf(new KalmanVertexFitter(true));
    TransientVertex tv = kvf->vertex(t_tks);
    if ( tv.isValid() and tv.normalisedChiSquared()<5. )
    {
      reco::Vertex _vtx = reco::Vertex(tv);
      _lxy = _vtx.position().rho();
      _lz  = _vtx.z();
    }

    jetLxy_.emplace_back(_lxy);
    jetLz_ .emplace_back(_lz);

    // finding the seed (own max pT) type
    reco::PFCandidatePtr&& seed(j.getPFConstituent(seedIndex));
    assert(seed.isNonnull());
    int _seedtype = static_cast<int>( seed->particleId() ); // http://cmsdoxygen.web.cern.ch/cmsdoxygen/CMSSW_9_4_8/doc/html/dc/d55/classreco_1_1PFCandidate.html#af39a4e9ae718041649773fa7ca0919bc
    if (seed->particleId() == reco::PFCandidate::ParticleType::mu)
    {
        // assert(seed->trackRef().isNonnull());
        if (seed->trackRef().isNonnull() &&
            seed->trackRef().id() == dSAMuHandle_.id())
        {
            _seedtype = 8;
        }
    }
    if (assignTypeAnyDsaMu_ and hasDsaMu) { _seedtype = 8; }
    jetSeedType_.emplace_back(_seedtype);


    // Filling MC info
    const auto& dp = *(j_dp.second.get());
    genDarkphotonEnergy_.emplace_back(dp.energy());
    genDarkphotonPt_    .emplace_back(dp.pt());
    genDarkphotonPz_    .emplace_back(dp.pz());
    genDarkphotonEta_   .emplace_back(dp.eta());
    genDarkphotonPhi_   .emplace_back(dp.phi());
    genDarkphotonLxy_   .emplace_back((dp.daughterRef(0)->vertex()).rho());
    genDarkphotonLz_    .emplace_back((dp.daughterRef(0)->vertex()).z());

  }

  jetT_->Fill();

  return;
}

void
pfJetAnalysis::endJob() {}