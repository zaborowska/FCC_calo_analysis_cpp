import calo_init
## add arguments relevant only for that script
calo_init.add_defaults()
calo_init.parse_args()

from ROOT import gSystem
from ROOT import TCanvas, TFile, gStyle, kGreen, kRed, kBlue, TColor, TF1, kBlack
from draw_functions import *

# gStyle.SetPalette(56) # kInvertedDarkBodyRadiator
gStyle.SetPalette(73) # kCMYK

for ifile, filename in enumerate(calo_init.filenamesIn):
    energy = calo_init.energy(ifile)
    print "Initial particle energy: " + str(energy) + "GeV"
    print "File with reconstruction histograms: " + filename
    # retrieve histograms to draw them
    fileHists = TFile(filename, "READ")
    hEn = fileHists.Get('energy')
    # hEnTotal = fileHists.Get('energyTotal')
    hEnFncPhi = fileHists.Get('energy_phi')
    hEta = fileHists.Get('eta')
    hPhi = fileHists.Get('phi')
    hPhiFncPhi = fileHists.Get('phi_phi')
    hEtaFncEta = fileHists.Get('eta_eta')
    hNo = fileHists.Get('clusters')
    hNoFncPhi = fileHists.Get('clusters_phi')
    hNoFncEta = fileHists.Get('clusters_eta')
    hEnMoreClu = fileHists.Get('energy_duplicates')
    hEnDiffMoreClu = fileHists.Get('energy_diff')
    hEtaMoreClu = fileHists.Get('eta_duplicates')
    hEtaDiffMoreClu = fileHists.Get('eta_diff')
    hPhiMoreClu = fileHists.Get('phi_duplicates')
    hPhiDiffMoreClu = fileHists.Get('phi_diff')
    hRDiffMoreClu = fileHists.Get('R_diff')
    h1dset1 = [hEn, hEta, hPhi, hNo, hEnMoreClu, hEtaMoreClu, hPhiMoreClu]
    for h in h1dset1:
        h.SetTitleSize(30)
        h.SetTitleFont(22)
        h.GetXaxis().SetLabelFont(132)
        h.GetXaxis().SetLabelSize(0.04)
        h.GetXaxis().SetTitleFont(132)
        h.GetXaxis().SetTitleSize(0.05)
        h.GetXaxis().CenterTitle(True)
        h.GetYaxis().SetLabelFont(132)
        h.GetYaxis().SetLabelSize(0.04)
        h.GetYaxis().SetTitleFont(132)
        h.GetYaxis().SetTitleSize(0.05)
        h.GetYaxis().CenterTitle(True)
        h.SetMarkerColor(kBlue+3)
    h2d = [hEnFncPhi, hPhiFncPhi, hEtaFncEta, hNoFncPhi, hNoFncEta]
    # hEn.Rebin(2)
    # hPhi.Rebin(3)
    hEta.GetXaxis().SetRangeUser(-0.02,0.02)
    hPhi.GetXaxis().SetRangeUser(-0.08,0.04)
    hEn.GetXaxis().SetRangeUser(0.5*energy,1.5*energy)
    hEta.SetMinimum(0)
    hPhi.SetMinimum(0)

    # fit functions
    fitEnergy = hEn.GetListOfFunctions().FirstLink().GetObject()
    fitPhi = hPhi.GetListOfFunctions().FirstLink().GetObject()
    fitEta = hEta.GetListOfFunctions().FirstLink().GetObject()
    # fitEnergy.GetXaxis().SetRangeUser(30,50)
    # fitPhi.GetXaxis().SetRangeUser(-0.002,0.002)
    # fitEta.GetXaxis().SetRangeUser(-0.003,0.003)
    # hEn.Fit(fitEnergy.GetName(),'R')
    # hPhi.GetXaxis().SetRangeUser(-0.002,0.002)
    # hPhi.Fit('gaus','R')
    # hPhi.GetXaxis().SetRangeUser(-0.02,0.02)
    # hEta.Fit(fitEta.GetName(),'R')
    # hPhi.GetXaxis().SetRangeUser(-0.02,0.02)
    # hEta.GetXaxis().SetRangeUser(-0.02,0.02)

    canv = TCanvas('ECal_presentation_e'+str(energy)+'GeV', 'ECal', 1200, 900 )
    canv.Divide(2,2)
    canv.cd(1)
    hEn.SetStats(0)
    hEn.GetYaxis().SetTitleOffset(1.1)
    hEn.SetTitle('energy distribution')
    hEn.Draw('bar')
    gStyle.SetOptStat(1)
    # draw_text(["Mean: "+str(round(hEn.GetMean(),1))+" GeV",
    #            "RMS: "+str(round(hEn.GetRMS(),1))+" GeV"],
    #           [0.6,0.79,0.9,0.9],
    #           kBlue+3)
    draw_text(["energy: "+str(round(fitEnergy.GetParameter(1),1))+" GeV",
               "           "+str(round(fitEnergy.GetParameter(1),1)/energy*100.)+" %",
               "resolution: "+str(round(fitEnergy.GetParameter(2)/fitEnergy.GetParameter(1)*100,1))+" %"],
              [0.6,0.7,0.9,0.9],
              kBlue+3)
    canv.cd(2)
    hNo.SetStats(0)
    hNo.SetTitle('number of reconstructed clusters')
    hNo.Draw('hist')
    hNo.Draw('text0 same')
    canv.cd(3)
    hEta.SetStats(0)
    hEta.SetTitle('#eta distribution')
    hEta.Draw('bar ')
    canv.cd(4)
    hPhi.SetStats(0)
    hPhi.SetTitle('#varphi distribution')
    hPhi.Draw('bar ')
    canv.cd()
    draw_text(["0.1#times0.4 window (eta#timesphi)",
               "9 GeV energy threshold"],[0.35,0.47,0.65,0.53], kBlue+3)

    if calo_init.output(ifile):
        canv.Print(calo_init.output(ifile)+"_nofit_stat.png")
    else:
        canv.Print("Reco_monitor.png")
        canv.Print("Reco_monitor.root")
