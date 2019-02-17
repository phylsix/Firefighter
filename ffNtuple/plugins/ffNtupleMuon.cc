#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonEnergy.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonIsolation.h"
#include "DataFormats/MuonReco/interface/MuonPFIsolation.h"
#include "DataFormats/MuonReco/interface/MuonQuality.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#define M_Mu 0.1056584
using LorentzVector = math::XYZTLorentzVectorF;

class ffNtupleMuon : public ffNtupleBase {
 public:
  ffNtupleMuon( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

 private:
  void clear() final;

  edm::EDGetToken muon_token_;
  int             muon_n_;

  // globalTrack
  std::vector<bool>                  muon_gfit_good_;
  math::XYZTLorentzVectorFCollection muon_gfit_p4_;
  std::vector<float>                 muon_gfit_normChi2_;
  std::vector<int>                   muon_gfit_nValStaHits_;
  std::vector<int>                   muon_gfit_tkQual_;
  std::vector<int>                   muon_gfit_algo_;
  std::vector<float>                 muon_gfit_d0Sig_;
  std::vector<float>                 muon_gfit_dzSig_;
  std::vector<float>                 muon_gfit_ptErr_;

  // innerTrack
  std::vector<bool>                  muon_ifit_good_;
  math::XYZTLorentzVectorFCollection muon_ifit_p4_;
  std::vector<float>                 muon_ifit_normChi2_;
  std::vector<int>                   muon_ifit_nValHits_;
  std::vector<int>                   muon_ifit_tkQual_;
  std::vector<int>                   muon_ifit_nLostHits_;
  std::vector<float>                 muon_ifit_d0Sig_;
  std::vector<float>                 muon_ifit_dzSig_;
  std::vector<float>                 muon_ifit_ptErr_;
  std::vector<int>                   muon_ifit_algo_;
  std::vector<int>                   muon_ifit_algoOrig_;
  std::vector<int>                   muon_ifit_nTkLyrs_;
  std::vector<int>                   muon_ifit_nValPxlHits_;

  // outerTrack
  std::vector<bool>                  muon_ofit_good_;
  math::XYZTLorentzVectorFCollection muon_ofit_p4_;
  std::vector<float>                 muon_ofit_normChi2_;
  std::vector<int>                   muon_ofit_nValStaHits_;
  std::vector<int>                   muon_ofit_tkQual_;
  std::vector<int>                   muon_ofit_algo_;
  std::vector<float>                 muon_ofit_ptErr_;
  std::vector<float>                 muon_ofit_d0Sig_;
  std::vector<float>                 muon_ofit_dzSig_;

  // muon quality
  std::vector<float> muon_qual_tkKink_;
  std::vector<float> muon_qual_chi2LocPos_;
  std::vector<float> muon_qual_chi2LocMom_;

  // muon id
  std::vector<float> muon_id_TMLastStationLoose_;
  std::vector<float> muon_id_TMLastStationTight_;
  std::vector<float> muon_id_TM2DCompatibilityLoose_;
  std::vector<float> muon_id_TM2DCompatibilityTight_;
  std::vector<float> muon_id_TMOneStationTight_;

  // MUON
  std::vector<int>                   muon_type_;
  std::vector<int>                   muon_bestTrackType_;
  std::vector<bool>                  muon_isPFMuon_;
  std::vector<int>                   muon_charge_;
  math::XYZTLorentzVectorFCollection muon_p4_;
  math::XYZTLorentzVectorFCollection muon_pfP4_;
  std::vector<float>                 muon_caloCompatibility_;
  std::vector<float>                 muon_segmCompatibility_;
  std::vector<int>                   muon_nMatchStations_;
  std::vector<float>                 muon_ecalTime_;
  std::vector<float>                 muon_hcalTime_;

  // isolation
  std::vector<float> muon_iso_tkPt_;
  std::vector<float> muon_iso_tkPtVeto_;
  std::vector<float> muon_iso_emEt_;
  std::vector<float> muon_iso_emEtVeto_;
  std::vector<float> muon_iso_hadEt_;
  std::vector<float> muon_iso_hadEtVeto_;
  std::vector<float> muon_iso_hoEt_;
  std::vector<float> muon_iso_hoEtVeto_;
  std::vector<float> muon_iso_nTks_;
  std::vector<float> muon_iso_nJets_;

  std::vector<float> muon_pfIso_chaHadPt_;
  std::vector<float> muon_pfIso_chaParPt_;
  std::vector<float> muon_pfIso_neuHadEt_;
  std::vector<float> muon_pfIso_photonEt_;
  std::vector<float> muon_pfIso_neuHadEt_highThres_;
  std::vector<float> muon_pfIso_photonEt_highThres_;
  std::vector<float> muon_pfIso_puPt_;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleMuon, "ffNtupleMuon" );

ffNtupleMuon::ffNtupleMuon( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleMuon::initialize( TTree&                   tree,
                          const edm::ParameterSet& ps,
                          edm::ConsumesCollector&& cc ) {
  muon_token_ = cc.consumes<reco::MuonCollection>(
      ps.getParameter<edm::InputTag>( "src" ) );

  tree.Branch( "muon_n", &muon_n_, "muon_n/I" );
  // globalTrack
  tree.Branch( "muon_gfit_good", &muon_gfit_good_ );
  tree.Branch( "muon_gfit_p4", &muon_gfit_p4_ );
  tree.Branch( "muon_gfit_normChi2", &muon_gfit_normChi2_ );
  tree.Branch( "muon_gfit_nValStaHits", &muon_gfit_nValStaHits_ );
  tree.Branch( "muon_gfit_tkQual", &muon_gfit_tkQual_ );
  tree.Branch( "muon_gfit_algo", &muon_gfit_algo_ );
  tree.Branch( "muon_gfit_d0Sig", &muon_gfit_d0Sig_ );
  tree.Branch( "muon_gfit_dzSig", &muon_gfit_dzSig_ );
  tree.Branch( "muon_gfit_ptErr", &muon_gfit_ptErr_ );

  // innerTrack
  tree.Branch( "muon_ifit_good", &muon_ifit_good_ );
  tree.Branch( "muon_ifit_p4", &muon_ifit_p4_ );
  tree.Branch( "muon_ifit_normChi2", &muon_ifit_normChi2_ );
  tree.Branch( "muon_ifit_nValHits", &muon_ifit_nValHits_ );
  tree.Branch( "muon_ifit_tkQual", &muon_ifit_tkQual_ );
  tree.Branch( "muon_ifit_nLostHits", &muon_ifit_nLostHits_ );
  tree.Branch( "muon_ifit_d0Sig", &muon_ifit_d0Sig_ );
  tree.Branch( "muon_ifit_dzSig", &muon_ifit_dzSig_ );
  tree.Branch( "muon_ifit_ptErr", &muon_ifit_ptErr_ );
  tree.Branch( "muon_ifit_algo", &muon_ifit_algo_ );
  tree.Branch( "muon_ifit_algoOrig", &muon_ifit_algoOrig_ );
  tree.Branch( "muon_ifit_nTkLyrs", &muon_ifit_nTkLyrs_ );
  tree.Branch( "muon_ifit_nValPxlHits", &muon_ifit_nValPxlHits_ );

  // outerTrack
  tree.Branch( "muon_ofit_good", &muon_ofit_good_ );
  tree.Branch( "muon_ofit_p4", &muon_ofit_p4_ );
  tree.Branch( "muon_ofit_normChi2", &muon_ofit_normChi2_ );
  tree.Branch( "muon_ofit_nValStaHits", &muon_ofit_nValStaHits_ );
  tree.Branch( "muon_ofit_tkQual", &muon_ofit_tkQual_ );
  tree.Branch( "muon_ofit_algo", &muon_ofit_algo_ );
  tree.Branch( "muon_ofit_ptErr", &muon_ofit_ptErr_ );
  tree.Branch( "muon_ofit_d0Sig", &muon_ofit_d0Sig_ );
  tree.Branch( "muon_ofit_dzSig", &muon_ofit_dzSig_ );

  // muon quality
  tree.Branch( "muon_qual_tkKink", &muon_qual_tkKink_ );
  tree.Branch( "muon_qual_chi2LocPos", &muon_qual_chi2LocPos_ );
  tree.Branch( "muon_qual_chi2LocMom", &muon_qual_chi2LocMom_ );

  // muon id
  tree.Branch( "muon_id_TMLastStationLoose", &muon_id_TMLastStationLoose_ );
  tree.Branch( "muon_id_TMLastStationTight", &muon_id_TMLastStationTight_ );
  tree.Branch( "muon_id_TM2DCompatibilityLoose",
               &muon_id_TM2DCompatibilityLoose_ );
  tree.Branch( "muon_id_TM2DCompatibilityTight",
               &muon_id_TM2DCompatibilityTight_ );
  tree.Branch( "muon_id_TMOneStationTight", &muon_id_TMOneStationTight_ );

  // MUON
  tree.Branch( "muon_type", &muon_type_ );
  tree.Branch( "muon_bestTrackType", &muon_bestTrackType_ );
  tree.Branch( "muon_isPFMuon", &muon_isPFMuon_ );
  tree.Branch( "muon_charge", &muon_charge_ );
  tree.Branch( "muon_p4", &muon_p4_ );
  tree.Branch( "muon_pfP4", &muon_pfP4_ );
  tree.Branch( "muon_caloCompatibility", &muon_caloCompatibility_ );
  tree.Branch( "muon_segmCompatibility", &muon_segmCompatibility_ );
  tree.Branch( "muon_nMatchStations", &muon_nMatchStations_ );
  tree.Branch( "muon_ecalTime", &muon_ecalTime_ );
  tree.Branch( "muon_hcalTime", &muon_hcalTime_ );

  // isolation
  tree.Branch( "muon_iso_tkPt", &muon_iso_tkPt_ );
  tree.Branch( "muon_iso_tkPtVeto", &muon_iso_tkPtVeto_ );
  tree.Branch( "muon_iso_emEt", &muon_iso_emEt_ );
  tree.Branch( "muon_iso_emEtVeto", &muon_iso_emEtVeto_ );
  tree.Branch( "muon_iso_hadEt", &muon_iso_hadEt_ );
  tree.Branch( "muon_iso_hadEtVeto", &muon_iso_hadEtVeto_ );
  tree.Branch( "muon_iso_hoEt", &muon_iso_hoEt_ );
  tree.Branch( "muon_iso_hoEtVeto", &muon_iso_hoEtVeto_ );
  tree.Branch( "muon_iso_nTks", &muon_iso_nTks_ );
  tree.Branch( "muon_iso_nJets", &muon_iso_nJets_ );

  tree.Branch( "muon_pfIso_chaHadPt", &muon_pfIso_chaHadPt_ );
  tree.Branch( "muon_pfIso_chaParPt", &muon_pfIso_chaParPt_ );
  tree.Branch( "muon_pfIso_neuHadEt", &muon_pfIso_neuHadEt_ );
  tree.Branch( "muon_pfIso_photonEt", &muon_pfIso_photonEt_ );
  tree.Branch( "muon_pfIso_neuHadEt_highThres",
               &muon_pfIso_neuHadEt_highThres_ );
  tree.Branch( "muon_pfIso_photonEt_highThres",
               &muon_pfIso_photonEt_highThres_ );
  tree.Branch( "muon_pfIso_puPt", &muon_pfIso_puPt_ );
}

void
ffNtupleMuon::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::MuonCollection> muon_h;
  e.getByToken( muon_token_, muon_h );
  assert( muon_h.isValid() );
  const reco::MuonCollection& muons = *muon_h;

  clear();

  muon_n_ = muons.size();
  for ( const auto& muon : muons ) {
    const reco::TrackRef globalTrack = muon.globalTrack();
    const reco::TrackRef innerTrack  = muon.innerTrack();
    const reco::TrackRef outerTrack  = muon.outerTrack();

    const reco::MuonQuality&     quality   = muon.combinedQuality();
    const reco::MuonEnergy&      calEnergy = muon.calEnergy();
    const reco::MuonIsolation&   isoR03    = muon.isolationR03();
    const reco::MuonPFIsolation& pfIsoR03  = muon.pfIsolationR03();

    // globalTrack
    muon_gfit_good_.emplace_back( globalTrack.isNonnull() );
    muon_gfit_p4_.push_back(
        globalTrack.isNonnull()
            ? LorentzVector( globalTrack->px(), globalTrack->py(),
                             globalTrack->pz(),
                             hypot( globalTrack->p(), M_Mu ) )
            : LorentzVector( NAN, NAN, NAN, NAN ) );
    muon_gfit_normChi2_.emplace_back(
        ( globalTrack.isNonnull() and globalTrack->ndof() != 0 )
            ? globalTrack->normalizedChi2()
            : NAN );
    muon_gfit_nValStaHits_.emplace_back(
        globalTrack.isNonnull()
            ? globalTrack->hitPattern().numberOfValidMuonHits()
            : -1 );
    muon_gfit_tkQual_.emplace_back(
        globalTrack.isNonnull() ? globalTrack->qualityMask() : -1 );
    muon_gfit_algo_.emplace_back( globalTrack.isNonnull() ? globalTrack->algo()
                                                          : -1 );
    muon_gfit_d0Sig_.emplace_back( globalTrack.isNonnull()
                                       ? fabs( globalTrack->d0() ) /
                                             globalTrack->d0Error()
                                       : NAN );
    muon_gfit_dzSig_.emplace_back( globalTrack.isNonnull()
                                       ? fabs( globalTrack->dz() ) /
                                             globalTrack->dzError()
                                       : NAN );
    muon_gfit_ptErr_.emplace_back(
        ( globalTrack.isNonnull() and globalTrack->charge() != 0 )
            ? globalTrack->ptError()
            : NAN );

    // innerTrack
    muon_ifit_good_.emplace_back( innerTrack.isNonnull() );
    muon_ifit_p4_.push_back(
        innerTrack.isNonnull()
            ? LorentzVector( innerTrack->px(), innerTrack->py(),
                             innerTrack->pz(), hypot( innerTrack->p(), M_Mu ) )
            : LorentzVector( NAN, NAN, NAN, NAN ) );
    muon_ifit_normChi2_.emplace_back(
        ( innerTrack.isNonnull() and innerTrack->ndof() != 0 )
            ? innerTrack->normalizedChi2()
            : NAN );
    muon_ifit_nValHits_.emplace_back(
        innerTrack.isNonnull() ? innerTrack->numberOfValidHits() : -1 );
    muon_ifit_tkQual_.emplace_back(
        innerTrack.isNonnull() ? innerTrack->qualityMask() : -1 );
    muon_ifit_nLostHits_.emplace_back(
        innerTrack.isNonnull() ? innerTrack->numberOfLostHits() : -1 );
    muon_ifit_d0Sig_.emplace_back( innerTrack.isNonnull()
                                       ? fabs( innerTrack->d0() ) /
                                             innerTrack->d0Error()
                                       : NAN );
    muon_ifit_dzSig_.emplace_back( innerTrack.isNonnull()
                                       ? fabs( innerTrack->dz() ) /
                                             innerTrack->dzError()
                                       : NAN );
    muon_ifit_ptErr_.emplace_back(
        ( innerTrack.isNonnull() and innerTrack->charge() != 0 )
            ? innerTrack->ptError()
            : NAN );
    muon_ifit_algo_.emplace_back( innerTrack.isNonnull() ? innerTrack->algo()
                                                         : -1 );
    muon_ifit_algoOrig_.emplace_back(
        innerTrack.isNonnull() ? innerTrack->originalAlgo() : -1 );
    muon_ifit_nTkLyrs_.emplace_back(
        innerTrack.isNonnull()
            ? innerTrack->hitPattern().trackerLayersWithMeasurement()
            : -1 );
    muon_ifit_nValPxlHits_.emplace_back(
        innerTrack.isNonnull()
            ? innerTrack->hitPattern().numberOfValidPixelHits()
            : -1 );

    // outerTrack
    muon_ofit_good_.emplace_back( outerTrack.isNonnull() );
    muon_ofit_p4_.push_back(
        outerTrack.isNonnull()
            ? LorentzVector( outerTrack->px(), outerTrack->py(),
                             outerTrack->pz(), hypot( outerTrack->p(), M_Mu ) )
            : LorentzVector( NAN, NAN, NAN, NAN ) );
    muon_ofit_normChi2_.emplace_back(
        ( outerTrack.isNonnull() and outerTrack->ndof() != 0 )
            ? outerTrack->normalizedChi2()
            : NAN );
    muon_ofit_nValStaHits_.emplace_back(
        outerTrack.isNonnull()
            ? outerTrack->hitPattern().numberOfValidMuonHits()
            : -1 );
    muon_ofit_tkQual_.emplace_back(
        outerTrack.isNonnull() ? outerTrack->qualityMask() : -1 );
    muon_ofit_algo_.emplace_back( outerTrack.isNonnull() ? outerTrack->algo()
                                                         : -1 );
    muon_ofit_ptErr_.emplace_back(
        ( outerTrack.isNonnull() and outerTrack->charge() != 0 )
            ? outerTrack->ptError()
            : NAN );
    muon_ofit_d0Sig_.emplace_back( outerTrack.isNonnull()
                                       ? fabs( outerTrack->d0() ) /
                                             outerTrack->d0Error()
                                       : NAN );
    muon_ofit_dzSig_.emplace_back( outerTrack.isNonnull()
                                       ? fabs( outerTrack->dz() ) /
                                             outerTrack->dzError()
                                       : NAN );

    // muon quality --
    // http://cmslxr.fnal.gov/source/DataFormats/MuonReco/interface/MuonQuality.h
    bool _isQualityValid = muon.isQualityValid();
    muon_qual_tkKink_.emplace_back(
        _isQualityValid ? quality.trkKink
                        : NAN );  // value of the kink algorithm applied to the
                                  // inner track stub
    muon_qual_chi2LocPos_.emplace_back(
        _isQualityValid
            ? quality.chi2LocalPosition
            : NAN );  // chi2 value for the STA-TK matching of local position
    muon_qual_chi2LocMom_.emplace_back(
        _isQualityValid
            ? quality.chi2LocalMomentum
            : NAN );  // chi2 value for the STA-TK matching of local momentum

    // muon id --
    // http://cmslxr.fnal.gov/source/DataFormats/MuonReco/interface/MuonSelectors.h
    muon_id_TMLastStationLoose_.emplace_back(
        muon.isMatchesValid()
            ? muon::isGoodMuon( muon, muon::TMLastStationLoose )
            : NAN );
    muon_id_TMLastStationTight_.emplace_back(
        muon.isMatchesValid()
            ? muon::isGoodMuon( muon, muon::TMLastStationTight )
            : NAN );
    muon_id_TM2DCompatibilityLoose_.emplace_back(
        muon.isMatchesValid()
            ? muon::isGoodMuon( muon, muon::TM2DCompatibilityLoose )
            : NAN );
    muon_id_TM2DCompatibilityTight_.emplace_back(
        muon.isMatchesValid()
            ? muon::isGoodMuon( muon, muon::TM2DCompatibilityTight )
            : NAN );
    muon_id_TMOneStationTight_.emplace_back(
        muon.isMatchesValid()
            ? muon::isGoodMuon( muon, muon::TMOneStationTight )
            : NAN );

    // MUON
    muon_type_.emplace_back( muon.type() );
    muon_bestTrackType_.emplace_back( muon.muonBestTrackType() );
    muon_isPFMuon_.emplace_back( muon.isPFMuon() );
    muon_charge_.emplace_back( muon.charge() );
    muon_p4_.push_back(
        LorentzVector( muon.px(), muon.py(), muon.pz(), muon.energy() ) );
    muon_pfP4_.push_back( LorentzVector( muon.pfP4().px(), muon.pfP4().py(),
                                         muon.pfP4().pz(),
                                         muon.pfP4().energy() ) );
    muon_caloCompatibility_.emplace_back(
        muon.caloCompatibility() );  /// Relative likelihood based on ECAL,
                                     /// HCAL, HO energy defined as
                                     /// L_muon/(L_muon+L_not_muon)
    muon_segmCompatibility_.emplace_back( muon::segmentCompatibility( muon ) );
    muon_nMatchStations_.emplace_back( muon.numberOfMatchedStations() );

    muon_ecalTime_.emplace_back( muon.isEnergyValid() ? calEnergy.ecal_time
                                                      : NAN );
    muon_hcalTime_.emplace_back( muon.isEnergyValid() ? calEnergy.hcal_time
                                                      : NAN );

    // isolation
    bool _isIsoValid = muon.isIsolationValid();
    muon_iso_tkPt_.emplace_back( _isIsoValid ? isoR03.sumPt : NAN );
    muon_iso_tkPtVeto_.emplace_back( _isIsoValid ? isoR03.trackerVetoPt : NAN );
    muon_iso_emEt_.emplace_back( _isIsoValid ? isoR03.emEt : NAN );
    muon_iso_emEtVeto_.emplace_back( _isIsoValid ? isoR03.emVetoEt : NAN );
    muon_iso_hadEt_.emplace_back( _isIsoValid ? isoR03.hadEt : NAN );
    muon_iso_hadEtVeto_.emplace_back( _isIsoValid ? isoR03.hadVetoEt : NAN );
    muon_iso_hoEt_.emplace_back( _isIsoValid ? isoR03.hoEt : NAN );
    muon_iso_hoEtVeto_.emplace_back( _isIsoValid ? isoR03.hoVetoEt : NAN );
    muon_iso_nTks_.emplace_back( _isIsoValid ? isoR03.nTracks : -1 );
    muon_iso_nJets_.emplace_back( _isIsoValid ? isoR03.nJets : -1 );

    bool _isPFIsoValid = muon.isPFIsolationValid();
    muon_pfIso_chaHadPt_.emplace_back(
        _isPFIsoValid ? pfIsoR03.sumChargedHadronPt : NAN );
    muon_pfIso_chaParPt_.emplace_back(
        _isPFIsoValid ? pfIsoR03.sumChargedParticlePt : NAN );
    muon_pfIso_neuHadEt_.emplace_back(
        _isPFIsoValid ? pfIsoR03.sumNeutralHadronEt : NAN );
    muon_pfIso_photonEt_.emplace_back( _isPFIsoValid ? pfIsoR03.sumPhotonEt
                                                     : NAN );
    muon_pfIso_neuHadEt_highThres_.emplace_back(
        _isPFIsoValid ? pfIsoR03.sumNeutralHadronEtHighThreshold : NAN );
    muon_pfIso_photonEt_highThres_.emplace_back(
        _isPFIsoValid ? pfIsoR03.sumPhotonEtHighThreshold : NAN );
    muon_pfIso_puPt_.emplace_back( _isPFIsoValid ? pfIsoR03.sumPUPt : NAN );
  }
}

void
ffNtupleMuon::clear() {
  muon_n_ = 0;
  // globalTrack
  muon_gfit_good_.clear();
  muon_gfit_p4_.clear();
  muon_gfit_normChi2_.clear();
  muon_gfit_nValStaHits_.clear();
  muon_gfit_tkQual_.clear();
  muon_gfit_algo_.clear();
  muon_gfit_d0Sig_.clear();
  muon_gfit_dzSig_.clear();
  muon_gfit_ptErr_.clear();

  // innerTrack
  muon_ifit_good_.clear();
  muon_ifit_p4_.clear();
  muon_ifit_normChi2_.clear();
  muon_ifit_nValHits_.clear();
  muon_ifit_tkQual_.clear();
  muon_ifit_nLostHits_.clear();
  muon_ifit_d0Sig_.clear();
  muon_ifit_dzSig_.clear();
  muon_ifit_ptErr_.clear();
  muon_ifit_algo_.clear();
  muon_ifit_algoOrig_.clear();
  muon_ifit_nTkLyrs_.clear();
  muon_ifit_nValPxlHits_.clear();

  // outerTrack
  muon_ofit_good_.clear();
  muon_ofit_p4_.clear();
  muon_ofit_normChi2_.clear();
  muon_ofit_nValStaHits_.clear();
  muon_ofit_tkQual_.clear();
  muon_ofit_algo_.clear();
  muon_ofit_ptErr_.clear();
  muon_ofit_d0Sig_.clear();
  muon_ofit_dzSig_.clear();

  // muon quality
  muon_qual_tkKink_.clear();
  muon_qual_chi2LocPos_.clear();
  muon_qual_chi2LocMom_.clear();

  // muon id
  muon_id_TMLastStationLoose_.clear();
  muon_id_TMLastStationTight_.clear();
  muon_id_TM2DCompatibilityLoose_.clear();
  muon_id_TM2DCompatibilityTight_.clear();
  muon_id_TMOneStationTight_.clear();

  // MUON
  muon_type_.clear();
  muon_bestTrackType_.clear();
  muon_isPFMuon_.clear();
  muon_charge_.clear();
  muon_p4_.clear();
  muon_pfP4_.clear();
  muon_caloCompatibility_.clear();
  muon_segmCompatibility_.clear();
  muon_nMatchStations_.clear();
  muon_ecalTime_.clear();
  muon_hcalTime_.clear();

  // isolation
  muon_iso_tkPt_.clear();
  muon_iso_tkPtVeto_.clear();
  muon_iso_emEt_.clear();
  muon_iso_emEtVeto_.clear();
  muon_iso_hadEt_.clear();
  muon_iso_hadEtVeto_.clear();
  muon_iso_hoEt_.clear();
  muon_iso_hoEtVeto_.clear();
  muon_iso_nTks_.clear();
  muon_iso_nJets_.clear();

  muon_pfIso_chaHadPt_.clear();
  muon_pfIso_chaParPt_.clear();
  muon_pfIso_neuHadEt_.clear();
  muon_pfIso_photonEt_.clear();
  muon_pfIso_neuHadEt_highThres_.clear();
  muon_pfIso_photonEt_highThres_.clear();
  muon_pfIso_puPt_.clear();
}

namespace {
/// ***********************************************
enum TrackAlgorithm {
  undefAlgorithm       = 0,
  ctf                  = 1,
  duplicateMerge       = 2,
  cosmics              = 3,
  initialStep          = 4,
  lowPtTripletStep     = 5,
  pixelPairStep        = 6,
  detachedTripletStep  = 7,
  mixedTripletStep     = 8,
  pixelLessStep        = 9,
  tobTecStep           = 10,
  jetCoreRegionalStep  = 11,
  conversionStep       = 12,
  muonSeededStepInOut  = 13,
  muonSeededStepOutIn  = 14,
  outInEcalSeededConv  = 15,
  inOutEcalSeededConv  = 16,
  nuclInter            = 17,
  standAloneMuon       = 18,
  globalMuon           = 19,
  cosmicStandAloneMuon = 20,
  cosmicGlobalMuon     = 21,
  // Phase1
  highPtTripletStep    = 22,
  lowPtQuadStep        = 23,
  detachedQuadStep     = 24,
  reservedForUpgrades1 = 25,
  reservedForUpgrades2 = 26,
  bTagGhostTracks      = 27,
  beamhalo             = 28,
  gsf                  = 29,
  // HLT algo name
  hltPixel = 30,
  // steps used by PF
  hltIter0 = 31,
  hltIter1 = 32,
  hltIter2 = 33,
  hltIter3 = 34,
  hltIter4 = 35,
  // steps used by all other objects @HLT
  hltIterX = 36,
  // steps used by HI muon regional iterative tracking
  hiRegitMuInitialStep         = 37,
  hiRegitMuLowPtTripletStep    = 38,
  hiRegitMuPixelPairStep       = 39,
  hiRegitMuDetachedTripletStep = 40,
  hiRegitMuMixedTripletStep    = 41,
  hiRegitMuPixelLessStep       = 42,
  hiRegitMuTobTecStep          = 43,
  hiRegitMuMuonSeededStepInOut = 44,
  hiRegitMuMuonSeededStepOutIn = 45,
  algoSize                     = 46
};

/// ***********************************************
enum TrackQuality {
  undefQuality        = -1,
  loose               = 0,
  tight               = 1,
  highPurity          = 2,
  confirmed           = 3,  // means found by more than one iteration
  goodIterative       = 4,  // meaningless
  looseSetWithPV      = 5,
  highPuritySetWithPV = 6,
  discarded = 7,  // because a better track found. kept in the collection for
                  // reference....
  qualitySize = 8
};

/// ***********************************************
enum MuonTrackType {
  None,
  InnerTrack,
  OuterTrack,
  CombinedTrack,
  TPFMS,
  Picky,
  DYT
};

/// ***********************************************
enum ArbitrationType {
  NoArbitration,
  SegmentArbitration,
  SegmentAndTrackArbitration,
  SegmentAndTrackArbitrationCleaned,
  RPCHitAndTrackArbitration,
  GEMSegmentAndTrackArbitration,
  ME0SegmentAndTrackArbitration
};

/// ***********************************************
enum Selector {
  CutBasedIdLoose        = 1UL << 0,
  CutBasedIdMedium       = 1UL << 1,
  CutBasedIdMediumPrompt = 1UL << 2,  // medium with IP cuts
  CutBasedIdTight        = 1UL << 3,
  CutBasedIdGlobalHighPt =
      1UL << 4,  // high pt muon for Z',W' (better momentum resolution)
  CutBasedIdTrkHighPt = 1UL
                        << 5,  // high pt muon for boosted Z (better efficiency)
  PFIsoVeryLoose     = 1UL << 6,   // reliso<0.40
  PFIsoLoose         = 1UL << 7,   // reliso<0.25
  PFIsoMedium        = 1UL << 8,   // reliso<0.20
  PFIsoTight         = 1UL << 9,   // reliso<0.15
  PFIsoVeryTight     = 1UL << 10,  // reliso<0.10
  TkIsoLoose         = 1UL << 11,  // reliso<0.10
  TkIsoTight         = 1UL << 12,  // reliso<0.05
  SoftCutBasedId     = 1UL << 13,
  SoftMvaId          = 1UL << 14,
  MvaLoose           = 1UL << 15,
  MvaMedium          = 1UL << 16,
  MvaTight           = 1UL << 17,
  MiniIsoLoose       = 1UL << 18,  // reliso<0.40
  MiniIsoMedium      = 1UL << 19,  // reliso<0.20
  MiniIsoTight       = 1UL << 20,  // reliso<0.10
  MiniIsoVeryTight   = 1UL << 21,  // reliso<0.05
  TriggerIdLoose     = 1UL << 22,  // robust selector for HLT
  InTimeMuon         = 1UL << 23,
  PFIsoVeryVeryTight = 1UL << 24,  // reliso<0.05
  MultiIsoLoose      = 1UL << 25,  // miniIso with ptRatio and ptRel
  MultiIsoMedium     = 1UL << 26   // miniIso with ptRatio and ptRel
};

/// ***********************************************
/// muon type - type of the algorithm that reconstructed this muon
/// multiple algorithms can reconstruct the same muon
enum MuonType {
  GlobalMuon     = 1 << 1,
  TrackerMuon    = 1 << 2,
  StandAloneMuon = 1 << 3,
  CaloMuon       = 1 << 4,
  PFMuon         = 1 << 5,
  RPCMuon        = 1 << 6,
  GEMMuon        = 1 << 7,
  ME0Muon        = 1 << 8
};
}  // namespace