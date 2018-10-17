#include "Firefighter/washAOD/interface/trigSelfEffiForMuon.h"
#include "Firefighter/recoStuff/interface/RecoHelpers.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/Math/interface/deltaR.h"

trigSelfEffiForMuon::trigSelfEffiForMuon(const edm::ParameterSet& ps) :
  muonTag_(ps.getParameter<edm::InputTag>("muon")),
  genParticleTag_(ps.getParameter<edm::InputTag>("genParticle")),
  trigResultsTag_(ps.getParameter<edm::InputTag>("trigResult")),
  trigEventTag_(ps.getParameter<edm::InputTag>("trigEvent")),
  trigPathNoVer_(ps.getParameter<std::vector<std::string>>("trigPath")),
  processName_(ps.getParameter<std::string>("processName")),
  nMuons_(ps.getParameter<int>("nMuons")),
  muonToken_(consumes<reco::MuonCollection>(muonTag_)),
  genParticleToken_(consumes<reco::GenParticleCollection>(genParticleTag_)),
  trigResultsToken_(consumes<edm::TriggerResults>(trigResultsTag_)),
  trigEventToken_(consumes<trigger::TriggerEvent>(trigEventTag_))
{
  usesResource("TFileService");
  for (const auto& p : trigPathNoVer_)
    fired_[p] = false;
}

trigSelfEffiForMuon::~trigSelfEffiForMuon() = default;

void
trigSelfEffiForMuon::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("muon", edm::InputTag("muons"));
  desc.add<edm::InputTag>("genParticle", edm::InputTag("genParticles"));
  desc.add<edm::InputTag>("trigResult", edm::InputTag("TriggerResults","","HLT"));
  desc.add<edm::InputTag>("trigEvent", edm::InputTag("hltTriggerSummaryAOD","","HLT"));
  desc.add<std::vector<std::string>>("trigPath", {});
  desc.add<std::string>("processName", "HLT");
  desc.add<int>("nMuons", 3);
  descriptions.add("trigSelfEffiForMuon", desc);
}

void
trigSelfEffiForMuon::beginJob()
{
  muonT_ = fs->make<TTree>("trigSelfEffiForMuon", "");

  for (const auto& p : trigPathNoVer_)
    muonT_->Branch(p.c_str(), &fired_[p], (p+"/O").c_str());

  muonT_->Branch("pt",   &pt_);
  muonT_->Branch("eta",  &eta_);
  muonT_->Branch("phi",  &phi_);
}

void
trigSelfEffiForMuon::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
  using namespace std;
  using namespace edm;

  bool changed(true);
  if (hltConfig_.init(iRun,iSetup,processName_,changed)) {
    if (changed) {
      LogInfo("trigSelfEffiForMuon")<<"trigSelfEffiForMuon::beginRun: "<<"hltConfig init for Run"<<iRun.run();
      hltConfig_.dump("ProcessName");
      hltConfig_.dump("GlobalTag");
      hltConfig_.dump("TableName");
    }
  } else {
    LogError("trigSelfEffiForMuon")<<"trigSelfEffiForMuon::beginRun: config extraction failure with processName -> "
      <<processName_;
  }

}

void
trigSelfEffiForMuon::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace std;
  using namespace edm;

  iEvent.getByToken(muonToken_, muonHandle_);
  if (!muonHandle_.isValid()) {
    LogError("trigSelfEffiForMuon")
      << "trigSelfEffiForMuon::analyze: Error in getting muon product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(trigResultsToken_, trigResultsHandle_);
  if (!trigResultsHandle_.isValid()) {
    LogError("trigSelfEffiForMuon")
      << "trigSelfEffiForMuon::analyze: Error in getting triggerResults product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(trigEventToken_, trigEventHandle_);
  if (!trigEventHandle_.isValid()) {
    LogError("trigSelfEffiForMuon")
      << "trigSelfEffiForMuon::analyze: Error in getting triggerEvent product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(genParticleToken_, genParticleHandle_);
  if (!genParticleHandle_.isValid()) {
    LogError("trigSelfEffiForMuon")
      << "trigSelfEffiForMuon::analyze: Error in getting genParticle product from Event!"
      << endl;
    return;
  }

  int nAccpted = count_if((*genParticleHandle_).begin(), (*genParticleHandle_).end(), ff::genAccept);
  if (nAccpted<4) return;

  vector<reco::MuonRef> muRefs{};
  for (size_t i(0); i!=muonHandle_->size(); ++i) {
    muRefs.emplace_back(muonHandle_, i);
  }

  /* general selection */
  auto generalSelection = [](const auto& m){
    bool pass = m->pt() > 5
             && abs(m->eta()) < 2.4
             && muon::isLooseMuon(*(m.get()));
    return !pass;
  };
  muRefs.erase(remove_if(muRefs.begin(), muRefs.end(), generalSelection), muRefs.end());
  if (int(muRefs.size())<nMuons_) return;

  /* sort mu by pT */
  sort(muRefs.begin(), muRefs.end(),
      [](const auto& lhs, const auto& rhs){ return lhs->pt() > rhs->pt(); });

  pt_  .clear(); pt_  .reserve(muRefs.size());
  eta_ .clear(); eta_ .reserve(muRefs.size());
  phi_ .clear(); phi_ .reserve(muRefs.size());

  for (const auto& recoMu : muRefs) {
    pt_ .push_back(recoMu->pt());
    eta_.push_back(recoMu->eta());
    phi_.push_back(recoMu->phi());
  }

  // trigger firing condition
  const vector<string>& pathNames = hltConfig_.triggerNames();
  for (const auto& p : trigPathNoVer_)
  {
    const vector<string> matchedPaths(hltConfig_.restoreVersion(pathNames, p));
    if (matchedPaths.size() == 0) {
      LogError("trigSelfEffiForMuon")<<"Could not find matched full trigger path with -> "<<p<<endl;
      return;
    }
    const std::string trigPath_ = matchedPaths[0];
    if (hltConfig_.triggerIndex(trigPath_) >= hltConfig_.size()) {
      LogError("trigSelfEffiForMuon")<<"Cannot find trigger path -> "<<trigPath_<<endl;
      return;
    }
    fired_[p] = trigResultsHandle_->accept(hltConfig_.triggerIndex(trigPath_));
  }

  muonT_->Fill();

  return;
}

void
trigSelfEffiForMuon::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void
trigSelfEffiForMuon::endJob() {}
