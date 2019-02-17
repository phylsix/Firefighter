#include "Firefighter/recoStuff/interface/TrackExtrapolator.h"

#include "DataFormats/GeometrySurface/interface/Cylinder.h"
#include "DataFormats/GeometrySurface/interface/Plane.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/TrajectoryParametrization/interface/GlobalTrajectoryParameters.h"

ff::TrackExtrapolator::TrackExtrapolator()
    : radius_( 0. ),
      absZ_( 0. ),
      bField_( nullptr ),
      propagator_( nullptr ),
      geometry_( nullptr ) {}

ff::TrackExtrapolator::TrackExtrapolator( const edm::EventSetup& es )
    : radius_( 0. ), absZ_( 0. ) {
  init( es );
}

ff::TrackExtrapolator::TrackExtrapolator( float                  radius,
                                          float                  absZ,
                                          const edm::EventSetup& es ) {
  setParameters( radius_, absZ_ );
  init( es );
}

void
ff::TrackExtrapolator::init( const edm::EventSetup& es ) {
  edm::ESHandle<MagneticField> field_h;
  es.get<IdealMagneticFieldRecord>().get( field_h );
  assert( field_h.isValid() );

  bField_ = field_h.product();

  edm::ESHandle<Propagator> propagator_h;
  es.get<TrackingComponentsRecord>().get( "SteppingHelixPropagatorAlong",
                                          propagator_h );
  assert( propagator_h.isValid() );

  propagator_ = propagator_h.product();

  edm::ESHandle<GlobalTrackingGeometry> geometry_h;
  es.get<GlobalTrackingGeometryRecord>().get( geometry_h );
  assert( geometry_h.isValid() );

  geometry_ = geometry_h.product();
}

void
ff::TrackExtrapolator::setParameters( float radius, float absZ ) {
  setRadius( radius );
  setZ( absZ );
}

void
ff::TrackExtrapolator::setRadius( float radius ) {
  radius_ = radius;
}

void
ff::TrackExtrapolator::setZ( float absZ ) {
  absZ_ = absZ;
}

void
ff::TrackExtrapolator::printParameters() const {
  std::stringstream ss;
  ss << "radius: " << radius_ << " cm"
     << ", Z: (" << -absZ_ << ", " << absZ_ << ") cm";

  std::cout << ss.str() << std::endl;
}

const MagneticField*
ff::TrackExtrapolator::getMagneticField() const {
  return bField_;
}

const Propagator*
ff::TrackExtrapolator::getPropagator() const {
  return propagator_;
}

const GlobalTrackingGeometry*
ff::TrackExtrapolator::getTrackingGeometry() const {
  return geometry_;
}

ff::TrackExtrapolator::~TrackExtrapolator() {
  bField_     = nullptr;
  propagator_ = nullptr;
  geometry_   = nullptr;
}

TrajectoryStateOnSurface
ff::TrackExtrapolator::propagate( const FreeTrajectoryState& fts ) const {
  TrajectoryStateOnSurface propagatedInfo = propagator_->propagate(
      fts, *Cylinder::build( radius_, Surface::PositionType( 0, 0, 0 ),
                             Surface::RotationType() ) );

  if ( !propagatedInfo.isValid() )
    return TrajectoryStateOnSurface();

  if ( propagatedInfo.globalPosition().z() > absZ_ ) {
    propagatedInfo = propagator_->propagate(
        fts, *Plane::build( Surface::PositionType( 0, 0, absZ_ ),
                            Surface::RotationType() ) );
  } else if ( propagatedInfo.globalPosition().z() < -absZ_ ) {
    propagatedInfo = propagator_->propagate(
        fts, *Plane::build( Surface::PositionType( 0, 0, -absZ_ ),
                            Surface::RotationType() ) );
  }

  return propagatedInfo.isValid() ? propagatedInfo : TrajectoryStateOnSurface();
}

TrajectoryStateOnSurface
ff::TrackExtrapolator::propagate( const GlobalPoint&  vertex,
                                  const GlobalVector& momentum,
                                  int                 charge ) const {
  GlobalTrajectoryParameters trackParams( vertex, momentum, charge, bField_ );
  FreeTrajectoryState        trackState( trackParams );

  return propagate( trackState );
}

TrajectoryStateOnSurface
ff::TrackExtrapolator::propagate( const reco::GenParticle& particle ) const {
  GlobalPoint vertex = GlobalPoint(
      particle.vertex().x(), particle.vertex().y(), particle.vertex().z() );

  GlobalVector momentum =
      GlobalVector( particle.px(), particle.py(), particle.pz() );

  return propagate( vertex, momentum, particle.charge() );
}

TrajectoryStateOnSurface
ff::TrackExtrapolator::propagate( const FreeTrajectoryState& fts,
                                  const Surface&             surface ) const {
  TrajectoryStateOnSurface propagatedInfo =
      propagator_->propagate( fts, surface );

  return propagatedInfo.isValid() ? propagatedInfo : TrajectoryStateOnSurface();
}