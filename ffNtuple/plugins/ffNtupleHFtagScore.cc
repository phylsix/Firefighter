#include "DataFormats/BTauReco/interface/JetTag.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

class ffNtupleHFtagScore : public ffNtupleBaseNoHLT {
 public:
  ffNtupleHFtagScore( const edm::ParameterSet& );

  void initialize( TTree&, const edm::ParameterSet&, edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken jet_token_;
  edm::EDGetToken deepcsv_b_token_;
  edm::EDGetToken deepcsv_bb_token_;
  edm::EDGetToken deepcsv_c_token_;
  edm::EDGetToken deepcsv_udsg_token_;
  edm::EDGetToken deepflavour_b_token_;
  edm::EDGetToken deepflavour_bb_token_;
  edm::EDGetToken deepflavour_lepb_token_;
  edm::EDGetToken deepflavour_uds_token_;
  edm::EDGetToken deepflavour_c_token_;
  edm::EDGetToken deepflavour_g_token_;

  // https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation102X#Supported_Algorithms_and_Operati
  const std::vector<float> kDeepCSV_b_wp{0.1241, 0.4184, 0.7527};
  const std::vector<float> kDeepCSV_cl_wp{0.04, 0.137, 0.66};
  const std::vector<float> kDeepCSV_cb_wp{0.35, 0.29, 0.10};
  const std::vector<float> kDeepFlavour_b_wp{0.0494, 0.2770, 0.7264};
  const std::vector<float> kDeepFlavour_cl_wp{0.03, 0.085, 0.48};
  const std::vector<float> kDeepFlavour_cb_wp{0.4, 0.29, 0.05};

  unsigned int              fNJets;
  std::vector<unsigned int> fDeepCSV_b_idResults;
  std::vector<unsigned int> fDeepFlavour_b_idResults;
  std::vector<unsigned int> fDeepCSV_cl_idResults;
  std::vector<unsigned int> fDeepFlavour_cl_idResults;
  std::vector<unsigned int> fDeepCSV_cb_idResults;
  std::vector<unsigned int> fDeepFlavour_cb_idResults;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleHFtagScore, "ffNtupleHFtagScore" );

ffNtupleHFtagScore::ffNtupleHFtagScore( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtupleHFtagScore::initialize( TTree& tree, const edm::ParameterSet& ps, edm::ConsumesCollector&& cc ) {
  jet_token_              = cc.consumes<edm::View<reco::Jet>>( ps.getParameter<edm::InputTag>( "jet" ) );
  deepcsv_b_token_        = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepcsv_b" ) );
  deepcsv_bb_token_       = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepcsv_bb" ) );
  deepcsv_c_token_        = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepcsv_c" ) );
  deepcsv_udsg_token_     = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepcsv_udsg" ) );
  deepflavour_b_token_    = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepflavour_b" ) );
  deepflavour_bb_token_   = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepflavour_bb" ) );
  deepflavour_lepb_token_ = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepflavour_lepb" ) );
  deepflavour_uds_token_  = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepflavour_uds" ) );
  deepflavour_c_token_    = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepflavour_c" ) );
  deepflavour_g_token_    = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepflavour_g" ) );

  tree.Branch( "hftagscore_n", &fNJets );
  tree.Branch( "hftagscore_DeepCSV_b", &fDeepCSV_b_idResults );
  tree.Branch( "hftagscore_DeepCSV_cl", &fDeepCSV_cl_idResults );
  tree.Branch( "hftagscore_DeepCSV_cb", &fDeepCSV_cb_idResults );
  tree.Branch( "hftagscore_DeepFlavour_b", &fDeepFlavour_b_idResults );
  tree.Branch( "hftagscore_DeepFlavour_cl", &fDeepFlavour_cl_idResults );
  tree.Branch( "hftagscore_DeepFlavour_cb", &fDeepFlavour_cb_idResults );
}

void
ffNtupleHFtagScore::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<View<reco::Jet>> jet_h;
  e.getByToken( jet_token_, jet_h );
  assert( jet_h.isValid() );

  Handle<reco::JetTagCollection> deepcsv_b_h;
  Handle<reco::JetTagCollection> deepcsv_bb_h;
  Handle<reco::JetTagCollection> deepcsv_c_h;
  Handle<reco::JetTagCollection> deepcsv_udsg_h;
  Handle<reco::JetTagCollection> deepflavour_b_h;
  Handle<reco::JetTagCollection> deepflavour_bb_h;
  Handle<reco::JetTagCollection> deepflavour_lepb_h;
  Handle<reco::JetTagCollection> deepflavour_uds_h;
  Handle<reco::JetTagCollection> deepflavour_c_h;
  Handle<reco::JetTagCollection> deepflavour_g_h;

  e.getByToken( deepcsv_b_token_, deepcsv_b_h );
  e.getByToken( deepcsv_bb_token_, deepcsv_bb_h );
  e.getByToken( deepcsv_c_token_, deepcsv_c_h );
  e.getByToken( deepcsv_udsg_token_, deepcsv_udsg_h );
  e.getByToken( deepflavour_b_token_, deepflavour_b_h );
  e.getByToken( deepflavour_bb_token_, deepflavour_bb_h );
  e.getByToken( deepflavour_lepb_token_, deepflavour_lepb_h );
  e.getByToken( deepflavour_uds_token_, deepflavour_uds_h );
  e.getByToken( deepflavour_c_token_, deepflavour_c_h );
  e.getByToken( deepflavour_g_token_, deepflavour_g_h );
  assert( deepcsv_b_h.isValid() );
  assert( deepcsv_bb_h.isValid() );
  assert( deepcsv_c_h.isValid() );
  assert( deepcsv_udsg_h.isValid() );
  assert( deepflavour_b_h.isValid() );
  assert( deepflavour_bb_h.isValid() );
  assert( deepflavour_lepb_h.isValid() );
  assert( deepflavour_uds_h.isValid() );
  assert( deepflavour_c_h.isValid() );
  assert( deepflavour_g_h.isValid() );
  const reco::JetTagCollection& deepcsv_b        = *deepcsv_b_h;
  const reco::JetTagCollection& deepcsv_bb       = *deepcsv_bb_h;
  const reco::JetTagCollection& deepcsv_c        = *deepcsv_c_h;
  const reco::JetTagCollection& deepcsv_udsg     = *deepcsv_udsg_h;
  const reco::JetTagCollection& deepflavour_b    = *deepflavour_b_h;
  const reco::JetTagCollection& deepflavour_bb   = *deepflavour_bb_h;
  const reco::JetTagCollection& deepflavour_lepb = *deepflavour_lepb_h;
  const reco::JetTagCollection& deepflavour_uds  = *deepflavour_uds_h;
  const reco::JetTagCollection& deepflavour_c    = *deepflavour_c_h;
  const reco::JetTagCollection& deepflavour_g    = *deepflavour_g_h;

  clear();

  fNJets = jet_h->size();
  for ( View<reco::Jet>::const_iterator jet = jet_h->begin(); jet != jet_h->end(); ++jet ) {
    size_t               idx    = jet - jet_h->begin();
    RefToBase<reco::Jet> jetRef = jet_h->refAt( idx );

    float deepcsv_b_score  = deepcsv_b[ jetRef ] + deepcsv_bb[ jetRef ];
    float deepcsv_cl_score = deepcsv_c[ jetRef ] / ( deepcsv_c[ jetRef ] + deepcsv_udsg[ jetRef ] );
    float deepcsv_cb_score = deepcsv_c[ jetRef ] / ( deepcsv_c[ jetRef ] + deepcsv_b[ jetRef ] + deepcsv_bb[ jetRef ] );

    float deepflavour_b_score  = deepflavour_b[ jetRef ] + deepflavour_bb[ jetRef ] + deepflavour_lepb[ jetRef ];
    float deepflavour_cl_score = deepflavour_c[ jetRef ] / ( deepflavour_c[ jetRef ] + deepflavour_uds[ jetRef ] + deepflavour_g[ jetRef ] );
    float deepflavour_cb_score = deepflavour_c[ jetRef ] / ( deepflavour_c[ jetRef ] + deepflavour_b_score );

    unsigned int deepcsv_b_idbit( 0 );
    for ( size_t i( 0 ); i != kDeepCSV_b_wp.size(); i++ ) {
      if ( deepcsv_b_score > kDeepCSV_b_wp[ i ] )
        deepcsv_b_idbit |= 1 << i;
    }
    fDeepCSV_b_idResults.emplace_back( deepcsv_b_idbit );

    unsigned int deepcsv_cl_idbit( 0 );
    for ( size_t i( 0 ); i != kDeepCSV_cl_wp.size(); i++ ) {
      if ( deepcsv_cl_score > kDeepCSV_cl_wp[ i ] )
        deepcsv_cl_idbit |= 1 << i;
    }
    fDeepCSV_cl_idResults.emplace_back( deepcsv_cl_idbit );

    unsigned int deepcsv_cb_idbit( 0 );
    for ( size_t i( 0 ); i != kDeepCSV_cb_wp.size(); i++ ) {
      if ( deepcsv_cb_score < kDeepCSV_cb_wp[ i ] )
        deepcsv_cb_idbit |= 1 << i;
    }
    fDeepCSV_cb_idResults.emplace_back( deepcsv_cb_idbit );

    unsigned int deepflvour_b_idbit( 0 );
    for ( size_t i( 0 ); i != kDeepFlavour_b_wp.size(); i++ ) {
      if ( deepflavour_b_score > kDeepFlavour_b_wp[ i ] )
        deepflvour_b_idbit |= 1 << i;
    }
    fDeepFlavour_b_idResults.emplace_back( deepflvour_b_idbit );

    unsigned int deepflvour_cl_idbit( 0 );
    for ( size_t i( 0 ); i != kDeepFlavour_cl_wp.size(); i++ ) {
      if ( deepflavour_cl_score > kDeepFlavour_cl_wp[ i ] )
        deepflvour_cl_idbit |= 1 << i;
    }
    fDeepFlavour_cl_idResults.emplace_back( deepflvour_cl_idbit );

    unsigned int deepflvour_cb_idbit( 0 );
    for ( size_t i( 0 ); i != kDeepFlavour_cb_wp.size(); i++ ) {
      if ( deepflavour_cb_score < kDeepFlavour_cb_wp[ i ] )
        deepflvour_cb_idbit |= 1 << i;
    }
    fDeepFlavour_cb_idResults.emplace_back( deepflvour_cb_idbit );
  }
}

void
ffNtupleHFtagScore::clear() {
  fNJets = 0;
  fDeepCSV_b_idResults.clear();
  fDeepFlavour_b_idResults.clear();
  fDeepCSV_cl_idResults.clear();
  fDeepFlavour_cl_idResults.clear();
  fDeepCSV_cb_idResults.clear();
  fDeepFlavour_cb_idResults.clear();
}
