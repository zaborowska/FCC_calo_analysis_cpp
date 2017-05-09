from ROOT import gSystem
gSystem.Load("libCaloAnalysis")
from ROOT import Decoder

dec = Decoder("sys:2,name:2,type:4")
print("0000 01 01 = ",dec.value("type",5),dec.value("name",5),dec.value("sys",5))
print("0000 01 11 = ",dec.value("type",7),dec.value("name",7),dec.value("sys",7))
print("0000 11 11 = ",dec.value("type",15),dec.value("name",15),dec.value("sys",15))
print("0001 01 01 = ",dec.value("type",21),dec.value("name",21),dec.value("sys",21))

fields=("system","cryo","module","type","subtype","cell","eta")
ecal = Decoder("system:4,cryo:1,module:11,type:3,subtype:3,cell:6,eta:9")
vals = {36280732133: [5, 0, 79, 0, 0, 10, 135],
        35987131109: [5, 0, 87, 0, 0, 4, 134],
        37358668421: [5, 0, 84, 0, 0, 11, 139],
        36263955141: [5, 0, 86, 0, 0, 6, 135],
        35995519653: [5, 0, 85, 0, 0, 6, 134]}
for val in vals:
    ecal.setValue(val)
    for ifield, field in enumerate(fields):
        assert(ecal.value(field) == vals[val][ifield])
