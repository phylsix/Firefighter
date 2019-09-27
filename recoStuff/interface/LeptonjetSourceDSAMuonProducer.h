#ifndef recoStuff_LeptonjetSourceDSAMuonProducer_H
#define recoStuff_LeptonjetSourceDSAMuonProducer_H

/**
 * \class LeptonjetSourceDSAMuonProducer
 * \description
 * producer to filter out DSAMuon satisfying DSA ID & loose Iso,
 * keep the non-isolated ones as leptonjet source for clustering.
 */

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSegmentMatch.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "Firefighter/recoStuff/interface/TrackExtrapolator.h"

class LeptonjetSourceDSAMuonProducer : public edm::stream::EDProducer<> {
 public:
  explicit LeptonjetSourceDSAMuonProducer( const edm::ParameterSet& );
  ~LeptonjetSourceDSAMuonProducer() override;

 private:
  void beginRun( const edm::Run&, const edm::EventSetup& ) override;
  void produce( edm::Event&, edm::EventSetup const& ) override;

  float maxSegmentOverlapRatio( const reco::Muon& ) const;
  float minDeltaRAtInnermostPoint( const reco::Muon& ) const;

  // https://cmssdt.cern.ch/dxr/CMSSW/source/DataFormats/MuonReco/interface/MuonSegmentMatch.h#15-31
  int   numberOfSegments( const reco::Muon&, unsigned int segmentArbitraionMask = reco::MuonSegmentMatch::BestInChamberByDR ) const;
  float getSegmentOverlapRatioArbitration( const reco::Muon&, const reco::Muon&, unsigned int segmentArbitraionMask = reco::MuonSegmentMatch::BestInChamberByDR ) const;

  const edm::EDGetTokenT<reco::PFCandidateFwdPtrVector> fDSACandsToken;
  const edm::EDGetTokenT<reco::PFCandidateFwdPtrVector> fPFMuonsToken;
  const edm::EDGetTokenT<reco::TrackCollection>         fDSATkToken;
  edm::Handle<reco::PFCandidateFwdPtrVector>            fDSACandsHdl;
  edm::Handle<reco::PFCandidateFwdPtrVector>            fPFMuonsHdl;
  edm::Handle<reco::TrackCollection>                    fDSATkHdl;

  std::unique_ptr<ff::TrackExtrapolator> fTkExtrapolator;
};

#endif