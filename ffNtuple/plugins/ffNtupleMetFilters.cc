#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

class ffNtupleMetFilters : public ffNtupleBase {
 public:
  ffNtupleMetFilters( const edm::ParameterSet& );
  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

 private:
  void clear() final;

  edm::EDGetToken fPrimaryVertexFilterToken;
  edm::EDGetToken fBadMuonFilterToken;
  edm::EDGetToken fCSCTightHalo2015FilterToken;
  edm::EDGetToken fGlobalTightHalo2016FilterToken;
  edm::EDGetToken fGlobalSuperTightHalo2016FilterToken;
  edm::EDGetToken fEcalDeadCellTriggerPrimitiveFilterToken;
  edm::EDGetToken fEcalBadCalibFilterToken;
  edm::EDGetToken fHBHENoiseFilterResultToken;
  edm::EDGetToken fHBHEIsoNoiseFilterResultToken;
  edm::EDGetToken fTriggerObjectMatchingFilterToken;

  bool fPrimaryVertexFilter;
  bool fBadMuonFilter;
  bool fCSCTightHalo2015Filter;
  bool fGlobalTightHalo2016Filter;
  bool fGlobalSuperTightHalo2016Filter;
  bool fEcalDeadCellTriggerPrimitiveFilter;
  bool fEcalBadCalibFilter;
  bool fHBHENoiseFilterResult;
  bool fHBHEIsoNoiseFilterResult;
  bool fTriggerObjectMatchingFilter;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleMetFilters, "ffNtupleMetFilters" );

ffNtupleMetFilters::ffNtupleMetFilters( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleMetFilters::initialize( TTree&                   tree,
                                const edm::ParameterSet& ps,
                                edm::ConsumesCollector&& cc ) {
  fPrimaryVertexFilterToken =
      cc.consumes<bool>( edm::InputTag( "primaryVertexFilter" ) );
  fBadMuonFilterToken = cc.consumes<bool>( edm::InputTag( "BadPFMuonFilter" ) );
  fCSCTightHalo2015FilterToken =
      cc.consumes<bool>( edm::InputTag( "CSCTightHalo2015Filter" ) );
  fGlobalTightHalo2016FilterToken =
      cc.consumes<bool>( edm::InputTag( "globalTightHalo2016Filter" ) );
  fGlobalSuperTightHalo2016FilterToken =
      cc.consumes<bool>( edm::InputTag( "globalSuperTightHalo2016Filter" ) );
  fEcalDeadCellTriggerPrimitiveFilterToken = cc.consumes<bool>(
      edm::InputTag( "EcalDeadCellTriggerPrimitiveFilter" ) );
  fEcalBadCalibFilterToken =
      cc.consumes<bool>( edm::InputTag( "ecalBadCalibFilter" ) );
  fHBHENoiseFilterResultToken    = cc.consumes<bool>( edm::InputTag(
      "HBHENoiseFilterResultProducer", "HBHENoiseFilterResult" ) );
  fHBHEIsoNoiseFilterResultToken = cc.consumes<bool>( edm::InputTag(
      "HBHENoiseFilterResultProducer", "HBHEIsoNoiseFilterResult" ) );
  fTriggerObjectMatchingFilterToken =
      cc.consumes<bool>( edm::InputTag( "triggerObjectMatchingFilter" ) );

  tree.Branch( "metfilters_PrimaryVertexFilter", &fPrimaryVertexFilter,
               "metfilters_PrimaryVertexFilter/O" );
  tree.Branch( "metfilters_BadMuonFilter", &fBadMuonFilter,
               "metfilters_BadMuonFilter/O" );
  tree.Branch( "metfilters_CSCTightHalo2015Filter", &fCSCTightHalo2015Filter,
               "metfilters_CSCTightHalo2015Filter/O" );
  tree.Branch( "metfilters_GlobalTightHalo2016Filter",
               &fGlobalTightHalo2016Filter,
               "metfilters_GlobalTightHalo2016Filter/O" );
  tree.Branch( "metfilters_GlobalSuperTightHalo2016Filter",
               &fGlobalSuperTightHalo2016Filter,
               "metfilters_GlobalSuperTightHalo2016Filter/O" );
  tree.Branch( "metfilters_EcalDeadCellTriggerPrimitiveFilter",
               &fEcalDeadCellTriggerPrimitiveFilter,
               "metfilters_EcalDeadCellTriggerPrimitiveFilter/O" );
  tree.Branch( "metfilters_EcalBadCalibFilter", &fEcalBadCalibFilter,
               "metfilters_EcalBadCalibFilter/O" );
  tree.Branch( "metfilters_HBHENoiseFilter", &fHBHENoiseFilterResult,
               "metfilters_HBHENoiseFilter/O" );
  tree.Branch( "metfilters_HBHEIsoNoiseFilter", &fHBHEIsoNoiseFilterResult,
               "metfilters_HBHEIsoNoiseFilter/O" );
  tree.Branch( "metfilters_TriggerObjectMatchingFilter",
               &fTriggerObjectMatchingFilter,
               "metfilters_TriggerObjectMatchingFilter/O" );
}

void
ffNtupleMetFilters::fill( const edm::Event& e, const edm::EventSetup& es ) {
  edm::Handle<bool> PrimaryVertexFilterHandle;
  edm::Handle<bool> BadMuonFilterHandle;
  edm::Handle<bool> CSCTightHalo2015FilterHandle;
  edm::Handle<bool> GlobalTightHalo2016FilterHandle;
  edm::Handle<bool> GlobalSuperTightHalo2016FilterHandle;
  edm::Handle<bool> EcalDeadCellTriggerPrimitiveFilterHandle;
  edm::Handle<bool> EcalBadCalibFilterHandle;
  edm::Handle<bool> HBHENoiseFilterResultHandle;
  edm::Handle<bool> HBHEIsoNoiseFilterResultHandle;
  edm::Handle<bool> TriggerObjectMatchingFilterHandle;

  e.getByToken( fPrimaryVertexFilterToken, PrimaryVertexFilterHandle );
  e.getByToken( fBadMuonFilterToken, BadMuonFilterHandle );
  e.getByToken( fCSCTightHalo2015FilterToken, CSCTightHalo2015FilterHandle );
  e.getByToken( fGlobalTightHalo2016FilterToken,
                GlobalTightHalo2016FilterHandle );
  e.getByToken( fGlobalSuperTightHalo2016FilterToken,
                GlobalSuperTightHalo2016FilterHandle );
  e.getByToken( fEcalDeadCellTriggerPrimitiveFilterToken,
                EcalDeadCellTriggerPrimitiveFilterHandle );
  e.getByToken( fEcalBadCalibFilterToken, EcalBadCalibFilterHandle );
  e.getByToken( fHBHENoiseFilterResultToken, HBHENoiseFilterResultHandle );
  e.getByToken( fHBHEIsoNoiseFilterResultToken,
                HBHEIsoNoiseFilterResultHandle );
  e.getByToken( fTriggerObjectMatchingFilterToken,
                TriggerObjectMatchingFilterHandle );

  assert( PrimaryVertexFilterHandle.isValid() );
  assert( BadMuonFilterHandle.isValid() );
  assert( CSCTightHalo2015FilterHandle.isValid() );
  assert( GlobalTightHalo2016FilterHandle.isValid() );
  assert( GlobalSuperTightHalo2016FilterHandle.isValid() );
  assert( EcalDeadCellTriggerPrimitiveFilterHandle.isValid() );
  assert( EcalBadCalibFilterHandle.isValid() );
  assert( HBHENoiseFilterResultHandle.isValid() );
  assert( HBHEIsoNoiseFilterResultHandle.isValid() );
  assert( TriggerObjectMatchingFilterHandle.isValid() );

  clear();

  fPrimaryVertexFilter            = *PrimaryVertexFilterHandle;
  fBadMuonFilter                  = *BadMuonFilterHandle;
  fCSCTightHalo2015Filter         = *CSCTightHalo2015FilterHandle;
  fGlobalTightHalo2016Filter      = *GlobalTightHalo2016FilterHandle;
  fGlobalSuperTightHalo2016Filter = *GlobalSuperTightHalo2016FilterHandle;
  fEcalDeadCellTriggerPrimitiveFilter =
      *EcalDeadCellTriggerPrimitiveFilterHandle;
  fEcalBadCalibFilter          = *EcalBadCalibFilterHandle;
  fHBHENoiseFilterResult       = *HBHENoiseFilterResultHandle;
  fHBHEIsoNoiseFilterResult    = *HBHEIsoNoiseFilterResultHandle;
  fTriggerObjectMatchingFilter = *TriggerObjectMatchingFilterHandle;
}

void
ffNtupleMetFilters::clear() {
  fPrimaryVertexFilter                = false;
  fBadMuonFilter                      = false;
  fCSCTightHalo2015Filter             = false;
  fGlobalTightHalo2016Filter          = false;
  fGlobalSuperTightHalo2016Filter     = false;
  fEcalDeadCellTriggerPrimitiveFilter = false;
  fEcalBadCalibFilter                 = false;
  fHBHENoiseFilterResult              = false;
  fHBHEIsoNoiseFilterResult           = false;
  fTriggerObjectMatchingFilter        = false;
}
