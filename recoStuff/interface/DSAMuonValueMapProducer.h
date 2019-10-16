#ifndef recoStuff_DSAMuonValueMapProducer_H
#define recoStuff_DSAMuonValueMapProducer_H

/**
 * \class DSAMuonValueMapProducer
 * \description
 * producer to make displacedStandAloneMuon matching values
 * valueMap wrt. identified (cutbased POG ID) PFMuons
 */

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DataFormats/MuonReco/interface/MuonSegmentMatch.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "Firefighter/recoStuff/interface/TrackExtrapolator.h"

class DSAMuonValueMapProducer : public edm::stream::EDProducer<> {
 public:
  explicit DSAMuonValueMapProducer( const edm::ParameterSet& );
  ~DSAMuonValueMapProducer() override;

 private:
  void beginRun( const edm::Run&, const edm::EventSetup& ) override;
  void produce( edm::Event&, edm::EventSetup const& ) override;

  /* with arbitrationMask */
  // https://cmssdt.cern.ch/dxr/CMSSW/source/DataFormats/MuonReco/interface/MuonSegmentMatch.h#15-31
  int   numberOfSegments( const reco::Muon&, unsigned int segmentArbitraionMask = reco::MuonSegmentMatch::BestInChamberByDR ) const;
  float getSegmentOverlapRatioArbitration( const reco::Muon&, const reco::Muon&, unsigned int segmentArbitraionMask = reco::MuonSegmentMatch::BestInChamberByDR ) const;
  float maxSegmentOverlapRatio( const reco::Muon& ) const;

  std::unique_ptr<ff::TrackExtrapolator> fTkExtrapolator;
  float                                  minDeltaRAtInnermostPoint( const reco::Muon& ) const;

  std::vector<DTChamberId> getDTDetIds( const reco::Muon& ) const;
  std::vector<DTChamberId> getDTDetIds( const reco::Track& ) const;

  std::vector<CSCDetId> getCSCDetIds( const reco::Muon& ) const;
  std::vector<CSCDetId> getCSCDetIds( const reco::Track& ) const;

  bool detIdsIsSubSetOfAnyPFMuon( const reco::Track&,
                                  const std::vector<std::vector<DTChamberId>>&,
                                  const std::vector<std::vector<CSCDetId>>& ) const;

  float getMuonIsolationValue( const reco::Muon& ) const;

  reco::MuonRef findOppositeMuon( const reco::MuonRef& ) const;

  float timingDiffDT(const reco::MuonRef&, const reco::MuonRef&) const;
  float timingDiffRPC(const reco::MuonRef&, const reco::MuonRef&) const;

  const edm::EDGetTokenT<reco::MuonCollection>          fDsaMuonToken;
  const edm::EDGetTokenT<reco::PFCandidateFwdPtrVector> fPFMuonToken;

  edm::Handle<reco::MuonCollection>          fDsaMuonHdl;
  edm::Handle<reco::PFCandidateFwdPtrVector> fPFMuonHdl;
};

#endif