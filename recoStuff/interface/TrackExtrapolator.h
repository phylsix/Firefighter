#ifndef recoStuff_TrackExtrapolator_H
#define recoStuff_TrackExtrapolator_H

#include "FWCore/Framework/interface/Frameworkfwd.h"

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/GeometrySurface/interface/Surface.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"


namespace ff {
  class TrackExtrapolator
  {
    public:
      TrackExtrapolator();
      TrackExtrapolator(const edm::EventSetup& es);
      TrackExtrapolator(float radius, float absZ, const edm::EventSetup& es);
      ~TrackExtrapolator();

      void init(const edm::EventSetup& es);
      void setParameters(float radius, float absZ);
      void setRadius(float radius);
      void setZ(float absZ);
      void printParameters() const;
      const MagneticField* getMagneticField() const;
      const Propagator* getPropagator() const;
      const GlobalTrackingGeometry* getTrackingGeometry() const;

      TrajectoryStateOnSurface propagate(const FreeTrajectoryState& fts) const;
      TrajectoryStateOnSurface propagate(const GlobalPoint& vertex,
                                         const GlobalVector& momentum,
                                         int charge) const;
      TrajectoryStateOnSurface propagate(const reco::GenParticle&) const;

      TrajectoryStateOnSurface propagate(const FreeTrajectoryState&,
                                         const Surface&) const;


    private:
      float radius_;
      float absZ_;

      const MagneticField* bField_;
      const Propagator* propagator_;
      const GlobalTrackingGeometry* geometry_;
    
  };
}

#endif