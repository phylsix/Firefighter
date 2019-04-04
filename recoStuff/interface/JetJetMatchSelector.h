#ifndef recoStuff_JetJetMatchSelector_H
#define recoStuff_JetJetMatchSelector_H

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

template <typename T1, typename T2>
class JetJetMatchSelector {
 public:
  JetJetMatchSelector( const edm::ParameterSet& iC )
      : jet1Cut_( iC.getParameter<std::string>( "cut" ) ),
        jet2Cut_( iC.getParameter<std::string>( "cut" ) ),
        maxdist_( iC.getParameter<double>( "maxdist" ) ){};

  bool operator()( const T1& j1, const T2& j2 ) const {
    if ( !jet1Cut_( j1 ) or !jet2Cut_( j2 ) )
      return false;
    if ( deltaR( j1, j2 ) > maxdist_ )
      return false;
    return true;
  }

 private:
  StringCutObjectSelector<T1, true> jet1Cut_;
  StringCutObjectSelector<T2, true> jet2Cut_;
  double                            maxdist_;
};

#endif