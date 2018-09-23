#ifndef washAOD_dsaMatching_H
#define washAOD_dsaMatching_H

/**
 * study how displacedStandAlone muons matched with
 * inner muons, determine criteria when to include
 * dSA muons into PFCandidates
 */

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"

#include "TTree.h"


class dsaMatching :
  public edm::one::EDAnalyzer<edm::one::SharedResources>
{
  public:
    explicit dsaMatching(const edm::ParameterSet&);
    ~dsaMatching();

    static void fillDescriptions(edm::ConfigurationDescriptions&);

  private:
    void beginJob() override;
    void analyze(const edm::Event&, const edm::EventSetup&) override;
    void endJob() override;

    edm::Service<TFileService> fs;

    edm::EDGetTokenT<reco::MuonCollection> recoMuToken_;
    edm::EDGetTokenT<reco::TrackCollection> dsaMuToken_;
    edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;

    edm::Handle<reco::MuonCollection> recoMuHandle_;
    edm::Handle<reco::TrackCollection> dsaMuHandle_;
    edm::Handle<reco::GenParticleCollection> genParticleHandle_;

    unsigned int nGenMuon_;
    unsigned int nRecoMuon_;
    unsigned int nDsaMuon_;

    std::vector<float> genMuPt_;
    std::vector<float> genMuEta_;
    std::vector<float> genMuPhi_;
    std::vector<float> genMuLxy_;
    std::vector<float> genMuLz_;

    std::vector<unsigned int> recoMuType_;
    std::vector<float> dR_genRecoMu_;
    std::vector<float> dPtOverPt_genRecoMu_;
    std::vector<float> dR_genDsaMu_;
    std::vector<float> dPtOverPt_genDsaMu_;
    std::vector<float> dR_recoDsaMu_;
    std::vector<float> dPtOverPt_recoDsaMu_;
    std::vector<float> dR_reItDsaMu_;
    std::vector<float> dPtOverPt_reItDsaMu_;
    std::vector<float> dR_reOtDsaMu_;
    std::vector<float> dPtOverPt_reOtDsaMu_;

    TTree *muT_;
};

#endif