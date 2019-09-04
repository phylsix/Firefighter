#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"
#include "Firefighter/recoStuff/interface/TrackExtrapolator.h"

#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"

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

  edm::EDGetToken  gen_token_;
  std::vector<int> allowedPids_;

  std::vector<std::pair<float, float>> cylinderBounds_;

  std::vector<int>                   gen_pid_;
  std::vector<int>                   gen_daupid_;
  std::vector<int>                   gen_charge_;
  math::XYZTLorentzVectorFCollection gen_p4_;
  std::vector<math::XYZPointF>       gen_vtx_;
  std::vector<math::XYZPointF>       gen_dauvtx_;
  std::vector<float>                 gen_daudr_;
  std::vector<std::vector<float>>    gen_posx_;
  std::vector<std::vector<float>>    gen_posy_;
  std::vector<std::vector<float>>    gen_posz_;
  std::vector<std::vector<float>>    gen_dauposdr_;
  std::vector<std::vector<float>>    gen_dauposdz_;
  std::vector<std::vector<float>>    gen_dauposdphi_;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleGen, "ffNtupleGen" );

ffNtupleGen::ffNtupleGen( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleGen::initialize( TTree&                   tree,
                         const edm::ParameterSet& ps,
                         edm::ConsumesCollector&& cc ) {
  gen_token_   = cc.consumes<reco::GenParticleCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  allowedPids_ = ps.getParameter<std::vector<int>>( "AllowedPids" );

  const std::vector<edm::ParameterSet>& cylinder_rz_ = ps.getParameterSetVector( "CylinderRZ" );
  for ( const auto& rz : cylinder_rz_ ) {
    const double& r_ = rz.getParameter<double>( "radius" );
    const double& z_ = rz.getParameter<double>( "absZ" );
    cylinderBounds_.emplace_back( r_, z_ );
  }

  tree.Branch( "gen_charge", &gen_charge_ );
  tree.Branch( "gen_pid", &gen_pid_ );
  tree.Branch( "gen_daupid", &gen_daupid_ );
  tree.Branch( "gen_p4", &gen_p4_ );
  tree.Branch( "gen_vtx", &gen_vtx_ );
  tree.Branch( "gen_dauvtx", &gen_dauvtx_ );
  tree.Branch( "gen_daudr", &gen_daudr_ );
  tree.Branch( "gen_posx", &gen_posx_ );
  tree.Branch( "gen_posy", &gen_posy_ );
  tree.Branch( "gen_posz", &gen_posz_ );
  tree.Branch( "gen_dauposdr", &gen_dauposdr_ );
  tree.Branch( "gen_dauposdz", &gen_dauposdz_ );
  tree.Branch( "gen_dauposdphi", &gen_dauposdphi_ );
}

void
ffNtupleGen::fill( const edm::Event& e, const edm::EventSetup& es ) {
  clear();

  using namespace edm;
  using namespace std;

  Handle<vector<reco::GenParticle>> gen_h;
  e.getByToken( gen_token_, gen_h );
  assert( gen_h.isValid() );

  ff::TrackExtrapolator tkExtrap( es );

  vector<reco::GenParticleRef> genRefs{};
  for ( size_t i( 0 ); i != gen_h->size(); ++i ) {
    const auto& particle    = ( *gen_h )[ i ];
    const auto& statusflags = particle.statusFlags();
    if ( !( statusflags.isLastCopy() && statusflags.fromHardProcess() && statusflags.isPrompt() ) )
      continue;

    if ( find( allowedPids_.cbegin(), allowedPids_.cend(), abs( particle.pdgId() ) ) == allowedPids_.cend() )
      continue;

    genRefs.emplace_back( gen_h, i );
  }

  for ( const auto& particle : genRefs ) {
    const auto& vtx = particle->vertex();

    gen_charge_.push_back( particle->charge() );
    gen_pid_.push_back( particle->pdgId() );
    gen_p4_.emplace_back( particle->px(), particle->py(), particle->pz(), particle->energy() );
    gen_vtx_.emplace_back( vtx.X(), vtx.Y(), vtx.Z() );

    vector<float> xp{};
    vector<float> yp{};
    vector<float> zp{};

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

    // gen daughter info.

    vector<float> dauposdr{};
    vector<float> dauposdz{};
    vector<float> dauposdphi{};

    if ( particle->charge() == 0 && particle->numberOfDaughters() == 2 ) {
      const reco::Candidate* _dau0 = particle->daughter( 0 );
      const reco::Candidate* _dau1 = particle->daughter( 1 );
      reco::GenParticleRef   dau0gen;
      reco::GenParticleRef   dau1gen;
      for ( size_t i( 0 ); i != gen_h->size(); ++i ) {
        reco::GenParticleRef dau( gen_h, i );
        const auto           daucand = dynamic_cast<const reco::Candidate*>( dau.get() );
        if ( daucand == _dau0 )
          dau0gen = dau;
        if ( daucand == _dau1 )
          dau1gen = dau;
        if ( dau0gen.isNonnull() and dau1gen.isNonnull() )
          break;
      }

      gen_daupid_.emplace_back( abs( dau0gen->pdgId() ) );
      gen_dauvtx_.emplace_back( dau0gen->vertex().X(), dau0gen->vertex().Y(), dau0gen->vertex().Z() );
      gen_dauposdr_.emplace_back( deltaR( *( dau0gen.get() ), *( dau1gen.get() ) ) );

      for ( const auto& cbound : cylinderBounds_ ) {
        const float& cb_r = cbound.first;
        const float& cb_z = cbound.second;

        float x0( NAN ), y0( NAN ), z0( NAN );
        if ( abs( dau0gen->vertex().Z() ) <= cb_z and abs( dau0gen->vertex().Rho() ) <= cb_r ) {
          tkExtrap.setParameters( cb_r, cb_z );
          TrajectoryStateOnSurface tsos = tkExtrap.propagate( *dau0gen.get() );
          if ( tsos.isValid() ) {
            x0 = tsos.globalPosition().x();
            y0 = tsos.globalPosition().y();
            z0 = tsos.globalPosition().z();
          }
        }

        float x1( NAN ), y1( NAN ), z1( NAN );
        if ( abs( dau1gen->vertex().Z() ) <= cb_z and abs( dau1gen->vertex().Rho() ) <= cb_r ) {
          tkExtrap.setParameters( cb_r, cb_z );
          TrajectoryStateOnSurface tsos = tkExtrap.propagate( *dau1gen.get() );
          if ( tsos.isValid() ) {
            x1 = tsos.globalPosition().x();
            y1 = tsos.globalPosition().y();
            z1 = tsos.globalPosition().z();
          }
        }

        if ( isnan( x0 ) or isnan( y0 ) or isnan( z0 ) or isnan( x1 ) or isnan( y1 ) or isnan( z1 ) ) {
          dauposdr.push_back( NAN );
          dauposdz.push_back( NAN );
          dauposdphi.push_back( NAN );
        } else {
          dauposdr.emplace_back( hypot( ( x0 - x1 ), ( y0 - y1 ) ) );
          dauposdz.emplace_back( fabs( z0 - z1 ) );
          float dphi = deltaPhi( math::XYZVectorF( x0, y0, z0 ).phi(),
                                 math::XYZVectorF( x1, y1, z1 ).phi() );
          dauposdphi.emplace_back( dau0gen->charge() > 0 ? dphi : -dphi );
        }
      }
      gen_dauposdr_.push_back( dauposdr );
      gen_dauposdz_.push_back( dauposdz );
      gen_dauposdphi_.push_back( dauposdphi );

    } else {
      gen_daupid_.emplace_back( 0 );
      gen_dauvtx_.emplace_back( NAN, NAN, NAN );
      gen_daudr_.emplace_back( NAN );
      gen_dauposdr_.push_back( dauposdr );
      gen_dauposdz_.push_back( dauposdz );
      gen_dauposdphi_.push_back( dauposdphi );
    }
  }
}

void
ffNtupleGen::clear() {
  gen_charge_.clear();
  gen_pid_.clear();
  gen_daupid_.clear();
  gen_p4_.clear();
  gen_vtx_.clear();
  gen_dauvtx_.clear();
  gen_daudr_.clear();
  gen_posx_.clear();
  gen_posy_.clear();
  gen_posz_.clear();
  gen_dauposdr_.clear();
  gen_dauposdz_.clear();
  gen_dauposdphi_.clear();
}