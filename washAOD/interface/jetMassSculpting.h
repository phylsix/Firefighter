#ifndef washAOD_jetMassSculpting_H
#define washAOD_jetMassSculpting_H

/**
 * see what cut lead to jet mass sculpting
 * =======================================
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

#include "TTree.h"

class jetMassSculpting :
  public edm::one::EDAnalyzer<edm::one::SharedResources>
{
  public:
    explicit jetMassSculpting(const edm::ParameterSet&);
    ~jetMassSculpting();

    static void fillDescriptions(edm::ConfigurationDescriptions&);

  private:
    void beginJob() override;
    void analyze(const edm::Event&, const edm::EventSetup&) override;
    void endJob() override;

    edm::Service<TFileService> fs;

    const edm::InputTag jetTag_;
    const edm::EDGetTokenT<reco::PFJetCollection> jetToken_;
    edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;

    edm::Handle<reco::PFJetCollection> jetHandle_;
    edm::Handle<reco::GenParticleCollection> genParticleHandle_;

    std::vector<float> jetMass_;
    
    TTree *jetT_;
};

#endif