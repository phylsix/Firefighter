#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"
#include "Firefighter/recoStuff/interface/TrackExtrapolator.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include <memory>



class ffNtupleGen : public ffNtupleBase
{
  public:
    ffNtupleGen(const edm::ParameterSet&);

    void initialize(TTree&, const edm::ParameterSet&, edm::ConsumesCollector&&) final;
    void fill(const edm::Event&, const edm::EventSetup&) final;
    void fill(const edm::Event&, const edm::EventSetup&, HLTConfigProvider&) override {}

  
  private:
    void clear() final;

    
    edm::EDGetToken gen_token_;
    edm::EDGetToken gen_PU_token_;

    std::vector<std::pair<float, float>> cylinderBounds_;

    int gen_PUNumInt_;
    float gen_TrueNumInt_;

    std::vector<int>   gen_pid_;
    std::vector<int>   gen_charge_;
    math::XYZTLorentzVectorFCollection gen_p4_;
    std::vector<math::XYZPointF> gen_vtx_;
    
    std::vector<std::vector<float>> gen_posx_;
    std::vector<std::vector<float>> gen_posy_;
    std::vector<std::vector<float>> gen_posz_;

    std::vector<int>   gen2_pid_;
    std::vector<math::XYZPointF> gen2_vtx_;
    std::vector<float> gen2_dr_;
    std::vector<std::vector<float>> gen2_posdr_;
    std::vector<std::vector<float>> gen2_posdz_;
    std::vector<std::vector<float>> gen2_posdphi_;
};

DEFINE_EDM_PLUGIN(ffNtupleFactory,
                  ffNtupleGen,
                  "ffNtupleGen");


ffNtupleGen::ffNtupleGen(const edm::ParameterSet& ps) :
  ffNtupleBase(ps)
{}

void
ffNtupleGen::initialize(TTree& tree,
                       const edm::ParameterSet& ps,
                       edm::ConsumesCollector&& cc)
{

  gen_token_ = cc.consumes<reco::GenParticleCollection>(ps.getParameter<edm::InputTag>("GenParticles"));
  gen_PU_token_ = cc.consumes<std::vector<PileupSummaryInfo>>(ps.getParameter<edm::InputTag>("GenPU"));

  tree.Branch("gen_PUNumInt",   &gen_PUNumInt_ ,   "gen_PUNumInt/I");
  tree.Branch("gen_TrueNumInt", &gen_TrueNumInt_ , "gen_TrueNumInt/F");

  const std::vector<edm::ParameterSet>& cylinder_rz_ = ps.getParameterSetVector("CylinderRZ");
  for (const auto& rz : cylinder_rz_)
  {
    const double& r_ = rz.getParameter<double>("radius");
    const double& z_ = rz.getParameter<double>("absZ");
    cylinderBounds_.emplace_back(r_, z_);
  }


  tree.Branch("gen_charge", &gen_charge_);
  tree.Branch("gen_pid",    &gen_pid_);
  tree.Branch("gen_p4",     &gen_p4_);
  tree.Branch("gen_vtx",    &gen_vtx_);
  tree.Branch("gen_posx",   &gen_posx_);
  tree.Branch("gen_posy",   &gen_posy_);
  tree.Branch("gen_posz",   &gen_posz_);

  tree.Branch("gen2_vtx",   &gen2_vtx_);
  tree.Branch("gen2_dr",    &gen2_dr_);
  tree.Branch("gen2_pid",   &gen2_pid_);
  tree.Branch("gen2_posdr", &gen2_posdr_);
  tree.Branch("gen2_posdz", &gen2_posdz_);
  tree.Branch("gen2_posdphi", &gen2_posdphi_);

}

void
ffNtupleGen::fill(const edm::Event& e,
                  const edm::EventSetup& es)
{
  clear();

  using namespace edm;
  using namespace std;
  Handle<vector<PileupSummaryInfo>> PupInfo_h;
  e.getByToken(gen_PU_token_, PupInfo_h);

  Handle<vector<reco::GenParticle>> gen_h;
  e.getByToken(gen_token_, gen_h);

  assert(PupInfo_h.isValid() && gen_h.isValid());


  map<size_t, size_t> myGenIndex; // bookkeeping
  vector<float> xp{}, yp{}, zp{};
  ff::TrackExtrapolator tkExtrap(es);

  for (const auto& particle : *gen_h)
  {
    if (!particle.isHardProcess()
        or abs(particle.pdgId())<9)
    {
      continue;
    }


    const auto& vtx = particle.vertex();
    
    gen_charge_.push_back(particle.charge());
    gen_pid_.push_back(particle.pdgId());
    gen_p4_.emplace_back(particle.px(), particle.py(), particle.pz(), particle.energy());
    gen_vtx_.emplace_back(vtx.X(), vtx.Y(), vtx.Z());



    xp.clear(); yp.clear(); zp.clear();

    // propagation for charged particles
    if (particle.charge()!=0)
    {
      for (const auto& cbound : cylinderBounds_)
      {
        const float& cb_r = cbound.first;
        const float& cb_z = cbound.second;

        float _x(NAN), _y(NAN), _z(NAN);

        if (    abs(vtx.Z())   <= cb_z
            and abs(vtx.Rho()) <= cb_r)
        {
          tkExtrap.setParameters(cb_r, cb_z);
          TrajectoryStateOnSurface tsos = tkExtrap.propagate(particle);
          if (tsos.isValid())
          {
            _x = tsos.globalPosition().x();
            _y = tsos.globalPosition().y();
            _z = tsos.globalPosition().z();
          }
        }

        xp  .push_back(_x);
        yp  .push_back(_y);
        zp  .push_back(_z);
      }
    }

    gen_posx_  .push_back(xp);
    gen_posy_  .push_back(yp);
    gen_posz_  .push_back(zp);
    
    size_t parIdx = &particle - &((*gen_h)[0]);
    myGenIndex[parIdx] = gen_posx_.size() - 1;

  }

  // gen2
  for (auto i(myGenIndex.begin()); i!=myGenIndex.end(); ++i)
  {
    const auto& parA = (*gen_h)[i->first];
    const size_t idxA = i->second;

    if (parA.charge()==0) continue;

    for (auto j(next(i)); j!=myGenIndex.end(); ++j)
    {
      const auto& parB = (*gen_h)[j->first];
      const size_t idxB = j->second;

      if (parB.charge()==0) continue;
      if (abs(parA.pdgId()) != abs(parB.pdgId())) continue;
      if (parA.vertex() != parB.vertex()) continue;

      const auto& vtx2 = parA.vertex();
      float _dr = deltaR(parA, parB);
      int _pid = abs(parA.pdgId());
      bool _aPosbNeg = parA.charge()>0 && parB.charge()<0; // parA is positive and parB is negative
      
      vector<float> _posdrs, _posdzs, _posdphis;
      for (size_t i(0); i!=cylinderBounds_.size(); ++i)
      {
        float& _xA = gen_posx_[idxA][i];
        float& _yA = gen_posy_[idxA][i];
        float& _zA = gen_posz_[idxA][i];
        float& _xB = gen_posx_[idxB][i];
        float& _yB = gen_posy_[idxB][i];
        float& _zB = gen_posz_[idxB][i];
        float _posdr(NAN), _posdz(NAN), _posdphi(NAN);

        if (    _xA!=NAN and _yA!=NAN and _zA!=NAN
            and _xB!=NAN and _yB!=NAN and _zB!=NAN)
        {
          _posdr = hypot((_xA - _xB), (_yA - _yB));
          _posdz = abs(_zA - _zB);
          float dphiAB = deltaPhi(
            math::XYZVectorF(_xA, _yA, _zA).phi(),
            math::XYZVectorF(_xB, _yB, _zB).phi()
          );
          _posdphi = _aPosbNeg ? dphiAB : -dphiAB; // make sure dphi means from positive to negative
        }
        
        _posdrs.push_back(_posdr);
        _posdzs.push_back(_posdz);
        _posdphis.push_back(_posdphi);
      }

      gen2_vtx_.emplace_back(vtx2.X(), vtx2.Y(), vtx2.Z());
      gen2_dr_.push_back(_dr);
      gen2_pid_.push_back(_pid);
      gen2_posdr_.push_back(_posdrs);
      gen2_posdz_.push_back(_posdzs);
      gen2_posdphi_.push_back(_posdphis);

    }
  }


  for(const auto& PVI : *PupInfo_h)
  {
    if(PVI.getBunchCrossing() == 0)
    { 
      gen_PUNumInt_ = PVI.getPU_NumInteractions();
      gen_TrueNumInt_ = PVI.getTrueNumInteractions();
    }
  }


}

void
ffNtupleGen::clear()
{
  gen_PUNumInt_ = 0;
  gen_TrueNumInt_ = 0.;

  gen_charge_.clear();
  gen_pid_.clear();
  gen_p4_.clear();
  gen_vtx_.clear();

  gen_posx_.clear();
  gen_posy_.clear();
  gen_posz_.clear();
  
  gen2_vtx_.clear();
  gen2_dr_.clear();
  gen2_pid_.clear();
  gen2_posdr_.clear();
  gen2_posdz_.clear();
  gen2_posdphi_.clear();
}