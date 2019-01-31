#ifndef leptonJetGun_ljGunProd_h
#define leptonJetGun_ljGunProd_h

#include "IOMC/ParticleGuns/interface/BaseFlatGunProducer.h"
#include "TLorentzVector.h"
#include "Firefighter/recoStuff/interface/GeometryBoundary.h"

class ljGunProd : public edm::BaseFlatGunProducer
{
  public:
    explicit ljGunProd(const edm::ParameterSet&);
    ~ljGunProd() {}

    void produce(edm::Event&, const edm::EventSetup&) override;


  private:
    float fMinPt;
    float fMaxPt;

    float fLvBeta;
    float fBoundEta;
    float fBoundR;
    float fBoundZ;

    float fMomMass;
    int   fMomPid;

    ff::GeometryBoundary bound_;
};


#endif
