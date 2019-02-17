#include "Firefighter/leptonJetGun/interface/ljGunProd.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/RandomNumberGenerator.h"

#include "CLHEP/Random/RandExponential.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandomEngine.h"

#include "TMath.h"

ljGunProd::ljGunProd( const edm::ParameterSet& ps )
    : edm::BaseFlatGunProducer( ps ) {
  edm::ParameterSet pgun_params = ps.getParameterSet( "PGunParameters" );

  fMinPt = pgun_params.getParameter<double>( "MinPt" );
  fMaxPt = pgun_params.getParameter<double>( "MaxPt" );

  fLvBeta   = ps.getParameter<double>( "LvBeta" );
  fBoundEta = ps.getParameter<double>( "BoundEta" );
  fBoundR   = ps.getParameter<double>( "BoundR" );
  fBoundZ   = ps.getParameter<double>( "BoundZ" );
  fMomMass  = ps.getParameter<double>( "MomMass" );
  fMomPid   = ps.getParameter<int>( "MomPid" );

  bound_ = ff::GeometryBoundary( fBoundEta, fBoundR, fBoundZ );

  produces<edm::HepMCProduct>( "unsmeared" );
  produces<GenEventInfoProduct>();
}

void
ljGunProd::produce( edm::Event& e, const edm::EventSetup& es ) {
  edm::Service<edm::RandomNumberGenerator> rng;
  CLHEP::HepRandomEngine& engine = rng->getEngine( e.streamID() );

  const float cm2mm = 10;

  fEvt = new HepMC::GenEvent();

  int               barcode = 1;
  HepMC::GenVertex* Vtx0 = new HepMC::GenVertex( HepMC::FourVector( 0, 0, 0 ) );
  for ( const auto& dauPid : fPartIDs ) {
    bool  momInRegion( false );
    float momLv( 0. );
    float momTheta( 0. );
    while ( !momInRegion ) {
      momLv    = CLHEP::RandExponential::shoot( &engine, fLvBeta * cm2mm );
      momTheta = CLHEP::RandFlat::shoot( &engine, 0., TMath::Pi() );

      momInRegion = bound_.inRegionByLTheta( momLv / cm2mm, momTheta );
      if ( momInRegion )
        break;
    }

    float momPhi = CLHEP::RandFlat::shoot( &engine, -TMath::Pi(), TMath::Pi() );
    float momPt  = CLHEP::RandFlat::shoot( &engine, fMinPt, fMaxPt );

    TLorentzVector momp4;
    momp4.SetPtEtaPhiM( momPt, -log( tan( momTheta / 2 ) ), momPhi, fMomMass );

    HepMC::GenParticle* parMom = new HepMC::GenParticle(
        HepMC::FourVector( momp4.Px(), momp4.Py(), momp4.Pz(), momp4.Energy() ),
        fMomPid, 21 );

    parMom->suggest_barcode( barcode );
    barcode++;

    Vtx0->add_particle_out( parMom );

    HepMC::GenVertex* Vtx1 = new HepMC::GenVertex( HepMC::FourVector(
        momLv * sin( momTheta ) * cos( momPhi ),
        momLv * sin( momTheta ) * sin( momPhi ), momLv * cos( momTheta ) ) );

    float dauTheta = CLHEP::RandFlat::shoot( &engine, 0., TMath::Pi() );
    float dauPhi = CLHEP::RandFlat::shoot( &engine, -TMath::Pi(), TMath::Pi() );
    const HepPDT::ParticleData* dauData =
        fPDGTable->particle( HepPDT::ParticleID( abs( dauPid ) ) );
    const float dauMass = dauData->mass().value();
    float       dauP = sqrt( fMomMass * fMomMass - 4 * dauMass * dauMass ) / 2;

    TLorentzVector dau0p4;
    dau0p4.SetPxPyPzE( dauP * sin( dauTheta ) * cos( dauPhi ),
                       dauP * sin( dauTheta ) * sin( dauPhi ),
                       dauP * cos( dauTheta ), fMomMass / 2 );
    dau0p4.Boost( momp4.BoostVector() );

    TLorentzVector dau1p4;
    dau1p4.SetPxPyPzE( -dauP * sin( dauTheta ) * cos( dauPhi ),
                       -dauP * sin( dauTheta ) * sin( dauPhi ),
                       -dauP * cos( dauTheta ), fMomMass / 2 );
    dau1p4.Boost( momp4.BoostVector() );

    HepMC::GenParticle* parDau0 = new HepMC::GenParticle(
        HepMC::FourVector( dau0p4.Px(), dau0p4.Py(), dau0p4.Pz(),
                           dau0p4.Energy() ),
        dauPid,
        22  // status code. [21,29] would be assert as true for isHardProcess()
            // by MCTruthHelper
    );

    parDau0->suggest_barcode( barcode );
    barcode++;

    Vtx1->add_particle_out( parDau0 );

    HepMC::GenParticle* parDau1 = new HepMC::GenParticle(
        HepMC::FourVector( dau1p4.Px(), dau1p4.Py(), dau1p4.Pz(),
                           dau1p4.Energy() ),
        -dauPid, 22 );

    parDau1->suggest_barcode( barcode );
    barcode++;

    Vtx1->add_particle_out( parDau1 );

    fEvt->add_vertex( Vtx1 );

    if ( fAddAntiParticle ) {
      HepMC::GenVertex* Vtx2 = new HepMC::GenVertex(
          HepMC::FourVector( -momLv * sin( momTheta ) * cos( momPhi ),
                             -momLv * sin( momTheta ) * sin( momPhi ),
                             -momLv * cos( momTheta ) ) );

      HepMC::GenParticle* parMomAnti = new HepMC::GenParticle(
          HepMC::FourVector( -momp4.Px(), -momp4.Py(), -momp4.Pz(),
                             momp4.Energy() ),
          fMomPid, 21 );

      parMomAnti->suggest_barcode( barcode );
      barcode++;

      Vtx2->add_particle_out( parMomAnti );

      dau0p4.SetPxPyPzE( dauP * sin( dauTheta ) * cos( dauPhi ),
                         dauP * sin( dauTheta ) * sin( dauPhi ),
                         dauP * cos( dauTheta ), fMomMass / 2 );
      dau0p4.Boost( -momp4.BoostVector() );

      dau1p4.SetPxPyPzE( -dauP * sin( dauTheta ) * cos( dauPhi ),
                         -dauP * sin( dauTheta ) * sin( dauPhi ),
                         -dauP * cos( dauTheta ), fMomMass / 2 );
      dau1p4.Boost( -momp4.BoostVector() );

      HepMC::GenParticle* parDau0Anti = new HepMC::GenParticle(
          HepMC::FourVector( dau0p4.Px(), dau0p4.Py(), dau0p4.Pz(),
                             dau0p4.Energy() ),
          dauPid, 22 );

      parDau0Anti->suggest_barcode( barcode );
      barcode++;

      Vtx2->add_particle_out( parDau0Anti );

      HepMC::GenParticle* parDau1Anti = new HepMC::GenParticle(
          HepMC::FourVector( dau1p4.Px(), dau1p4.Py(), dau1p4.Pz(),
                             dau1p4.Energy() ),
          -dauPid, 22 );

      parDau1Anti->suggest_barcode( barcode );
      barcode++;

      Vtx2->add_particle_out( parDau1Anti );

      fEvt->add_vertex( Vtx2 );
    }
  }
  fEvt->add_vertex( Vtx0 );

  fEvt->set_event_number( e.id().event() );
  fEvt->set_signal_process_id( 20 );

  std::unique_ptr<edm::HepMCProduct> BProduct( new edm::HepMCProduct() );
  BProduct->addHepMCData( fEvt );
  e.put( std::move( BProduct ), "unsmeared" );

  std::unique_ptr<GenEventInfoProduct> genEventInfo(
      new GenEventInfoProduct( fEvt ) );
  e.put( std::move( genEventInfo ) );

  if ( fVerbosity > 0 ) {
    std::cout << " LeptonJetGunProducer : End New Event Generation"
              << std::endl;
    fEvt->print();
  }
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( ljGunProd );
