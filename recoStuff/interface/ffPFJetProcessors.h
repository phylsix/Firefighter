#ifndef recoStuff_ffPFJetProcessors_H
#define recoStuff_ffPFJetProcessors_H

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"

/**
 * group of "processors" that take a `const reco::PFJet&` (and some others)
 * as input and spit out stuff
 */

namespace ff {

std::vector<reco::PFCandidatePtr>
getPFCands( const reco::PFJet& );

std::vector<reco::PFCandidatePtr>
getChargedPFCands( const reco::PFJet& );

std::vector<reco::PFCandidatePtr>
getTrackEmbededPFCands( const reco::PFJet& );

std::vector<const reco::Track*>
getSelectedTracks( const reco::PFJet&,
                   const StringCutObjectSelector<reco::Track>& );

float
chargedMass( const reco::PFJet& );

bool
muonInTime( const reco::PFJet&, float );

int
getNumberOfDisplacedStandAloneMuons(
    const reco::PFJet&,
    const edm::Handle<reco::TrackCollection>& );

/**
 * @brief genralTracks Isolation
 *
 * pT of tracks in cone that NOT associated with the jet
 * ---------------------- over ----------------------
 * pT of tracks in cone that associated with the jet + above
 * --> The lower the value, the more isolated
 *
 * @return float
 */
float
getTkIsolation( const reco::PFJet&,
                const edm::Handle<reco::TrackCollection>&,
                const float& );

/**
 * @brief PFCandidate Isolation
 *
 * energy of candidates in cone that NOT associated with the jet
 * ---------------------- over ----------------------
 * energy of candidates in cone that associated with the jet + above
 * --> The lower the value, the more isolated
 *
 * @return float
 */
float
getPfIsolation( const reco::PFJet&,
                const edm::Handle<reco::PFCandidateCollection>&,
                const float& );

/**
 * @brief Neutral isolation
 *
 * energy of neutral candidates in clone (NOT associated with the jet)
 * ---------------------- over ----------------------
 * energy of candidates in cone associated with the jet + above
 * --> The lower value, the more isolated
 *
 * @return float
 */
float
getNeutralIsolation( const reco::PFJet&,
                     const edm::Handle<reco::PFCandidateCollection>&,
                     const float& );

std::vector<reco::TransientTrack>
transientTracksFromPFJet( const reco::PFJet&,
                          const StringCutObjectSelector<reco::Track>&,
                          const edm::EventSetup& );
}  // namespace ff

#endif