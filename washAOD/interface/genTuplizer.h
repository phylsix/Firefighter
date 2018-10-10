#ifndef washAOD_genTuplizer_H
#define washAOD_genTuplizer_H

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include "TTree.h"

class genTuplizer : 
  public edm::one::EDAnalyzer<edm::one::SharedResources>
{
  public:
    explicit genTuplizer(const edm::ParameterSet&);
    ~genTuplizer();

    static void fillDescriptions(edm::ConfigurationDescriptions&);

  private:
    virtual void beginJob() override;
    virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
    virtual void endJob() override;

    const edm::InputTag genParticleTag_;
    const edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;

    edm::Service<TFileService> fs;
    edm::Handle<reco::GenParticleCollection> genParticleHandle_;

    unsigned int nGenP_;

    std::vector<int> pid_;
    std::vector<int> charge_;
    std::vector<float> pt_, pz_;
    std::vector<float> eta_, phi_;
    std::vector<float> vxy_, vz_;
    std::vector<float> mass_, energy_;

    std::vector<float> pairInvM_;
    std::vector<float> pairDphi_;
    std::vector<float> pairDeltaR_;
    std::vector<int>   pairPid_;

    TTree *genT_;
};

#endif
