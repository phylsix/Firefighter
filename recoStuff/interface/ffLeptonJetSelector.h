#ifndef recoStuff_ffLeptonJetSelector_H
#define recoStuff_ffLeptonJetSelector_H

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "Firefighter/recoStuff/interface/ffPFJetProcessors.h"

class ffLeptonJetSelector {
 public:
  ffLeptonJetSelector( const edm::ParameterSet& ps )
      : fStringCut( ps.getParameter<std::string>( "cut" ), true ),
        fUseChargedMass( ps.getParameter<bool>( "useChargedMass" ) ),
        fUseMuonTime( ps.getParameter<bool>( "useMuonTime" ) ) {
    if ( fUseChargedMass )
      fMinChargedMass = ps.getParameter<double>( "minChargedMass" );
    if ( fUseMuonTime )
      fMaxTimeLimit = ps.getParameter<double>( "maxTimeLimit" );
  }
  bool operator()( const reco::PFJet& jet ) const {
    if ( !fStringCut( jet ) )
      return false;

    if ( jet.muonMultiplicity() % 2 != 0 )
      return false;  // even muon multiplicity
    if ( jet.muonMultiplicity() > 0 and ff::sumCharge( jet ) != 0 )
      return false;  // mu-type must be neutral

    if ( fUseChargedMass and ff::chargedMass( jet ) < fMinChargedMass )
      return false;
    if ( fUseMuonTime and !ff::muonInTime( jet, fMaxTimeLimit ) )
      return false;

    return true;
  }

 private:
  StringCutObjectSelector<reco::PFJet> fStringCut;

  bool fUseChargedMass;
  bool fUseMuonTime;

  float fMinChargedMass;
  float fMaxTimeLimit;
};

#endif
