#include <algorithm>

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

class ffNtupleGenBkg : public ffNtupleBaseNoHLT {
 public:
  ffNtupleGenBkg( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken  fGenToken;
  std::vector<int> fAllowedPids;
  std::vector<int> fAllowedStatusOnePids;

  unsigned int                       fNGen;
  std::vector<int>                   fGenPid;
  std::vector<int>                   fGenCharge;
  math::XYZTLorentzVectorFCollection fGenP4;
  std::vector<unsigned long>         fGenStatusFlags;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleGenBkg, "ffNtupleGenBkg" );

ffNtupleGenBkg::ffNtupleGenBkg( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtupleGenBkg::initialize( TTree&                   tree,
                            const edm::ParameterSet& ps,
                            edm::ConsumesCollector&& cc ) {
  fGenToken             = cc.consumes<reco::GenParticleCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  fAllowedPids          = ps.getParameter<std::vector<int>>( "AllowedPids" );
  fAllowedStatusOnePids = ps.getParameter<std::vector<int>>( "AllowedStatusOnePids" );

  tree.Branch( "gen_n", &fNGen );
  tree.Branch( "gen_p4", &fGenP4 );
  tree.Branch( "gen_charge", &fGenCharge );
  tree.Branch( "gen_pid", &fGenPid );
  tree.Branch( "gen_statusflags", &fGenStatusFlags );
}

void
ffNtupleGenBkg::fill( const edm::Event& e, const edm::EventSetup& es ) {
  clear();

  using namespace std;
  using namespace edm;

  Handle<reco::GenParticleCollection> genHdl;
  e.getByToken( fGenToken, genHdl );
  assert( genHdl.isValid() );

  for ( const auto& p : *genHdl ) {
    if ( !p.isLastCopy() )
      continue;

    if ( std::find( fAllowedPids.cbegin(), fAllowedPids.cend(), abs( p.pdgId() ) ) != fAllowedPids.cend() ) {
      fGenP4.emplace_back( p.px(), p.py(), p.pz(), p.energy() );
      fGenCharge.emplace_back( p.charge() );
      fGenPid.emplace_back( p.pdgId() );
      fGenStatusFlags.emplace_back( p.statusFlags().flags_.to_ulong() );
    }

    if ( std::find( fAllowedStatusOnePids.cbegin(), fAllowedStatusOnePids.cend(), abs( p.pdgId() ) ) != fAllowedStatusOnePids.cend() ) {
      if ( p.status() != 1 )
        continue;
      fGenP4.emplace_back( p.px(), p.py(), p.pz(), p.energy() );
      fGenCharge.emplace_back( p.charge() );
      fGenPid.emplace_back( p.pdgId() );
      fGenStatusFlags.emplace_back( p.statusFlags().flags_.to_ulong() );
    }
  }

  fNGen = fGenP4.size();
}

void
ffNtupleGenBkg::clear() {
  fNGen = 0;
  fGenP4.clear();
  fGenCharge.clear();
  fGenPid.clear();
  fGenStatusFlags.clear();
}