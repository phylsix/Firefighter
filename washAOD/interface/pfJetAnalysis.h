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
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "TTree.h"

class pfJetAnalysis :
  public edm::one::EDAnalyzer<edm::one::WatchRuns, edm::one::SharedResources>
{
  public:
    explicit pfJetAnalysis(const edm::ParameterSet&);
    ~pfJetAnalysis();

    static void fillDescriptions(edm::ConfigurationDescriptions&);
  
  private:
    void beginJob() override;
    void beginRun(edm::Run const&, edm::EventSetup const&) override;
    void analyze(const edm::Event&, const edm::EventSetup&) override;
    void endRun(edm::Run const&, edm::EventSetup const&) override;
    void endJob() override;

    const edm::InputTag jetTag_;
    const edm::EDGetTokenT<reco::PFJetCollection> jetToken_;
    const bool assignTypeAnyDsaMu_;
    const edm::InputTag dSAMuTag_;
    const edm::EDGetTokenT<reco::TrackCollection> dSAMuToken_;
    const edm::ParameterSet kvfParam_;

    const edm::InputTag trigResultsTag_;
    const edm::InputTag trigEventTag_;
    const std::string trigPathNoVer_;
    const std::string processName_;
    const edm::EDGetTokenT<edm::TriggerResults> trigResultsToken_;
    const edm::EDGetTokenT<trigger::TriggerEvent> trigEventToken_;
    const edm::InputTag pvsTag_;
    const edm::EDGetTokenT<reco::VertexCollection> pvsToken_;

    edm::Service<TFileService> fs;
    edm::Handle<reco::PFJetCollection> jetHandle_;
    edm::Handle<reco::TrackCollection> dSAMuHandle_;
    edm::Handle<edm::TriggerResults> trigResultsHandle_;
    edm::Handle<trigger::TriggerEvent> trigEventHandle_;
    edm::Handle<reco::VertexCollection> pvsHandle_;
    
    std::string trigPath_;
    HLTConfigProvider hltConfig_;

    edm::EDGetTokenT<reco::TrackCollection> generalTrackToken_;
    edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;

    edm::Handle<reco::TrackCollection> generalTrackHandle_;
    edm::Handle<reco::GenParticleCollection> genParticleHandle_;

    bool triggered_;
    unsigned int nJet_;

    std::vector<float> jetEnergy_;
    std::vector<float> jetMass_;
    std::vector<float> jetChargedMass_;
    std::vector<float> jetPt_;
    std::vector<float> jetPz_;
    std::vector<float> jetEta_;
    std::vector<float> jetPhi_;
    std::vector<float> jetMatchDist_;
    std::vector<float> jetChargedEmEnergyFrac_;
    std::vector<float> jetChargedHadEnergyFrac_;
    std::vector<float> jetNeutralEmEnergyFrac_;
    std::vector<float> jetNeutralHadEnergyFrac_;
    std::vector<std::vector<float>> jetTrackPt_;
    std::vector<std::vector<float>> jetTrackEta_;
    std::vector<std::vector<float>> jetTrackD0Sig_;
    std::vector<std::vector<float>> jetTrackDzSig_;   
    std::vector<std::vector<float>> jetTrackNormChi2_;
    std::vector<std::vector<bool>>  jetTrackIsDsa_;
    std::vector<float> jetVtxLxy_;
    std::vector<float> jetVtxL3D_;
    std::vector<float> jetVtxLxySig_;
    std::vector<float> jetVtxL3DSig_;
    std::vector<float> jetVtxMatchDist_;
    std::vector<float> jetVtxMatchDistT_;
    std::vector<float> jetVtxNormChi2_;
    std::vector<float> jetVtxMass_;
    std::vector<int> jetChargedMultiplicity_;
    std::vector<int> jetMuonMultiplicity_;
    std::vector<int> jetNConstituents_;
    std::vector<int> jetNTracks_;
    std::vector<int> jetSeedType_;
    std::vector<bool> jetMatched_;

    std::vector<float> genDarkphotonEnergy_;
    std::vector<float> genDarkphotonPt_;
    std::vector<float> genDarkphotonPz_;
    std::vector<float> genDarkphotonEta_;
    std::vector<float> genDarkphotonPhi_;
    std::vector<float> genDarkphotonLxy_;
    std::vector<float> genDarkphotonL3D_;

    unsigned int ndSA_;
    
    std::vector<float> genMuonPt_;
    std::vector<float> genMuonPz_;
    std::vector<float> genMuonEta_;
    std::vector<float> genMuonPhi_;

    std::vector<float> dSAPt_;
    std::vector<float> dSAPz_;
    std::vector<float> dSAEta_;
    std::vector<float> dSAPhi_;
    std::vector<float> dSAd0Sig_;
    std::vector<float> dSAMatchDist_;
    std::vector<int>   dSATrackQual_;

    TTree *jetT_;
    TTree *dSAT_;
};

#endif