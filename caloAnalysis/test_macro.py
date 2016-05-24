from ROOT import gSystem
gSystem.Load("libcaloanalysis-myanalysis")
from ROOT import CaloAnalysis, TCanvas, TFile, gPad

PARTICLE = "e"
ENERGY = 100
SUFFIX = "primvertexX0Y2599Z0"

SF1=6.11 #no B field
#filename1 = "/tmp/novaj/hits_fccsw_ecal_bfield0_e100GeV_v2.root"
#filename1 = "/tmp/novaj/hits_fccsw_ecal_bfield0_"+PARTICLE+str(ENERGY)+"GeV_phi"+SUFFIX+"_v1.root"
filename1 = "/tmp/novaj/hits_fccsw_ecal_bfield0_"+PARTICLE+str(ENERGY)+"GeV_eta025_"+SUFFIX+".root"
#filename1 = "/tmp/novaj/hits_fccsw_ecal_bfield0_mu100GeV_v2.root"
#SF2=5.66 #B field
SF2=5.73 #B field
#filename2 = "/tmp/novaj/hits_fccsw_ecal_bfield1_e100GeV_v2.root"
#filename2 = "/tmp/novaj/hits_fccsw_ecal_bfield1_"+PARTICLE+str(ENERGY)+"GeV_phi"+SUFFIX+"_v1.root"
filename2 = "/tmp/novaj/hits_fccsw_ecal_bfield1_"+PARTICLE+str(ENERGY)+"GeV_eta025_"+SUFFIX+".root"
#filename2 = "/tmp/novaj/hits_fccsw_ecal_bfield1_mu100GeV_v2.root"

print "Processing file ",filename1
ma = CaloAnalysis(SF1, ENERGY, PARTICLE)
ma.loop(filename1)

print "Mean hit energy: ", ma.hitenergy.GetMean()
print "1/SF calculated: ", ENERGY/(ma.hitenergy.GetMean())
if ( (PARTICLE=="e") and (abs(ENERGY/(ma.hitenergy.GetMean())-SF1)>0.01) ):
   print "Correct sampling fraction used??? Using ", SF1


c11 = TCanvas("c11","c11",1000,1000)
c11.Divide(2,2)
c11.cd(1)
ma.cellenergy.Draw()
#ma.r_max.Draw()
c11.cd(2)
ma.phi_max.Draw()
c11.cd(3)
ma.x_max.Draw()
c11.cd(4)
ma.y_max.Draw()
gPad.Update()

c22 = TCanvas("c22","c22",1000,1000)
c22.Divide(2,2)
c22.cd(1)
ma.r_outliers.Draw()
c22.cd(2)
ma.phi_outliers.Draw()
c22.cd(3)
ma.x_outliers.Draw()
c22.cd(4)
ma.y_outliers.Draw()
gPad.Update()



f = TFile("output-histo-"+PARTICLE+str(ENERGY)+"-b0-"+SUFFIX+".root", "recreate")
c11.Write()
c22.Write()
ma.hitenergy.Write()
ma.cellenergy.Write()
ma.hitphi.Write()
ma.deltaphi.Write()
ma.longprofile.Write()
ma.phi_out.Write()
ma.phi_outliers.Write()
ma.phi_max.Write()
ma.x_max.Write()
ma.y_max.Write()
ma.r_max.Write()
ma.x_outliers.Write()
ma.y_outliers.Write()
ma.z_outliers.Write()
ma.r_outliers.Write()

f.Close()


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


f2 = TFile("output-histo-"+PARTICLE+str(ENERGY)+"-b1-"+SUFFIX+".root", "recreate")
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

