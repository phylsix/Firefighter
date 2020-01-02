#ifndef cosmics_CosmicHelper_H
#define cosmics_CosmicHelper_H

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/CSCRecHit/interface/CSCSegmentCollection.h"
#include "DataFormats/DTRecHit/interface/DTRecSegment4DCollection.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "FWCore/Framework/interface/Event.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"



class CosmicHelper {
 public:
  CosmicHelper(){};
  ~CosmicHelper() = default;

  static bool passDSApreselection(const reco::Track&);
  static bool oppositeHemisphere(const reco::Track&, const reco::Track&);
  static bool oppositeHemisphere(const reco::Track&, const DTRecSegment4D&, const edm::EventSetup&);
  static bool oppositeHemisphere(const reco::Track&, const CSCSegment&, const edm::EventSetup&);

  static std::pair<bool, double> twoTrackMinDistance(const reco::Track&, const reco::Track&, const edm::EventSetup&);
  static std::pair<bool, std::tuple<double, GlobalVector, GlobalVector>> propagateDSAtoDT(const reco::Track&, const DTRecSegment4D&, const edm::EventSetup&);
  static std::pair<bool, std::tuple<double, GlobalVector, GlobalVector>> propagateDSAtoCSC(const reco::Track&, const CSCSegment&, const edm::EventSetup&);

  static double cosmicDeltaR(const reco::Track&, const reco::Track&);

  static TrajectoryStateOnSurface propagateTrackToCylinderSurface(const reco::Track& tk, const edm::EventSetup& es, double radius, int mode=0);

};

#endif