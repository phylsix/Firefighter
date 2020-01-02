#include "Firefighter/cosmics/interface/CosmicHelper.h"

#include <cmath>

#include "DataFormats/GeometrySurface/interface/Cylinder.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistance.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"

bool
CosmicHelper::passDSApreselection( const reco::Track& dsa ) {
  const auto& hitptn = dsa.hitPattern();

  if ( ( hitptn.dtStationsWithValidHits() + hitptn.cscStationsWithValidHits() ) < 2 ) return false;
  if ( ( hitptn.numberOfValidMuonDTHits() + hitptn.numberOfValidMuonCSCHits() ) < 13 ) return false;
  if ( hitptn.numberOfValidMuonCSCHits() == 0 && hitptn.numberOfValidMuonDTHits() <= 18 ) return false;

  if ( dsa.pt() < 10. ) return false;
  if ( fabs( dsa.eta() ) > 2.4 ) return false;
  if ( dsa.normalizedChi2() > 4 ) return false;
  if ( ( dsa.ptError() / dsa.pt() ) > 1. ) return false;

  return true;
}

bool
CosmicHelper::oppositeHemisphere( const reco::Track& idsa, const reco::Track& jdsa ) {
  if ( idsa.phi() * jdsa.phi() >= 0 ) return false;

  try {
    if ( idsa.outerOk() && jdsa.outerOk() )
      return idsa.outerY() * jdsa.outerY() < 0;
    else
      return true;
  } catch ( ... ) {
    // corresponding *trackExtra* collection not present in Event
    return true;
  }
}

bool
CosmicHelper::oppositeHemisphere( const reco::Track& dsa, const DTRecSegment4D& dtSeg, const edm::EventSetup& es ) {
  edm::ESHandle<DTGeometry> dtG;
  es.get<MuonGeometryRecord>().get( dtG );

  const DTChamber* dtchamber = dtG->chamber( DTChamberId( dtSeg.geographicalId() ) );

  GlobalPoint  gpos = dtchamber->toGlobal( dtSeg.localPosition() );
  GlobalVector gdir = dtchamber->toGlobal( dtSeg.localDirection() );

  if ( dsa.phi() * gdir.barePhi() >= 0 ) return false;

  try {
    if ( dsa.outerOk() )
      return dsa.outerY() * gpos.y() < 0;
    else
      return true;
  } catch ( ... ) {
    return true;
  }
}

bool
CosmicHelper::oppositeHemisphere( const reco::Track& dsa, const CSCSegment& cscSeg, const edm::EventSetup& es ) {
  edm::ESHandle<CSCGeometry> cscG;
  es.get<MuonGeometryRecord>().get( cscG );

  const CSCChamber* cscchamber = cscG->chamber( CSCDetId( cscSeg.cscDetId() ) );

  GlobalPoint  gpos = cscchamber->toGlobal( cscSeg.localPosition() );
  GlobalVector gdir = cscchamber->toGlobal( cscSeg.localDirection() );

  if ( dsa.phi() * gdir.barePhi() >= 0 ) return false;

  try {
    if ( dsa.outerOk() )
      return dsa.outerY() * gpos.y() < 0;
    else
      return true;
  } catch ( ... ) {
    return true;
  }
}

std::pair<bool, double>
CosmicHelper::twoTrackMinDistance( const reco::Track& idsa, const reco::Track& jdsa, const edm::EventSetup& es ) {
  edm::ESHandle<MagneticField> field_h;
  es.get<IdealMagneticFieldRecord>().get( field_h );
  assert( field_h.isValid() );
  const MagneticField* bField = field_h.product();

  TwoTrackMinimumDistance ttmd;

  bool status = ttmd.calculate( trajectoryStateTransform::initialFreeState( idsa, bField ),
                                trajectoryStateTransform::initialFreeState( jdsa, bField ) );
  if ( status )
    return std::make_pair( true, ttmd.distance() );
  else
    return std::make_pair( false, -1. );
}

std::pair<bool, std::tuple<double, GlobalVector, GlobalVector>>
CosmicHelper::propagateDSAtoDT( const reco::Track& dsa, const DTRecSegment4D& dtSeg, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  ESHandle<DTGeometry> dtG;
  es.get<MuonGeometryRecord>().get( dtG );

  ESHandle<MagneticField> field_h;
  es.get<IdealMagneticFieldRecord>().get( field_h );
  assert( field_h.isValid() );
  const MagneticField* bField = field_h.product();

  ESHandle<Propagator> propagator_h;
  es.get<TrackingComponentsRecord>().get( "SteppingHelixPropagatorAny", propagator_h );
  assert( propagator_h.isValid() );
  const Propagator* propagator = propagator_h.product();

  FreeTrajectoryState startingState = trajectoryStateTransform::initialFreeState( dsa, bField );

  const DTChamber* dtchamber = dtG->chamber( DTChamberId( dtSeg.geographicalId() ) );

  GlobalPoint  gpos = dtchamber->toGlobal( dtSeg.localPosition() );
  GlobalVector gdir = dtchamber->toGlobal( dtSeg.localDirection() );

  TrajectoryStateOnSurface propagationResult = propagator->propagate( startingState, dtchamber->surface() );
  if ( !propagationResult.isValid() )
    return make_pair( false, make_tuple( 999., GlobalVector( 0, 0, 0 ), GlobalVector( 0, 0, 0 ) ) );

  GlobalPoint  ppos = propagationResult.globalPosition();
  GlobalVector pdir = propagationResult.globalDirection();

  return make_pair( true, make_tuple( ( ppos - gpos ).mag(), pdir, gdir ) );
}

std::pair<bool, std::tuple<double, GlobalVector, GlobalVector>>
CosmicHelper::propagateDSAtoCSC( const reco::Track& dsa, const CSCSegment& cscSeg, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  ESHandle<CSCGeometry> cscG;
  es.get<MuonGeometryRecord>().get( cscG );

  ESHandle<MagneticField> field_h;
  es.get<IdealMagneticFieldRecord>().get( field_h );
  assert( field_h.isValid() );
  const MagneticField* bField = field_h.product();

  ESHandle<Propagator> propagator_h;
  es.get<TrackingComponentsRecord>().get( "SteppingHelixPropagatorAny", propagator_h );
  assert( propagator_h.isValid() );
  const Propagator* propagator = propagator_h.product();

  FreeTrajectoryState startingState = trajectoryStateTransform::initialFreeState( dsa, bField );

  const CSCChamber* cscchamber = cscG->chamber( CSCDetId( cscSeg.cscDetId() ) );

  GlobalPoint  gpos = cscchamber->toGlobal( cscSeg.localPosition() );
  GlobalVector gdir = cscchamber->toGlobal( cscSeg.localDirection() );

  TrajectoryStateOnSurface propagationResult = propagator->propagate( startingState, cscchamber->surface() );
  if ( !propagationResult.isValid() )
    return make_pair( false, make_tuple( 999., GlobalVector( 0, 0, 0 ), GlobalVector( 0, 0, 0 ) ) );
  ;

  GlobalPoint  ppos = propagationResult.globalPosition();
  GlobalVector pdir = propagationResult.globalDirection();

  return make_pair( true, make_tuple( ( ppos - gpos ).mag(), pdir, gdir ) );
}

double
CosmicHelper::cosmicDeltaR( const reco::Track& idsa, const reco::Track& jdsa ) {
  double etaSum    = idsa.eta() + jdsa.eta();
  double phiPiDiff = M_PI - fabs( deltaPhi( idsa.phi(), jdsa.phi() ) );

  return std::hypot( etaSum, phiPiDiff );
}

TrajectoryStateOnSurface
CosmicHelper::propagateTrackToCylinderSurface( const reco::Track& tk, const edm::EventSetup& es, double radius, int mode ) {
  using namespace std;
  using namespace edm;

  ESHandle<MagneticField> field_h;
  es.get<IdealMagneticFieldRecord>().get( field_h );
  assert( field_h.isValid() );
  const MagneticField* bField = field_h.product();

  ESHandle<Propagator> propagator_h;
  if ( mode == 0 )
    es.get<TrackingComponentsRecord>().get( "SteppingHelixPropagatorAny", propagator_h );
  else if ( mode == 1 )
    es.get<TrackingComponentsRecord>().get( "SteppingHelixPropagatorAlong", propagator_h );
  else if ( mode == -1 )
    es.get<TrackingComponentsRecord>().get( "SteppingHelixPropagatorOpposite", propagator_h );

  assert( propagator_h.isValid() );
  const Propagator* propagator = propagator_h.product();

  FreeTrajectoryState      startingState     = trajectoryStateTransform::initialFreeState( tk, bField );
  TrajectoryStateOnSurface propagationResult = propagator->propagate( startingState,
                                                                      *Cylinder::build( radius,
                                                                                        Surface::PositionType( 0, 0, 0 ),
                                                                                        Surface::RotationType() ) );

  return propagationResult.isValid() ? propagationResult : TrajectoryStateOnSurface();
}