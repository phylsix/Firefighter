#include "Firefighter/recoStuff/src/PFCandidateConverter.h"
#include "DataFormats/Common/interface/View.h"
#include "FWCore/Framework/interface/MakerMacros.h"

using TrackToPFCandidateProd =
    CandidateProducer<edm::View<reco::Track>,
                      reco::PFCandidateCollection,
                      AnySelector,
                      ff::TrackToPFCandidate,
                      ff::TrackToPFCandConcreteCreator>;

using MuonToPFCandidateProd =
    CandidateProducer<edm::View<reco::Muon>,
                      reco::PFCandidateCollection,
                      AnySelector,
                      ff::MuonToPFCandidate,
                      ff::MuonToPFCandConcreteCreator>;

DEFINE_FWK_MODULE( TrackToPFCandidateProd );
DEFINE_FWK_MODULE( MuonToPFCandidateProd );
