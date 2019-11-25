#ifndef recoStuff_MuonChamberMatchProducer_H
#define recoStuff_MuonChamberMatchProducer_H

/**
 * \class MuonChamberMatchProducer
 * \description
 * producer to make valueMap of muonMatches for reco::Track (standaloneMuons)
 */

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHit.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "TrackingTools/TrackAssociator/interface/TrackDetectorAssociator.h"

class MuonChamberMatchProducer : public edm::stream::EDProducer<> {
 public:
  explicit MuonChamberMatchProducer( const edm::ParameterSet& );
  ~MuonChamberMatchProducer() override;

 private:
  void produce( edm::Event&, edm::EventSetup const& ) override;

  const edm::EDGetTokenT<reco::TrackCollection> fInputTrackToken;
  const edm::EDGetTokenT<RPCRecHitCollection>   fRPCHitToken;

  edm::Handle<reco::TrackCollection> fInputTrackHdl;
  edm::Handle<RPCRecHitCollection>   fRPCHitHdl;

  TrackDetectorAssociator   fTrackAssociator;
  TrackAssociatorParameters fTrackAssociatorParameters;

  bool fPropagateOpposite;

  // matching
  double fMaxAbsDx;
  double fMaxAbsPullX;
  double fMaxAbsDy;
  double fMaxAbsPullY;
};

#endif
