#include "Firefighter/washAOD/interface/pfJetAnalysis.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

pfJetAnalysis::pfJetAnalysis(const edm::ParameterSet& ps) :
  jetTag_(ps.getParameter<edm::InputTag>("src")),
  jetToken_(consumes<reco::PFJetCollection>(jetTag_)),
  assignTypeAnyDsaMu_(ps.getParameter<bool>("anydSAForJetType"))
{
  using namespace std;
  using namespace edm;

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
  jetT_->Branch("jetSeedType",            &jetSeedType_);
  jetT_->Branch("jetEnergy",              &jetEnergy_);
  jetT_->Branch("jetMass",                &jetMass_);
  jetT_->Branch("jetPt",                  &jetPt_);
  jetT_->Branch("jetPz",                  &jetPz_);
  jetT_->Branch("jetEta",                 &jetEta_);
  jetT_->Branch("jetPhi",                 &jetPhi_);
  jetT_->Branch("jetChargedMultiplicity", &jetChargedMultiplicity_);
  jetT_->Branch("jetNConstituents",       &jetNConstituents_);
  jetT_->Branch("jetNTracks",             &jetNTracks_);
}

void
pfJetAnalysis::analyze(const edm::Event& iEvent,
                       const edm::EventSetup& iSetup)
{
  using namespace std;
  using namespace edm;

  iEvent.getByToken(jetToken_, jetHandle_);
  assert(jetHandle_.isValid());

  iEvent.getByToken(generalTrackToken_, generalTrackHandle_);
  iEvent.getByToken(dSAMuToken_, dSAMuHandle_);
  assert(generalTrackHandle_.isValid());
  assert(dSAMuHandle_.isValid());

  nJet_ = (*jetHandle_).size();
  jetSeedType_.clear();
  jetSeedType_.reserve(nJet_);
  jetEnergy_.clear();
  jetEnergy_.reserve(nJet_);
  jetMass_.clear();
  jetMass_.reserve(nJet_);
  jetPt_.clear();
  jetPt_.reserve(nJet_);
  jetPz_.clear();
  jetPz_.reserve(nJet_);
  jetEta_.clear();
  jetEta_.reserve(nJet_);
  jetPhi_.clear();
  jetPhi_.reserve(nJet_);
  jetChargedMultiplicity_.clear();
  jetChargedMultiplicity_.reserve(nJet_);
  jetNConstituents_.clear();
  jetNConstituents_.reserve(nJet_);
  jetNTracks_.clear();
  jetNTracks_.reserve(nJet_);


  for (const auto& j : *jetHandle_)
  {
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
    for (int i(0); i!=jetNConstituents_.back(); ++i)
    {
      if (j.getPFConstituent(i)->pt() > seedPt)
      {
        seedIndex = i;
        seedPt = j.getPFConstituent(i)->pt();
      }
      if (j.getPFConstituent(i)->trackRef().isNonnull())
      {
        tks.emplace_back(j.getPFConstituent(i)->trackRef());
        if (!hasDsaMu and j.getPFConstituent(i)->trackRef().id() == dSAMuHandle_.id())
        {
          hasDsaMu = true;
        }
      }
    }
    jetNTracks_.emplace_back(tks.size());

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

  }

  jetT_->Fill();

  return;
}

void
pfJetAnalysis::endJob() {}