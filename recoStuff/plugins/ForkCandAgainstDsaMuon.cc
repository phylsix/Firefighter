#include "Firefighter/recoStuff/interface/ForkCandAgainstDsaMuon.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include <algorithm>

ForkCandAgainstDsaMuon::ForkCandAgainstDsaMuon(const edm::ParameterSet& iC) :
  srcToken_(consumes<edm::View<reco::Candidate>>(iC.getParameter<edm::InputTag>("src"))),
  matchedToken_(consumes<edm::View<reco::Candidate>>(iC.getParameter<edm::InputTag>("matched"))),
  srcCut_(iC.getParameter<std::string>("srcPreselection")),
  algo_(iC)
{
  produces<reco::PFCandidateCollection>("matched");
  produces<reco::PFCandidateCollection>("nonMatched");
}

ForkCandAgainstDsaMuon::~ForkCandAgainstDsaMuon() = default;

void
ForkCandAgainstDsaMuon::produce(edm::Event& iEvent,
                                edm::EventSetup const& iSetup)
{
  using namespace std;
  using namespace edm;

  algo_.init(iSetup);

  auto matchCol    = make_unique<reco::PFCandidateCollection>();
  auto nonMatchCol = make_unique<reco::PFCandidateCollection>();

  iEvent.getByToken(srcToken_, srcHdl_);
  iEvent.getByToken(matchedToken_, matchedHdl_);

  assert(srcHdl_.isValid() && matchedHdl_.isValid());

  View<reco::Candidate>::const_iterator itsrc = srcHdl_->begin();
  View<reco::Candidate>::const_iterator edsrc = srcHdl_->end();
  int nsrc = srcHdl_->size();

  vector<int> matchIdx(nsrc, -1);
  vector<float> deltaRs(nsrc, 999.);
  vector<float> deltaEtas(nsrc, 999.);
  vector<float> deltaPhis(nsrc, 999.);
  vector<float> deltaPtRel(nsrc, 999.);
  vector<float> deltaLocalPos(nsrc, 999.);
  vector<float> chi2(nsrc, 999.);

  for (int isrc(0); itsrc!=edsrc; ++itsrc, ++isrc)
  {
    if (!srcCut_(*itsrc)) {continue;}

    const reco::PFCandidate* iPF = dynamic_cast<const reco::PFCandidate*>(&*itsrc);
    if ( iPF->particleId() != reco::PFCandidate::ParticleType::mu ) { continue; }

    matchIdx[isrc] = algo_.match(*itsrc, *matchedHdl_,
                                 deltaRs[isrc], deltaEtas[isrc], deltaPhis[isrc],
                                 deltaLocalPos[isrc], deltaPtRel[isrc], chi2[isrc]);
  }

  vector<int>::iterator it;
  it = unique(matchIdx.begin(),
              remove(matchIdx.begin(), matchIdx.end(), -1));
  matchIdx.resize(distance(matchIdx.begin(), it));
  
  for (int im(0); im!=(int)matchedHdl_->size(); ++im)
  {
    const reco::PFCandidate* iPF = dynamic_cast<const reco::PFCandidate*>(&(*matchedHdl_)[im]);
    if (find(matchIdx.begin(), matchIdx.end(), im) == matchIdx.end())
    {
      nonMatchCol->push_back(*iPF);
    }
    else
    {
      matchCol->push_back(*iPF);
    }
  }

    
  iEvent.put(move(matchCol),    "matched");
  iEvent.put(move(nonMatchCol), "nonMatched");

}

#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"
#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(ForkCandAgainstDsaMuon);