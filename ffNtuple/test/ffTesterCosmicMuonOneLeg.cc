// framework headers
#include "DataFormats/CSCRecHit/interface/CSCSegmentCollection.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/DTRecHit/interface/DTRecSegment4DCollection.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "Firefighter/recoStuff/interface/DSAMuonHelper.h"

/**
 * \class ffTesterCosmicMuonOneLeg
 * \description
 * tester for using cosmicMuonOneLeg(reco::Track) to identify cosmic muons
 * in displacedStandAlone muons. This is done by checking if the DT/CSC DetIds
 * associated with a DSAMuon are subset of any DT/CSC DetIds associated with a
 * cosmicMuonOneLeg track.
 */

class ffTesterCosmicMuonOneLeg : public edm::one::EDAnalyzer<edm::one::SharedResources> {
 public:
  explicit ffTesterCosmicMuonOneLeg( const edm::ParameterSet& );
  ~ffTesterCosmicMuonOneLeg() = default;

  static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

 private:
  virtual void beginJob() override;
  virtual void analyze( const edm::Event&, const edm::EventSetup& ) override;
  virtual void endJob() override;

  edm::EDGetTokenT<reco::TrackCollection>                              fCosmicOneLegToken;
  edm::EDGetTokenT<reco::MuonCollection>                               fDSAMuonToken;
  edm::EDGetTokenT<edm::ValueMap<std::vector<reco::MuonChamberMatch>>> fCosmicMatchVMToken;

  edm::Handle<reco::TrackCollection>                              fCosmicOneLegHdl;
  edm::Handle<reco::MuonCollection>                               fDSAMuonHdl;
  edm::Handle<edm::ValueMap<std::vector<reco::MuonChamberMatch>>> fCosmcMatchVMHdl;
};

ffTesterCosmicMuonOneLeg::ffTesterCosmicMuonOneLeg( const edm::ParameterSet& ps )
    : fCosmicOneLegToken( consumes<reco::TrackCollection>( edm::InputTag( "cosmicMuons1Leg" ) ) ),
      fDSAMuonToken( consumes<reco::MuonCollection>( edm::InputTag( "muonsFromdSA" ) ) ),
      fCosmicMatchVMToken( consumes<edm::ValueMap<std::vector<reco::MuonChamberMatch>>>( edm::InputTag( "muonChamberMatchProducer" ) ) ) {
}

void
ffTesterCosmicMuonOneLeg::analyze( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  cout << "~~~~~~~~~~~~  " << e.run() << ":" << e.luminosityBlock() << ":" << e.id().event() << "  ~~~~~~~~~~~~" << endl;

  e.getByToken( fCosmicOneLegToken, fCosmicOneLegHdl );
  assert( fCosmicOneLegHdl.isValid() );
  e.getByToken( fDSAMuonToken, fDSAMuonHdl );
  assert( fDSAMuonHdl.isValid() );
  e.getByToken( fCosmicMatchVMToken, fCosmcMatchVMHdl );
  assert( fCosmcMatchVMHdl.isValid() );

  // collect CosmicMuonOneLeg DetIds, SegRefs
  vector<vector<DTChamberId>>       cosmicMuonOneLegDTIds{};
  vector<vector<CSCDetId>>          cosmicMuonOneLegCSCIds{};
  vector<vector<DTRecSegment4DRef>> cosmicMuonOneLegDTSegs{};
  vector<vector<CSCSegmentRef>>     cosmicMuonOneLegCSCSegs{};
  vector<vector<DTRecSegment4DRef>> cosmicMuonOneLegDTSegsTwoHemi{};
  vector<vector<CSCSegmentRef>>     cosmicMuonOneLegCSCSegsTwoHemi{};

  for ( size_t i( 0 ); i != fCosmicOneLegHdl->size(); i++ ) {
    reco::TrackRef                             cosmicOneLegRef( fCosmicOneLegHdl, i );
    const std::vector<reco::MuonChamberMatch>& cosmicmatch = ( *fCosmcMatchVMHdl )[ cosmicOneLegRef ];
    cosmicMuonOneLegDTIds.push_back( DSAMuonHelper::getDTDetIds( cosmicmatch ) );
    cosmicMuonOneLegCSCIds.push_back( DSAMuonHelper::getCSCDetIds( cosmicmatch ) );
    cosmicMuonOneLegDTSegs.push_back( DSAMuonHelper::getDTSegments( cosmicmatch ) );
    cosmicMuonOneLegCSCSegs.push_back( DSAMuonHelper::getCSCSegements( cosmicmatch ) );
    if ( DSAMuonHelper::bothHemisphereBySegments( cosmicmatch, es ) ) {
      cosmicMuonOneLegDTSegsTwoHemi.push_back( DSAMuonHelper::getDTSegments( cosmicmatch ) );
      cosmicMuonOneLegCSCSegsTwoHemi.push_back( DSAMuonHelper::getCSCSegements( cosmicmatch ) );
    }

    cout<<"[cosmicMuonOneLeg] "<<i<<": pT "<<cosmicOneLegRef->pt()
        <<" dxy: "<<cosmicOneLegRef->dxy()
        <<" dz: "<<cosmicOneLegRef->dz()
        <<" NDTSeg: "<<cosmicMuonOneLegDTSegs.back().size()
        <<endl;
  }

  for ( const auto& dsa : *fDSAMuonHdl ) {
    bool dsaDetIdIsSubset      = DSAMuonHelper::detIdsIsSubSetOfDTCSCIds( *dsa.outerTrack(), cosmicMuonOneLegDTIds, cosmicMuonOneLegCSCIds );
    bool dsaSegIsSubset        = DSAMuonHelper::segmentsIsSubsetOfDTCSCSegs( dsa, cosmicMuonOneLegDTSegs, cosmicMuonOneLegCSCSegs );
    bool dsaSegIsSubsetTwoHemi = DSAMuonHelper::segmentsIsSubsetOfDTCSCSegs( dsa, cosmicMuonOneLegDTSegsTwoHemi, cosmicMuonOneLegCSCSegsTwoHemi );

    cout << "DSA subset of CosmicOneLeg (DetID): " << dsaDetIdIsSubset
         << " (Segments): " << dsaSegIsSubset
         << " (seg2hemi): " << dsaSegIsSubsetTwoHemi << endl;
  }

  cout << "++++++++++++++++++++++++++++++++++" << endl;
}

void
ffTesterCosmicMuonOneLeg::beginJob() {}
void
ffTesterCosmicMuonOneLeg::endJob() {}

void
ffTesterCosmicMuonOneLeg::fillDescriptions( edm::ConfigurationDescriptions& descriptions ) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( ffTesterCosmicMuonOneLeg );