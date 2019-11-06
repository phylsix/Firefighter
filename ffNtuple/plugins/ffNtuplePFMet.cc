#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETFwd.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"
#include "Math/Vector2D.h"
#include "Math/Vector2Dfwd.h"

using XYVectorF = ROOT::Math::XYVectorF;

class ffNtuplePFMet : public ffNtupleBaseNoHLT {
 public:
  ffNtuplePFMet( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  std::vector<edm::InputTag>             fPFMetInputTags;
  std::map<std::string, edm::EDGetToken> fPFMetTokenMap;
  std::map<std::string, XYVectorF>       fPFMetMap;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtuplePFMet, "ffNtuplePFMet" );

ffNtuplePFMet::ffNtuplePFMet( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtuplePFMet::initialize( TTree&                   tree,
                           const edm::ParameterSet& ps,
                           edm::ConsumesCollector&& cc ) {
  fPFMetInputTags = ps.getParameter<std::vector<edm::InputTag>>( "srcs" );

  for ( const auto& tag : fPFMetInputTags ) {
    const auto& label       = tag.label();
    fPFMetTokenMap[ label ] = cc.consumes<reco::PFMETCollection>( tag );
    fPFMetMap[ label ]      = XYVectorF();
    tree.Branch( label.c_str(), &fPFMetMap[ label ] )\
      ->SetTitle("PFMet (with corrections, type indicated by the suffix)");
  }
}

void
ffNtuplePFMet::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace edm;

  clear();

  for ( const auto& tag : fPFMetInputTags ) {
    const auto& label = tag.label();

    Handle<reco::PFMETCollection> pfmetHdl;
    e.getByToken( fPFMetTokenMap[ label ], pfmetHdl );
    assert( pfmetHdl.isValid() );

    fPFMetMap[ label ].SetXY( pfmetHdl->begin()->px(), pfmetHdl->begin()->py() );
  }
}

void
ffNtuplePFMet::clear() {
  for ( const auto& tag : fPFMetInputTags ) {
    const auto& label  = tag.label();
    fPFMetMap[ label ] = XYVectorF();
  }
}