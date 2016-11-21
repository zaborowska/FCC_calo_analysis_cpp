import calo_init
## add arguments relevant only for that script
calo_init.add_defaults()
calo_init.parser.add_argument("--dEta", help="Size of the tower in eta", type = float, nargs=2)
calo_init.parser.add_argument("--maxEta", help="Maximum eta", type = float)
group = calo_init.parser.add_mutually_exclusive_group()
group.add_argument("--dPhi", help="Size of the tower in phi", type = float)
group.add_argument("--numPhi", help="Number of the towers in phi", type = int)
calo_init.parser.add_argument("inputSim", help="Additional input file name with the simulated events", type = str)
calo_init.parse_args()

from math import pi, floor
# set of default parameters
maxEta = 1.79
nPhi = 629 # artificially increase by 1 (odd number) - to make plots look OK
dEta = 0.01
nEta = int(2*maxEta/dEta + 1)
dPhi = 2*pi/nPhi
nameClusterCollection = "caloClusters"
nameParticlesCollection = "GenParticles"
filenamesSim, checkRegexInSimInput = calo_init.substitute(calo_init.args.inputSim)

# get parameters if passed from command line
if calo_init.args.maxEta:
    maxEta = calo_init.args.maxEta
if calo_init.args.numPhi:
    nPhi = calo_init.args.numPhi
    dPhi = 2*pi/nPhi
if calo_init.args.dPhi:
    dPhi = calo_init.args.dPhi
    nPhi = int(dPhi*2*pi)

from ROOT import gSystem
gSystem.Load("libCaloAnalysis")
from ROOT import CaloAnalysis_recoMonitor, TCanvas, TFile, gStyle, gPad, kGreen, kRed, kBlue, TColor, TF1
from draw_functions import *

# use this script for multiple files
# gStyle.SetPalette(56) # kInvertedDarkBodyRadiator
gStyle.SetPalette(73) # kCMYK
gStyle.SetOptFit(1)

for ifile, filename in enumerate(calo_init.filenamesIn):
    energy = calo_init.energy(ifile)
    filenameSim = filenamesSim[ifile] if checkRegexInSimInput else filenamesSim[0]
    print "Initial particle energy: " + str(energy) + "GeV"
    print "File with simulation results: " + filenameSim
    print "File with reconstruction results: " + filename
    analysis = CaloAnalysis_recoMonitor(nameClusterCollection,
                                        nameParticlesCollection,
                                        energy,
                                        maxEta, # max eta
                                        nEta, # number of bins in eta
                                        nPhi, # number of bins in phi
                                        dEta, # tower size in eta
                                        dPhi) # tower size in phi
    analysis.loop(filenameSim, filename,calo_init.verbose)
    # retrieve histograms to draw them
    histograms = analysis.histograms()
    hEn = histograms.hEn
    hEnFncPhi = histograms.hEnFncPhi
    hEta = histograms.hEta
    hPhi = histograms.hPhi
    hPhiFncPhi = histograms.hPhiFncPhi
    hEtaFncEta = histograms.hEtaFncEta
    hNo = histograms.hNo
    hNoFncPhi = histograms.hNoFncPhi
    hEnDiffMoreClu = histograms.hEnDiffMoreClu
    hEtaDiffMoreClu = histograms.hEtaDiffMoreClu
    hPhiDiffMoreClu = histograms.hPhiDiffMoreClu
    hRDiffMoreClu = histograms.hRDiffMoreClu
    h1dset1 = [hEn, hEta, hPhi, hNo]
    for h in h1dset1:
        h.SetMarkerColor(kBlue+1)
        h.SetFillColor(39)
        h.SetLineColor(39)
        h.SetMarkerSize(2.2)
    h1dset2 = [hEnDiffMoreClu, hEtaDiffMoreClu, hPhiDiffMoreClu, hRDiffMoreClu]
    for h in h1dset2:
        h.SetMarkerColor(39)
        h.SetMarkerStyle(21)
        h.SetLineColor(39)
        h.SetMarkerSize(1.5)
    h2d = [hEnFncPhi, hPhiFncPhi, hEtaFncEta, hNoFncPhi]
    hNo.GetXaxis().SetNdivisions(5)
    hEta.GetXaxis().SetRangeUser(-dEta,dEta)
    hEtaFncEta.GetXaxis().SetRangeUser(-0.1,0.1)
    hEtaFncEta.GetYaxis().SetRangeUser(-dEta,dEta)
    hPhi.GetXaxis().SetRangeUser(-2*dPhi,2*dPhi)
    hPhiFncPhi.GetYaxis().SetRangeUser(-2*dPhi,2*dPhi)

    # fit functions
    fitEnergy = TF1('fitEnergy','gaus',0.8*energy,1.2*energy)
    resultFitEn = hEn.Fit('fitEnergy','S')
    fitEnergy2 = TF1('fitEnergy2','gaus',resultFitEn.Get().Parameter(1)-2.*resultFitEn.Get().Parameter(2),
                     resultFitEn.Get().Parameter(1)+2.*resultFitEn.Get().Parameter(2))
    resultFitEn2 = hEn.Fit('fitEnergy2','SR')
    fitEta = TF1('fitEta','gaus',-10*dEta,10*dEta)
    resEta = hEta.Fit('fitEta','S')
    fitPhi = TF1('fitPhi','gaus',-10*dPhi,10*dPhi)
    resPhi = hPhi.Fit('fitPhi','S')


    canv = TCanvas('ECal_monitor_plots_e'+str(energy)+'GeV', 'ECal', 1600, 1400 )
    canv.Divide(4,3)
    canv.cd(1)
    hEn.Draw('bar')
    draw_text(["energy: "+str(round(resultFitEn2.Get().Parameter(1),1))+" GeV",
               "           "+str(round(resultFitEn2.Get().Parameter(1),1)/energy*100.)+" %",
               "resolution: "+str(round(resultFitEn2.Get().Parameter(2)/resultFitEn2.Get().Parameter(1)*100,1))+" %"],
              [0.6,0.2,0.9,0.4])
    canv.cd(2)
    draw_hist2d(hEnFncPhi)
    canv.cd(3)
    hNo.Draw('hist')
    hNo.Draw('text0 same')
    canv.cd(4)
    draw_hist2d(hNoFncPhi)
    canv.cd(5)
    hEnDiffMoreClu.Draw()
    canv.cd(6)
    hEtaDiffMoreClu.Draw('bar')
    canv.cd(7)
    hPhiDiffMoreClu.Draw()
    canv.cd(8)
    hRDiffMoreClu.Draw()
    canv.cd(9)
    hEta.Draw('bar')
    canv.cd(10)
    draw_hist2d(hEtaFncEta)
    canv.cd(11)
    hPhi.Draw('bar')
    canv.cd(12)
    draw_hist2d(hPhiFncPhi)

    if calo_init.output(ifile):
        canv.Print(calo_init.output(ifile)+".png")
        plots = TFile(calo_init.output(ifile)+".root","RECREATE")
    else:
        canv.Print("Reco_monitor.png")
        plots = TFile("Reco_monitor_plots.root","RECREATE")

    plots.cd()
    for hset in [h1dset1, h1dset2, h2d]:
        for h in hset:
            h.Write()
    plots.Close()
