from ROOT import gSystem
gSystem.Load("libcaloanalysis-myanalysis")
from ROOT import CaloAnalysis, TCanvas, TFile, TF1, gPad

#PARTICLE = "mu"
#ENERGY = 50
#SUFFIX = "0.6-0.8"

PARTICLE = "e"
ENERGY = 50
SUFFIX = ""

#SF2=5.39
SF2=1.0
#SF2=5.73 #B field
#SF2 = 6.12 #no B field
#filename2 = "/tmp/novaj/hits_fccsw_ecal_bfield1_"+PARTICLE+str(ENERGY)+"GeV_dim"+SUFFIX+".root"
#filename2 = "/tmp/novaj/hits_fccsw_ecal_bfield0_"+PARTICLE+str(ENERGY)+"GeV_eta025_"+SUFFIX+".root"
#filename2 = "/tmp/novaj/hits_fccsw_ecal_bfield1_mu100GeV_v2.root"
#filename2 = "../../FCCSW/output_b1_e50_n500_mctruthbranc_classicalrk4_mfieldfix.root"
#filename2 = "../../FCCSW/output_b1.root"
#filename2 = "/localscratch2/novaj/HEP-FCCSW/FCCSW/e20_LAr4mm_Lead2mm_nocryo.root"
filename2="../../FCCSW/output_calibHits.root"
#filename2="../../FCCSW/output.root" 

print "Processing file ",filename2
ma2 = CaloAnalysis(SF2, ENERGY, PARTICLE)
ma2.loop(filename2)
print "Mean hit energy: ", ma2.hitenergy.GetMean()
print "1/SF calculated: ", ENERGY/(ma2.hitenergy.GetMean())
#if ( (PARTICLE=="e") and (abs(ENERGY/(ma2.hitenergy.GetMean())-SF2)>0.01) ) :
#   print "Correct sampling fraction used??? Using ", SF2

#c1 = TCanvas("c1","c1",1000,1000)
#c1.Divide(2,2)
#c1.cd(1)
#ma2.cellenergy.Draw()
#ma2.cellenergy.Rebin(2)
#ma2.cellenergy.Fit("gaus")
#ma2.r_max.Draw()
#c1.cd(2)
#ma2.phi_max.Draw()
#c1.cd(3)
#ma2.x_max.Draw()
#c1.cd(4)
#ma2.y_max.Draw()
#gPad.Update()

closeInput = raw_input("Press ENTER to exit") 

#f2 = TFile("output-histo-"+PARTICLE+str(ENERGY)+"-b0-"+SUFFIX+".root", "recreate")
#c1.Write()
#c2.Write()
#ma2.hitenergy.Write()
#ma2.cellenergy.Write()
#ma2.hitphi.Write()
#ma2.deltaphi.Write()
#ma2.longprofile.Write()
#ma2.phi_out.Write()
#ma2.phi_outliers.Write()
#ma2.phi_max.Write()
#ma2.x_max.Write()
#ma2.y_max.Write()
#ma2.r_max.Write()
#ma2.x_outliers.Write()
#ma2.y_outliers.Write()
#ma2.z_outliers.Write()
#ma2.r_outliers.Write()
#f2.Close()

