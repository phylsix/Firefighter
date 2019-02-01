from Firefighter.recoStuff.ffDsaPFCandMergeCluster_cff import *

for _m in ffLeptonJetSeq._seq._collection:
    if _m._TypedParameterizable__type.startswith('MC'):
        ffLeptonJetSeq.remove(_m)