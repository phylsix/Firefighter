#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include "DataFormats/Math/interface/LorentzVectorFwd.h"

class ffNtupleGenBkgCheck : public ffNtupleBase {
 public:
  ffNtupleGenBkgCheck( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

 private:
  void clear() final;

  edm::EDGetToken fGenToken;
  edm::EDGetToken fLeptonjetToken;
  edm::EDGetToken fPFCandToken;

  math::XYZTLorentzVectorFCollection fGenP4;
  std::vector<int>                   fGenPid;
  std::vector<unsigned long>         fGenStatusFlags;
  math::XYZTLorentzVectorFCollection fPFCandP4;
  std::vector<int>                   fPFCandParticleId;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleGenBkgCheck, "ffNtupleGenBkgCheck" );

ffNtupleGenBkgCheck::ffNtupleGenBkgCheck( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleGenBkgCheck::initialize( TTree&                   tree,
                                 const edm::ParameterSet& ps,
                                 edm::ConsumesCollector&& cc ) {
  fGenToken       = cc.consumes<reco::GenParticleCollection>( edm::InputTag( "genParticles" ) );
  fLeptonjetToken = cc.consumes<reco::PFJetCollection>( edm::InputTag( "filteredLeptonJet" ) );
  fPFCandToken    = cc.consumes<reco::PFCandidateCollection>( edm::InputTag( "particleFlow" ) );

  tree.Branch( "allgen_p4", &fGenP4 );
  tree.Branch( "allgen_pid", &fGenPid );
  tree.Branch( "allgen_statusflags", &fGenStatusFlags );
  tree.Branch( "allpfcand_p4", &fPFCandP4 );
  tree.Branch( "allpfcand_pid", &fPFCandParticleId );
}

void
ffNtupleGenBkgCheck::fill( const edm::Event& e, const edm::EventSetup& es ) {
  clear();

  using namespace std;
  using namespace edm;

  Handle<reco::GenParticleCollection> genHdl;
  e.getByToken( fGenToken, genHdl );
  assert( genHdl.isValid() );
  Handle<reco::PFJetCollection> leptonjetHdl;
  e.getByToken( fLeptonjetToken, leptonjetHdl );
  assert( leptonjetHdl.isValid() );
  Handle<reco::PFCandidateCollection> pfcandHdl;
  e.getByToken( fPFCandToken, pfcandHdl );
  assert( pfcandHdl.isValid() );

  if ( leptonjetHdl->size() < 2 )
    return;  // fill only when there are >=2 leptonjets reconstructed

  for ( const auto& p : *genHdl ) {
    if ( !p.isLastCopy() )
      continue;

    fGenP4.emplace_back( p.px(), p.py(), p.pz(), p.energy() );
    fGenPid.emplace_back( p.pdgId() );
    fGenStatusFlags.emplace_back( p.statusFlags().flags_.to_ulong() );
  }

  for ( const auto& p : *pfcandHdl ) {
    fPFCandP4.emplace_back( p.px(), p.py(), p.pz(), p.energy() );
    fPFCandParticleId.emplace_back( p.particleId() );
  }
}

void
ffNtupleGenBkgCheck::clear() {
  fGenP4.clear();
  fGenPid.clear();
  fGenStatusFlags.clear();
  fPFCandP4.clear();
  fPFCandParticleId.clear();
}