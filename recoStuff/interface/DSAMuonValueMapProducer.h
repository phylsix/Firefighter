#ifndef recoStuff_DSAMuonValueMapProducer_H
#define recoStuff_DSAMuonValueMapProducer_H

/**
 * \class DSAMuonValueMapProducer
 * \description
 * producer to make displacedStandAloneMuon matching values
 * valueMap wrt. trackerMuon or globalMuon
 */

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/MuonReco/interface/MuonSegmentMatch.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "Firefighter/recoStuff/interface/TrackExtrapolator.h"


class DSAMuonValueMapProducer : public edm::stream::EDProducer<> {
 public:
  explicit DSAMuonValueMapProducer( const edm::ParameterSet& );
  ~DSAMuonValueMapProducer() override;

 private:
  void beginRun(const edm::Run&, const edm::EventSetup&) override;
  void produce( edm::Event&, edm::EventSetup const& ) override;

  /* count all segments keys */
  std::vector<int> getCSCSegmentKeys(const reco::Muon&) const;
  std::vector<int> getDTSegmentKeys(const reco::Muon&) const;
  float getSegmentOverlapRatio( const reco::Muon&, const reco::Muon& ) const;

  /* with arbitrationMask */
  // https://cmssdt.cern.ch/dxr/CMSSW/source/DataFormats/MuonReco/interface/MuonSegmentMatch.h#15-31
  int numberOfSegments(const reco::Muon&, unsigned int segmentArbitraionMask=reco::MuonSegmentMatch::BestInChamberByDR) const;
  float getSegmentOverlapRatioArbitration(const reco::Muon&, const reco::Muon&,
                                          unsigned int segmentArbitraionMask=reco::MuonSegmentMatch::BestInChamberByDR) const;

  std::unique_ptr<ff::TrackExtrapolator> fTkExtrapolator;
  std::pair<float, float> getExtrapolateInnermostDistance( const reco::Muon&, const reco::Muon& ) const;

  const edm::EDGetTokenT<reco::MuonCollection> fDsaMuonToken;
  const edm::EDGetTokenT<reco::MuonCollection> fRecoMuonToken;

  edm::Handle<reco::MuonCollection> fDsaMuonHdl;
  edm::Handle<reco::MuonCollection> fRecoMuonHdl;
};

#endif