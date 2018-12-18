from Firefighter.recoStuff.ffDsaPFCandMergeCluster_cff import *

for m in ffLeptonJetSeq._seq._collection:
    if m._TypedParameterizable__type.startswith('MC'):
        ffLeptonJetSeq.remove(m)