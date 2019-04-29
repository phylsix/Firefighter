#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "DataFormats/METReco/interface/BeamHaloSummary.h"

class ffNtupleBeamHalo : public ffNtupleBase {
 public:
  ffNtupleBeamHalo( const edm::ParameterSet& );
  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

 private:
  void clear() final;

  edm::EDGetToken fBeamHaloToken;

  bool fCSCTightHaloId2015;
  bool fGlobalTightHaloId2016;
  bool fGlobalSuperTightHaloId2016;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleBeamHalo, "ffNtupleBeamHalo" );

ffNtupleBeamHalo::ffNtupleBeamHalo( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleBeamHalo::initialize( TTree&                   tree,
                              const edm::ParameterSet& ps,
                              edm::ConsumesCollector&& cc ) {
  fBeamHaloToken = cc.consumes<reco::BeamHaloSummary>(
      ps.getParameter<edm::InputTag>( "src" ) );

  tree.Branch( "beamhalo_CSCTightHaloId2015", &fCSCTightHaloId2015,
               "beamhalo_CSCTightHaloId2015/O" );
  tree.Branch( "beamhalo_GlobalTightHaloId2016", &fGlobalTightHaloId2016,
               "beamhalo_GlobalTightHaloId2016/O" );
  tree.Branch( "beamhalo_GlobalSuperTightHaloId2016",
               &fGlobalSuperTightHaloId2016,
               "beamhalo_GlobalSuperTightHaloId2016/O" );
}

void
ffNtupleBeamHalo::fill( const edm::Event& e, const edm::EventSetup& es ) {
  edm::Handle<reco::BeamHaloSummary> fBeamHaloHdl;
  e.getByToken( fBeamHaloToken, fBeamHaloHdl );
  assert( fBeamHaloHdl.isValid() );
  const auto& beamhalo = *fBeamHaloHdl;

  clear();

  fCSCTightHaloId2015         = beamhalo.CSCTightHaloId2015();
  fGlobalTightHaloId2016      = beamhalo.GlobalTightHaloId2016();
  fGlobalSuperTightHaloId2016 = beamhalo.GlobalSuperTightHaloId2016();
}

void
ffNtupleBeamHalo::clear() {
  fCSCTightHaloId2015         = false;
  fGlobalTightHaloId2016      = false;
  fGlobalSuperTightHaloId2016 = false;
}