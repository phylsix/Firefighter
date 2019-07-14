#include "DataFormats/BTauReco/interface/JetTag.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

class ffNtupleHFtagScore : public ffNtupleBase {
 public:
  ffNtupleHFtagScore( const edm::ParameterSet& );

  void initialize( TTree&, const edm::ParameterSet&, edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&, const edm::EventSetup&, HLTConfigProvider& ) override {}

 private:
  void clear() final;

  edm::EDGetToken jet_token_;
  edm::EDGetToken deepcsv_b_token_;
  edm::EDGetToken deepcsv_bb_token_;
  edm::EDGetToken deepcsv_c_token_;
  edm::EDGetToken deepcsv_udsg_token_;
  edm::EDGetToken csvv2_b_token_;
  edm::EDGetToken deepflavour_b_token_;
  edm::EDGetToken deepflavour_bb_token_;
  edm::EDGetToken deepflavour_lepb_token_;
  edm::EDGetToken deepflavour_uds_token_;
  edm::EDGetToken deepflavour_c_token_;
  edm::EDGetToken deepflavour_g_token_;

  std::vector<float> hftagscore_DeepCSV_b_;
  std::vector<float> hftagscore_DeepCSV_cl_;
  std::vector<float> hftagscore_DeepCSV_cb_;
  std::vector<float> hftagscore_CSVv2_b_;
  std::vector<float> hftagscore_DeepFlavour_b_;
  std::vector<float> hftagscore_DeepFlavour_cl_;
  std::vector<float> hftagscore_DeepFlavour_cb_;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleHFtagScore, "ffNtupleHFtagScore" );

ffNtupleHFtagScore::ffNtupleHFtagScore( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleHFtagScore::initialize( TTree& tree, const edm::ParameterSet& ps, edm::ConsumesCollector&& cc ) {
  jet_token_              = cc.consumes<edm::View<reco::Jet>>( ps.getParameter<edm::InputTag>( "jet" ) );
  deepcsv_b_token_        = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepcsv_b" ) );
  deepcsv_bb_token_       = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepcsv_bb" ) );
  deepcsv_c_token_        = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepcsv_c" ) );
  deepcsv_udsg_token_     = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepcsv_udsg" ) );
  csvv2_b_token_          = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "csvv2_b" ) );
  deepflavour_b_token_    = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepflavour_b" ) );
  deepflavour_bb_token_   = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepflavour_bb" ) );
  deepflavour_lepb_token_ = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepflavour_lepb" ) );
  deepflavour_uds_token_  = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepflavour_uds" ) );
  deepflavour_c_token_    = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepflavour_c" ) );
  deepflavour_g_token_    = cc.consumes<reco::JetTagCollection>( ps.getParameter<edm::InputTag>( "deepflavour_g" ) );

  tree.Branch( "hftagscore_DeepCSV_b", &hftagscore_DeepCSV_b_ );
  tree.Branch( "hftagscore_DeepCSV_cl", &hftagscore_DeepCSV_cl_ );
  tree.Branch( "hftagscore_DeepCSV_cb", &hftagscore_DeepCSV_cb_ );
  tree.Branch( "hftagscore_CSVv2_b", &hftagscore_CSVv2_b_ );
  tree.Branch( "hftagscore_DeepFlavour_b", &hftagscore_DeepFlavour_b_ );
  tree.Branch( "hftagscore_DeepFlavour_cl", &hftagscore_DeepFlavour_cl_ );
  tree.Branch( "hftagscore_DeepFlavour_cb", &hftagscore_DeepFlavour_cb_ );
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
  Handle<reco::JetTagCollection> csvv2_b_h;
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
  e.getByToken( csvv2_b_token_, csvv2_b_h );
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
  assert( csvv2_b_h.isValid() );
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
  const reco::JetTagCollection& csvv2_b          = *csvv2_b_h;
  const reco::JetTagCollection& deepflavour_b    = *deepflavour_b_h;
  const reco::JetTagCollection& deepflavour_bb   = *deepflavour_bb_h;
  const reco::JetTagCollection& deepflavour_lepb = *deepflavour_lepb_h;
  const reco::JetTagCollection& deepflavour_uds  = *deepflavour_uds_h;
  const reco::JetTagCollection& deepflavour_c    = *deepflavour_c_h;
  const reco::JetTagCollection& deepflavour_g    = *deepflavour_g_h;

  clear();

  for ( View<reco::Jet>::const_iterator jet = jet_h->begin(); jet != jet_h->end(); ++jet ) {
    size_t               idx    = jet - jet_h->begin();
    RefToBase<reco::Jet> jetRef = jet_h->refAt( idx );

    float deepcsv_b_score  = deepcsv_b[ jetRef ] + deepcsv_bb[ jetRef ];
    float deepcsv_cl_score = deepcsv_c[ jetRef ] / ( deepcsv_c[ jetRef ] + deepcsv_udsg[ jetRef ] );
    float deepcsv_cb_score = deepcsv_c[ jetRef ] / ( deepcsv_c[ jetRef ] + deepcsv_b[ jetRef ] + deepcsv_bb[ jetRef ] );

    float csvv2_b_score = csvv2_b[ jetRef ];

    float deepflavour_b_score  = deepflavour_b[ jetRef ] + deepflavour_bb[ jetRef ] + deepflavour_lepb[ jetRef ];
    float deepflavour_cl_score = deepflavour_c[ jetRef ] / ( deepflavour_c[ jetRef ] + deepflavour_uds[ jetRef ] + deepflavour_g[ jetRef ] );
    float deepflavour_cb_score = deepflavour_c[ jetRef ] / ( deepflavour_c[ jetRef ] + deepflavour_b_score );

    if ( deepcsv_b_score > 0 )
      hftagscore_DeepCSV_b_.emplace_back( deepcsv_b_score );
    if ( deepcsv_cl_score > 0 )
      hftagscore_DeepCSV_cl_.emplace_back( deepcsv_cl_score );
    if ( deepcsv_cb_score > 0 )
      hftagscore_DeepCSV_cb_.emplace_back( deepcsv_cb_score );
    if ( csvv2_b_score > 0 )
      hftagscore_CSVv2_b_.emplace_back( csvv2_b_score );
    if ( deepflavour_b_score > 0 )
      hftagscore_DeepFlavour_b_.emplace_back( deepflavour_b_score );
    if ( deepflavour_cl_score > 0 )
      hftagscore_DeepFlavour_cl_.emplace_back( deepflavour_cl_score );
    if ( deepflavour_cb_score > 0 )
      hftagscore_DeepFlavour_cb_.emplace_back( deepflavour_cb_score );
  }
}

void
ffNtupleHFtagScore::clear() {
  hftagscore_DeepCSV_b_.clear();
  hftagscore_DeepCSV_cl_.clear();
  hftagscore_DeepCSV_cb_.clear();
  hftagscore_CSVv2_b_.clear();
  hftagscore_DeepFlavour_b_.clear();
  hftagscore_DeepFlavour_cl_.clear();
  hftagscore_DeepFlavour_cb_.clear();
}
