#include "Firefighter/recoStuff/interface/RecoHelpers.h"

#include "DataFormats/GeometrySurface/interface/Line.h"
#include "DataFormats/GeometryVector/interface/GlobalTag.h"
#include "DataFormats/GeometryVector/interface/Vector2DBase.h"
#include "Firefighter/recoStuff/interface/KalmanVertexFitter.h"
#include "Firefighter/recoStuff/interface/KinematicParticleVertexFitter.h"
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicParticleFactoryFromTransientTrack.h"
#include "RecoVertex/KinematicFitPrimitives/interface/RefCountedKinematicParticle.h"
#include "RecoVertex/KinematicFitPrimitives/interface/RefCountedKinematicVertex.h"
#include "RecoVertex/KinematicFitPrimitives/interface/TransientTrackKinematicParticle.h"
#include "RecoVertex/KinematicFitPrimitives/interface/TransientTrackKinematicStateBuilder.h"
#include "TrackingTools/GeomPropagators/interface/AnalyticalImpactPointExtrapolator.h"
#include "TrackingTools/PatternTools/interface/TransverseImpactPointExtrapolator.h"

using Point          = math::XYZPointF;
using LorentzVector  = math::XYZTLorentzVectorF;
using Global2DVector = Vector2DBase<float, GlobalTag>;

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

std::pair<bool, Measurement1D>
ff::absoluteImpactParameter3D( const reco::TransientTrack& tt,
                               const VertexState&          vs ) {
  AnalyticalImpactPointExtrapolator extrapolator( tt.field() );
  VertexDistance3D                  dist;

  return ff::absoluteImpactParameter(
      extrapolator.extrapolate( tt.impactPointState(), vs.position() ), vs,
      dist );
}

//-----------------------------------------------------------------------------

std::pair<bool, Measurement1D>
ff::absoluteTransverseImpactParameter( const reco::TransientTrack& tt,
                                       const VertexState&          vs ) {
  TransverseImpactPointExtrapolator extrapolator( tt.field() );
  VertexDistanceXY                  dist;

  return ff::absoluteImpactParameter(
      extrapolator.extrapolate( tt.impactPointState(), vs.position() ), vs,
      dist );
}

//-----------------------------------------------------------------------------

reco::PFCandidatePtr
ff::getCandWithMaxPt( const std::vector<reco::PFCandidatePtr>& cands ) {
  if ( cands.size() == 0 )
    return reco::PFCandidatePtr();

  float  _ptmax( 0. );
  size_t _idxmax( cands.size() );
  for ( size_t id( 0 ); id != cands.size(); ++id ) {
    if ( cands[ id ]->pt() > _ptmax ) {
      _ptmax  = cands[ id ]->pt();
      _idxmax = id;
    }
  }

  return _idxmax == cands.size() ? reco::PFCandidatePtr() : cands[ _idxmax ];
}

//-----------------------------------------------------------------------------

int
ff::getCandType( const reco::PFCandidatePtr&               cand,
                 const edm::Handle<reco::TrackCollection>& generalTkH ) {
  if ( cand.isNull() )
    return 0;

  if ( cand->trackRef().isNonnull() &&
       cand->trackRef().id() != generalTkH.id() )
    return 8;  // This is coming from a displacedStandAloneMuon

  return cand->particleId();
}

//-----------------------------------------------------------------------------

float
ff::getMuonIsolationValue( const reco::Muon& muon ) {
  const auto& pfiso04 = muon.pfIsolationR04();
  float       val     = ( pfiso04.sumChargedHadronPt + std::max( 0., pfiso04.sumNeutralHadronEt + pfiso04.sumPhotonEt - 0.5 * pfiso04.sumPUPt ) ) / muon.pt();
  return val;
}

//-----------------------------------------------------------------------------

Point
ff::estimatedVertexFromMedianReferencePoints(
    const std::vector<const reco::Track*>& pTks ) {
  std::vector<float> cXinnerPos, cYinnerPos, cZinnerPos;
  for ( const auto& cTk : pTks ) {
    cXinnerPos.push_back( cTk->referencePoint().X() );
    cYinnerPos.push_back( cTk->referencePoint().Y() );
    cZinnerPos.push_back( cTk->referencePoint().Z() );
  }

  return Point( ff::medianValue<float>( cXinnerPos ),
                ff::medianValue<float>( cYinnerPos ),
                ff::medianValue<float>( cZinnerPos ) );
}

//-----------------------------------------------------------------------------

Point
ff::estimatedVertexFromAverageReferencePoints(
    const std::vector<const reco::Track*>& pTks ) {
  std::vector<float> cXinnerPos, cYinnerPos, cZinnerPos;
  for ( const auto& cTk : pTks ) {
    cXinnerPos.push_back( cTk->referencePoint().X() );
    cYinnerPos.push_back( cTk->referencePoint().Y() );
    cZinnerPos.push_back( cTk->referencePoint().Z() );
  }

  return Point( ff::averageValue<float>( cXinnerPos ),
                ff::averageValue<float>( cYinnerPos ),
                ff::averageValue<float>( cZinnerPos ) );
}

//-----------------------------------------------------------------------------

std::pair<TransientVertex, float>
ff::kalmanVertexFromTransientTracks(
    const std::vector<reco::TransientTrack>& t_tks,
    const edm::ParameterSet&                 kvfParam ) {
  /// vertexing
  if ( t_tks.size() < 2 )
    return std::make_pair( TransientVertex(), NAN );

  std::unique_ptr<ff::KalmanVertexFitter> kvf( new ff::KalmanVertexFitter(
      kvfParam, kvfParam.getParameter<bool>( "doSmoothing" ) ) );

  TransientVertex tv;
  try {
    tv = kvf->vertex( t_tks );
  } catch ( ... ) {
    std::cout << "+++ Exception from ff::kalmanVertexFromTransientTracks !"
              << std::endl;
    return std::make_pair( TransientVertex(), NAN );
  }

  if ( !tv.isValid() )
    return std::make_pair( TransientVertex(), NAN );

  /// mass
  LorentzVector vtxp4;
  for ( const auto& refitTks : tv.refittedTracks() ) {
    const reco::Track& tk   = refitTks.track();
    reco::CandidatePtr cand = refitTks.basicTransientTrack()->candidate();
    float              mass = cand.isNonnull() ? cand->mass() : 0.;

    vtxp4 +=
        LorentzVector( tk.px(), tk.py(), tk.pz(), std::hypot( tk.p(), mass ) );
  }

  return std::make_pair( tv, vtxp4.M() );
}

//-----------------------------------------------------------------------------

std::pair<KinematicVertex, float>
ff::kinematicVertexFromTransientTracks(
    const std::vector<reco::TransientTrack>& t_tks ) {
  /// vertexing
  if ( t_tks.size() < 2 )
    return std::make_pair( KinematicVertex(), NAN );

  KinematicParticleFactoryFromTransientTrack pFactory;

  std::vector<RefCountedKinematicParticle> allParticles;
  TransientTrackKinematicStateBuilder      ttkStateBuilder;
  // sigma to avoid singularities in the covariance matrix.
  float pSigma = 0.0000001;
  // initial chi2 and ndf before kinematic fits.
  // The chi2 of the reconstruction is not considered
  float chi = 0.;
  float ndf = 0.;

  for ( const auto& ttk : t_tks ) {
    reco::CandidatePtr cand  = ttk.basicTransientTrack()->candidate();
    double             pMass = cand.isNonnull() ? cand->mass() : 0.;
    allParticles.push_back( pFactory.particle( ttk, pMass, chi, ndf, pSigma ) );
  }

  std::unique_ptr<ff::KinematicParticleVertexFitter> kinFitter(
      new ff::KinematicParticleVertexFitter() );
  RefCountedKinematicTree kinTree;
  try {
    kinTree = kinFitter->fit( allParticles );
  } catch ( ... ) {
    std::cout << "+++ Exception from ff::kinematicVertexFromTransientTracks !"
              << std::endl;
    return std::make_pair( KinematicVertex(), NAN );
  }

  if ( !kinTree->isValid() )
    return std::make_pair( KinematicVertex(), NAN );

  kinTree->movePointerToTheTop();
  RefCountedKinematicVertex kinV = kinTree->currentDecayVertex();

  if ( !kinV->vertexIsValid() or kinV->correspondingTree() == nullptr )
    return std::make_pair( KinematicVertex(), NAN );

  /// mass
  LorentzVector vtxp4;
  for ( const auto& dau : kinTree->daughterParticles() ) {
    const TransientTrackKinematicParticle* ttkp =
        dynamic_cast<const TransientTrackKinematicParticle*>( dau.get() );
    if ( ttkp == nullptr ) {
      continue;
    }

    reco::Track dauTk   = ttkp->refittedTransientTrack().track();
    double      dauMass = dau->initialState().mass();

    vtxp4 += LorentzVector( dauTk.px(), dauTk.py(), dauTk.pz(),
                            std::hypot( dauTk.p(), dauMass ) );
  }

  return std::make_pair( *kinV, vtxp4.M() );
}

//-----------------------------------------------------------------------------

Measurement1D
ff::signedDistanceXY( const reco::Vertex& vtx1,
                      const VertexState&  vs2,
                      const GlobalVector& ref ) {
  VertexDistanceXY vdistXY;

  Measurement1D unsignedDistance = vdistXY.distance( vtx1, vs2 );
  GlobalVector  diff             = vs2.position() - GlobalPoint( Basic3DVector<float>( vtx1.position() ) );

  if ( ( ref.x() * diff.x() + ref.y() * diff.y() ) < 0 )
    return Measurement1D( -1.0 * unsignedDistance.value(),
                          unsignedDistance.error() );

  return unsignedDistance;
}

//-----------------------------------------------------------------------------

Measurement1D
ff::signedDistance3D( const reco::Vertex& vtx1,
                      const VertexState&  vs2,
                      const GlobalVector& ref ) {
  VertexDistance3D vdist3D;

  Measurement1D unsignedDistance = vdist3D.distance( vtx1, vs2 );
  GlobalVector  diff             = vs2.position() - GlobalPoint( Basic3DVector<float>( vtx1.position() ) );

  if ( ( ref.x() * diff.x() + ref.y() * diff.y() + ref.z() * diff.z() ) < 0 )
    return Measurement1D( -1.0 * unsignedDistance.value(),
                          unsignedDistance.error() );

  return unsignedDistance;
}

//-----------------------------------------------------------------------------

float
ff::cosThetaOfJetPvXY( const reco::Vertex& vtx1,
                       const VertexState&  vs2,
                       const GlobalVector& ref ) {
  GlobalVector   diff   = vs2.position() - GlobalPoint( Basic3DVector<float>( vtx1.position() ) );
  Global2DVector diff2D = Global2DVector( diff.x(), diff.y() );
  Global2DVector ref2D  = Global2DVector( ref.x(), ref.y() );
  return diff2D.unit().dot( ref2D.unit() );
}

//-----------------------------------------------------------------------------

float
ff::cosThetaOfJetPv3D( const reco::Vertex& vtx1,
                       const VertexState&  vs2,
                       const GlobalVector& ref ) {
  GlobalVector diff = vs2.position() - GlobalPoint( Basic3DVector<float>( vtx1.position() ) );
  return diff.unit().dot( ref.unit() );
}

//-----------------------------------------------------------------------------

float
ff::impactDistanceXY( const reco::Vertex& vtx1,
                      const VertexState&  vs2,
                      const GlobalVector& ref ) {
  Line::PositionType  pos( GlobalPoint( vs2.position().x(), vs2.position().y(), 0 ) );
  Line::DirectionType dir( GlobalVector( ref.x(), ref.y(), 0 ).unit() );
  Line                jetDirectionLineXY( pos, dir );

  GlobalPoint pv( vtx1.position().x(), vtx1.position().y(), 0 );

  return jetDirectionLineXY.distance( pv ).mag();
}

//-----------------------------------------------------------------------------

float
ff::impactDistance3D( const reco::Vertex& vtx1,
                      const VertexState&  vs2,
                      const GlobalVector& ref ) {
  Line::PositionType  pos( vs2.position() );
  Line::DirectionType dir( ref.unit() );
  Line                jetDirectionLine( pos, dir );

  GlobalPoint pv( vtx1.position().x(), vtx1.position().y(), vtx1.position().z() );

  return jetDirectionLine.distance( pv ).mag();
}
