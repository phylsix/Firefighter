#include "Firefighter/washAOD/interface/dsaMatching.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Math/interface/deltaR.h"

#include <cmath>

dsaMatching::dsaMatching(const edm::ParameterSet& ps)
{
  using namespace std;
  using namespace edm;

  usesResource("TFileService");

  recoMuToken_ = consumes<reco::MuonCollection>(InputTag("muons"));
  dsaMuToken_  = consumes<reco::TrackCollection>(InputTag("displacedStandAloneMuons"));
  genParticleToken_ = consumes<reco::GenParticleCollection>(InputTag("genParticles"));
}

dsaMatching::~dsaMatching() = default;

void
dsaMatching::fillDescriptions(edm::ConfigurationDescriptions& ds)
{
  edm::ParameterSetDescription desc;
  ds.add("dsaMatching", desc);  
}

void
dsaMatching::beginJob()
{
  muT_ = fs->make<TTree>("mu", "");

  muT_->Branch("nGenMuon", &nGenMuon_, "nGenMuon/i");
  muT_->Branch("nRecoMuon", &nRecoMuon_, "nRecoMuon/i");
  muT_->Branch("nDsaMuon", &nDsaMuon_, "nDsaMuon/i");

  muT_->Branch("genMuPt",  &genMuPt_);
  muT_->Branch("genMuEta", &genMuEta_);
  muT_->Branch("genMuPhi", &genMuPhi_);
  muT_->Branch("genMuLxy", &genMuLxy_);
  muT_->Branch("genMuLz",  &genMuLz_);
  muT_->Branch("recoMuType",          &recoMuType_);
  muT_->Branch("dR_genRecoMu",        &dR_genRecoMu_);
  muT_->Branch("dPtOverPt_genRecoMu", &dPtOverPt_genRecoMu_);
  muT_->Branch("dR_genDsaMu",         &dR_genDsaMu_);
  muT_->Branch("dPtOverPt_genDsaMu",  &dPtOverPt_genDsaMu_);
  muT_->Branch("dR_recoDsaMu",        &dR_recoDsaMu_);
  muT_->Branch("dPtOverPt_recoDsaMu", &dPtOverPt_recoDsaMu_);
  muT_->Branch("dR_reItDsaMu",        &dR_reItDsaMu_);
  muT_->Branch("dPtOverPt_reItDsaMu", &dPtOverPt_reItDsaMu_);
  muT_->Branch("dR_reOtDsaMu",        &dR_reOtDsaMu_);
  muT_->Branch("dPtOverPt_reOtDsaMu", &dPtOverPt_reOtDsaMu_);
}

void
dsaMatching::analyze(const edm::Event& iEvent,
                     const edm::EventSetup& iSetup)
{
  using namespace std;
  using namespace edm;

  //***********************************************

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

  vector<reco::GenParticleRef> genMuons{};
  for (size_t i(0); i!=genParticleHandle_->size(); ++i)
  {
    const reco::GenParticle& genp = (*genParticleHandle_)[i];
    if ( !(abs(genp.pdgId())==13 and genp.isHardProcess()) ) { continue; }
    if ( fabs(genp.eta())>2.4 ) { continue; }
    genMuons.emplace_back(genParticleHandle_, i);
  }
  nGenMuon_ = genMuons.size();
  if ( nGenMuon_==0 ) { return; }

  //***********************************************

  iEvent.getByToken(recoMuToken_, recoMuHandle_);
  assert(recoMuHandle_.isValid());

  nRecoMuon_ = recoMuHandle_->size();
  map<reco::GenParticleRef, reco::MuonRef> genRecoMuonMap{};
  for (const auto& gmu : genMuons)
  {
    int genRecoMuon_bestIdx(-1);
    float genRecoMuon_mindR(999.);
    for (size_t rid(0); rid!=recoMuHandle_->size(); ++rid)
    {
      auto _found = find_if(genRecoMuonMap.begin(), genRecoMuonMap.end(),
                            [rid](const auto& gr_mu){ return gr_mu.second.key()==rid; });
      if ( _found!=genRecoMuonMap.end() ) { continue; }
      float _dR = deltaR( *(gmu.get()),(*recoMuHandle_)[rid] );
      if ( _dR >= genRecoMuon_mindR ) { continue; }
      genRecoMuon_bestIdx = rid;
      genRecoMuon_mindR = _dR;
    }
    
    if ( genRecoMuon_bestIdx==-1 or
         genRecoMuon_bestIdx>=static_cast<int>(recoMuHandle_->size()) )
    { continue; }
    if ( genRecoMuon_mindR>0.3 ) { continue; }
    genRecoMuonMap.emplace(gmu, reco::MuonRef(recoMuHandle_, genRecoMuon_bestIdx));
  }

  //***********************************************

  iEvent.getByToken(dsaMuToken_, dsaMuHandle_);
  assert(dsaMuHandle_.isValid());

  nDsaMuon_ = dsaMuHandle_->size();
  map<reco::GenParticleRef, reco::TrackRef> genDsaMuonMap{};
  for (const auto& gmu : genMuons)
  {
    int genDsaMuon_bestIdx(-1);
    float genDsaMuon_mindR(999.);
    for (size_t rid(0); rid!=dsaMuHandle_->size(); ++rid)
    {
      auto _found = find_if(genDsaMuonMap.begin(), genDsaMuonMap.end(),
                            [rid](const auto& gr_mu){ return gr_mu.second.key()==rid; });
      if ( _found!=genDsaMuonMap.end() ) { continue; }
      float _dR = deltaR( *(gmu.get()),(*dsaMuHandle_)[rid] );
      if ( _dR >= genDsaMuon_mindR ) { continue; }
      genDsaMuon_bestIdx = rid;
      genDsaMuon_mindR = _dR;
    }
    
    if ( genDsaMuon_bestIdx==-1 or
         genDsaMuon_bestIdx>=static_cast<int>(dsaMuHandle_->size()) )
    { continue; }
    if ( genDsaMuon_mindR>0.3 ) { continue; }
    genDsaMuonMap.emplace(gmu, reco::TrackRef(dsaMuHandle_, genDsaMuon_bestIdx));
  }

  //***********************************************

  genMuPt_.clear();
  genMuPt_.reserve(nGenMuon_);
  genMuEta_.clear();
  genMuEta_.reserve(nGenMuon_);
  genMuPhi_.clear();
  genMuPhi_.reserve(nGenMuon_);
  genMuLxy_.clear();
  genMuLxy_.reserve(nGenMuon_);
  genMuLz_.clear();
  genMuLz_.reserve(nGenMuon_);

  recoMuType_.clear();
  recoMuType_.reserve(nGenMuon_);
  dR_genRecoMu_.clear();
  dR_genRecoMu_.reserve(nGenMuon_);
  dPtOverPt_genRecoMu_.clear();
  dPtOverPt_genRecoMu_.reserve(nGenMuon_);

  dR_genDsaMu_.clear();
  dR_genDsaMu_.reserve(nGenMuon_);
  dPtOverPt_genDsaMu_.clear();
  dPtOverPt_genDsaMu_.reserve(nGenMuon_);

  dR_recoDsaMu_.clear();
  dR_recoDsaMu_.reserve(nGenMuon_);
  dPtOverPt_recoDsaMu_.clear();
  dPtOverPt_recoDsaMu_.reserve(nGenMuon_);
  dR_reItDsaMu_.clear();
  dR_reItDsaMu_.reserve(nGenMuon_);
  dPtOverPt_reItDsaMu_.clear();
  dPtOverPt_reItDsaMu_.reserve(nGenMuon_);
  dR_reOtDsaMu_.clear();
  dR_reOtDsaMu_.reserve(nGenMuon_);
  dPtOverPt_reOtDsaMu_.clear();
  dPtOverPt_reOtDsaMu_.reserve(nGenMuon_);

  for (const auto& gm : genMuons)
  {
    genMuPt_.emplace_back(gm->pt());
    genMuEta_.emplace_back(gm->eta());
    genMuPhi_.emplace_back(gm->phi());
    genMuLxy_.emplace_back(gm->vertex().rho());
    genMuLz_.emplace_back(gm->vertex().z());

    if ( genRecoMuonMap.count(gm)>0 )
    {
      const auto& rm = genRecoMuonMap[gm];
      recoMuType_.emplace_back(rm->type());
      dR_genRecoMu_.emplace_back( deltaR(*(gm.get()), *(rm.get())) );
      dPtOverPt_genRecoMu_.emplace_back( fabs(rm->pt()-gm->pt())/gm->pt() );
    } else
    {
      recoMuType_.emplace_back( NAN );
      dR_genRecoMu_.emplace_back( NAN );
      dPtOverPt_genRecoMu_.emplace_back( NAN );
    }

    if ( genDsaMuonMap.count(gm)>0 )
    {
      const auto& dsa = genDsaMuonMap[gm];
      dR_genDsaMu_.emplace_back( deltaR(*(gm.get()), *(dsa.get())) );
      dPtOverPt_genDsaMu_.emplace_back( fabs(dsa->pt()-gm->pt())/gm->pt() );
    } else
    {
      dR_genDsaMu_.emplace_back( NAN );
      dPtOverPt_genDsaMu_.emplace_back( NAN );
    }

    if ( genRecoMuonMap.count(gm)>0 and genDsaMuonMap.count(gm)>0 )
    {
      const auto& rm = genRecoMuonMap[gm];
      const auto& dsa = genDsaMuonMap[gm];

      dR_recoDsaMu_.emplace_back( deltaR(*(rm.get()), *(dsa.get())) );
      dPtOverPt_recoDsaMu_.emplace_back( fabs(dsa->pt()-rm->pt())/rm->pt() );

      const auto& rmIt = rm->innerTrack();
      if (rmIt.isNonnull())
      {
        dR_reItDsaMu_.emplace_back( deltaR(*(rmIt.get()), *(dsa.get())) );
        dPtOverPt_reItDsaMu_.emplace_back( fabs(rmIt->pt()-dsa->pt())/rmIt->pt() );
      } else
      {
        dR_reItDsaMu_.emplace_back( NAN );
        dPtOverPt_reItDsaMu_.emplace_back( NAN );
      }

      const auto& rmOt = rm->outerTrack();
      if (rmOt.isNonnull())
      {
        dR_reOtDsaMu_.emplace_back( deltaR(*(rmOt.get()), *(dsa.get())) );
        dPtOverPt_reOtDsaMu_.emplace_back( fabs(rmOt->pt()-dsa->pt())/rmOt->pt() );
      } else
      {
        dR_reOtDsaMu_.emplace_back( NAN );
        dPtOverPt_reOtDsaMu_.emplace_back( NAN );
      }
    } else
    {
      dR_recoDsaMu_.emplace_back(NAN);
      dPtOverPt_recoDsaMu_.emplace_back(NAN);
      dR_reItDsaMu_.emplace_back(NAN);
      dPtOverPt_reItDsaMu_.emplace_back(NAN);
      dR_reOtDsaMu_.emplace_back(NAN);
      dPtOverPt_reOtDsaMu_.emplace_back(NAN);
    }

  }

  muT_->Fill();

  return;

}

void
dsaMatching::endJob() {}