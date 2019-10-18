#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

class ffNtupleGenJet : public ffNtupleBaseNoHLT {
 public:
  ffNtupleGenJet( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken genjet_token_;

  int                                genjet_n_;
  math::XYZTLorentzVectorFCollection genjet_p4_;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleGenJet, "ffNtupleGenJet" );

ffNtupleGenJet::ffNtupleGenJet( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtupleGenJet::initialize( TTree&                   tree,
                            const edm::ParameterSet& ps,
                            edm::ConsumesCollector&& cc ) {
  genjet_token_ = cc.consumes<reco::GenJetCollection>(
      ps.getParameter<edm::InputTag>( "GenJets" ) );
  tree.Branch( "genjet_n", &genjet_n_, "genjet_n/I" );
  tree.Branch( "genjet_p4", &genjet_p4_ );
}

void
ffNtupleGenJet::fill( const edm::Event& e, const edm::EventSetup& es ) {
  edm::Handle<reco::GenJetCollection> genjets_h;
  e.getByToken( genjet_token_, genjets_h );
  const reco::GenJetCollection& genjets = *genjets_h;

  clear();
  genjet_n_ = genjets.size();
  genjet_p4_.reserve( genjet_n_ );
  for ( const auto& jet : genjets ) {
    genjet_p4_.emplace_back( jet.px(), jet.py(), jet.pz(), jet.energy() );
  }
}

void
ffNtupleGenJet::clear() {
  genjet_n_ = 0;
  genjet_p4_.clear();
}