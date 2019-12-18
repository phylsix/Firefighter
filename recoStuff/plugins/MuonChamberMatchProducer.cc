#include "Firefighter/recoStuff/interface/MuonChamberMatchProducer.h"

#include "DataFormats/MuonReco/interface/MuonChamberMatch.h"
#include "DataFormats/MuonReco/interface/MuonRPCHitMatch.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"

MuonChamberMatchProducer::MuonChamberMatchProducer( const edm::ParameterSet& ps )
    : fInputTrackToken( consumes<reco::TrackCollection>( ps.getParameter<edm::InputTag>( "src" ) ) ),
      fRPCHitToken( consumes<RPCRecHitCollection>( edm::InputTag( "rpcRecHits" ) ) ) {
  // load TrackAssociator parameters
  const edm::ParameterSet parameters = ps.getParameter<edm::ParameterSet>( "TrackAssociatorParameters" );
  edm::ConsumesCollector  iC         = consumesCollector();
  fTrackAssociatorParameters.loadParameters( parameters, iC );

  fPropagateOpposite = ps.getParameter<bool>( "propagateOpposite" );

  fMaxAbsDx    = ps.getParameter<double>( "maxAbsDx" );
  fMaxAbsPullX = ps.getParameter<double>( "maxAbsPullX" );
  fMaxAbsDy    = ps.getParameter<double>( "maxAbsDy" );
  fMaxAbsPullY = ps.getParameter<double>( "maxAbsPullY" );

  produces<edm::ValueMap<std::vector<reco::MuonChamberMatch>>>();
}

MuonChamberMatchProducer::~MuonChamberMatchProducer() = default;

void
MuonChamberMatchProducer::produce( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  auto                                   vm_mcm = make_unique<ValueMap<vector<reco::MuonChamberMatch>>>();
  vector<vector<reco::MuonChamberMatch>> v_mcm{};

  e.getByToken( fInputTrackToken, fInputTrackHdl );
  assert( fInputTrackHdl.isValid() );
  e.getByToken( fRPCHitToken, fRPCHitHdl );

  ESHandle<Propagator> propagator;
  es.get<TrackingComponentsRecord>().get( "SteppingHelixPropagatorAny", propagator );
  fTrackAssociator.setPropagator( propagator.product() );

  TrackDetectorAssociator::Direction direction = TrackDetectorAssociator::OutsideIn;

  for ( const auto& track : *fInputTrackHdl ) {
    // track.setExtra( reco::TrackExtraRef() );

    GlobalVector oppositeMomentum( -track.momentum().x(), -track.momentum().y(), -track.momentum().z() );
    // GlobalPoint  trackVertex( track.referencePoint().x(), track.referencePoint().y(), track.referencePoint().z() );
    GlobalPoint trackVertex( track.innerPosition().x(), track.innerPosition().y(), track.innerPosition().z() );

    TrackDetMatchInfo info = fTrackAssociator.associate( e, es, track, fTrackAssociatorParameters, direction );
    // TrackDetMatchInfo oppoInfo = fTrackAssociator.associate( e, es, oppositeMomentum, trackVertex, track.charge(), fTrackAssociatorParameters );
    TrackDetMatchInfo oppoInfo = fTrackAssociator.associate( e, es, track, fTrackAssociatorParameters, TrackDetectorAssociator::InsideOut );

    vector<reco::MuonChamberMatch> muonChamberMatches;
    unsigned int                   nubmerOfMatchesAccordingToTrackAssociator = 0;

    for ( const auto& chamber : info.chambers ) {
      if ( chamber.id.subdetId() == 3 && fRPCHitHdl.isValid() ) continue;  // Skip RPC chambers, they are taken care of below)
      reco::MuonChamberMatch matchedChamber;

      const auto& lErr = chamber.tState.localError();
      const auto& lPos = chamber.tState.localPosition();
      const auto& lDir = chamber.tState.localDirection();

      const auto& localError = lErr.positionError();
      matchedChamber.x       = lPos.x();
      matchedChamber.y       = lPos.y();
      matchedChamber.xErr    = sqrt( localError.xx() );
      matchedChamber.yErr    = sqrt( localError.yy() );

      matchedChamber.dXdZ = lDir.z() != 0 ? lDir.x() / lDir.z() : 9999;
      matchedChamber.dYdZ = lDir.z() != 0 ? lDir.y() / lDir.z() : 9999;
      // DANGEROUS - compiler cannot guaranty parameters ordering
      AlgebraicSymMatrix55 trajectoryCovMatrix = lErr.matrix();
      matchedChamber.dXdZErr                   = trajectoryCovMatrix( 1, 1 ) > 0 ? sqrt( trajectoryCovMatrix( 1, 1 ) ) : 0;
      matchedChamber.dYdZErr                   = trajectoryCovMatrix( 2, 2 ) > 0 ? sqrt( trajectoryCovMatrix( 2, 2 ) ) : 0;

      matchedChamber.edgeX = chamber.localDistanceX;
      matchedChamber.edgeY = chamber.localDistanceY;

      matchedChamber.id = chamber.id;

      // if ( fillShowerDigis_ ) {
      //   theShowerDigiFiller_->fill( matchedChamber );
      // } else {
      //   theShowerDigiFiller_->fillDefault( matchedChamber );
      // }

      if ( !chamber.segments.empty() )
        ++nubmerOfMatchesAccordingToTrackAssociator;

      // fill segments
      for ( const auto& segment : chamber.segments ) {
        reco::MuonSegmentMatch matchedSegment;
        matchedSegment.x             = segment.segmentLocalPosition.x();
        matchedSegment.y             = segment.segmentLocalPosition.y();
        matchedSegment.dXdZ          = segment.segmentLocalDirection.z() ? segment.segmentLocalDirection.x() / segment.segmentLocalDirection.z() : 0;
        matchedSegment.dYdZ          = segment.segmentLocalDirection.z() ? segment.segmentLocalDirection.y() / segment.segmentLocalDirection.z() : 0;
        matchedSegment.xErr          = segment.segmentLocalErrorXX > 0 ? sqrt( segment.segmentLocalErrorXX ) : 0;
        matchedSegment.yErr          = segment.segmentLocalErrorYY > 0 ? sqrt( segment.segmentLocalErrorYY ) : 0;
        matchedSegment.dXdZErr       = segment.segmentLocalErrorDxDz > 0 ? sqrt( segment.segmentLocalErrorDxDz ) : 0;
        matchedSegment.dYdZErr       = segment.segmentLocalErrorDyDz > 0 ? sqrt( segment.segmentLocalErrorDyDz ) : 0;
        matchedSegment.t0            = segment.t0;
        matchedSegment.mask          = 0;
        matchedSegment.dtSegmentRef  = segment.dtSegmentRef;
        matchedSegment.cscSegmentRef = segment.cscSegmentRef;
        matchedSegment.gemSegmentRef = segment.gemSegmentRef;
        matchedSegment.me0SegmentRef = segment.me0SegmentRef;
        matchedSegment.hasZed_       = segment.hasZed;
        matchedSegment.hasPhi_       = segment.hasPhi;
        // test segment
        bool matchedX = false;
        bool matchedY = false;
        LogTrace( "MuonChamberMatchProducer" ) << " matching local x, segment x: " << matchedSegment.x << ", chamber x: " << matchedChamber.x << ", max: " << fMaxAbsDx;
        LogTrace( "MuonChamberMatchProducer" ) << " matching local y, segment y: " << matchedSegment.y << ", chamber y: " << matchedChamber.y << ", max: " << fMaxAbsDy;
        const double matchedSegChDx    = std::abs( matchedSegment.x - matchedChamber.x );
        const double matchedSegChDy    = std::abs( matchedSegment.y - matchedChamber.y );
        const double matchedSegChPullX = matchedSegChDx / std::hypot( matchedSegment.xErr, matchedChamber.xErr );
        const double matchedSegChPullY = matchedSegChDy / std::hypot( matchedSegment.yErr, matchedChamber.yErr );
        if ( matchedSegment.xErr > 0 && matchedChamber.xErr > 0 )
          LogTrace( "MuonChamberMatchProducer" ) << " xpull: " << matchedSegChPullX;
        if ( matchedSegment.yErr > 0 && matchedChamber.yErr > 0 )
          LogTrace( "MuonChamberMatchProducer" ) << " ypull: " << matchedSegChPullY;

        if ( matchedSegChDx < fMaxAbsDx ) matchedX = true;
        if ( matchedSegChDy < fMaxAbsDy ) matchedY = true;
        if ( matchedSegment.xErr > 0 && matchedChamber.xErr > 0 && matchedSegChPullX < fMaxAbsPullX ) matchedX = true;
        if ( matchedSegment.yErr > 0 && matchedChamber.yErr > 0 && matchedSegChPullY < fMaxAbsPullY ) matchedY = true;
        if ( matchedX && matchedY ) {
          if ( matchedChamber.id.subdetId() == MuonSubdetId::ME0 )
            matchedChamber.me0Matches.push_back( matchedSegment );
          else if ( matchedChamber.id.subdetId() == MuonSubdetId::GEM )
            matchedChamber.gemMatches.push_back( matchedSegment );
          else
            matchedChamber.segmentMatches.push_back( matchedSegment );
        }
      }
      muonChamberMatches.push_back( matchedChamber );
    }

    if ( fPropagateOpposite ) {
      // fill opposite chamberMatches
      for ( const auto& chamber : oppoInfo.chambers ) {
        if ( chamber.id.subdetId() == 3 && fRPCHitHdl.isValid() ) continue;  // Skip RPC chambers, they are taken care of below)
        reco::MuonChamberMatch matchedChamber;

        const auto& lErr = chamber.tState.localError();
        const auto& lPos = chamber.tState.localPosition();
        const auto& lDir = chamber.tState.localDirection();

        const auto& localError = lErr.positionError();
        matchedChamber.x       = lPos.x();
        matchedChamber.y       = lPos.y();
        matchedChamber.xErr    = sqrt( localError.xx() );
        matchedChamber.yErr    = sqrt( localError.yy() );

        matchedChamber.dXdZ = lDir.z() != 0 ? lDir.x() / lDir.z() : 9999;
        matchedChamber.dYdZ = lDir.z() != 0 ? lDir.y() / lDir.z() : 9999;
        // DANGEROUS - compiler cannot guaranty parameters ordering
        AlgebraicSymMatrix55 trajectoryCovMatrix = lErr.matrix();
        matchedChamber.dXdZErr                   = trajectoryCovMatrix( 1, 1 ) > 0 ? sqrt( trajectoryCovMatrix( 1, 1 ) ) : 0;
        matchedChamber.dYdZErr                   = trajectoryCovMatrix( 2, 2 ) > 0 ? sqrt( trajectoryCovMatrix( 2, 2 ) ) : 0;

        matchedChamber.edgeX = chamber.localDistanceX;
        matchedChamber.edgeY = chamber.localDistanceY;

        matchedChamber.id = chamber.id;

        // if ( fillShowerDigis_ ) {
        //   theShowerDigiFiller_->fill( matchedChamber );
        // } else {
        //   theShowerDigiFiller_->fillDefault( matchedChamber );
        // }

        if ( !chamber.segments.empty() )
          ++nubmerOfMatchesAccordingToTrackAssociator;

        // fill segments
        for ( const auto& segment : chamber.segments ) {
          reco::MuonSegmentMatch matchedSegment;
          matchedSegment.x             = segment.segmentLocalPosition.x();
          matchedSegment.y             = segment.segmentLocalPosition.y();
          matchedSegment.dXdZ          = segment.segmentLocalDirection.z() ? segment.segmentLocalDirection.x() / segment.segmentLocalDirection.z() : 0;
          matchedSegment.dYdZ          = segment.segmentLocalDirection.z() ? segment.segmentLocalDirection.y() / segment.segmentLocalDirection.z() : 0;
          matchedSegment.xErr          = segment.segmentLocalErrorXX > 0 ? sqrt( segment.segmentLocalErrorXX ) : 0;
          matchedSegment.yErr          = segment.segmentLocalErrorYY > 0 ? sqrt( segment.segmentLocalErrorYY ) : 0;
          matchedSegment.dXdZErr       = segment.segmentLocalErrorDxDz > 0 ? sqrt( segment.segmentLocalErrorDxDz ) : 0;
          matchedSegment.dYdZErr       = segment.segmentLocalErrorDyDz > 0 ? sqrt( segment.segmentLocalErrorDyDz ) : 0;
          matchedSegment.t0            = segment.t0;
          matchedSegment.mask          = 0;
          matchedSegment.dtSegmentRef  = segment.dtSegmentRef;
          matchedSegment.cscSegmentRef = segment.cscSegmentRef;
          matchedSegment.gemSegmentRef = segment.gemSegmentRef;
          matchedSegment.me0SegmentRef = segment.me0SegmentRef;
          matchedSegment.hasZed_       = segment.hasZed;
          matchedSegment.hasPhi_       = segment.hasPhi;
          // test segment
          bool matchedX = false;
          bool matchedY = false;
          LogTrace( "MuonChamberMatchProducer" ) << " matching local x, segment x: " << matchedSegment.x << ", chamber x: " << matchedChamber.x << ", max: " << fMaxAbsDx;
          LogTrace( "MuonChamberMatchProducer" ) << " matching local y, segment y: " << matchedSegment.y << ", chamber y: " << matchedChamber.y << ", max: " << fMaxAbsDy;
          const double matchedSegChDx    = std::abs( matchedSegment.x - matchedChamber.x );
          const double matchedSegChDy    = std::abs( matchedSegment.y - matchedChamber.y );
          const double matchedSegChPullX = matchedSegChDx / std::hypot( matchedSegment.xErr, matchedChamber.xErr );
          const double matchedSegChPullY = matchedSegChDy / std::hypot( matchedSegment.yErr, matchedChamber.yErr );
          if ( matchedSegment.xErr > 0 && matchedChamber.xErr > 0 )
            LogTrace( "MuonChamberMatchProducer" ) << " xpull: " << matchedSegChPullX;
          if ( matchedSegment.yErr > 0 && matchedChamber.yErr > 0 )
            LogTrace( "MuonChamberMatchProducer" ) << " ypull: " << matchedSegChPullY;

          if ( matchedSegChDx < fMaxAbsDx ) matchedX = true;
          if ( matchedSegChDy < fMaxAbsDy ) matchedY = true;
          if ( matchedSegment.xErr > 0 && matchedChamber.xErr > 0 && matchedSegChPullX < fMaxAbsPullX ) matchedX = true;
          if ( matchedSegment.yErr > 0 && matchedChamber.yErr > 0 && matchedSegChPullY < fMaxAbsPullY ) matchedY = true;
          if ( matchedX && matchedY ) {
            if ( matchedChamber.id.subdetId() == MuonSubdetId::ME0 )
              matchedChamber.me0Matches.push_back( matchedSegment );
            else if ( matchedChamber.id.subdetId() == MuonSubdetId::GEM )
              matchedChamber.gemMatches.push_back( matchedSegment );
            else
              matchedChamber.segmentMatches.push_back( matchedSegment );
          }
        }
        if ( find_if( muonChamberMatches.begin(), muonChamberMatches.end(),
                      [&matchedChamber]( const auto& mcm ) { return mcm.id == matchedChamber.id; } ) == muonChamberMatches.end() )
          muonChamberMatches.push_back( matchedChamber );
      }
    }

    // Fill RPC info
    if ( fRPCHitHdl.isValid() ) {
      for ( const auto& chamber : info.chambers ) {
        if ( chamber.id.subdetId() != 3 ) continue;  // Consider RPC chambers only
        const auto& lErr = chamber.tState.localError();
        const auto& lPos = chamber.tState.localPosition();
        const auto& lDir = chamber.tState.localDirection();

        reco::MuonChamberMatch matchedChamber;

        LocalError localError = lErr.positionError();
        matchedChamber.x      = lPos.x();
        matchedChamber.y      = lPos.y();
        matchedChamber.xErr   = sqrt( localError.xx() );
        matchedChamber.yErr   = sqrt( localError.yy() );

        matchedChamber.dXdZ = lDir.z() != 0 ? lDir.x() / lDir.z() : 9999;
        matchedChamber.dYdZ = lDir.z() != 0 ? lDir.y() / lDir.z() : 9999;
        // DANGEROUS - compiler cannot guaranty parameters ordering
        AlgebraicSymMatrix55 trajectoryCovMatrix = lErr.matrix();
        matchedChamber.dXdZErr                   = trajectoryCovMatrix( 1, 1 ) > 0 ? sqrt( trajectoryCovMatrix( 1, 1 ) ) : 0;
        matchedChamber.dYdZErr                   = trajectoryCovMatrix( 2, 2 ) > 0 ? sqrt( trajectoryCovMatrix( 2, 2 ) ) : 0;

        matchedChamber.edgeX = chamber.localDistanceX;
        matchedChamber.edgeY = chamber.localDistanceY;

        // theShowerDigiFiller_->fillDefault( matchedChamber );

        matchedChamber.id = chamber.id;

        for ( const auto& rpcRecHit : *fRPCHitHdl ) {
          reco::MuonRPCHitMatch rpcHitMatch;

          if ( rpcRecHit.rawId() != chamber.id.rawId() ) continue;

          rpcHitMatch.x    = rpcRecHit.localPosition().x();
          rpcHitMatch.mask = 0;
          rpcHitMatch.bx   = rpcRecHit.BunchX();

          const double absDx = std::abs( rpcRecHit.localPosition().x() - chamber.tState.localPosition().x() );
          if ( absDx <= 20 or absDx / sqrt( localError.xx() ) <= 4 )
            matchedChamber.rpcMatches.push_back( rpcHitMatch );
        }

        muonChamberMatches.push_back( matchedChamber );
      }

      if ( fPropagateOpposite ) {
        // fill opposite chamberMatches
        for ( const auto& chamber : oppoInfo.chambers ) {
          if ( chamber.id.subdetId() != 3 ) continue;  // Consider RPC chambers only
          const auto& lErr = chamber.tState.localError();
          const auto& lPos = chamber.tState.localPosition();
          const auto& lDir = chamber.tState.localDirection();

          reco::MuonChamberMatch matchedChamber;

          LocalError localError = lErr.positionError();
          matchedChamber.x      = lPos.x();
          matchedChamber.y      = lPos.y();
          matchedChamber.xErr   = sqrt( localError.xx() );
          matchedChamber.yErr   = sqrt( localError.yy() );

          matchedChamber.dXdZ = lDir.z() != 0 ? lDir.x() / lDir.z() : 9999;
          matchedChamber.dYdZ = lDir.z() != 0 ? lDir.y() / lDir.z() : 9999;
          // DANGEROUS - compiler cannot guaranty parameters ordering
          AlgebraicSymMatrix55 trajectoryCovMatrix = lErr.matrix();
          matchedChamber.dXdZErr                   = trajectoryCovMatrix( 1, 1 ) > 0 ? sqrt( trajectoryCovMatrix( 1, 1 ) ) : 0;
          matchedChamber.dYdZErr                   = trajectoryCovMatrix( 2, 2 ) > 0 ? sqrt( trajectoryCovMatrix( 2, 2 ) ) : 0;

          matchedChamber.edgeX = chamber.localDistanceX;
          matchedChamber.edgeY = chamber.localDistanceY;

          // theShowerDigiFiller_->fillDefault( matchedChamber );

          matchedChamber.id = chamber.id;

          for ( const auto& rpcRecHit : *fRPCHitHdl ) {
            reco::MuonRPCHitMatch rpcHitMatch;

            if ( rpcRecHit.rawId() != chamber.id.rawId() ) continue;

            rpcHitMatch.x    = rpcRecHit.localPosition().x();
            rpcHitMatch.mask = 0;
            rpcHitMatch.bx   = rpcRecHit.BunchX();

            const double absDx = std::abs( rpcRecHit.localPosition().x() - chamber.tState.localPosition().x() );
            if ( absDx <= 20 or absDx / sqrt( localError.xx() ) <= 4 )
              matchedChamber.rpcMatches.push_back( rpcHitMatch );
          }

          if ( find_if( muonChamberMatches.begin(), muonChamberMatches.end(),
                        [&matchedChamber]( const auto& mcm ) { return mcm.id == matchedChamber.id; } ) == muonChamberMatches.end() )
            muonChamberMatches.push_back( matchedChamber );
        }
      }
    }

    v_mcm.push_back( muonChamberMatches );
  }

  ValueMap<vector<reco::MuonChamberMatch>>::Filler mcmFiller( *vm_mcm );
  mcmFiller.insert( fInputTrackHdl, v_mcm.begin(), v_mcm.end() );
  mcmFiller.fill();
  e.put( move( vm_mcm ) );
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( MuonChamberMatchProducer );
