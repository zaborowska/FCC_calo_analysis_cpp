from ROOT import gSystem
from ROOT import TCanvas, TFile, TF1, TMath, TLegend, gPad
#import langaus

def truncated_mean(histo,trunctation) :
    entries_truncation = histo.GetEntries() - histo.GetEntries()*truncation
    for j in range(0,1000) :
        if histo.Integral(0,j)>entries_truncation :
       #print "Truncated mean (1%) at bin: ", i
            index = j
            break
    print "Entries outside truncated region : ", histo.Integral(index,(histo.GetNbinsX()+1))
    histo.GetXaxis().SetRange(0,index)
    print "Truncated Mean Ecell (B=0) %5.2f " % histo.GetMean(), " RMS %5.2f " % histo.GetRMS() 


PARTICLE = "e"
ENERGY = 100

#Truncated mean used for muons (1%)
truncation = 0.01

#SUFFIX = ["15","16","17","1.5-1.7","00","-0.1-0.1","0.6-0.8"]
SUFFIX = ["-01-01","15-17","07"]

filename0=[]
filename1=[]
f0=[]
f1=[]

cellene_0=[]
cellene_1=[]
deltaphi_0=[]
deltaphi_1=[]
phimax_0=[]
phimax_1=[]
longprofile_0=[]
longprofile_1=[]

for i in range(len(SUFFIX)):

    filename0.append("output-histo-"+PARTICLE+str(ENERGY)+"-b0-phi"+SUFFIX[i]+".root")
    filename1.append("output-histo-"+PARTICLE+str(ENERGY)+"-b1-phi"+SUFFIX[i]+".root")

    print "Plots from files: ", filename0[i], " ", filename1[i]

    f0.append(TFile.Open(filename0[i],"read"))
    f1.append(TFile.Open(filename1[i],"read"))

    cellene_0.append(f0[i].Get("cellenergy"))
    cellene_1.append(f1[i].Get("cellenergy"))
    deltaphi_0.append(f0[i].Get("deltaphi"))
    deltaphi_1.append(f1[i].Get("deltaphi"))
    phimax_0.append(f0[i].Get("phi_max"))
    phimax_1.append(f1[i].Get("phi_max"))
    longprofile_0.append(f0[i].Get("longprofile"))
    longprofile_1.append(f1[i].Get("longprofile"))

    print "Phi mean (B=0): %7.5f" % phimax_0[i].GetMean(), " RMS %7.5f" % phimax_0[i].GetRMS()
    print "Phi mean (B=1): %7.5f" % phimax_1[i].GetMean(), " RMS %7.5f" % phimax_1[i].GetRMS()

    print "Mean Ecell (B=0) %5.2f " % cellene_0[i].GetMean(), " RMS %5.2f " % cellene_0[i].GetRMS()
    print "Mean Ecell (B=1) %5.2f " % cellene_1[i].GetMean(), " RMS %5.2f " % cellene_1[i].GetRMS()
   
    #Truncated mean:
    #truncated_mean(cellene_0[i],truncation) 
    #truncated_mean(cellene_1[i],truncation) 

   

c0 = TCanvas("c0","c0",1000,1000)
c0.Divide(2,2)

leg = TLegend(0.6,0.75,0.9,0.9)
leg.SetFillColor(0)

for i in range(len(cellene_0)):
   c0.cd(1)
   #cellene_0[i].SetAxisRange(0.,ENERGY/10.)
   #cellene_0[i].Rebin(20)
   cellene_0[i].SetLineColor(i+1)
   cellene_0[i].GetXaxis().SetTitle("Cell energy [GeV]")
   leg.AddEntry(cellene_0[i],SUFFIX[i],"lep")
   if i==0:
       cellene_0[i].Draw()
   else:
       cellene_0[i].Draw("same")
   #gPad.SetLogy(1)
   leg.Draw()
   c0.cd(2)
   #cellene_1[i].SetAxisRange(0.,ENERGY/10.)
   #cellene_1[i].Rebin(20)
   cellene_1[i].SetLineColor(i+1)
   cellene_1[i].GetXaxis().SetTitle("Cell energy [GeV]")
   if i==0:
       cellene_1[i].Draw()
   else:
       cellene_1[i].Draw("same")
   #gPad.SetLogy(1)
   leg.Draw()

   c0.cd(3)
   #longprofile_0[i].SetMinimum(0.03)
   #longprofile_0[i].SetMaximum(0.05)
   longprofile_0[i].SetLineColor(i+1)
   longprofile_0[i].GetXaxis().SetTitle("Rho [mm]")
   if i==0:
       longprofile_0[i].Draw()
   else:
       longprofile_0[i].Draw("same")
   leg.Draw()
   c0.cd(4)
   #longprofile_1[i].SetMinimum(0.03)
   #longprofile_1[i].SetMaximum(0.05)
   longprofile_1[i].SetLineColor(i+1)
   longprofile_1[i].GetXaxis().SetTitle("Rho [mm]")
   if i==0:
       longprofile_1[i].Draw()
   else:
       longprofile_1[i].Draw("same")
   leg.Draw()

gPad.Update()

#c0.SaveAs("c_"+PARTICLE+str(ENERGY)+"_all.eps")

closeInput = raw_input("Press ENTER to exit") 
