#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/GetterOfProducts.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ProcessMatch.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "SimDataFormats/GeneratorProducts/interface/GenLumiInfoHeader.h"

#include <iostream>

class MCSignalParamProducer : public edm::stream::EDProducer<> {
 public:
  explicit MCSignalParamProducer( const edm::ParameterSet& );
  ~MCSignalParamProducer() override;

 private:
  void produce( edm::Event&, const edm::EventSetup& ) override;
  void beginLuminosityBlock( const edm::LuminosityBlock&,
                             const edm::EventSetup& ) override;
  void getComment( const GenLumiInfoHeader& );

  edm::EDGetTokenT<GenLumiInfoHeader> genLumiHeaderToken_;
  std::vector<double>                 signalParameters_;
};

MCSignalParamProducer::MCSignalParamProducer( const edm::ParameterSet& ps )
    : genLumiHeaderToken_( consumes<GenLumiInfoHeader, edm::InLumi>(
          edm::InputTag( "generator" ) ) ) {
  produces<std::vector<double>>( "SignalParameters" );
}

MCSignalParamProducer::~MCSignalParamProducer() {}

void
MCSignalParamProducer::produce( edm::Event& e, const edm::EventSetup& es ) {
  auto signalParameters =
      std::make_unique<std::vector<double>>( signalParameters_ );
  e.put( std::move( signalParameters ), "SignalParameters" );
}

void
MCSignalParamProducer::beginLuminosityBlock( const edm::LuminosityBlock& lb,
                                             const edm::EventSetup&      es ) {
  signalParameters_.clear();
  edm::Handle<GenLumiInfoHeader> gen_header;
  lb.getByToken( genLumiHeaderToken_, gen_header );
  assert( gen_header.isValid() );

  getComment( *gen_header );
}

void
MCSignalParamProducer::getComment( const GenLumiInfoHeader& gen ) {
  std::string model = gen.configDescription();  // parse info here and set stuff
  // std::cout << "Comment: " << model << std::endl; // placeholder - will parse
  // this string and assign them to signalParameters_ ref:
  // https://github.com/TreeMaker/TreeMaker/blob/Run2_2017/Utils/src/SignalScanProducer.cc#L226
}

DEFINE_FWK_MODULE( MCSignalParamProducer );
