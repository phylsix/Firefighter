#include "Firefighter/recoStuff/interface/RecoHelpers.h"

#include "TrackingTools/GeomPropagators/interface/AnalyticalImpactPointExtrapolator.h"
#include "TrackingTools/PatternTools/interface/TransverseImpactPointExtrapolator.h"

bool
ff::genAccept( reco::GenParticle const& g ) {
  return ( abs( g.pdgId() ) == 13 or
           abs( g.pdgId() ) == 11 )         // is muon OR electron
         and g.isHardProcess()              // final states
         and abs( g.eta() ) < 2.4           // |eta|<2.4
         and g.pt() > 5.                    // pT>5
         and abs( g.vertex().rho() ) < 740  // decay inside CMS
         and abs( g.vz() ) < 960;
}

std::pair<bool, Measurement1D>
ff::absoluteImpactParameter( const TrajectoryStateOnSurface& tsos,
                             const VertexState&              vs,
                             VertexDistance&                 vd ) {
  if ( !tsos.isValid() )
    return std::pair<bool, Measurement1D>( false, Measurement1D( 0., 0. ) );

  GlobalPoint refPoint    = tsos.globalPosition();
  GlobalError refPointErr = tsos.cartesianError().position();

  return std::make_pair(
      true, vd.distance( vs, VertexState( refPoint, refPointErr ) ) );
}

std::pair<bool, Measurement1D>
ff::absoluteImpactParameter3D( const reco::TransientTrack& tt,
                               const VertexState&          vs ) {
  AnalyticalImpactPointExtrapolator extrapolator( tt.field() );
  VertexDistance3D                  dist;

  return ff::absoluteImpactParameter(
      extrapolator.extrapolate( tt.impactPointState(), vs.position() ), vs,
      dist );
}

std::pair<bool, Measurement1D>
ff::absoluteTransverseImpactParameter( const reco::TransientTrack& tt,
                                       const VertexState&          vs ) {
  TransverseImpactPointExtrapolator extrapolator( tt.field() );
  VertexDistanceXY                  dist;

  return ff::absoluteImpactParameter(
      extrapolator.extrapolate( tt.impactPointState(), vs.position() ), vs,
      dist );
}