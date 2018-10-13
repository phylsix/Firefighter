#include "Firefighter/washAOD/interface/genTuplizer.h"
#include "Firefighter/recoStuff/interface/RecoHelpers.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Math/interface/deltaR.h"


genTuplizer::genTuplizer(const edm::ParameterSet& ps) :
  genParticleTag_(ps.getParameter<edm::InputTag>("genParticle")),
  genParticleToken_(consumes<reco::GenParticleCollection>(genParticleTag_))
{
  usesResource("TFileService");
}

genTuplizer::~genTuplizer() = default;

void
genTuplizer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("genParticle", edm::InputTag("genParticles"));
    descriptions.add("genTuplizer", desc);
}

void
genTuplizer::beginJob()
{
  genT_ = fs->make<TTree>("gen", "");

  genT_->Branch("nGen", &nGenP_, "nGen/i");
  genT_->Branch("pid", &pid_);
  genT_->Branch("charge", &charge_);
  genT_->Branch("pt", &pt_);
  genT_->Branch("pz", &pz_);
  genT_->Branch("eta", &eta_);
  genT_->Branch("phi", &phi_);
  genT_->Branch("mass", &mass_);
  genT_->Branch("energy", &energy_);
  genT_->Branch("vxy", &vxy_);
  genT_->Branch("vz", &vz_);
  genT_->Branch("pairInvM", &pairInvM_);
  genT_->Branch("pairDphi", &pairDphi_);
  genT_->Branch("pairDeltaR", &pairDeltaR_);
  genT_->Branch("pairPid", &pairPid_);
}

void
genTuplizer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace std;
  using namespace edm;

  iEvent.getByToken(genParticleToken_, genParticleHandle_);
  if (!genParticleHandle_.isValid()) {
    LogVerbatim("genTuplizer") << "genTuplizer::analyze: Error in getting genParticle product from Event!"
      << endl;
      return;
  }

  // skip if sensible lepton less than 4;
  int nAccept = count_if((*genParticleHandle_).begin(), (*genParticleHandle_).end(), ff::genAccept);
  if (nAccept<4) { return; }

  nGenP_ = count_if((*genParticleHandle_).begin(), (*genParticleHandle_).end(),
      [](const reco::GenParticle& g){return g.isHardProcess() and abs(g.pdgId())>8;});

  pid_.clear(); pid_.reserve(nGenP_);
  charge_.clear(); charge_.reserve(nGenP_);
  pt_.clear(); pt_.reserve(nGenP_);
  pz_.clear(); pz_.reserve(nGenP_);
  eta_.clear(); eta_.reserve(nGenP_);
  phi_.clear(); phi_.reserve(nGenP_);
  mass_.clear(); mass_.reserve(nGenP_);
  energy_.clear(); energy_.reserve(nGenP_);
  vxy_.clear(); vxy_.reserve(nGenP_);
  vz_.clear(); vz_.reserve(nGenP_);

  vector<pair<reco::GenParticleRef, reco::GenParticleRef>> genPairs{};
  for (size_t ig(0); ig!=genParticleHandle_->size(); ++ig) {
    reco::GenParticleRef genA(genParticleHandle_, ig);
    if (!genA->isHardProcess() or abs(genA->pdgId())<9) continue;
    for (size_t jg(ig+1); jg!=genParticleHandle_->size(); ++jg) {
      reco::GenParticleRef genB(genParticleHandle_, jg);
      if (!genB->isHardProcess() or abs(genB->pdgId())<9) continue;
      // conditions to be counted as a pair:
      // 1. opposite or same pdgId/charge
      // 2. same vertex.
      if (abs(genA->pdgId()) != abs(genB->pdgId())) {continue;}
      if (genA->vertex()!=genB->vertex()) {continue;}
      genPairs.emplace_back(genA, genB);
    }
  }
  pairInvM_.clear(); pairInvM_.reserve(genPairs.size());
  pairDeltaR_.clear(); pairDeltaR_.reserve(genPairs.size());
  pairDphi_.clear(); pairDphi_.reserve(genPairs.size());
  pairPid_.clear(); pairPid_.reserve(genPairs.size());

  for (const reco::GenParticle& gp : *genParticleHandle_) {
    if (!gp.isHardProcess() or abs(gp.pdgId())<9) continue;
    //cout<<gp.pdgId()<<":\t"<<gp.vx()<<"\t"<<gp.vy()<<"\t"<<gp.vz()<<"\t"<<gp.eta()<<"\t"<<gp.phi()<<"\t"<<gp.pt()<<endl;
    //cout<<gp.pdgId()<<":\t"<<gp.statusFlags().flags_<<endl;
    pid_.emplace_back(gp.pdgId());
    charge_.emplace_back(gp.charge());
    pt_.emplace_back(gp.pt());
    pz_.emplace_back(gp.pz());
    eta_.emplace_back(gp.eta());
    phi_.emplace_back(gp.phi());
    mass_.emplace_back(gp.mass());
    energy_.emplace_back(gp.energy());

    vxy_.emplace_back(gp.vertex().rho());
    vz_.emplace_back(gp.vz());
  }

  for (const auto& gp : genPairs) {
    pairDphi_.emplace_back( fabs(deltaPhi(gp.first->phi(), gp.second->phi())) );
    pairDeltaR_.emplace_back(deltaR(*(gp.first.get()), *(gp.second.get())));
    pairInvM_.emplace_back((gp.first->p4() + gp.second->p4()).M());
    pairPid_.emplace_back(abs(gp.first->pdgId()));
  }

  genT_->Fill();

  return;

}

void
genTuplizer::endJob() {}
