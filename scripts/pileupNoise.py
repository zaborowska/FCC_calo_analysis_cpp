import calo_init
## add arguments relevant only for that script
calo_init.add_defaults()
calo_init.parser.add_argument("--histoname", help="Name of the pile-up histograms", type = str)
calo_init.parser.add_argument("--nLayers", help="Number of layers", type = int)
calo_init.parser.add_argument("--nBins", help="Number of |eta|-bins", type = int)
calo_init.parser.add_argument("--mu", help="Number of pileup collisions to scale data to", type = int)
calo_init.parse_args()

from math import sqrt
from ROOT import gSystem
gSystem.Load("libCaloAnalysis")
from ROOT import TCanvas, TFile, gStyle, gPad, kGreen, kRed, kBlue, TColor, TPad, TH1, TH2
from draw_functions import *

# use this script for multiple files
# gStyle.SetPalette(56) # kInvertedDarkBodyRadiator
gStyle.SetPalette(73) # kCMYK
gStyle.SetOptFit(1)
gStyle.SetOptStat(0)

nlayers = 8
nbins = 20
mu = 200
histoname1 = "energyVsAbsEta"
histoname2 = "energyAllEventsVsAbsEta"


if calo_init.args.histoname:
    histoname1 = calo_init.args.histoname
if calo_init.args.nLayers:
    nlayers = calo_init.args.nLayers
if calo_init.args.nBins:
    nbins = calo_init.args.nBins
if calo_init.args.mu:
    mu = calo_init.args.mu

# Scaling factor to a different number of average pile-up events per bunch crossing
# 1.6: correction for out-of-time pile-up
#scale = 1.6*sqrt(mu)
# only in-time pile-up
scale = sqrt(mu)
print "Scaling factor: ", scale

# 2D histograms read from the input file
hEnVsAbsEta = []
hEnAllEventsVsAbsEta = []
# Final pileup plot per layer
hPileup = []
hPileupAllEvents = []
hPileupMeanE = []
hPileupMeanEAllEvents = []


for ifile, filename in enumerate(calo_init.filenamesIn):
    print "Files with pileup: " + filename
    f = ROOT.TFile(filename,"r")
    # get the TH2 histograms and make a projection to extract the RMS of energy distribution per eta bin
    for index in xrange(nlayers):
        histoname1_i = histoname1+str(index)
        hEnVsAbsEta.append(f.Get(histoname1_i))
        if (hEnVsAbsEta[index].GetNbinsX()<nbins):
            nbins = hEnVsAbsEta[index].GetNbinsX()
            print "More bins than available required!!! Setting nbins to ", nbins
        etaMax = hEnVsAbsEta[index].GetXaxis().GetBinUpEdge(nbins)
        
        hPile = TH1F("h_pileup_layer"+str(index+1),"Pile-up in "+str(index+1)+". layer", nbins, 0, etaMax)
        hPile.GetXaxis().SetTitle("|#eta|")
        hPile.GetYaxis().SetTitle("Pile-up noise [GeV]")
        prepare_graph(hPile, hPile.GetName(), hPile.GetTitle(), 1)
        hPileMeanE = TH1F("h_pileupMeanE_layer"+str(index+1),"Pile-up mean energy in "+str(index+1)+". layer", nbins, 0, etaMax)
        hPileMeanE.GetXaxis().SetTitle("|#eta|")
        hPileMeanE.GetYaxis().SetTitle("Pile-up mean energy [GeV]")
        prepare_graph(hPileMeanE, hPileMeanE.GetName(), hPileMeanE.GetTitle(), 1)
        for i in range(1, nbins):
            hProj = hEnVsAbsEta[index].ProjectionY( "hprojection_"+str(index), i, i,"e")
            hPile.SetBinContent(i,hProj.GetRMS()*scale)
            hPile.SetBinError(i,hProj.GetRMSError()*scale)
            #Scaling for <E> not clear, distribution very different from sum of all events
            hPileMeanE.SetBinContent(i,hProj.GetMean())
            hPileMeanE.SetBinError(i,hProj.GetMeanError())
            if index==1:
                print "Layer: ", index+1, " eta bin ", i, " RMS ", hProj.GetRMS()*scale, "+/-", hProj.GetRMSError()*scale, " mean ", hProj.GetMean(), "+/-",hProj.GetMeanError()
        hPileup.append(hPile)
        hPileupMeanE.append(hPileMeanE)

        histoname2_i = histoname2+str(index)
        hEnAllEventsVsAbsEta.append(f.Get(histoname2_i))

        hPileAllEvents = TH1F("h_pileupAllEvents_layer"+str(index+1),"Pile-up in "+str(index+1)+". layer", nbins, 0, etaMax)
        hPileAllEvents.GetXaxis().SetTitle("|#eta|")
        hPileAllEvents.GetYaxis().SetTitle("Pile-up noise [GeV]")
        hPileAllEvents.SetLineColor(2)
        hPileAllEvents.SetMarkerColor(2)
        prepare_graph(hPileAllEvents, hPileAllEvents.GetName(), hPileAllEvents.GetTitle(), 2)
        hPileMeanEAllEvents = TH1F("h_pileupMeanEAllEvents_layer"+str(index+1),"Pile-up mean energy in "+str(index+1)+". layer", nbins, 0, etaMax)
        hPileMeanEAllEvents.GetXaxis().SetTitle("|#eta|")
        hPileMeanEAllEvents.GetYaxis().SetTitle("Pile-up mean energy [GeV]")
        hPileMeanEAllEvents.SetLineColor(2)
        hPileMeanEAllEvents.SetMarkerColor(2)
        prepare_graph(hPileMeanEAllEvents, hPileMeanEAllEvents.GetName(), hPileMeanEAllEvents.GetTitle(), 2)

        for i in range(1, nbins):
            hProjAllEvents = hEnAllEventsVsAbsEta[index].ProjectionY( "hprojection_allevents_"+str(index), i, i,"e")
            hPileAllEvents.SetBinContent(i,hProjAllEvents.GetRMS())
            hPileAllEvents.SetBinError(i,hProjAllEvents.GetRMSError())
            hPileMeanEAllEvents.SetBinContent(i,hProjAllEvents.GetMean())
            hPileMeanEAllEvents.SetBinError(i,hProjAllEvents.GetMeanError())
            if index==1:
                print "Layer: ", index+1, "eta bin ", i, " RMS ", hProjAllEvents.GetRMS(), "+/-", hProjAllEvents.GetRMSError()*scale, " mean ", hProjAllEvents.GetMean(), "+/-", hProjAllEvents.GetMeanError()
        hPileupAllEvents.append(hPileAllEvents)
        hPileupMeanEAllEvents.append(hPileMeanEAllEvents)

legend = TLegend(0.12,0.7,0.5,0.85)
legend.SetBorderSize(0)

#Draw pile-up per layer
canv = TCanvas('Pile-up noise', 'canv', 1600, 1000 )
canv.Divide(3,3)
canv1 = TCanvas('Pile-up mean energy', 'canv1', 1600, 1000 )
canv1.Divide(3,3)
for i in xrange(nlayers):
    canv.cd(i+1)
    if i==0:
        legend.AddEntry(hPileup[i],"1 min. bias event scaled","pl")
        legend.AddEntry(hPileupAllEvents[i],"200 min. bias events","pl")
    hPileupAllEvents[i].SetMarkerSize(1.1)
    hPileup[i].SetMarkerSize(1.1)
    hPileupAllEvents[i].GetXaxis().SetTitleOffset(0.95)
    hPileupAllEvents[i].GetYaxis().SetTitleOffset(0.95)
    hPileupAllEvents[i].GetXaxis().SetTitleSize(0.05)
    hPileupAllEvents[i].GetYaxis().SetTitleSize(0.05)
    hPileupAllEvents[i].Draw()
    hPileup[i].Draw("same")
    legend.Draw()
    canv1.cd(i+1)
    hPileupMeanEAllEvents[i].SetMarkerSize(1.1)
    hPileupMeanE[i].SetMarkerSize(1.1)
    hPileupMeanEAllEvents[i].GetXaxis().SetTitleOffset(0.95)
    hPileupMeanEAllEvents[i].GetYaxis().SetTitleOffset(0.95)
    hPileupMeanEAllEvents[i].GetXaxis().SetTitleSize(0.05)
    hPileupMeanEAllEvents[i].GetYaxis().SetTitleSize(0.05)
    hPileupMeanEAllEvents[i].Draw()
    #hPileupMeanE[i].Draw("same")

if calo_init.output(ifile):
    canv.Print(calo_init.output(ifile)+".gif")
    canv1.Print(calo_init.output(ifile)+"_meanE.gif")
    canv.Print(calo_init.output(ifile)+".eps")
    canv1.Print(calo_init.output(ifile)+"_meanE.eps")
    plots = TFile(calo_init.output(ifile)+".root","RECREATE")
else:
    canv.Print("TestPileup_mu"+str(mu)+".gif")
    canv1.Print("TestPileup_mu"+str(mu)+"_meanE.gif")
    canv.Print("TestPileup_mu"+str(mu)+".eps")
    canv1.Print("TestPileup_mu"+str(mu)+"_meanE.eps")
    plots = TFile("TestPileup_mu"+str(mu)+".root","RECREATE")

plots.cd()
for hset in [hPileup, hPileupAllEvents, hPileupMeanE, hPileupMeanEAllEvents]:
    for h in hset:
        h.Write()
plots.Close()
        
raw_input("Press ENTER to exit")
