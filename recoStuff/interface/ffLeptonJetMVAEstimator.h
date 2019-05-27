#ifndef recoStuff_ffLeptonJetMVAEstimator_h
#define recoStuff_ffLeptonJetMVAEstimator_h

#include "CommonTools/MVAUtils/interface/GBRForestTools.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "Firefighter/recoStuff/interface/ffMVAEstimatorBase.h"

#include "DataFormats/JetReco/interface/PFJet.h"

class ffLeptonJetMVAEstimator : public ffMVAEstimatorBase {
 public:
  ffLeptonJetMVAEstimator( const edm::ParameterSet& ps );
  ~ffLeptonJetMVAEstimator() override {}

  float mvaValue(
      const reco::Candidate*              candidate,
      const std::map<std::string, float>& variableMap ) const override;

  int findCategory( const reco::Candidate* candidate ) const override;

 private:
  void init( const std::vector<std::string>& weightFileNames );
  int  findCategory( const reco::PFJet& jet ) const;

  std::vector<StringCutObjectSelector<reco::PFJet>> fCategoryFunctions;
  std::vector<std::unique_ptr<const GBRForest>>     fGbrForests;
  std::vector<std::vector<std::string>>             fCategorizedVariableNames;
};

#endif