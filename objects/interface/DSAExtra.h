#ifndef objects_DSAExtra_h
#define objects_DSAExtra_h

#include <vector>

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"

class DSAExtra {
 public:
  DSAExtra()
      : fMaxPFMuonSegmentOverlapRatio( 0. ),
        fMinPFMuonExtrapolateToInnermostLocalDeltaR( 999. ),
        fIsDetIdSubsetOfAnyPFMuon( true ),
        fPFIso04( 999. ),
        fOppositeMuon( reco::MuonRef() ),
        fOppositeTimeDiffDtcsc( -999. ),
        fOppositeTimeDiffRpc( -999. ),
        fIsDetIdSubsetOfFilteredCosmic1Leg( false ),
        fMinDSADeltaRCosmic( 999. ),
        fMinDSADeltaRCosmicEtasum( 999. ),
        fMinDSADeltaRCosmicPhipidiff( 999. ),
        fMinSegDeltaRCosmic( 999. ),
        fMinSegDeltaRCosmicEtasum( 999. ),
        fMinSegDeltaRCosmicPhipidiff( 999. ){};
  ~DSAExtra() = default;


  // getter
  inline float pfmuon_maxSegmentOverlapRatio() const { return fMaxPFMuonSegmentOverlapRatio; }
  inline float pfmuon_minLocalDeltaRAtInnermost() const { return fMinPFMuonExtrapolateToInnermostLocalDeltaR; }
  inline bool  pfmuon_detIdSubsetOfAny() const { return fIsDetIdSubsetOfAnyPFMuon; }

  inline float         pfiso04() const { return fPFIso04; }
  inline reco::MuonRef oppositeMuon() const { return fOppositeMuon; }
  inline float         oppositeTimeDiff_dtcsc() const { return fOppositeTimeDiffDtcsc; }
  inline float         oppositeTimeDiff_rpc() const { return fOppositeTimeDiffRpc; }
  inline bool          cosmci1leg_detIdSubsetOfFiltered() const { return fIsDetIdSubsetOfFilteredCosmic1Leg; }

  inline float dsamuon_minDeltaRCosmic() const { return fMinDSADeltaRCosmic; }
  inline float dsamuon_minDeltaRCosmicEtasum() const { return fMinDSADeltaRCosmicEtasum; }
  inline float dsamuon_minDeltaRCosmicPhipidiff() const { return fMinDSADeltaRCosmicPhipidiff; }
  inline float segment_minDeltaRCosmic() const { return fMinSegDeltaRCosmic; }
  inline float segment_minDeltaRCosmicEtasum() const { return fMinSegDeltaRCosmicEtasum; }
  inline float segment_minDeltaRCosmicPhipidiff() const { return fMinSegDeltaRCosmicPhipidiff; }


  // setter
  inline void set_pfmuon_maxSegmentOverlapRatio( float ratio ) { fMaxPFMuonSegmentOverlapRatio = ratio; }
  inline void set_pfmuon_minLocalDeltaRAtInnermost( float dr ) { fMinPFMuonExtrapolateToInnermostLocalDeltaR = dr; }
  inline void set_pfmuon_detIdSubsetOfAny( bool issubset ) { fIsDetIdSubsetOfAnyPFMuon = issubset; }
  inline void set_pfiso04( float iso ) { fPFIso04 = iso; }
  inline void set_oppositeMuon( reco::MuonRef muon ) { fOppositeMuon = muon; }
  inline void set_oppositeTimeDiffDtcsc( float time ) { fOppositeTimeDiffDtcsc = time; }
  inline void set_oppositeTimeDiffRpc( float time ) { fOppositeTimeDiffRpc = time; }
  inline void set_cosmci1leg_detIdSubsetOfFiltered( bool issubset ) { fIsDetIdSubsetOfFilteredCosmic1Leg = issubset; }

  inline void set_dsamuon_minDeltaRCosmic( float v ) { fMinDSADeltaRCosmic = v; }
  inline void set_dsamuon_minDeltaRCosmicEtasum( float v ) { fMinDSADeltaRCosmicEtasum = v; }
  inline void set_dsamuon_minDeltaRCosmicPhipidiff( float v ) { fMinDSADeltaRCosmicPhipidiff = v; }
  inline void set_segment_minDeltaRCosmic( float v ) { fMinSegDeltaRCosmic = v; }
  inline void set_segment_minDeltaRCosmicEtasum( float v ) { fMinSegDeltaRCosmicEtasum = v; }
  inline void set_segment_minDeltaRCosmicPhipidiff( float v ) { fMinSegDeltaRCosmicPhipidiff = v; }

 private:
  // PFMuon overlap
  float fMaxPFMuonSegmentOverlapRatio;
  float fMinPFMuonExtrapolateToInnermostLocalDeltaR;
  bool  fIsDetIdSubsetOfAnyPFMuon;

  // isolation
  float fPFIso04;

  // cosmic veto
  reco::MuonRef fOppositeMuon;
  float         fOppositeTimeDiffDtcsc;
  float         fOppositeTimeDiffRpc;
  bool          fIsDetIdSubsetOfFilteredCosmic1Leg;
  // dr_cosmic, w/ DSA
  float fMinDSADeltaRCosmic;
  float fMinDSADeltaRCosmicEtasum;
  float fMinDSADeltaRCosmicPhipidiff;
  // dr_cosmic, w/ segments
  float fMinSegDeltaRCosmic;
  float fMinSegDeltaRCosmicEtasum;
  float fMinSegDeltaRCosmicPhipidiff;
};

using DSAExtraCollection = std::vector<DSAExtra>;

#endif