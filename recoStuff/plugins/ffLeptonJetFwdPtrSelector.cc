#include "CommonTools/UtilAlgos/interface/FwdPtrCollectionFilter.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "Firefighter/recoStuff/interface/ffLeptonJetSelector.h"

using ffLeptonJetFwdPtrSelector =
    edm::FwdPtrCollectionFilter<reco::PFJet, ffLeptonJetSelector>;

DEFINE_FWK_MODULE( ffLeptonJetFwdPtrSelector );
