#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"
#include "Firefighter/recoStuff/interface/TrackExtrapolator.h"

#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include <memory>

class ffNtupleGen : public ffNtupleBase {
 public:
  ffNtupleGen( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

 private:
  void clear() final;

  edm::EDGetToken gen_token_;
  edm::EDGetToken gen_PU_token_;

  std::vector<std::pair<float, float>> cylinderBounds_;

  int   gen_PUNumInt_;
  float gen_TrueNumInt_;

  std::vector<int>                   gen_pid_;
  std::vector<int>                   gen_charge_;
  math::XYZTLorentzVectorFCollection gen_p4_;
  std::vector<math::XYZPointF>       gen_vtx_;

  std::vector<std::vector<float>> gen_posx_;
  std::vector<std::vector<float>> gen_posy_;
  std::vector<std::vector<float>> gen_posz_;

  std::vector<int>                   gen2_pid_;
  math::XYZTLorentzVectorFCollection gen2_p4_;
  std::vector<math::XYZPointF>       gen2_vtx_;
  std::vector<float>                 gen2_dr_;
  std::vector<std::vector<float>>    gen2_posdr_;
  std::vector<std::vector<float>>    gen2_posdz_;
  std::vector<std::vector<float>>    gen2_posdphi_;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleGen, "ffNtupleGen" );

ffNtupleGen::ffNtupleGen( const edm::ParameterSet& ps ) : ffNtupleBase( ps ) {}

void
ffNtupleGen::initialize( TTree&                   tree,
                         const edm::ParameterSet& ps,
                         edm::ConsumesCollector&& cc ) {
  gen_token_ = cc.consumes<reco::GenParticleCollection>(
      ps.getParameter<edm::InputTag>( "GenParticles" ) );
  gen_PU_token_ = cc.consumes<std::vector<PileupSummaryInfo>>(
      ps.getParameter<edm::InputTag>( "GenPU" ) );

  tree.Branch( "gen_PUNumInt", &gen_PUNumInt_, "gen_PUNumInt/I" );
  tree.Branch( "gen_TrueNumInt", &gen_TrueNumInt_, "gen_TrueNumInt/F" );

  const std::vector<edm::ParameterSet>& cylinder_rz_ =
      ps.getParameterSetVector( "CylinderRZ" );
  for ( const auto& rz : cylinder_rz_ ) {
    const double& r_ = rz.getParameter<double>( "radius" );
    const double& z_ = rz.getParameter<double>( "absZ" );
    cylinderBounds_.emplace_back( r_, z_ );
  }

  tree.Branch( "gen_charge", &gen_charge_ );
  tree.Branch( "gen_pid", &gen_pid_ );
  tree.Branch( "gen_p4", &gen_p4_ );
  tree.Branch( "gen_vtx", &gen_vtx_ );
  tree.Branch( "gen_posx", &gen_posx_ );
  tree.Branch( "gen_posy", &gen_posy_ );
  tree.Branch( "gen_posz", &gen_posz_ );

  tree.Branch( "gen2_p4", &gen2_p4_ );
  tree.Branch( "gen2_vtx", &gen2_vtx_ );
  tree.Branch( "gen2_dr", &gen2_dr_ );
  tree.Branch( "gen2_pid", &gen2_pid_ );
  tree.Branch( "gen2_posdr", &gen2_posdr_ );
  tree.Branch( "gen2_posdz", &gen2_posdz_ );
  tree.Branch( "gen2_posdphi", &gen2_posdphi_ );
}

void
ffNtupleGen::fill( const edm::Event& e, const edm::EventSetup& es ) {
  clear();

  using namespace edm;
  using namespace std;
  Handle<vector<PileupSummaryInfo>> PupInfo_h;
  e.getByToken( gen_PU_token_, PupInfo_h );

  Handle<vector<reco::GenParticle>> gen_h;
  e.getByToken( gen_token_, gen_h );

  assert( PupInfo_h.isValid() && gen_h.isValid() );

  map<size_t, size_t>   myGenIndex;  // bookkeeping
  vector<float>         xp{}, yp{}, zp{};
  ff::TrackExtrapolator tkExtrap( es );

  vector<reco::GenParticleRef> genRefs{};
  for ( size_t i( 0 ); i != gen_h->size(); ++i ) {
    const auto& particle = ( *gen_h )[ i ];
    if ( !particle.isHardProcess() )
      continue;
    if ( abs( particle.pdgId() ) < 9 )
      continue;
    genRefs.emplace_back( gen_h, i );
  }

  for ( const auto& particle : genRefs ) {
    const auto& vtx = particle->vertex();

    gen_charge_.push_back( particle->charge() );
    gen_pid_.push_back( particle->pdgId() );
    gen_p4_.emplace_back( particle->px(), particle->py(), particle->pz(),
                          particle->energy() );
    gen_vtx_.emplace_back( vtx.X(), vtx.Y(), vtx.Z() );

    xp.clear();
    yp.clear();
    zp.clear();

    // propagation for charged particles
    if ( particle->charge() != 0 ) {
      for ( const auto& cbound : cylinderBounds_ ) {
        const float& cb_r = cbound.first;
        const float& cb_z = cbound.second;

        float _x( NAN ), _y( NAN ), _z( NAN );

        if ( abs( vtx.Z() ) <= cb_z and abs( vtx.Rho() ) <= cb_r ) {
          tkExtrap.setParameters( cb_r, cb_z );
          TrajectoryStateOnSurface tsos = tkExtrap.propagate( *particle.get() );
          if ( tsos.isValid() ) {
            _x = tsos.globalPosition().x();
            _y = tsos.globalPosition().y();
            _z = tsos.globalPosition().z();
          }
        }

        xp.push_back( _x );
        yp.push_back( _y );
        zp.push_back( _z );
      }
    }

    gen_posx_.push_back( xp );
    gen_posy_.push_back( yp );
    gen_posz_.push_back( zp );

    myGenIndex[ particle.key() ] = gen_posx_.size() - 1;
  }

  map<size_t, pair<size_t, size_t>>
      dp2dauLink{};  // default key_compare is *less*
  for ( const auto& particle : genRefs ) {
    if ( particle->charge() != 0 )
      continue;
    if ( particle->numberOfDaughters() != 2 )
      continue;

    const reco::Candidate* _dau0 = particle->daughter( 0 );
    const reco::Candidate* _dau1 = particle->daughter( 1 );
    if ( _dau0->charge() == 0 or _dau1->charge() == 0 )
      continue;

    size_t dau0Index( 9999 ), dau1Index( 9999 );
    for ( const auto& dau : genRefs ) {
      if ( dau->charge() == 0 )
        continue;
      if ( dynamic_cast<const reco::Candidate*>( dau.get() ) == _dau0 ) {
        dau0Index = dau.key();
        continue;
      }
      if ( dynamic_cast<const reco::Candidate*>( dau.get() ) == _dau1 ) {
        dau1Index = dau.key();
        continue;
      }
    }
    if ( dau0Index == 9999 or dau1Index == 9999 )
      continue;
    dp2dauLink[ particle.key() ] = make_pair( dau0Index, dau1Index );
  }

  // gen2
  for ( auto i( dp2dauLink.begin() ); i != dp2dauLink.end(); ++i ) {
    const auto& dpIdx   = i->first;
    const auto& daus    = i->second;
    const auto& dau0Idx = daus.first;
    const auto& dau1Idx = daus.second;

    const size_t& dau0IdxLocal = myGenIndex[ dau0Idx ];
    const size_t& dau1IdxLocal = myGenIndex[ dau1Idx ];

    const auto& dp   = ( *gen_h )[ dpIdx ];
    const auto& dau0 = ( *gen_h )[ dau0Idx ];
    const auto& dau1 = ( *gen_h )[ dau1Idx ];

    const auto& vtxCommon = dau0.vertex();
    float       _dr       = deltaR( dau0, dau1 );
    int         _pid      = abs( dau0.pdgId() );
    bool        _aPos_bNeg =
        dau0.charge() > 0 &&
        dau1.charge() < 0;  // dau0 is positive and dau1 is negative

    vector<float> _posdrs, _posdzs, _posdphis;  // "pos" => position
    for ( size_t cb( 0 ); cb != cylinderBounds_.size(); ++cb ) {
      float& _x0 = gen_posx_[ dau0IdxLocal ][ cb ];
      float& _y0 = gen_posy_[ dau0IdxLocal ][ cb ];
      float& _z0 = gen_posz_[ dau0IdxLocal ][ cb ];
      float& _x1 = gen_posx_[ dau1IdxLocal ][ cb ];
      float& _y1 = gen_posy_[ dau1IdxLocal ][ cb ];
      float& _z1 = gen_posz_[ dau1IdxLocal ][ cb ];
      float  _posdr( NAN ), _posdz( NAN ), _posdphi( NAN );

      if ( _x0 != NAN and _y0 != NAN and _z0 != NAN and _x1 != NAN and
           _y1 != NAN and _z1 != NAN ) {
        _posdr     = hypot( ( _x0 - _x1 ), ( _y0 - _y1 ) );
        _posdz     = abs( _z0 - _z1 );
        float dphi = deltaPhi( math::XYZVectorF( _x0, _y0, _z0 ).phi(),
                               math::XYZVectorF( _x1, _y1, _z1 ).phi() );
        _posdphi =
            _aPos_bNeg
                ? dphi
                : -dphi;  // make sure dphi means from positive to negative
      }

      _posdrs.push_back( _posdr );
      _posdzs.push_back( _posdz );
      _posdphis.push_back( _posdphi );
    }

    gen2_p4_.emplace_back( dp.px(), dp.py(), dp.pz(), dp.energy() );
    gen2_vtx_.emplace_back( vtxCommon.X(), vtxCommon.Y(), vtxCommon.Z() );
    gen2_dr_.push_back( _dr );
    gen2_pid_.push_back( _pid );
    gen2_posdr_.push_back( _posdrs );
    gen2_posdz_.push_back( _posdzs );
    gen2_posdphi_.push_back( _posdphis );
  }

  for ( const auto& PVI : *PupInfo_h ) {
    if ( PVI.getBunchCrossing() == 0 ) {
      gen_PUNumInt_   = PVI.getPU_NumInteractions();
      gen_TrueNumInt_ = PVI.getTrueNumInteractions();
    }
  }
}

void
ffNtupleGen::clear() {
  gen_PUNumInt_   = 0;
  gen_TrueNumInt_ = 0.;

  gen_charge_.clear();
  gen_pid_.clear();
  gen_p4_.clear();
  gen_vtx_.clear();

  gen_posx_.clear();
  gen_posy_.clear();
  gen_posz_.clear();

  gen2_p4_.clear();
  gen2_vtx_.clear();
  gen2_dr_.clear();
  gen2_pid_.clear();
  gen2_posdr_.clear();
  gen2_posdz_.clear();
  gen2_posdphi_.clear();
}