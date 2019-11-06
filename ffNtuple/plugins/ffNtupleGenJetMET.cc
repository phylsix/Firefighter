#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/METReco/interface/GenMET.h"
#include "DataFormats/METReco/interface/GenMETFwd.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"
#include "Math/Vector2D.h"
#include "Math/Vector2Dfwd.h"

using XYVectorF = ROOT::Math::XYVectorF;

class ffNtupleGenJetMET : public ffNtupleBaseNoHLT {
 public:
  ffNtupleGenJetMET( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken genjet_token_;
  edm::EDGetToken genmet_token_;

  math::XYZTLorentzVectorFCollection genjet_p4_;
  XYVectorF                          genmet_;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleGenJetMET, "ffNtupleGenJetMET" );

ffNtupleGenJetMET::ffNtupleGenJetMET( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtupleGenJetMET::initialize( TTree&                   tree,
                               const edm::ParameterSet& ps,
                               edm::ConsumesCollector&& cc ) {
  genjet_token_ = cc.consumes<reco::GenJetCollection>( edm::InputTag( "ak4GenJetsNoNu" ) );
  genmet_token_ = cc.consumes<reco::GenMETCollection>( edm::InputTag( "genMetTrue" ) );

  tree.Branch( "genjet_p4", &genjet_p4_ );
  tree.Branch( "genmet", &genmet_ );
}

void
ffNtupleGenJetMET::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace edm;

  Handle<reco::GenJetCollection> genjets_h;
  e.getByToken( genjet_token_, genjets_h );
  assert( genjets_h.isValid() );

  Handle<reco::GenMETCollection> genmet_h;
  e.getByToken( genmet_token_, genmet_h );
  assert( genmet_h.isValid() );

  clear();

  for ( const auto& jet : *genjets_h ) {
    genjet_p4_.emplace_back( jet.px(), jet.py(), jet.pz(), jet.energy() );
  }

  genmet_.SetXY( genmet_h->begin()->px(), genmet_h->begin()->py() );
}

void
ffNtupleGenJetMET::clear() {
  genjet_p4_.clear();
  genmet_ = XYVectorF();
}