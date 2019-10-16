#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"
#include "Firefighter/recoStuff/interface/RecoHelpers.h"

#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

using LorentzVector = math::XYZTLorentzVectorF;

class ffNtupleLeptonJetSource : public ffNtupleBase {
 public:
  ffNtupleLeptonJetSource( const edm::ParameterSet& );
  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

 private:
  void clear() final;

  edm::EDGetToken            fLjSrcToken;
  edm::EDGetToken            fGeneralTkToken;
  std::vector<LorentzVector> fLjSrcP4;
  std::vector<int>           fLjSrcCharge;
  std::vector<int>           fLjSrcType;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleLeptonJetSource, "ffNtupleLeptonJetSource" );

ffNtupleLeptonJetSource::ffNtupleLeptonJetSource( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleLeptonJetSource::initialize( TTree&                   tree,
                                     const edm::ParameterSet& ps,
                                     edm::ConsumesCollector&& cc ) {
  fLjSrcToken     = cc.consumes<reco::PFCandidateFwdPtrVector>( ps.getParameter<edm::InputTag>( "src" ) );
  fGeneralTkToken = cc.consumes<reco::TrackCollection>( edm::InputTag( "generalTracks" ) );

  tree.Branch( "ljsource_p4", &fLjSrcP4 );
  tree.Branch( "ljsource_charge", &fLjSrcCharge );
  tree.Branch( "ljsource_type", &fLjSrcType );
}

void
ffNtupleLeptonJetSource::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;
  using namespace ff;

  Handle<reco::PFCandidateFwdPtrVector> ljsrcHdl;
  e.getByToken( fLjSrcToken, ljsrcHdl );
  assert( ljsrcHdl.isValid() );
  const reco::PFCandidateFwdPtrVector& ljsrcs = *ljsrcHdl;

  Handle<reco::TrackCollection> generalTkHdl;
  e.getByToken( fGeneralTkToken, generalTkHdl );
  assert( generalTkHdl.isValid() );

  clear();

  for ( const auto& ljsrc : ljsrcs ) {
    const auto& cand = *( ljsrc.get() );
    fLjSrcP4.push_back( LorentzVector( cand.px(), cand.py(), cand.pz(), cand.energy() ) );
    fLjSrcCharge.emplace_back( cand.charge() );
    fLjSrcType.emplace_back( getCandType( ljsrc.ptr(), generalTkHdl ) );
  }
}

void
ffNtupleLeptonJetSource::clear() {
  fLjSrcP4.clear();
  fLjSrcCharge.clear();
  fLjSrcType.clear();
}