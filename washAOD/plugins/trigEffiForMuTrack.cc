#include "Firefighter/washAOD/interface/trigEffiForMuTrack.h"
#include "Firefighter/recoStuff/interface/RecoHelpers.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Math/interface/deltaR.h"

trigEffiForMuTrack::trigEffiForMuTrack(const edm::ParameterSet& ps) :
  muTrackTag_(ps.getParameter<edm::InputTag>("muTrack")),
  genParticleTag_(ps.getParameter<edm::InputTag>("genParticle")),
  trigResultsTag_(ps.getParameter<edm::InputTag>("trigResult")),
  trigEventTag_(ps.getParameter<edm::InputTag>("trigEvent")),
  trigPathNoVer_(ps.getParameter<std::vector<std::string>>("trigPath")),
  processName_(ps.getParameter<std::string>("processName")),
  nMuons_(ps.getParameter<int>("nMuons")),
  muTrackToken_(consumes<reco::TrackCollection>(muTrackTag_)),
  genParticleToken_(consumes<reco::GenParticleCollection>(genParticleTag_)),
  trigResultsToken_(consumes<edm::TriggerResults>(trigResultsTag_)),
  trigEventToken_(consumes<trigger::TriggerEvent>(trigEventTag_))
{
  usesResource("TFileService");
  for (const auto& p : trigPathNoVer_)
    fired_[p] = false;
}

trigEffiForMuTrack::~trigEffiForMuTrack() = default;

void
trigEffiForMuTrack::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("muTrack", edm::InputTag("displacedStandAloneMuons"));
  desc.add<edm::InputTag>("genParticle", edm::InputTag("genParticles"));
  desc.add<edm::InputTag>("trigResult", edm::InputTag("TriggerResults","","HLT"));
  desc.add<edm::InputTag>("trigEvent", edm::InputTag("hltTriggerSummaryAOD","","HLT"));
  desc.add<std::vector<std::string>>("trigPath", {});
  desc.add<std::string>("processName", "HLT");
  desc.add<int>("nMuons", 2);
  descriptions.add("trigEffiForMuTrack", desc);
}

void
trigEffiForMuTrack::beginJob()
{
  muTrackT_ = fs->make<TTree>("trigEffiForMuTrack", "");

  for (const auto& p : trigPathNoVer_)
    muTrackT_->Branch(p.c_str(), &fired_[p], (p+"/O").c_str());

  muTrackT_->Branch("pt",   &pt_);
  muTrackT_->Branch("eta",  &eta_);
  muTrackT_->Branch("phi",  &phi_);
  muTrackT_->Branch("dR",   &dR_);
  muTrackT_->Branch("vxy",  &vxy_);
  muTrackT_->Branch("vz",   &vz_);
}

void
trigEffiForMuTrack::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
  using namespace std;
  using namespace edm;

  bool changed(true);
  if (hltConfig_.init(iRun,iSetup,processName_,changed)) {
    if (changed) {
      LogInfo("trigEffiForMuTrack")<<"trigEffiForMuTrack::beginRun: "<<"hltConfig init for Run"<<iRun.run();
      hltConfig_.dump("ProcessName");
      hltConfig_.dump("GlobalTag");
      hltConfig_.dump("TableName");
    }
  } else {
    LogError("trigEffiForMuTrack")<<"trigEffiForMuTrack::beginRun: config extraction failure with processName -> "
      <<processName_;
  }

}

void
trigEffiForMuTrack::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace std;
  using namespace edm;

  iEvent.getByToken(muTrackToken_, muTrackHandle_);
  if (!muTrackHandle_.isValid()) {
    LogError("trigEffiForMuTrack")
      << "trigEffiForMuTrack::analyze: Error in getting muTrack product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(genParticleToken_, genParticleHandle_);
  if (!genParticleHandle_.isValid()) {
    LogError("trigEffiForMuTrack")
      << "trigEffiForMuTrack::analyze: Error in getting genParticle product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(trigResultsToken_, trigResultsHandle_);
  if (!trigResultsHandle_.isValid()) {
    LogError("trigEffiForMuTrack")
      << "trigEffiForMuTrack::analyze: Error in getting triggerResults product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(trigEventToken_, trigEventHandle_);
  if (!trigEventHandle_.isValid()) {
    LogError("trigEffiForMuTrack")
      << "trigEffiForMuTrack::analyze: Error in getting triggerEvent product from Event!"
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
  if ((int)muTrackHandle_->size()<nMuons_) return;


  vector<size_t> muTrackIdx{};
  for (size_t i(0); i!=muTrackHandle_->size(); ++i) muTrackIdx.push_back(i);

  // general selection [RECO]
  auto generalSelection = [&](const auto tid){
    reco::TrackRef t(muTrackHandle_, tid);
    bool pass = t->pt() > 5
             && abs(t->eta()) < 2
             && t->hitPattern().numberOfValidMuonHits() > 16
             && t->hitPattern().muonStationsWithValidHits() > 1
             && t->normalizedChi2() < 10;
    return !pass;
  };

  muTrackIdx.erase(remove_if(muTrackIdx.begin(), muTrackIdx.end(), generalSelection), muTrackIdx.end());
  if ((int)muTrackIdx.size()<nMuons_) return;

  // MC match
  vector<size_t> matchedGenMuIdx{};
  for (const size_t muid : muTrackIdx) {
    reco::TrackRef recoMu(muTrackHandle_, muid);
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
  sort(muTrackIdx.begin(), muTrackIdx.end(),
      [&](size_t l, size_t r){
        reco::TrackRef lhs(muTrackHandle_, l);
        reco::TrackRef rhs(muTrackHandle_, r);
        return lhs->pt() > rhs->pt();
      });

  pt_  .clear(); pt_  .reserve(4);
  eta_ .clear(); eta_ .reserve(4);
  phi_ .clear(); phi_ .reserve(4);

  for (const size_t muid : muTrackIdx) {
    reco::TrackRef recoMu(muTrackHandle_, muid);
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
      LogError("trigEffiForMuTrack")<<"Could not find matched full trigger path with -> "<<p<<endl;
      return;
    }
    const std::string trigPath_ = matchedPaths[0];
    if (hltConfig_.triggerIndex(trigPath_) >= hltConfig_.size()) {
      LogError("trigEffiForMuTrack")<<"Cannot find trigger path -> "<<trigPath_<<endl;
      return;
    }
    fired_[p] = trigResultsHandle_->accept(hltConfig_.triggerIndex(trigPath_));
  }
  

  muTrackT_->Fill();

  return;
}

void
trigEffiForMuTrack::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void
trigEffiForMuTrack::endJob() {}
