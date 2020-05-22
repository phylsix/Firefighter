#include "Firefighter/recoStuff/interface/LeptonjetSourceDSAMuonProducer.h"

#include "DataFormats/Math/interface/deltaR.h"

LeptonjetSourceDSAMuonProducer::LeptonjetSourceDSAMuonProducer( const edm::ParameterSet& ps )
    : fDSACandsToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "pfcandsFromMuondSAPtr" ) ) ),
      fDSAExtraToken (consumes<edm::ValueMap<DSAExtra>>(edm::InputTag("dsamuonExtra"))),
      fMinDTTimeDiff( ps.getParameter<double>( "minDTTimeDiff" ) ),
      fMinRPCTimeDiff( ps.getParameter<double>( "minRPCTimeDiff" ) ) {
  produces<reco::PFCandidateFwdPtrVector>( "inclusive" );
  produces<reco::PFCandidateFwdPtrVector>( "nonisolated" );
}

LeptonjetSourceDSAMuonProducer::~LeptonjetSourceDSAMuonProducer() = default;

void
LeptonjetSourceDSAMuonProducer::produce( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  auto inclusiveColl   = make_unique<reco::PFCandidateFwdPtrVector>();
  auto nonisolatedColl = make_unique<reco::PFCandidateFwdPtrVector>();

  e.getByToken( fDSACandsToken, fDSACandsHdl );
  assert( fDSACandsHdl.isValid() );
  e.getByToken(fDSAExtraToken, fDSAExtraHdl);
  assert(fDSAExtraHdl.isValid());


  for ( const auto& candfwdptr : *fDSACandsHdl ) {
    const auto& candptr  = candfwdptr.ptr();
    const auto& muonref  = candptr->muonRef();
    const auto& trackref = muonref->outerTrack();

    if ( candptr.isNull() or muonref.isNull() or trackref.isNull() ) continue;

    const auto& hitpattern = trackref->hitPattern();

    //pre -id
    if ( ( hitpattern.cscStationsWithValidHits() + hitpattern.dtStationsWithValidHits() ) < 2 ) continue;
    if ( ( hitpattern.numberOfValidMuonCSCHits() + hitpattern.numberOfValidMuonDTHits() ) < 13 ) continue;
    if ( ( trackref->ptError() / trackref->pt() ) > 1. ) continue;

    const auto& dsaExtra = (*fDSAExtraHdl)[muonref];

    //loose iso. ref: https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideMuonIdRun2#Particle_Flow_isolation
    // wsi 21/05/20 - drop iso cut
    // if ( dsaExtra.pfiso04() > 0.25 ) continue; // 0.4, 0.25, 0.20, 0.15, 0.10, 0.05


    //matching with loose PFMuon
    if ( dsaExtra.pfmuon_maxSegmentOverlapRatio() > 0.66 ) continue;
    if ( dsaExtra.pfmuon_minLocalDeltaRAtInnermost() < 0.2 ) continue;
    if ( dsaExtra.pfmuon_detIdSubsetOfAny() ) continue;

    //further -id
    if ( trackref->pt() < 10. ) continue;
    if ( fabs( trackref->eta() ) > 2.4 ) continue;
    if ( trackref->normalizedChi2() > 4 ) continue;
    if ( hitpattern.numberOfValidMuonCSCHits() == 0 and hitpattern.numberOfValidMuonDTHits() <= 18 ) continue;

    // reject cosmic-like
    // if (dsaExtra.oppositeMuon().isNonnull() && !(dsaExtra.oppositeTimeDiff_dtcsc()>fMinDTTimeDiff && dsaExtra.oppositeTimeDiff_rpc()>fMinRPCTimeDiff)) continue;

    inclusiveColl->push_back( candfwdptr );
  }

  for ( const auto& candfwdptr : *inclusiveColl ) {
    for ( const auto& candfwdptr2 : *inclusiveColl ) {
      if ( candfwdptr.ptr() == candfwdptr2.ptr() )
        continue;
      if ( deltaR( *( candfwdptr.ptr() ), *( candfwdptr2.ptr() ) ) > 0.4 )
        continue;
      if ( ( candfwdptr.ptr()->charge() * candfwdptr2.ptr()->charge() ) != -1 )
        continue;
      nonisolatedColl->push_back( candfwdptr );
      break;
    }
  }

  e.put( move( inclusiveColl ), "inclusive" );
  e.put( move( nonisolatedColl ), "nonisolated" );
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( LeptonjetSourceDSAMuonProducer );
