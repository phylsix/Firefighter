// -*- C++ -*-

#include <iostream>
#include "TFile.h"
#include "TSystem.h"

#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/InputSource.h"
#include "DataFormats/FWLite/interface/OutputFiles.h"
#include "DataFormats/JetReco/interface/BasicJetCollection.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "FWCore/FWLite/interface/FWLiteEnabler.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
#include "DataFormats/Common/interface/RefToPtr.h"

int
main( int argc, char* argv[] ) {
  // load framework libraries
  gSystem->Load( "libFWCoreFWLite" );
  FWLiteEnabler::enable();

  // parse arguments
  if ( argc < 2 ) {
    std::cout << "Usage : " << argv[ 0 ] << " [parameters.py]" << std::endl;
    return 0;
  }

  // get the python configuration
  if ( !edm::readPSetsFrom( argv[ 1 ] )
            ->existsAs<edm::ParameterSet>( "process" ) ) {
    std::cout << " ERROR: ParametersSet 'process' is missing in your "
                 "configuration file"
              << std::endl;
    exit( 0 );
  }
  // get the python configuration
  const edm::ParameterSet& process =
      edm::readPSetsFrom( argv[ 1 ] )
          ->getParameter<edm::ParameterSet>( "process" );
  fwlite::InputSource inputHandler_( process );
  fwlite::OutputFiles outputHandler_( process );

  const edm::ParameterSet& ps =
      process.getParameter<edm::ParameterSet>( "subjetResults" );

  int ievt( 0 );
  int maxEvents_( inputHandler_.maxEvents() );

  for ( unsigned int iFile = 0; iFile < inputHandler_.files().size();
        ++iFile ) {
    TFile* inFile = TFile::Open( inputHandler_.files()[ iFile ].c_str() );
    if ( !inFile )
      continue;

    fwlite::Event ev( inFile );

    edm::Handle<reco::PFJetCollection> jetH;
    edm::Handle<reco::BasicJetCollection> sjetH;
    edm::InputTag                     jetLbl( "ffLeptonJet" );
    edm::InputTag                     sjetLbl( "ffLeptonJetSubjets" );

    edm::Handle<edm::Association<reco::PFJetCollection>> jetsjetMapH;
    edm::InputTag jetsjetMapLbl( "ffLeptonJetSLeptonJetMap" );

    for ( ev.toBegin(); !ev.atEnd(); ++ev, ++ievt ) {
      if ( maxEvents_ > 0 ? ievt + 1 > maxEvents_ : false )
        break;

      ev.getByLabel( jetLbl, jetH );
      assert( jetH.isValid() );
      ev.getByLabel( sjetLbl, sjetH );
      assert( sjetH.isValid() );
      ev.getByLabel( jetsjetMapLbl, jetsjetMapH );
      assert( jetsjetMapH.isValid() );

      std::cout << jetH->size() << ", " << sjetH->size() << ", "
                << jetsjetMapH->size() << std::endl;

      for ( size_t i( 0 ); i < sjetH->size(); ++i ) {
        edm::Ptr<reco::Jet> sjet( sjetH, i );
        std::cout << "sjet pt: " << sjet->pt();
        edm::Ptr<reco::PFJet> jet = edm::refToPtr(( *jetsjetMapH )[ sjet ]);
        if ( jet.isNonnull() ) {
          std::cout << "\tjet [" << jet.key() << "] pt: " << jet->pt();
        }
        std::cout << std::endl;
      }
    }

    inFile->Close();
    // break loop if maximal number of events is reached:
    // this has to be done twice to stop the file loop as well
    if ( maxEvents_ > 0 ? ievt + 1 > maxEvents_ : false )
      break;
  }

  return 0;
}
