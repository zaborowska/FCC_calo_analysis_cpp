from ROOT import gSystem
from ROOT import TCanvas, TFile, TF1, TMath, gPad
#import langaus

PARTICLE = "e"
ENERGY = 100
SUFFIX = "-01-01"
#Truncated mean used for muons (1%)
truncation = 0.01

filename0 = "output-histo-"+PARTICLE+str(ENERGY)+"-b0-phi"+SUFFIX+".root"
filename1 = "output-histo-"+PARTICLE+str(ENERGY)+"-b1-phi"+SUFFIX+".root"
#filename0 = "output-histo-e100-b0-v2.root"
#filename1 = "output-histo-e100-b1-v2.root"

print "Plots from files: ", filename0, " ", filename1

f0 = TFile.Open(filename0,"read")
#f0.ls()
f1 = TFile.Open(filename1,"read")

cellene_0 = f0.Get("cellenergy")
cellene_1 = f1.Get("cellenergy")
deltaphi_0 = f0.Get("deltaphi")
deltaphi_1 = f1.Get("deltaphi")
phimax_0 = f0.Get("phi_max")
phimax_1 = f1.Get("phi_max")

c0 = TCanvas("c0","c0",1000,1000)
c0.Divide(2,2)
c0.cd(1)
if (PARTICLE=="e"):
    cellene_0.SetAxisRange(90.,110.)
    cellene_1.SetAxisRange(90.,110.)
else:
    cellene_0.SetAxisRange(0.,ENERGY/10.)
    cellene_1.SetAxisRange(0.,ENERGY/10.)
cellene_1.Draw()
cellene_0.Draw("same")
cellene_1.SetLineColor(2)
 
if (PARTICLE=="e"):
    fit_gaus = TF1("fit_gaus","gaus")

    cellene_0.Fit("fit_gaus","Q")
    mean = fit_gaus.GetParameter(1)
    sigma = fit_gaus.GetParameter(2)
    cellene_0.Fit("fit_gaus","Q","",mean-2*sigma,mean+2*sigma)

    print "Mean Ecell (B=0) %5.2f " % cellene_0.GetMean(), " RMS %5.2f " % cellene_0.GetRMS()
    print "Fit Ecell (B=0) %5.2f" % fit_gaus.GetParameter(1), " sigma %5.2f " % fit_gaus.GetParameter(2)

    cellene_1.Fit("fit_gaus","Q")
    mean = fit_gaus.GetParameter(1)
    sigma = fit_gaus.GetParameter(2)
    cellene_1.Fit("fit_gaus","Q","",mean-2*sigma,mean+2*sigma)

    print "Mean Ecell (B=1) %5.2f " % cellene_1.GetMean(), " RMS %5.2f " % cellene_1.GetRMS()
    print "Fit Ecell (B=1) %5.2f" % fit_gaus.GetParameter(1), " sigma %5.2f " % fit_gaus.GetParameter(2)

else:
    #fit_landau = TF1("fit_landau","landau")
    #cellene_0.Fit("fit_landau","Q")
    #mpv = fit_landau.GetParameter(1)
    #sigma = fit_landau.GetParameter(2)  
    print "Mean Ecell (B=0) %5.2f " % cellene_0.GetMean(), " RMS %5.2f " % cellene_0.GetRMS()
    #print "Fit Ecell (B=0) mop %5.2f" % mpv, " scale sigma %5.2f " % sigma

    #cellene_1.Fit("fit_landau","Q")
    #mpv = fit_landau.GetParameter(1)
    #sigma = fit_landau.GetParameter(2)
    #fit_landau.SetLineColor(2)
    print "Mean Ecell (B=1) %5.2f " % cellene_1.GetMean(), " RMS %5.2f " % cellene_1.GetRMS()
    #print "Fit Ecell (B=1) mop %5.2f" % mpv, " scale sigma %5.2f " % sigma
    #Truncated mean:
    entries_truncation = cellene_0.GetEntries() -  cellene_0.GetEntries()*truncation
    for i in range(0,1000) :
        if cellene_0.Integral(0,i)>entries_truncation :
            #print "Truncated mean (1%) at bin: ", i
            break
    
    #print "Outside truncated mean : ", cellene_0.Integral(i,(cellene_0.GetNbinsX()+1))
    cellene_0.GetXaxis().SetRange(0,i)
    print "Truncated Mean Ecell (B=0) %5.2f " % cellene_0.GetMean(), " RMS %5.2f " % cellene_0.GetRMS()   

    for i in range(0,1000) :
        if cellene_1.Integral(0,i)>entries_truncation :
            #print "Truncated mean (1%) at bin: ", i
            break
    
    #print "Outside truncated mean : ", cellene_1.Integral(i,(cellene_1.GetNbinsX()+1))
    cellene_1.GetXaxis().SetRange(0,i)
    print "Truncated Mean Ecell (B=1) %5.2f " % cellene_1.GetMean(), " RMS %5.2f " % cellene_1.GetRMS()   

c0.cd(2)
if (PARTICLE=="e"):
    deltaphi_0.SetAxisRange(0.,1.)
    deltaphi_1.SetAxisRange(0.,1.)
else:
    deltaphi_0.SetAxisRange(0.,0.1)
    deltaphi_1.SetAxisRange(0.,0.1)
deltaphi_0.Draw()
deltaphi_1.Draw("same")
deltaphi_1.SetLineColor(2)

print "RMS of phi (B=0): mean %5.3f"  % deltaphi_0.GetMean(), " RMS %5.3f " % deltaphi_0.GetRMS()
print "RMS of phi (B=1): mean %5.3f"  % deltaphi_1.GetMean(), " RMS %5.3f " % deltaphi_1.GetRMS()

print "nbins ", cellene_0.GetNbinsX(), " bin width ",cellene_0.GetBinWidth(10)  
nbin_merge = 10
 
c0.cd(3)
phimax_0.Draw()
phimax_1.Draw("same")
phimax_1.SetLineColor(2)

print "Phi mean (B=0): %7.5f" % phimax_0.GetMean(), " RMS %7.5f" % phimax_0.GetRMS()
print "Phi mean (B=1): %7.5f" % phimax_1.GetMean(), " RMS %7.5f" % phimax_1.GetRMS()

c0.Update()

#c0.SaveAs("c_"+PARTICLE+str(ENERGY)+"_phi"+SUFFIX+".eps")

closeInput = raw_input("Press ENTER to exit")
