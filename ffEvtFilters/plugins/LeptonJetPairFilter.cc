#include "Firefighter/ffEvtFilters/interface/LeptonJetPairFilter.h"

#include "DataFormats/Math/interface/deltaPhi.h"

#include <cmath>

LeptonJetPairFilter::LeptonJetPairFilter( const edm::ParameterSet& ps )
    : fJetToken( consumes<reco::PFJetCollection>(
          ps.getParameter<edm::InputTag>( "src" ) ) ),
      fLogic( ps.getParameter<std::string>( "logic" ) ),
      fDPhiThreshold( ps.getParameter<double>( "dphi" ) ) {
  assert( fLogic == "min" or fLogic == "max" );
  assert( fDPhiThreshold >= 0 and fDPhiThreshold <= M_PI );
}

bool
LeptonJetPairFilter::filter( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  e.getByToken( fJetToken, fJetHdl );
  assert( fJetHdl.isValid() );

  if ( fJetHdl->size() < 2 )
    return false;

  vector<Ptr<reco::PFJet>> jetptrs{};
  for ( size_t i( 0 ); i != fJetHdl->size(); ++i )
    jetptrs.emplace_back( fJetHdl, i );

  sort( jetptrs.begin(), jetptrs.end(), []( const auto& lhs, const auto& rhs ) {
    return lhs->pt() > rhs->pt();
  } );

  double absdphi = fabs( deltaPhi( jetptrs[ 0 ]->phi(), jetptrs[ 1 ]->phi() ) );

  if ( fLogic == "min" )
    return absdphi > fDPhiThreshold;
  if ( fLogic == "max" )
    return absdphi < fDPhiThreshold;

  return true;
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( LeptonJetPairFilter );