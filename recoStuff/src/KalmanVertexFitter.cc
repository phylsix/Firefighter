#include "Firefighter/recoStuff/interface/KalmanVertexFitter.h"

#include "RecoVertex/KalmanVertexFit/interface/KalmanSmoothedVertexChi2Estimator.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanTrackToTrackCovCalculator.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexTrackUpdator.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexUpdator.h"
#include "RecoVertex/LinearizationPointFinders/interface/FsmwLinearizationPointFinder.h"
#include "RecoVertex/VertexTools/interface/DummyVertexSmoother.h"
#include "RecoVertex/VertexTools/interface/SequentialVertexSmoother.h"

ff::KalmanVertexFitter::KalmanVertexFitter( bool useSmoothing ) {
  edm::ParameterSet pSet = defaultParameters();
  setup( pSet, useSmoothing );
}

ff::KalmanVertexFitter::KalmanVertexFitter( const edm::ParameterSet& pSet,
                                            bool useSmoothing ) {
  setup( pSet, useSmoothing );
}

void
ff::KalmanVertexFitter::setup( const edm::ParameterSet& pSet,
                               bool                     useSmoothing ) {
  if ( useSmoothing ) {
    KalmanVertexTrackUpdator<5>          vtu;
    KalmanSmoothedVertexChi2Estimator<5> vse;
    KalmanTrackToTrackCovCalculator<5>   covCalc;
    SequentialVertexSmoother<5>          smoother( vtu, vse, covCalc );
    theSequentialFitter = new ff::SequentialVertexFitter<5>(
        pSet, FsmwLinearizationPointFinder( 20, -2., 0.4, 10. ),
        KalmanVertexUpdator<5>(), smoother, LinearizedTrackStateFactory() );
  } else {
    DummyVertexSmoother<5> smoother;
    theSequentialFitter = new ff::SequentialVertexFitter<5>(
        pSet, FsmwLinearizationPointFinder( 20, -2., 0.4, 10. ),
        KalmanVertexUpdator<5>(), smoother, LinearizedTrackStateFactory() );
  }
}

edm::ParameterSet
ff::KalmanVertexFitter::defaultParameters() const {
  edm::ParameterSet pSet;
  pSet.addParameter<double>( "maxDistance", 0.01 );
  pSet.addParameter<int>( "maxNbrOfIterations", 10 );  // 10
  return pSet;
}
