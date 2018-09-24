#include "Firefighter/recoStuff/interface/ForkCandAgainstDsaMuon.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Math/interface/deltaR.h"


ForkCandAgainstDsaMuon::ForkCandAgainstDsaMuon(const edm::ParameterSet& iC) :
    targetColToken_(consumes<reco::PFCandidateCollection>(iC.getParameter<edm::InputTag>("targetcol"))),
    toForkColToken_(consumes<reco::PFCandidateCollection>(iC.getParameter<edm::InputTag>("toforkcol"))),
    maxDR_(iC.getParameter<double>("maxDR")),
    maxPtReso_(iC.getParameter<double>("maxPtReso"))
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

    auto matchCol    = make_unique<reco::PFCandidateCollection>();
    auto nonMatchCol = make_unique<reco::PFCandidateCollection>();

    iEvent.getByToken(targetColToken_, targetColHdl_);
    iEvent.getByToken(toForkColToken_, toForkColHdl_);

    assert(targetColHdl_.isValid() && toForkColHdl_.isValid());

    for (const auto& o : *toForkColHdl_) {
        bool isMatched = false;
        for (const auto& t : *targetColHdl_) {
            if (o.particleId() == reco::PFCandidate::ParticleType::mu &&
                deltaR(o, t) < maxDR_ &&
                fabs(o.pt()-t.pt())/t.pt() < maxPtReso_ &&
                o.charge() == t.charge()) {
                isMatched = true;
                break;
            } 
        }
        if (isMatched) {matchCol->push_back(o);}
        else {nonMatchCol->push_back(o);}
    }
    
    iEvent.put(move(matchCol),    "matched");
    iEvent.put(move(nonMatchCol), "nonMatched");

}

void
ForkCandAgainstDsaMuon::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("targetcol", edm::InputTag("particleFlow"));
    desc.add<edm::InputTag>("toforkcol", edm::InputTag("dSAmuPFCand"));
    desc.add<double>("maxDR", 0.1);
    desc.add<double>("maxPtReso", 0.5);
    descriptions.add("ForkCandAgainstDsaMuon", desc);
}