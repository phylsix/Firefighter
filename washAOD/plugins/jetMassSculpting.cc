#include "Firefighter/washAOD/interface/jetMassSculpting.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

jetMassSculpting::jetMassSculpting(const edm::ParameterSet& ps) :
  jetTag_(ps.getParameter<edm::InputTag>("src")),
  jetToken_(consumes<reco::PFJetCollection>(jetTag_))
{
  using namespace std;
  using namespace edm;

  usesResource("TFileService");
  genParticleToken_ = consumes<reco::GenParticleCollection>(InputTag("genParticles"));

}

jetMassSculpting::~jetMassSculpting() = default;

void
jetMassSculpting::fillDescriptions(edm::ConfigurationDescriptions& ds)
{
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src", edm::InputTag("ak4PFJets"));
  ds.add("jetMassSculpting", desc);
}

void
jetMassSculpting::beginJob()
{
  jetT_ = fs->make<TTree>("Jet", "");
  jetT_->Branch("jetMass", &jetMass_);
}

void
jetMassSculpting::analyze(const edm::Event& iEvent,
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

  //********************************************************

  iEvent.getByToken(jetToken_, jetHandle_);
  assert(jetHandle_.isValid());

  jetMass_.clear();
  jetMass_.reserve(2);

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

  for (const auto& j_dp : jetDarkphotonMap)
  {
    const auto& j  = *(j_dp.first.get());
    // const auto& dp = *(j_dp.second.get());

    //***********************************

    jetMass_.emplace_back(j.mass());
  }

  jetT_->Fill();

  return;
}

void
jetMassSculpting::endJob() {}