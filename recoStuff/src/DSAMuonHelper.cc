#include "Firefighter/recoStuff/interface/DSAMuonHelper.h"

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "Geometry/CSCGeometry/interface/CSCChamber.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/DTGeometry/interface/DTChamber.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

std::vector<DTChamberId>
DSAMuonHelper::getDTDetIds( const std::vector<reco::MuonChamberMatch>& mcms ) {
  using namespace std;
  vector<DTChamberId> res{};

  for ( const auto& chambermatch : mcms ) {
    if ( chambermatch.detector() == MuonSubdetId::DT ) {
      DTChamberId dtId( chambermatch.id.rawId() );

      if ( find( res.begin(), res.end(), dtId ) == res.end() )
        res.push_back( dtId );
    }
  }

  sort( res.begin(), res.end() );
  return res;
}

std::vector<DTChamberId>
DSAMuonHelper::getDTDetIds( const reco::Muon& muon ) {
  return getDTDetIds( muon.matches() );
}

std::vector<DTChamberId>
DSAMuonHelper::getDTDetIds( const reco::Track&     track,
                            const edm::EventSetup& es ) {
  using namespace std;
  vector<DTChamberId> res{};

  edm::ESHandle<DTGeometry> dtG;
  es.get<MuonGeometryRecord>().get( dtG );

  for ( auto hitIter = track.recHitsBegin(); hitIter != track.recHitsEnd(); ++hitIter ) {
    const auto& hit = *( *hitIter );
    if ( !hit.isValid() ) continue;
    if ( hit.geographicalId().det() != DetId::Muon ) continue;
    if ( hit.geographicalId().subdetId() == MuonSubdetId::DT ) {
      const DTChamber* dtchamber = dtG->chamber( hit.geographicalId() );
      DTChamberId      chamberId = DTChamberId( dtchamber->id() );
      if ( find( res.begin(), res.end(), chamberId ) == res.end() )
        res.push_back( chamberId );
    }
  }

  sort( res.begin(), res.end() );
  return res;
}

std::vector<CSCDetId>
DSAMuonHelper::getCSCDetIds( const std::vector<reco::MuonChamberMatch>& mcms ) {
  using namespace std;
  vector<CSCDetId> res{};

  for ( const auto& chambermatch : mcms ) {
    if ( chambermatch.detector() == MuonSubdetId::CSC ) {
      CSCDetId cscId( chambermatch.id.rawId() );

      if ( find( res.begin(), res.end(), cscId ) == res.end() )
        res.push_back( cscId );
    }
  }

  sort( res.begin(), res.end() );
  return res;
}

std::vector<CSCDetId>
DSAMuonHelper::getCSCDetIds( const reco::Muon& muon ) {
  return getCSCDetIds( muon.matches() );
}

std::vector<CSCDetId>
DSAMuonHelper::getCSCDetIds( const reco::Track&     track,
                             const edm::EventSetup& es ) {
  using namespace std;
  vector<CSCDetId> res{};

  edm::ESHandle<CSCGeometry> cscG;
  es.get<MuonGeometryRecord>().get( cscG );

  for ( auto hitIter = track.recHitsBegin(); hitIter != track.recHitsEnd(); ++hitIter ) {
    const auto& hit = *( *hitIter );
    if ( !hit.isValid() ) continue;
    if ( hit.geographicalId().det() != DetId::Muon ) continue;
    if ( hit.geographicalId().subdetId() == MuonSubdetId::CSC ) {
      const CSCChamber* cscchamber = cscG->chamber( hit.geographicalId() );
      CSCDetId          chamberId  = CSCDetId( cscchamber->id() );
      if ( find( res.begin(), res.end(), chamberId ) == res.end() )
        res.push_back( chamberId );
    }
  }

  sort( res.begin(), res.end() );
  return res;
}

std::vector<RPCDetId>
DSAMuonHelper::getRPCDetIds( const reco::Track&     track,
                             const edm::EventSetup& es ) {
  using namespace std;
  vector<RPCDetId> res{};

  edm::ESHandle<RPCGeometry> rpcG;
  es.get<MuonGeometryRecord>().get( rpcG );

  for ( auto hitIter = track.recHitsBegin(); hitIter != track.recHitsEnd(); ++hitIter ) {
    const auto& hit = *( *hitIter );
    if ( !hit.isValid() ) continue;
    if ( hit.geographicalId().det() != DetId::Muon ) continue;
    if ( hit.geographicalId().subdetId() == MuonSubdetId::RPC ) {
      const RPCChamber* rpcchamber = rpcG->chamber( hit.geographicalId() );
      RPCDetId          chamberId  = RPCDetId( rpcchamber->id() );
      if ( find( res.begin(), res.end(), chamberId ) == res.end() )
        res.push_back( chamberId );
    }
  }

  sort( res.begin(), res.end() );
  return res;
}

bool
DSAMuonHelper::detIdsIsSubSetOfDTCSCIds( const reco::Track&                           track,
                                         const std::vector<std::vector<DTChamberId>>& dtids,
                                         const std::vector<std::vector<CSCDetId>>&    cscids,
                                         const edm::EventSetup&                       es ) {
  using namespace std;
  bool isSubsetPFMuon( false );

  vector<DTChamberId> _dtDetId  = getDTDetIds( track, es );
  vector<CSCDetId>    _cscDetId = getCSCDetIds( track, es );

  for ( pair<vector<vector<DTChamberId>>::const_iterator, vector<vector<CSCDetId>>::const_iterator> iter( dtids.begin(), cscids.begin() );
        iter.first != dtids.end() && iter.second != cscids.end();
        ++iter.first, ++iter.second ) {
    bool isDTSubsetPFMuon  = includes( iter.first->begin(), iter.first->end(), _dtDetId.begin(), _dtDetId.end() );
    bool isCSCSubsetPFMuon = includes( iter.second->begin(), iter.second->end(), _cscDetId.begin(), _cscDetId.end() );

    isSubsetPFMuon = isDTSubsetPFMuon && isCSCSubsetPFMuon;
    if ( isSubsetPFMuon )
      break;
  }

  return isSubsetPFMuon;
}

bool
DSAMuonHelper::detIdsIsSubSetOfVDetIds( const reco::Track&                     track,
                                        const std::vector<std::vector<DetId>>& dtids,
                                        const edm::EventSetup&                 es ) {
  using namespace std;
  bool res( false );

  vector<DetId> tkDetIds{};
  for ( auto did : getDTDetIds( track, es ) )
    tkDetIds.emplace_back( did );
  for ( auto did : getCSCDetIds( track, es ) )
    tkDetIds.emplace_back( did );
  sort( tkDetIds.begin(), tkDetIds.end() );

  for ( const auto& compDetIds : dtids ) {
    res = includes( compDetIds.begin(), compDetIds.end(), tkDetIds.begin(), tkDetIds.end() );
    if ( res ) break;
  }

  return res;
}

std::vector<DTRecSegment4DRef>
DSAMuonHelper::getDTSegments( const std::vector<reco::MuonChamberMatch>& mcms ) {
  using namespace std;
  vector<DTRecSegment4DRef> res{};

  for ( const auto& chambermatch : mcms ) {
    if ( chambermatch.detector() != MuonSubdetId::DT ) continue;
    for ( const auto& segmentmatch : chambermatch.segmentMatches ) {
      if ( segmentmatch.dtSegmentRef.isNonnull() )
        res.push_back( segmentmatch.dtSegmentRef );
    }
  }

  return res;
}

std::vector<DTRecSegment4DRef>
DSAMuonHelper::getDTSegments( const reco::Muon& muon ) {
  return getDTSegments( muon.matches() );
}

std::vector<CSCSegmentRef>
DSAMuonHelper::getCSCSegements( const std::vector<reco::MuonChamberMatch>& mcms ) {
  using namespace std;
  vector<CSCSegmentRef> res{};

  for ( const auto& chambermatch : mcms ) {
    if ( chambermatch.detector() != MuonSubdetId::CSC ) continue;
    for ( const auto& segmentmatch : chambermatch.segmentMatches ) {
      if ( segmentmatch.cscSegmentRef.isNonnull() )
        res.push_back( segmentmatch.cscSegmentRef );
    }
  }

  return res;
}

std::vector<CSCSegmentRef>
DSAMuonHelper::getCSCSegements( const reco::Muon& muon ) {
  return getCSCSegements( muon.matches() );
}

bool
DSAMuonHelper::segmentsIsSubsetOfDTCSCSegs( const std::vector<reco::MuonChamberMatch>&         mcms,
                                            const std::vector<std::vector<DTRecSegment4DRef>>& dtsegs,
                                            const std::vector<std::vector<CSCSegmentRef>>&     cscsegs ) {
  using namespace std;
  bool isSubset( false );

  vector<DTRecSegment4DRef> _dtSegRefs  = getDTSegments( mcms );
  vector<CSCSegmentRef>     _cscSegRefs = getCSCSegements( mcms );

  for ( pair<vector<vector<DTRecSegment4DRef>>::const_iterator, vector<vector<CSCSegmentRef>>::const_iterator> iter( dtsegs.begin(), cscsegs.begin() );
        iter.first != dtsegs.end() && iter.second != cscsegs.end();
        ++iter.first, ++iter.second ) {
    bool isDTSubset  = includes( iter.first->begin(), iter.first->end(), _dtSegRefs.begin(), _dtSegRefs.end() );
    bool isCSCSubset = includes( iter.second->begin(), iter.second->end(), _cscSegRefs.begin(), _cscSegRefs.end() );

    isSubset = isDTSubset && isCSCSubset;
    if ( isSubset ) break;
  }

  return isSubset;
}

bool
DSAMuonHelper::segmentsIsSubsetOfDTCSCSegs( const reco::Muon&                                  muon,
                                            const std::vector<std::vector<DTRecSegment4DRef>>& dtsegs,
                                            const std::vector<std::vector<CSCSegmentRef>>&     cscsegs ) {
  return segmentsIsSubsetOfDTCSCSegs( muon.matches(), dtsegs, cscsegs );
}

int
DSAMuonHelper::getDTSegmentHemisphere( const DTRecSegment4DRef& dtseg,
                                       const edm::EventSetup&   es ) {
  edm::ESHandle<DTGeometry> dtGeom;
  es.get<MuonGeometryRecord>().get( dtGeom );
  int hemi( 0 );  // 1: top, -1: bottom

  const DTChamber* dtchamber = dtGeom->chamber( dtseg->chamberId() );
  if ( dtchamber ) {
    GlobalPoint globalPosition = dtchamber->toGlobal( dtseg->localPosition() );
    if ( globalPosition.y() > 0 ) hemi = 1;
    if ( globalPosition.y() < 0 ) hemi = -1;
  }

  return hemi;
}

int
DSAMuonHelper::getCSCSegmentHemisphere( const CSCSegmentRef&   cscseg,
                                        const edm::EventSetup& es ) {
  edm::ESHandle<CSCGeometry> cscGeom;
  es.get<MuonGeometryRecord>().get( cscGeom );
  int hemi( 0 );  // 1: top, -1: bottom

  const CSCChamber* cscchamber = cscGeom->chamber( cscseg->cscDetId() );
  if ( cscchamber ) {
    GlobalPoint globalPosition = cscchamber->toGlobal( cscseg->localPosition() );
    if ( globalPosition.y() > 0 ) hemi = 1;
    if ( globalPosition.y() < 0 ) hemi = -1;
  }

  return hemi;
}

bool
DSAMuonHelper::bothHemisphereBySegments( const std::vector<reco::MuonChamberMatch>& mcms,
                                         const edm::EventSetup&                     es,
                                         int                                        minSegMinorSide ) {
  using namespace std;

  vector<int> dtSegHemi{};
  vector<int> cscSegHemi{};

  for ( const auto& dtseg : getDTSegments( mcms ) )
    dtSegHemi.push_back( getDTSegmentHemisphere( dtseg, es ) );
  for ( const auto& cscseg : getCSCSegements( mcms ) )
    cscSegHemi.push_back( getCSCSegmentHemisphere( cscseg, es ) );

  int numDTSegTop    = count( dtSegHemi.begin(), dtSegHemi.end(), 1 );
  int numDTSegBottom = count( dtSegHemi.begin(), dtSegHemi.end(), -1 );

  int numCSCSegTop    = count( cscSegHemi.begin(), cscSegHemi.end(), 1 );
  int numCSCSegBottom = count( cscSegHemi.begin(), cscSegHemi.end(), -1 );

  int numSegTop    = numDTSegTop + numCSCSegTop;
  int numSegBottom = numDTSegBottom + numCSCSegBottom;

  return min( numSegTop, numSegBottom ) >= minSegMinorSide;
}

bool
DSAMuonHelper::bothHemisphereBySegments( const reco::Muon&      muon,
                                         const edm::EventSetup& es,
                                         int                    minSegMinorSide ) {
  return bothHemisphereBySegments( muon.matches(), es, minSegMinorSide );
}