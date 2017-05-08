from ROOT import gSystem
gSystem.Load("libCaloAnalysis")
from ROOT import Decoder

dec = Decoder("sys:2,name:2,type:4")
print("0000 01 01 = ",dec.value("type",5),dec.value("name",5),dec.value("sys",5))
print("0000 01 11 = ",dec.value("type",7),dec.value("name",7),dec.value("sys",7))
print("0000 11 11 = ",dec.value("type",15),dec.value("name",15),dec.value("sys",15))
print("0001 01 01 = ",dec.value("type",21),dec.value("name",21),dec.value("sys",21))
