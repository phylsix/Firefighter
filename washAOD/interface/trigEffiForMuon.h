#ifndef washAOD_trigEffiForMuon_H
#define washAOD_trigEffiForMuon_H

/**
 * Trigger efficiencies in terms of events
 * =======================================
 * Require:
 *   - 4 gen muons in |eta|<2.4
 *   - 4 tracker muons matched with gen muons (dR<0.3)
 *   - >=3 tracker muons passing loose quality cut
 * Check trigger firing condition
 */

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include <map>
#include "TTree.h"

class trigEffiForMuon :
  public edm::one::EDAnalyzer<edm::one::WatchRuns, edm::one::SharedResources>
{
  public:
    explicit trigEffiForMuon(const edm::ParameterSet&);
    ~trigEffiForMuon();

    static void fillDescriptions(edm::ConfigurationDescriptions&);

  private:
    virtual void beginJob() override;
    virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
    virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
    virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
    virtual void endJob() override;

    const edm::InputTag muonTag_;
    const edm::InputTag genParticleTag_;
    const edm::InputTag trigResultsTag_;
    const edm::InputTag trigEventTag_;
    const std::vector<std::string> trigPathNoVer_;
    const std::string processName_;
    const int nMuons_;
    const edm::EDGetTokenT<reco::MuonCollection> muonToken_;
    const edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;
    const edm::EDGetTokenT<edm::TriggerResults> trigResultsToken_;
    const edm::EDGetTokenT<trigger::TriggerEvent> trigEventToken_;

    edm::Service<TFileService> fs;
    edm::Handle<reco::MuonCollection> muonHandle_;
    edm::Handle<reco::GenParticleCollection> genParticleHandle_;
    edm::Handle<edm::TriggerResults> trigResultsHandle_;
    edm::Handle<trigger::TriggerEvent> trigEventHandle_;

    HLTConfigProvider hltConfig_;

    std::map<std::string, bool> fired_;
    std::vector<float> pt_;
    std::vector<float> eta_;
    std::vector<float> phi_;
    std::vector<float> dR_;
    std::vector<float> vxy_;
    std::vector<float> vz_;

    TTree *muonT_;
};

#endif
