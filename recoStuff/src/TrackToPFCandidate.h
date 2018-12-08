#ifndef recoStuff_TrackToPFCandidate_H
#define recoStuff_TrackToPFCandidate_H

#include "CommonTools/RecoAlgos/src/MassiveCandidateConverter.h"
#include "CommonTools/RecoAlgos/src/CandidateProducer.h"
#include "CommonTools/UtilAlgos/interface/MasterCollectionHelper.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"

namespace ff {

  struct TrackToPFCandidate : public converter::MassiveCandidateConverter {

    TrackToPFCandidate(const edm::ParameterSet & cfg) :
      MassiveCandidateConverter(cfg) {
    }

    void convert(reco::TrackRef trkRef, reco::PFCandidate & c) const {
      const reco::Track & trk = * trkRef;
      c.setCharge(trk.charge());
      c.setVertex(trk.vertex());
      const reco::Track::Vector& p = trk.momentum();
      double t = sqrt(massSqr_ + p.mag2());
      c.setP4(reco::Candidate::LorentzVector(p.x(), p.y(), p.z(), t));
      c.setTrackRef(trkRef);
      c.setPdgId(abs(particle_.pdgId()) * trk.charge());
      c.setTime(0., 0.); // make sure isTimeValid() == true
    }

  };

  struct ConcreteCreator {
    static void create(size_t idx,
                       reco::PFCandidateCollection& cands,
                       const ::helper::MasterCollection<edm::View<reco::Track>>& components,
                       ff::TrackToPFCandidate& converter)
    {
      reco::PFCandidate c = reco::PFCandidate();
      using ref_type = edm::Ref<std::vector<reco::Track>>;

      ref_type ref = components.getConcreteRef<ref_type>(idx);
      converter.convert(ref, c);
      cands.push_back(c);
    }
  };


}


#endif