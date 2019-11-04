#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETFwd.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

class ffNtuplePFMet : public ffNtupleBaseNoHLT {
 public:
  ffNtuplePFMet( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken fPFMetToken;
  edm::EDGetToken fPFMetT1Token;
  edm::EDGetToken fPFMetT0T1Token;
  edm::EDGetToken fPFMetT0T1TxyToken;

  float fPFMetPx, fPFMetPy;
  float fPFMetT1Px, fPFMetT1Py;
  float fPFMetT0T1Px, fPFMetT0T1Py;
  float fPFMetT0T1TxyPx, fPFMetT0T1TxyPy;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtuplePFMet, "ffNtuplePFMet" );

ffNtuplePFMet::ffNtuplePFMet( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtuplePFMet::initialize( TTree&                   tree,
                           const edm::ParameterSet& ps,
                           edm::ConsumesCollector&& cc ) {
  fPFMetToken        = cc.consumes<reco::PFMETCollection>( edm::InputTag( "pfMet" ) );
  fPFMetT1Token      = cc.consumes<reco::PFMETCollection>( edm::InputTag( "pfMetT1" ) );
  fPFMetT0T1Token    = cc.consumes<reco::PFMETCollection>( edm::InputTag( "pfMetT0pcT1" ) );
  fPFMetT0T1TxyToken = cc.consumes<reco::PFMETCollection>( edm::InputTag( "pfMetT0pcT1Txy" ) );

  tree.Branch( "pfmet_px", &fPFMetPx );
  tree.Branch( "pfmet_py", &fPFMetPy );
  tree.Branch( "pfmetT1_px", &fPFMetT1Px );
  tree.Branch( "pfmetT1_py", &fPFMetT1Py );
  tree.Branch( "pfmetT0T1_px", &fPFMetT0T1Px );
  tree.Branch( "pfmetT0T1_py", &fPFMetT0T1Py );
  tree.Branch( "pfmetT0T1Txy_px", &fPFMetT0T1TxyPx );
  tree.Branch( "pfmetT0T1Txy_py", &fPFMetT0T1TxyPy );
}

void
ffNtuplePFMet::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace edm;

  Handle<reco::PFMETCollection> pfmetHdl;
  e.getByToken( fPFMetToken, pfmetHdl );
  assert( pfmetHdl.isValid() );

  Handle<reco::PFMETCollection> pfmett1Hdl;
  e.getByToken( fPFMetT1Token, pfmett1Hdl );
  assert( pfmett1Hdl.isValid() );

  Handle<reco::PFMETCollection> pfmett0t1Hdl;
  e.getByToken( fPFMetT0T1Token, pfmett0t1Hdl );
  assert( pfmett0t1Hdl.isValid() );

  Handle<reco::PFMETCollection> pfmett0t1txyHdl;
  e.getByToken( fPFMetT0T1TxyToken, pfmett0t1txyHdl );
  assert( pfmett0t1txyHdl.isValid() );

  fPFMetPx = pfmetHdl->begin()->px();
  fPFMetPy = pfmetHdl->begin()->py();

  fPFMetT1Px = pfmett1Hdl->begin()->px();
  fPFMetT1Py = pfmett1Hdl->begin()->py();

  fPFMetT0T1Px = pfmett0t1Hdl->begin()->px();
  fPFMetT0T1Py = pfmett0t1Hdl->begin()->py();

  fPFMetT0T1TxyPx = pfmett0t1txyHdl->begin()->px();
  fPFMetT0T1TxyPy = pfmett0t1txyHdl->begin()->py();
}

void
ffNtuplePFMet::clear() {
  fPFMetPx        = 0.;
  fPFMetPy        = 0.;
  fPFMetT1Px      = 0.;
  fPFMetT1Py      = 0.;
  fPFMetT0T1Px    = 0.;
  fPFMetT0T1Py    = 0.;
  fPFMetT0T1TxyPx = 0.;
  fPFMetT0T1TxyPy = 0.;
}