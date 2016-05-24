from ROOT import gSystem
gSystem.Load("libcaloanalysis-myanalysis")
from ROOT import CaloAnalysis, TCanvas, TFile, gPad

#PARTICLE = "mu"
#ENERGY = 50
#SUFFIX = "0.6-0.8"

PARTICLE = "e"
ENERGY = 50
SUFFIX = "test"

SF2=5.73 #B field
#SF2 = 6.11 #no B field
#filename2 = "/tmp/novaj/hits_fccsw_ecal_bfield1_e100GeV_v2.root"
#filename2 = "/tmp/novaj/hits_fccsw_ecal_bfield1_"+PARTICLE+str(ENERGY)+"GeV_n10000_phi"+SUFFIX+"_v1.root"
#filename2 = "/tmp/novaj/hits_fccsw_ecal_bfield0_"+PARTICLE+str(ENERGY)+"GeV_eta025_"+SUFFIX+".root"
#filename2 = "/tmp/novaj/hits_fccsw_ecal_bfield1_mu100GeV_v2.root"

#filename2 = "../../FCCSW/output_b1_n100.root"
filename2 = "../../FCCSW/output.root"   

print "Processing file ",filename2
ma2 = CaloAnalysis(SF2, ENERGY, PARTICLE)
ma2.loop(filename2)
print "Mean hit energy: ", ma2.hitenergy.GetMean()
print "1/SF calculated: ", ENERGY/(ma2.hitenergy.GetMean())
if ( (PARTICLE=="e") and (abs(ENERGY/(ma2.hitenergy.GetMean())-SF2)>0.01) ) :
   print "Correct sampling fraction used??? Using ", SF2

c1 = TCanvas("c1","c1",1000,1000)
c1.Divide(2,2)
c1.cd(1)
ma2.cellenergy.Draw()
#ma2.r_max.Draw()
c1.cd(2)
ma2.phi_max.Draw()
c1.cd(3)
ma2.x_max.Draw()
c1.cd(4)
ma2.y_max.Draw()
gPad.Update()

c2 = TCanvas("c2","c2",1000,1000)
c2.Divide(2,2)
c2.cd(1)
ma2.r_outliers.Draw()
c2.cd(2)
ma2.phi_outliers.Draw()
c2.cd(3)
ma2.x_outliers.Draw()
c2.cd(4)
ma2.y_outliers.Draw()
gPad.Update()


f2 = TFile("output-histo-"+PARTICLE+str(ENERGY)+"-b1-phi"+SUFFIX+".root", "recreate")
#f2 = TFile("output-histo-"+PARTICLE+str(ENERGY)+"-b0-"+SUFFIX+".root", "recreate")
c1.Write()
c2.Write()
ma2.hitenergy.Write()
ma2.cellenergy.Write()
ma2.hitphi.Write()
ma2.deltaphi.Write()
ma2.longprofile.Write()
ma2.phi_out.Write()
ma2.phi_outliers.Write()
ma2.phi_max.Write()
ma2.x_max.Write()
ma2.y_max.Write()
ma2.r_max.Write()
ma2.x_outliers.Write()
ma2.y_outliers.Write()
ma2.z_outliers.Write()
ma2.r_outliers.Write()
f2.Close()

