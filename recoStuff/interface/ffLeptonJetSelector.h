#ifndef recoStuff_ffLeptonJetSelector_H
#define recoStuff_ffLeptonJetSelector_H

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "Firefighter/recoStuff/interface/ffPFJetProcessors.h"

class ffLeptonJetSelector {
 public:
  ffLeptonJetSelector( const edm::ParameterSet& ps )
      : fStringCut( ps.getParameter<std::string>( "cut" ), true ),
        fMinChargedMass( ps.getParameter<double>( "minChargedMass" ) ),
        fMaxTimeLimit( ps.getParameter<double>( "maxTimeLimit" ) ) {}
  bool operator()( const reco::PFJet& jet ) const {
    return fStringCut( jet ) and ff::chargedMass( jet ) >= fMinChargedMass and
           ff::muonInTime( jet, fMaxTimeLimit );
  }

 private:
  StringCutObjectSelector<reco::PFJet> fStringCut;

  float fMinChargedMass;
  float fMaxTimeLimit;
};

#endif
