#ifndef recoStuff_PFCandidateConverter_H
#define recoStuff_PFCandidateConverter_H

#include "CommonTools/RecoAlgos/src/CandidateProducer.h"
#include "CommonTools/RecoAlgos/src/MassiveCandidateConverter.h"
#include "CommonTools/UtilAlgos/interface/MasterCollectionHelper.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"

namespace ff {

struct TrackToPFCandidate : public converter::MassiveCandidateConverter {
  TrackToPFCandidate( const edm::ParameterSet& cfg )
      : MassiveCandidateConverter( cfg ) {}

  void convert( reco::TrackRef trkRef, reco::PFCandidate& c ) const {
    const reco::Track& trk = *trkRef;
    c.setCharge( trk.charge() );
    c.setVertex( trk.vertex() );
    const reco::Track::Vector& p = trk.momentum();
    double                     t = sqrt( massSqr_ + p.mag2() );
    c.setP4( reco::Candidate::LorentzVector( p.x(), p.y(), p.z(), t ) );
    c.setTrackRef( trkRef );
    c.setPdgId( abs( particle_.pdgId() ) * trk.charge() );
    c.setTime( 0., 0. );  // make sure isTimeValid() == true
  }
};

struct TrackToPFCandConcreteCreator {
  static void create(
      size_t                                                    idx,
      reco::PFCandidateCollection&                              cands,
      const ::helper::MasterCollection<edm::View<reco::Track>>& components,
      ff::TrackToPFCandidate&                                   converter ) {
    reco::PFCandidate c = reco::PFCandidate();
    using ref_type      = edm::Ref<std::vector<reco::Track>>;

    ref_type ref = components.getConcreteRef<ref_type>( idx );
    converter.convert( ref, c );
    cands.push_back( c );
  }
};

//*******************************************************

struct MuonToPFCandidate : public converter::MassiveCandidateConverter {
  MuonToPFCandidate( const edm::ParameterSet& cfg )
      : MassiveCandidateConverter( cfg ) {}

  void convert( reco::MuonRef muonRef, reco::PFCandidate& c ) const {
    const reco::Muon& mu = *muonRef;
    c.setCharge( mu.charge() );
    c.setVertex( mu.vertex() );
    c.setP4( mu.p4() );
    c.setMuonRef( muonRef );  // This has to come before setTrackRef. Because
                              // the method requires the c.trackRef() to be the
                              // same as mu.track(). In this order, both are
                              // null initially, so they are equal, and this is
                              // the way to get around.
    c.setTrackRef( mu.muonBestTrack() );
    c.setPdgId( abs( particle_.pdgId() ) * mu.charge() );
    c.setTime( 0., 0. );  // make sure isTimeValid() == true
  }
};

struct MuonToPFCandConcreteCreator {
  static void create(
      size_t                                                   idx,
      reco::PFCandidateCollection&                             cands,
      const ::helper::MasterCollection<edm::View<reco::Muon>>& components,
      ff::MuonToPFCandidate&                                   converter ) {
    reco::PFCandidate c = reco::PFCandidate();
    using ref_type      = edm::Ref<std::vector<reco::Muon>>;
    ref_type ref        = components.getConcreteRef<ref_type>( idx );
    converter.convert( ref, c );
    cands.push_back( c );
  }
};

}  // namespace ff

#endif
