#include "Firefighter/recoStuff/src/TrackToPFCandidate.h"
#include "DataFormats/Common/interface/View.h"
#include "FWCore/Framework/interface/MakerMacros.h"

using TrackToPFCandidateProd = CandidateProducer<edm::View<reco::Track>,
                                                 reco::PFCandidateCollection,
                                                 AnySelector,
                                                 ff::TrackToPFCandidate,
                                                 ff::ConcreteCreator>;

DEFINE_FWK_MODULE( TrackToPFCandidateProd );