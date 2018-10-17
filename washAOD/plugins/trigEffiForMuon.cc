#include "Firefighter/washAOD/interface/trigEffiForMuon.h"
#include "Firefighter/recoStuff/interface/RecoHelpers.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/Math/interface/deltaR.h"

trigEffiForMuon::trigEffiForMuon(const edm::ParameterSet& ps) :
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

trigEffiForMuon::~trigEffiForMuon() = default;

void
trigEffiForMuon::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("muon", edm::InputTag("muons"));
  desc.add<edm::InputTag>("genParticle", edm::InputTag("genParticles"));
  desc.add<edm::InputTag>("trigResult", edm::InputTag("TriggerResults","","HLT"));
  desc.add<edm::InputTag>("trigEvent", edm::InputTag("hltTriggerSummaryAOD","","HLT"));
  desc.add<std::vector<std::string>>("trigPath", {});
  desc.add<std::string>("processName", "HLT");
  desc.add<int>("nMuons", 3);
  descriptions.add("trigEffiForMuon", desc);
}

void
trigEffiForMuon::beginJob()
{
  muonT_ = fs->make<TTree>("trigEffiForMuon", "");

  for (const auto& p : trigPathNoVer_)
    muonT_->Branch(p.c_str(), &fired_[p], (p+"/O").c_str());

  muonT_->Branch("pt",   &pt_);
  muonT_->Branch("eta",  &eta_);
  muonT_->Branch("phi",  &phi_);
  muonT_->Branch("dR",   &dR_);
  muonT_->Branch("vxy",  &vxy_);
  muonT_->Branch("vz",   &vz_);
}

void
trigEffiForMuon::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
  using namespace std;
  using namespace edm;

  bool changed(true);
  if (hltConfig_.init(iRun,iSetup,processName_,changed)) {
    if (changed) {
      LogInfo("trigEffiForMuon")<<"trigEffiForMuon::beginRun: "<<"hltConfig init for Run"<<iRun.run();
      hltConfig_.dump("ProcessName");
      hltConfig_.dump("GlobalTag");
      hltConfig_.dump("TableName");
    }
  } else {
    LogError("trigEffiForMuon")<<"trigEffiForMuon::beginRun: config extraction failure with processName -> "
      <<processName_;
  }

}

void
trigEffiForMuon::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace std;
  using namespace edm;

  iEvent.getByToken(muonToken_, muonHandle_);
  if (!muonHandle_.isValid()) {
    LogError("trigEffiForMuon")
      << "trigEffiForMuon::analyze: Error in getting muon product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(genParticleToken_, genParticleHandle_);
  if (!genParticleHandle_.isValid()) {
    LogError("trigEffiForMuon")
      << "trigEffiForMuon::analyze: Error in getting genParticle product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(trigResultsToken_, trigResultsHandle_);
  if (!trigResultsHandle_.isValid()) {
    LogError("trigEffiForMuon")
      << "trigEffiForMuon::analyze: Error in getting triggerResults product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(trigEventToken_, trigEventHandle_);
  if (!trigEventHandle_.isValid()) {
    LogError("trigEffiForMuon")
      << "trigEffiForMuon::analyze: Error in getting triggerEvent product from Event!"
      << endl;
    return;
  }

  // acceptence
  vector<size_t> acceptedGenIdx{};
  for (size_t ig(0); ig!=genParticleHandle_->size(); ++ig)
  {
    if (!ff::genAccept((*genParticleHandle_)[ig])) { continue; }
    acceptedGenIdx.push_back(ig);
  }
  if (acceptedGenIdx.size()<4) return;
  if ((int)muonHandle_->size()<nMuons_) return;


  vector<size_t> muonIdx{};
  for (size_t i(0); i!=muonHandle_->size(); ++i) muonIdx.push_back(i);

  // general selection [RECO]
  auto generalSelection = [&](const auto id){
    reco::MuonRef m(muonHandle_, id);
    bool pass = m->pt() > 5
             && abs(m->eta()) < 2.4
             && muon::isLooseMuon(*(m.get()));
    return !pass;
  };

  muonIdx.erase(remove_if(muonIdx.begin(), muonIdx.end(), generalSelection), muonIdx.end());
  if ((int)muonIdx.size()<nMuons_) return;

  // MC match
  vector<size_t> matchedGenMuIdx{};
  for (const size_t muid : muonIdx) {
    reco::MuonRef recoMu(muonHandle_, muid);
    for (const size_t ig : acceptedGenIdx) {
      if (find(matchedGenMuIdx.begin(), matchedGenMuIdx.end(), ig) != matchedGenMuIdx.end()) continue;
      reco::GenParticleRef genMu(genParticleHandle_, ig);
      if (abs(genMu->pdgId())!=13) continue;
      if (deltaR(*(recoMu.get()), *(genMu.get())) > 0.3) continue;
      // if (recoMu->charge() != genMu->charge()) continue;
      matchedGenMuIdx.push_back(ig);
    }
  }
  if ((int)matchedGenMuIdx.size()<nMuons_) return;

  // sort by pt [RECO]
  sort(muonIdx.begin(), muonIdx.end(),
      [&](size_t l, size_t r){
        reco::MuonRef lhs(muonHandle_, l);
        reco::MuonRef rhs(muonHandle_, r);
        return lhs->pt() > rhs->pt();
      });

  pt_  .clear(); pt_  .reserve(4);
  eta_ .clear(); eta_ .reserve(4);
  phi_ .clear(); phi_ .reserve(4);

  for (const size_t muid : muonIdx) {
    reco::MuonRef recoMu(muonHandle_, muid);
    pt_ .push_back(recoMu->pt());
    eta_.push_back(recoMu->eta());
    phi_.push_back(recoMu->phi());
  }

  dR_ .clear(); dR_ .reserve(2);
  vxy_.clear(); vxy_.reserve(2);
  vz_ .clear(); vz_ .reserve(2);

  for (size_t igmu(0); igmu!=matchedGenMuIdx.size(); ++igmu)
  {
    reco::GenParticleRef gmuA(genParticleHandle_, matchedGenMuIdx[igmu]);
    for (size_t jgmu(igmu+1); jgmu!=matchedGenMuIdx.size(); ++jgmu)
    {
      reco::GenParticleRef gmuB(genParticleHandle_, matchedGenMuIdx[jgmu]);
      if (gmuA->vertex() != gmuB->vertex()) { continue; }
      
      dR_.emplace_back( deltaR(*(gmuA.get()), *(gmuB.get())) );
      vxy_.emplace_back(gmuA->vertex().rho());
      vz_.emplace_back( gmuA->vz() );
    }
  }

  // trigger firing condition
  const vector<string>& pathNames = hltConfig_.triggerNames();
  for (const auto& p : trigPathNoVer_)
  {
    const vector<string> matchedPaths(hltConfig_.restoreVersion(pathNames, p));
    if (matchedPaths.size() == 0) {
      LogError("trigEffiForMuon")<<"Could not find matched full trigger path with -> "<<p<<endl;
      return;
    }
    const std::string trigPath_ = matchedPaths[0];
    if (hltConfig_.triggerIndex(trigPath_) >= hltConfig_.size()) {
      LogError("trigEffiForMuon")<<"Cannot find trigger path -> "<<trigPath_<<endl;
      return;
    }
    fired_[p] = trigResultsHandle_->accept(hltConfig_.triggerIndex(trigPath_));
  }
  

  muonT_->Fill();

  return;
}

void
trigEffiForMuon::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void
trigEffiForMuon::endJob() {}
