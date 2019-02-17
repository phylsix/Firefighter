#include "Firefighter/recoStuff/interface/MCKinematicFilter.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include <algorithm>

MCKinematicFilter::MCKinematicFilter( const edm::ParameterSet& ps )
    : gen_token_( consumes<reco::GenParticleCollection>(
          ps.getParameter<edm::InputTag>( "GenParticles" ) ) ),
      pdgId_( ps.getParameter<std::vector<int>>( "pdgId" ) ),
      minPt_( ps.getParameter<double>( "minPt" ) ) {}

bool
MCKinematicFilter::filter( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::GenParticleCollection> gen_h;
  e.getByToken( gen_token_, gen_h );
  assert( gen_h.isValid() );

  const reco::GenParticleCollection& genparticles = *gen_h;

  auto daughterParticleInRange = [this]( const reco::GenParticle& p ) {
    return find( pdgId_.cbegin(), pdgId_.cend(), abs( p.pdgId() ) ) !=
               pdgId_.cend() and
           p.isHardProcess() and p.pt() >= minPt_;
  };

  return count_if( genparticles.begin(), genparticles.end(),
                   daughterParticleInRange ) >= 4;
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( MCKinematicFilter );