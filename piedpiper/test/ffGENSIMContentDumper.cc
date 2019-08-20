// framework headers
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"

// dataformats
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"

#include <HepMC/GenEvent.h>

class ffGENSIMContentDumper : public edm::one::EDAnalyzer<edm::one::SharedResources> {
 public:
  explicit ffGENSIMContentDumper( const edm::ParameterSet& );
  ~ffGENSIMContentDumper();

  static void fillDescriptions( edm::ConfigurationDescriptions& );

  void dumpHepMCProduct( const edm::Event& );

 private:
  virtual void analyze( const edm::Event&, const edm::EventSetup& ) override;

  edm::EDGetTokenT<edm::HepMCProduct> fEdmHepMCProdToken;
  edm::Handle<edm::HepMCProduct>      fEdmHepMCProdHdl;
};

ffGENSIMContentDumper::ffGENSIMContentDumper( const edm::ParameterSet& iC ) {
  fEdmHepMCProdToken = consumes<edm::HepMCProduct>( edm::InputTag( "generatorSmeared" ) );
}

ffGENSIMContentDumper::~ffGENSIMContentDumper() {}

void
ffGENSIMContentDumper::analyze( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;

  cout << "++++ "
       << e.id().run() << " : " << e.luminosityBlock() << " : " << e.id().event()
       << " ++++" << endl;

  dumpHepMCProduct( e );

  cout << "**************************************" << endl;
}

void
ffGENSIMContentDumper::dumpHepMCProduct( const edm::Event& e ) {
  using namespace edm;
  using namespace std;

  e.getByToken( fEdmHepMCProdToken, fEdmHepMCProdHdl );
  assert( fEdmHepMCProdHdl.isValid() );

  const auto&            hepmcprod = *fEdmHepMCProdHdl;
  const HepMC::GenEvent& evt       = hepmcprod.getHepMCData();
  //   evt.print();
  for ( HepMC::GenEvent::vertex_const_iterator vitr = evt.vertices_begin();
        vitr != evt.vertices_end(); ++vitr ) {
    for ( HepMC::GenVertex::particle_iterator pitr = ( *vitr )->particles_begin( HepMC::children );
          pitr != ( *vitr )->particles_end( HepMC::children ); ++pitr ) {
      int status = ( *pitr )->status();
      int pdg    = ( *pitr )->pdg_id();
      if ( pdg == 32 ) {
        cout << status << endl;
      }
    }
  }
}

void
ffGENSIMContentDumper::fillDescriptions( edm::ConfigurationDescriptions& descriptions ) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( ffGENSIMContentDumper );
