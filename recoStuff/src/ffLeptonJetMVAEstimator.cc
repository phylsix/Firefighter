#include "Firefighter/recoStuff/interface/ffLeptonJetMVAEstimator.h"

#include <cmath>

ffLeptonJetMVAEstimator::ffLeptonJetMVAEstimator( const edm::ParameterSet& ps )
    : ffMVAEstimatorBase( ps ) {
  const auto weightFileNames =
      ps.getParameter<std::vector<std::string>>( "weightFileNames" );
  const auto categoryCutStrings =
      ps.getParameter<std::vector<std::string>>( "categoryCuts" );

  // make sure: len(weightFilenames) == len(categoryCuts) == nCategories
  if ( (int)( categoryCutStrings.size() ) != getNCategories() )
    throw cms::Exception( "MVA config failure: " )
        << "wrong number of category cuts in ffLeptonJetMVAEstimator"
        << getTag() << std::endl;
  if ( (int)( weightFileNames.size() ) != getNCategories() ) {
    throw cms::Exception( "MVA config failure: " )
        << "wrong number of weightfiles in ffLeptonJetMVAEstimator" << getTag()
        << std::endl;
  }

  for ( const auto& cutstr : categoryCutStrings ) {
    fCategoryFunctions.emplace_back( cutstr, true );
  }

  init( weightFileNames );
}

void
ffLeptonJetMVAEstimator::init(
    const std::vector<std::string>& weightFileNames ) {
  fGbrForests.clear();
  fCategorizedVariableNames.clear();

  // create a TMVA reader object for each category
  for ( int i( 0 ); i != getNCategories(); ++i ) {
    std::vector<std::string> variableNamesInCategory;
    fGbrForests.push_back(
        createGBRForest( weightFileNames[ i ], variableNamesInCategory ) );
    fCategorizedVariableNames.push_back( variableNamesInCategory );
  }
}

float
ffLeptonJetMVAEstimator::mvaValue(
    const reco::Candidate*              candidate,
    const std::map<std::string, float>& variableMap ) const {
  int iCategory = findCategory( candidate );

  std::vector<float> variableValues{};
  for ( const auto& varName : fCategorizedVariableNames[ iCategory ] ) {
    variableValues.emplace_back( variableMap.at( varName ) );
  }

  return fGbrForests[ iCategory ]->GetResponse( variableValues.data() );
}

int
ffLeptonJetMVAEstimator::findCategory(
    const reco::Candidate* candidate ) const {
  const reco::PFJet* jet = dynamic_cast<const reco::PFJet*>( candidate );
  if ( jet == nullptr )
    throw cms::Exception( "MVA failure: " )
        << "given object is not reco::PFJet, which is what "
           "ffLeptonJetMVAEstimator is "
           "expected to be operating on."
        << std::endl;

  return findCategory( *jet );
}

int
ffLeptonJetMVAEstimator::findCategory( const reco::PFJet& jet ) const {
  for ( int i( 0 ); i < getNCategories(); ++i ) {
    if ( fCategoryFunctions[ i ]( jet ) )
      return i;
  }

  return -1;
}
