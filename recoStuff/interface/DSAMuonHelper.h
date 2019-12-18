#ifndef recoStuff_DSAMuonHelper_H
#define recoStuff_DSAMuonHelper_H

#include "DataFormats/CSCRecHit/interface/CSCSegmentCollection.h"
#include "DataFormats/DTRecHit/interface/DTRecSegment4DCollection.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonChamberMatch.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/Framework/interface/Event.h"

class DSAMuonHelper {
 public:
  DSAMuonHelper(){};
  ~DSAMuonHelper() = default;

  static std::vector<DTChamberId> getDTDetIds( const std::vector<reco::MuonChamberMatch>& );
  static std::vector<DTChamberId> getDTDetIds( const reco::Muon& );
  static std::vector<DTChamberId> getDTDetIds( const reco::Track&, const edm::EventSetup& );

  static std::vector<CSCDetId> getCSCDetIds( const std::vector<reco::MuonChamberMatch>& );
  static std::vector<CSCDetId> getCSCDetIds( const reco::Muon& );
  static std::vector<CSCDetId> getCSCDetIds( const reco::Track&, const edm::EventSetup& );

  static std::vector<RPCDetId> getRPCDetIds( const reco::Track&, const edm::EventSetup& );

  static bool detIdsIsSubSetOfDTCSCIds( const reco::Track&,
                                        const std::vector<std::vector<DTChamberId>>&,
                                        const std::vector<std::vector<CSCDetId>>&,
                                        const edm::EventSetup& );

  static std::vector<DTRecSegment4DRef> getDTSegments( const std::vector<reco::MuonChamberMatch>& );
  static std::vector<DTRecSegment4DRef> getDTSegments( const reco::Muon& );

  static std::vector<CSCSegmentRef> getCSCSegements( const std::vector<reco::MuonChamberMatch>& );
  static std::vector<CSCSegmentRef> getCSCSegements( const reco::Muon& );

  static bool segmentsIsSubsetOfDTCSCSegs( const std::vector<reco::MuonChamberMatch>&,
                                           const std::vector<std::vector<DTRecSegment4DRef>>&,
                                           const std::vector<std::vector<CSCSegmentRef>>& );
  static bool segmentsIsSubsetOfDTCSCSegs( const reco::Muon&,
                                           const std::vector<std::vector<DTRecSegment4DRef>>&,
                                           const std::vector<std::vector<CSCSegmentRef>>& );

  static int getDTSegmentHemisphere( const DTRecSegment4DRef&, const edm::EventSetup& );
  static int getCSCSegmentHemisphere( const CSCSegmentRef&, const edm::EventSetup& );

  static bool bothHemisphereBySegments( const std::vector<reco::MuonChamberMatch>&,
                                        const edm::EventSetup&,
                                        int minSegMinorSide = 1 );
  static bool bothHemisphereBySegments( const reco::Muon&,
                                        const edm::EventSetup&,
                                        int minSegMinorSide = 1 );
};

#endif