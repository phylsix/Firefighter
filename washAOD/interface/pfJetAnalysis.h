#ifndef washAOD_pfJetAnalysis_H
#define washAOD_pfJetAnalysis_H

/**
 * analyze jet etc.
 * ================
 */

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"

#include "TTree.h"

class pfJetAnalysis :
  public edm::one::EDAnalyzer<edm::one::SharedResources>
{
  public:
    explicit pfJetAnalysis(const edm::ParameterSet&);
    ~pfJetAnalysis();

    static void fillDescriptions(edm::ConfigurationDescriptions&);
  
  private:
    virtual void beginJob() override;
    virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
    virtual void endJob() override;

    const edm::InputTag jetTag_;
    const edm::EDGetTokenT<reco::PFJetCollection> jetToken_;
    const bool assignTypeAnyDsaMu_;

    edm::Service<TFileService> fs;
    edm::Handle<reco::PFJetCollection> jetHandle_;
    
    edm::EDGetTokenT<reco::TrackCollection> generalTrackToken_, dSAMuToken_;
    edm::Handle<reco::TrackCollection> generalTrackHandle_, dSAMuHandle_;

    unsigned int nJet_;

    std::vector<float> jetEnergy_;
    std::vector<float> jetMass_;
    std::vector<float> jetPt_;
    std::vector<float> jetPz_;
    std::vector<float> jetEta_;
    std::vector<float> jetPhi_;
    std::vector<int> jetChargedMultiplicity_;
    std::vector<int> jetNConstituents_;
    std::vector<int> jetNTracks_;
    std::vector<int> jetSeedType_;

    TTree *jetT_;
};

#endif