#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/Math/interface/Point3D.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/TrackExtraFwd.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"

#define M_Mu 0.1056584
using Point = math::XYZPointF;
using LorentzVector = math::XYZTLorentzVectorF;

class ffNtupleDsaMuon : public ffNtupleBase
{
  public:
    ffNtupleDsaMuon(const edm::ParameterSet&);

    void initialize(TTree&, const edm::ParameterSet&, edm::ConsumesCollector&&) final;
    void fill(const edm::Event&, const edm::EventSetup&) final;
    void fill(const edm::Event&, const edm::EventSetup&, HLTConfigProvider&) override {}

  private:
    void clear() final;

    bool useMuonHypothesis_;
    edm::EDGetToken dsamuon_token_;

    int dsamuon_n_;
    math::XYZTLorentzVectorFCollection dsamuon_p4_;
    std::vector<float> dsamuon_normChi2_   ;
    std::vector<int>   dsamuon_nValStaHits_;
    std::vector<int>   dsamuon_nLosStaHits_;
    std::vector<int>   dsamuon_tkQual_     ;
    std::vector<int>   dsamuon_algo_       ;
    std::vector<int>   dsamuon_algoOrig_   ;
    std::vector<float> dsamuon_ptErr_      ;
    std::vector<float> dsamuon_d0Sig_      ;
    std::vector<float> dsamuon_dzSig_      ;
    std::vector<int>   dsamuon_charge_     ;
    std::vector<Point> dsamuon_refPoint_   ;
    
    std::vector<Point> dsamuon_innerPos_   ;
    std::vector<Point> dsamuon_outerPos_   ;
    math::XYZTLorentzVectorFCollection dsamuon_innerP4_;
    math::XYZTLorentzVectorFCollection dsamuon_outerP4_;

};

DEFINE_EDM_PLUGIN(ffNtupleFactory,
                  ffNtupleDsaMuon,
                  "ffNtupleDsaMuon");

ffNtupleDsaMuon::ffNtupleDsaMuon(const edm::ParameterSet& ps) :
  ffNtupleBase(ps)
{}

void
ffNtupleDsaMuon::initialize(TTree& tree,
                            const edm::ParameterSet& ps,
                            edm::ConsumesCollector&& cc)
{
  useMuonHypothesis_ = ps.getParameter<bool>("UseMuonHypothesis");
  dsamuon_token_ = cc.consumes<reco::TrackCollection>(ps.getParameter<edm::InputTag>("src"));
  tree.Branch("dsamuon_n", &dsamuon_n_, "dsamuon_n/I");
  tree.Branch("dsamuon_p4",          &dsamuon_p4_         );
  tree.Branch("dsamuon_normChi2",    &dsamuon_normChi2_   );
  tree.Branch("dsamuon_nValStaHits", &dsamuon_nValStaHits_);
  tree.Branch("dsamuon_nLosStaHits", &dsamuon_nLosStaHits_);
  tree.Branch("dsamuon_tkQual",      &dsamuon_tkQual_     );
  tree.Branch("dsamuon_algo",        &dsamuon_algo_       );
  tree.Branch("dsamuon_algoOrig",    &dsamuon_algoOrig_   );
  tree.Branch("dsamuon_ptErr",       &dsamuon_ptErr_      );
  tree.Branch("dsamuon_d0Sig",       &dsamuon_d0Sig_      );
  tree.Branch("dsamuon_dzSig",       &dsamuon_dzSig_      );
  tree.Branch("dsamuon_charge",      &dsamuon_charge_     );
  tree.Branch("dsamuon_refPoint",    &dsamuon_refPoint_   );
  tree.Branch("dsamuon_innerPos_",   &dsamuon_innerPos_   );
  tree.Branch("dsamuon_outerPos_",   &dsamuon_outerPos_   );
  tree.Branch("dsamuon_innerP4_",    &dsamuon_innerP4_    );
  tree.Branch("dsamuon_outerP4_",    &dsamuon_outerP4_    );
}

void
ffNtupleDsaMuon::fill(const edm::Event& e,
                      const edm::EventSetup& es)
{
  using namespace std;
  using namespace edm;

  Handle<reco::TrackCollection> dsamuon_h;
  e.getByToken(dsamuon_token_, dsamuon_h);
  assert(dsamuon_h.isValid());
  const reco::TrackCollection& dsamuons = *dsamuon_h;

  clear();

  dsamuon_n_ = dsamuons.size();
  for (const auto& dsamuon : dsamuons)
  {
    const reco::HitPattern& hitPattern = dsamuon.hitPattern();
    const reco::TrackExtraRef& trackExtra = dsamuon.extra();

    dsamuon_p4_.push_back(
      useMuonHypothesis_ ?
      LorentzVector(dsamuon.px(), dsamuon.py(), dsamuon.pz(), hypot(dsamuon.p(), M_Mu)) :
      LorentzVector(dsamuon.px(), dsamuon.py(), dsamuon.pz(), dsamuon.p())
    );
    dsamuon_normChi2_   .emplace_back((dsamuon.ndof()!=0) ? dsamuon.normalizedChi2() : NAN);
    dsamuon_nValStaHits_.emplace_back(hitPattern.numberOfValidMuonHits());
    dsamuon_nLosStaHits_.emplace_back(hitPattern.numberOfLostMuonHits());
    dsamuon_tkQual_     .emplace_back(dsamuon.qualityMask());
    dsamuon_algo_       .emplace_back(dsamuon.algo());
    dsamuon_algoOrig_   .emplace_back(dsamuon.originalAlgo());
    dsamuon_ptErr_      .emplace_back((dsamuon.charge()!=0) ? dsamuon.ptError() : NAN);
    dsamuon_d0Sig_      .emplace_back(fabs(dsamuon.d0())/dsamuon.d0Error());
    dsamuon_dzSig_      .emplace_back(fabs(dsamuon.dz())/dsamuon.dzError());
    dsamuon_refPoint_   .push_back(Point(dsamuon.referencePoint().X(), dsamuon.referencePoint().Y(), dsamuon.referencePoint().Z()));
    dsamuon_charge_     .emplace_back(dsamuon.charge());

    bool _tkExtraAvail = trackExtra.isNonnull();
    bool _innerOk = _tkExtraAvail and dsamuon.innerOk();
    bool _outerOk = _tkExtraAvail and dsamuon.outerOk();
    
    dsamuon_innerPos_.push_back(
      _innerOk ?
      Point(dsamuon.innerPosition().X(), dsamuon.innerPosition().Y(), dsamuon.innerPosition().Z()) :
      Point(NAN,NAN,NAN)
    );
    dsamuon_innerP4_.push_back(
      _innerOk ?
      (
        useMuonHypothesis_ ?
        LorentzVector(
          dsamuon.innerMomentum().X(),
          dsamuon.innerMomentum().Y(),
          dsamuon.innerMomentum().Z(),
          hypot(dsamuon.innerMomentum().R(), M_Mu)
        ) :
        LorentzVector(
          dsamuon.innerMomentum().X(),
          dsamuon.innerMomentum().Y(),
          dsamuon.innerMomentum().Z(),
          dsamuon.innerMomentum().R()
        )
      ) :
      LorentzVector(NAN,NAN,NAN, NAN)
    );
    dsamuon_outerPos_.push_back(
      _outerOk ?
      Point(dsamuon.outerPosition().X(), dsamuon.outerPosition().Y(), dsamuon.outerPosition().Z()) :
      Point(NAN,NAN,NAN)
    );
    dsamuon_outerP4_.push_back(
      _outerOk ?
      (
        useMuonHypothesis_ ?
        LorentzVector(
          dsamuon.outerMomentum().X(),
          dsamuon.outerMomentum().Y(),
          dsamuon.outerMomentum().Z(),
          hypot(dsamuon.outerMomentum().R(), M_Mu)
        ) :
        LorentzVector(
          dsamuon.outerMomentum().X(),
          dsamuon.outerMomentum().Y(),
          dsamuon.outerMomentum().Z(),
          dsamuon.outerMomentum().R()
        )
      ) :
      LorentzVector(NAN,NAN,NAN, NAN)
    );

  }
}

void
ffNtupleDsaMuon::clear()
{
  dsamuon_n_ = 0;
  dsamuon_p4_         .clear();
  dsamuon_normChi2_   .clear();
  dsamuon_nValStaHits_.clear();
  dsamuon_nLosStaHits_.clear();
  dsamuon_tkQual_     .clear();
  dsamuon_algo_       .clear();
  dsamuon_algoOrig_   .clear();
  dsamuon_ptErr_      .clear();
  dsamuon_d0Sig_      .clear();
  dsamuon_dzSig_      .clear();
  dsamuon_charge_     .clear();
  dsamuon_refPoint_   .clear();
  dsamuon_innerPos_   .clear();
  dsamuon_outerPos_   .clear();
  dsamuon_innerP4_    .clear();
  dsamuon_outerP4_    .clear();
}


/// ***********************************************
enum TrackAlgorithm
{
  undefAlgorithm = 0,
  ctf = 1,
  duplicateMerge = 2,
  cosmics = 3,
  initialStep = 4,
  lowPtTripletStep = 5,
  pixelPairStep = 6,
  detachedTripletStep = 7,
  mixedTripletStep = 8,
  pixelLessStep = 9,
  tobTecStep = 10,
  jetCoreRegionalStep = 11,
  conversionStep = 12,
  muonSeededStepInOut = 13,
  muonSeededStepOutIn = 14,
  outInEcalSeededConv = 15,
  inOutEcalSeededConv = 16,
  nuclInter = 17,
  standAloneMuon = 18,
  globalMuon = 19,
  cosmicStandAloneMuon = 20,
  cosmicGlobalMuon = 21,
  // Phase1
  highPtTripletStep = 22,
  lowPtQuadStep = 23,
  detachedQuadStep = 24,
  reservedForUpgrades1 = 25,
  reservedForUpgrades2 = 26,
  bTagGhostTracks = 27,
  beamhalo = 28,
  gsf = 29,
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
  hiRegitMuInitialStep = 37,
  hiRegitMuLowPtTripletStep = 38,
  hiRegitMuPixelPairStep = 39,
  hiRegitMuDetachedTripletStep = 40,
  hiRegitMuMixedTripletStep = 41,
  hiRegitMuPixelLessStep = 42,
  hiRegitMuTobTecStep = 43,
  hiRegitMuMuonSeededStepInOut = 44,
  hiRegitMuMuonSeededStepOutIn = 45,
  algoSize = 46
};

/// ***********************************************
enum TrackQuality
{
  undefQuality = -1,
  loose = 0,
  tight = 1,
  highPurity = 2,
  confirmed = 3,     // means found by more than one iteration
  goodIterative = 4, // meaningless
  looseSetWithPV = 5,
  highPuritySetWithPV = 6,
  discarded = 7, // because a better track found. kept in the collection for reference....
  qualitySize = 8
};