#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

using LorentzVector = math::XYZTLorentzVectorF;

class ffNtuplePfMuon : public ffNtupleBase {
 public:
  ffNtuplePfMuon( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

 private:
  void clear() final;

  edm::EDGetToken fPFCandsToken;

  math::XYZTLorentzVectorFCollection fPFMuonP4;
  std::vector<unsigned int>          fPFMuonSelectors;
  std::vector<bool>                  fPFMuonFoundAdjacent;    // range 0.4
  std::vector<bool>                  fPFMuonFoundAdjacentOS;  // opposite sign
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtuplePfMuon, "ffNtuplePfMuon" );

ffNtuplePfMuon::ffNtuplePfMuon( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtuplePfMuon::initialize( TTree&                   tree,
                            const edm::ParameterSet& ps,
                            edm::ConsumesCollector&& cc ) {
  fPFCandsToken = cc.consumes<reco::PFCandidateCollection>( edm::InputTag( "particleFlow" ) );

  tree.Branch( "pfmuon_p4", &fPFMuonP4 );
  tree.Branch( "pfmuon_selectors", &fPFMuonSelectors );
  tree.Branch( "pfmuon_foundAdjacent", &fPFMuonFoundAdjacent );
  tree.Branch( "pfmuon_foundAdjacentOS", &fPFMuonFoundAdjacentOS );
}

void
ffNtuplePfMuon::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::PFCandidateCollection> pfcandsHdl;
  e.getByToken( fPFCandsToken, pfcandsHdl );
  assert( pfcandsHdl.isValid() );

  clear();

  vector<Ptr<reco::PFCandidate>> pfmuons{};
  for ( size_t i( 0 ); i != pfcandsHdl->size(); ++i ) {
    const auto& cand = ( *pfcandsHdl )[ i ];
    if ( cand.particleId() != reco::PFCandidate::mu )
      continue;
    if ( cand.muonRef().isNull() )
      continue;
    if ( cand.pt() < 5. )
      continue;
    if ( fabs( cand.eta() ) > 2.4 )
      continue;
    pfmuons.emplace_back( pfcandsHdl, i );
  }

  for ( const auto& pfmuptr : pfmuons ) {
    fPFMuonP4.emplace_back( pfmuptr->px(), pfmuptr->py(), pfmuptr->pz(), pfmuptr->energy() );
    fPFMuonSelectors.emplace_back( pfmuptr->muonRef()->selectors() );

    bool foundAdjacent_( false );
    bool foundAdjacentOS_( false );
    for ( const auto& pfmuptr2 : pfmuons ) {
      if ( pfmuptr == pfmuptr2 )
        continue;
      if ( deltaR( *pfmuptr, *pfmuptr2 ) < 0.4 ) {
        if ( !foundAdjacent_ )
          foundAdjacent_ = true;
        if ( !foundAdjacentOS_ and ( pfmuptr->charge() * pfmuptr2->charge() == -1 ) )
          foundAdjacentOS_ = true;
        if ( foundAdjacent_ and foundAdjacentOS_ )
          break;
      }
    }
    fPFMuonFoundAdjacent.emplace_back( foundAdjacent_ );
    fPFMuonFoundAdjacentOS.emplace_back( foundAdjacentOS_ );
  }
}

void
ffNtuplePfMuon::clear() {
  fPFMuonP4.clear();
  fPFMuonSelectors.clear();
  fPFMuonFoundAdjacent.clear();
  fPFMuonFoundAdjacentOS.clear();
}