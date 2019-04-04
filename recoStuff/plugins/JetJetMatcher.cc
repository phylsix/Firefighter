#include "CommonTools/UtilAlgos/interface/MatchByDR.h"
#include "CommonTools/UtilAlgos/interface/MatchByDRDPt.h"
#include "CommonTools/UtilAlgos/interface/PhysObjectMatcher.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "Firefighter/recoStuff/interface/JetJetMatchSelector.h"

using JetPFJetMatcherDRLessByR = reco::PhysObjectMatcher<
    reco::JetView,
    reco::PFJetCollection,
    JetJetMatchSelector<reco::JetView::value_type, reco::PFJetCollection::value_type>>;

using JetPFJetMatcherDRDPtLessByR = reco::PhysObjectMatcher<
    reco::JetView,
    reco::PFJetCollection,
    JetJetMatchSelector<reco::JetView::value_type, reco::PFJetCollection::value_type>,
    reco::MatchByDRDPt<reco::JetView::value_type, reco::PFJetCollection::value_type>>;

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( JetPFJetMatcherDRLessByR );
DEFINE_FWK_MODULE( JetPFJetMatcherDRDPtLessByR );