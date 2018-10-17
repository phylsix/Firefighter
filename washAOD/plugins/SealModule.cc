#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "Firefighter/washAOD/interface/genTuplizer.h"
#include "Firefighter/washAOD/interface/recoEffiForMuTrack.h"
#include "Firefighter/washAOD/interface/trigEffiForMuTrack.h"
#include "Firefighter/washAOD/interface/trigEffiForMuon.h"
#include "Firefighter/washAOD/interface/trigSelfEffiForMuTrack.h"
#include "Firefighter/washAOD/interface/trigSelfEffiForMuon.h"
#include "Firefighter/washAOD/interface/pfJetAnalysis.h"
#include "Firefighter/washAOD/interface/dsaMatching.h"
#include "Firefighter/washAOD/interface/jetMassSculpting.h"

DEFINE_FWK_MODULE(genTuplizer);
DEFINE_FWK_MODULE(recoEffiForMuTrack);
DEFINE_FWK_MODULE(trigEffiForMuTrack);
DEFINE_FWK_MODULE(trigEffiForMuon);
DEFINE_FWK_MODULE(trigSelfEffiForMuTrack);
DEFINE_FWK_MODULE(trigSelfEffiForMuon);
DEFINE_FWK_MODULE(pfJetAnalysis);
DEFINE_FWK_MODULE(dsaMatching);
DEFINE_FWK_MODULE(jetMassSculpting);