import argparse
parser = argparse.ArgumentParser()
## add arguments relevant only for that script
parser.add_argument("-i","--inputFiles", help="Input file name for analysis", type = str, nargs="+")
parser.add_argument("--histoname", help="Name of the pile-up histograms", type = str, default = "ecalBarrelEnergyVsAbsEta")
parser.add_argument("--etaSize", help="Number of |eta| bins in a cluster", type = int, default=[7], nargs="+")
parser.add_argument("--phiSize", help="Number of phi bins in a cluster", type = int, default=[17], nargs="+")
parser.add_argument("--nBins", help="Number of |eta|-bins", type = int, default = 20)
parser.add_argument("--mu", help="Number of pileup collisions to scale data to", type = int, default = 1000)
parser.add_argument("--nLayers", help="Number of layers", type = int, default = 8 )
parser.add_argument("--file", help="Number of input files", type = int, default = 0)
parser.add_argument("--addElecNoise", help="Add electronics noise", action = 'store_true')
parser.add_argument("--elecNoiseFile", help="Path to the electronics noise", type = str, default = "/afs/cern.ch/user/a/azaborow/public/FCCSW/elecNoise_ecalBarrel_50Ohm_traces2_2shieldWidth_noise.root")
parser.add_argument("--elecNoiseHist", help="Name of the histogram with electronics noise", type = str, default = "h_elecNoise_fcc_")
parser.add_argument("-o","--output", help="Output file name", type = str)
args = parser.parse_args()

filenamesIn =args.inputFiles
outname = args.output

from math import sqrt
from ROOT import gSystem
# gSystem.Load("libCaloAnalysis")
from ROOT import TCanvas, TFile, gStyle, gPad, kGreen, kRed, kBlue, TColor, TPad, TH1, TH2
from draw_functions import *

# use this script for multiple files
# gStyle.SetPalette(56) # kInvertedDarkBodyRadiator
gStyle.SetPalette(73) # kCMYK
gStyle.SetOptFit(1)
gStyle.SetOptStat(0)
layerColours = [1, ROOT.kAzure-3, ROOT.kRed, ROOT.kGreen+2, ROOT.kOrange+4, ROOT.kGray+2, ROOT.kViolet-5, ROOT.kYellow-3, ROOT.kMagenta+2, ROOT.kGreen-3, ROOT.kBlue-2, 46, 44, 40,41,42,43,45,47,48,49,50]

histoname = args.histoname
nbins = args.nBins
mu = args.mu
nlayers = args.nLayers
etaSize = args.etaSize
phiSize = args.phiSize
dEta = 1 #0.01
from math import pi
dPhi = 1 #2.*pi/704.

if len(etaSize) != len(phiSize):
    print "ERROR, length of eta size and phi must agree"
    exit()
nClusters = len(etaSize)

# Scaling factor to a different number of average pile-up events per bunch crossing
# 1.6: correction for out-of-time pile-up
#scale = 1.6*sqrt(mu)
# only in-time pile-up
scale = sqrt(mu)
print "Scaling factor: ", scale

# Final pileup plot per layer
hPileup = []
hPileupCell = []
files = []
num_good_files = 0

# Read file with electronics noise constants
if args.addElecNoise:
    hElecNoiseInLayer = []
    fElecNoise = ROOT.TFile(args.elecNoiseFile,"r")
    if fElecNoise.IsZombie():
        print "Cannot add electronics noise, file is Zombie"
        exit()
    for iLayer in xrange(nlayers):
        histoname_i = args.elecNoiseHist+str(iLayer+1)
        hLayer = fElecNoise.Get(histoname_i)
        if (hLayer.GetNbinsX()<nbins):
            nbins = hLayer.GetNbinsX()
            print "More bins than available required!!! Setting nbins to ", nbins
        etaMax = hLayer.GetXaxis().GetBinUpEdge(nbins)
        hElecNoiseInLayer.append(hLayer)

for ifile, filename in enumerate(filenamesIn):
    f = ROOT.TFile(filename,"r")
    if not f.IsZombie() and (ifile < args.file or args.file == 0):
        files.append(f) # makes root not close files
        num_good_files += 1
    # get the TH2 histograms and make a projection to extract the RMS of energy distribution per eta bin
        for index in xrange(nClusters):
            histoname_i = histoname + "_clusterEta" + str(etaSize[index]) + "Phi" + str(phiSize[index])
            hEnVsEtaAbs = f.Get(histoname_i)
            if (hEnVsEtaAbs.GetNbinsX()<nbins):
                nbins = hEnVsEtaAbs.GetNbinsX()
                print "More bins than available required!!! Setting nbins to ", nbins
            if ifile == 0:
                etaMax = hEnVsEtaAbs.GetXaxis().GetBinUpEdge(nbins)
                hPile = TH1F("h_pileup_cluster" + "Eta" + str(etaSize[index]) + "Phi" + str(phiSize[index]),"Pileup in cluster of size #Delta#eta#times#Delta#varphi = " + str(etaSize[index]) + "#times" + str(phiSize[index]), nbins, 0, etaMax)
                hPile.GetXaxis().SetTitle("|#eta| of cluster's centre")
                hPile.GetYaxis().SetTitle("Pileup [GeV]")
                hPile.Sumw2()
                for i in range(1, nbins):
                    hProj = hEnVsEtaAbs.ProjectionY( "hprojection_"+str(index), i, i,"e")
                    hPile.Fill(hPile.GetBinCenter(i),hProj.GetRMS()*scale)
                hPileup.append(hPile)
            else:
                for i in range(1, nbins):
                    hProj = hEnVsEtaAbs.ProjectionY( "hprojection_"+str(index), i, i,"e")
                    hPileup[index].Fill(hPileup[index].GetBinCenter(i),hProj.GetRMS()*scale)
        for index in xrange(nlayers):
            histoname_i = histoname+str(index)
            hLayer = f.Get(histoname_i)
            if (hLayer.GetNbinsX()<nbins):
                nbins = hLayer.GetNbinsX()
                print "More bins than available required!!! Setting nbins to ", nbins
            etaMax = hLayer.GetXaxis().GetBinUpEdge(nbins)
            if ifile == 0:
                hPileC = TH1F("h_pileup_layer"+str(index+1),"Pileup in "+str(index+1)+". layer", nbins, 0, etaMax)
                hPileC.GetXaxis().SetTitle("|#eta|")
                hPileC.GetYaxis().SetTitle("Pileup [GeV]")
                hPileC.Sumw2()
                for i in range(1, nbins):
                    hProj = hLayer.ProjectionY( "hprojection_"+str(index), i, i,"e")
                    hPileC.Fill(hPile.GetBinCenter(i),hProj.GetRMS()*scale)
                hPileupCell.append(hPileC)
            else:
                for i in range(1, nbins):
                    hProj = hLayer.ProjectionY( "hprojection_"+str(index), i, i,"e")
                    hPileupCell[index].Fill(hPileupCell[index].GetBinCenter(i),hProj.GetRMS()*scale)

print ("==== READ PROPERLY ", num_good_files, " files")
for index in xrange(nClusters):
    hPileup[index].Scale(1./num_good_files)
for iLayer in xrange(nlayers):
    hPileupCell[iLayer].Scale(1./num_good_files)
all_graphs = []
all_graphs_lin = []
all_graphs_sqrt = []
fits = []
fitNamesParTypes = ["Lin", "Sqrt", "Measured"]
fitNamesExplanation = ["#sum#sigma_{cell}", "#sqrt{#sum#sigma_{cell}^{2}}", "#sigma_{cluster}"]
fitColour = [ROOT.kRed+2, ROOT.kGreen+2, 9]
fitStyle = [20,21,25]
for i in range (0,3):
    for j in range (0,2):
        fits.append(ROOT.TGraphErrors())
        fits[2*i+j].SetName("fitParam"+fitNamesParTypes[i]+"_p"+str(j))
from math import sqrt, pow
for i in range(1, nbins):
    grMeasured = ROOT.TGraphErrors()
    grLinear = ROOT.TGraphErrors()
    grSqrt = ROOT.TGraphErrors()
    sumNoiseTower_lin= 0
    errNoiseTower_lin = 0
    sumNoiseTower_sqrt = 0
    errNoiseTower_sqrt = 0
    for iLayer in xrange(nlayers):
        if args.addElecNoise:
            sumNoiseTower_lin += sqrt(hPileupCell[iLayer].GetBinContent(i)**2 + hElecNoiseInLayer[iLayer].GetBinContent(hElecNoiseInLayer[iLayer].FindBin(hPileupCell[iLayer].GetBinCenter(i)))**2)
            sumNoiseTower_sqrt += (hPileupCell[iLayer].GetBinContent(i)**2 + hElecNoiseInLayer[iLayer].GetBinContent(hElecNoiseInLayer[iLayer].FindBin(hPileupCell[iLayer].GetBinCenter(i)))**2)
            print hElecNoiseInLayer[iLayer].GetBinCenter(hElecNoiseInLayer[iLayer].FindBin(hPileupCell[iLayer].GetBinCenter(i))), hPileupCell[iLayer].GetBinCenter(i)
            errNoiseTower_lin += hPileupCell[iLayer].GetBinError(i)
            errNoiseTower_sqrt += (hPileupCell[iLayer].GetBinError(i)**2)
        else:
            sumNoiseTower_lin += hPileupCell[iLayer].GetBinContent(i)
            sumNoiseTower_sqrt += (hPileupCell[iLayer].GetBinContent(i)**2)
            errNoiseTower_lin += hPileupCell[iLayer].GetBinError(i)
            errNoiseTower_sqrt += (hPileupCell[iLayer].GetBinError(i)**2)
    sumNoiseTower_sqrt = sqrt(sumNoiseTower_sqrt)
    errNoiseTower_sqrt = sqrt(errNoiseTower_sqrt)
    if i==1:
        print (sumNoiseTower_lin, sumNoiseTower_sqrt)
    canvBins = prepare_divided_canvas('Minimum bias event in ECal - noise per cluster', 'per_cluster', nClusters )
    for index in xrange(nClusters):
        grMeasured.SetPoint(index, etaSize[index] * phiSize[index] * nlayers * dEta * dPhi, hPileup[index].GetBinContent(i))
        grMeasured.SetPointError(index, 0, hPileup[index].GetBinError(i))
        grLinear.SetPoint(index, etaSize[index] * phiSize[index] * nlayers * dEta * dPhi , etaSize[index] * phiSize[index] * sumNoiseTower_lin)
        grLinear.SetPointError(index, 0,  etaSize[index] * phiSize[index] * errNoiseTower_lin)
        grSqrt.SetPoint(index, etaSize[index] * phiSize[index]  * nlayers * dEta * dPhi, sqrt(etaSize[index] * phiSize[index]) * sumNoiseTower_sqrt)
        grSqrt.SetPointError(index, 0, sqrt(etaSize[index] * phiSize[index]) * errNoiseTower_sqrt )
        if i==1:
            print (sqrt(etaSize[index] * phiSize[index]) * sumNoiseTower_sqrt, hPileup[index].GetBinContent(i),  etaSize[index] * phiSize[index] * sumNoiseTower_lin)
        grCoeff = ROOT.TGraphErrors()
        grCoeff.SetPoint(0,0.5,sqrt(etaSize[index] * phiSize[index]) * sumNoiseTower_sqrt)
        grCoeff.SetPoint(0,1,etaSize[index] * phiSize[index] * sumNoiseTower_lin)
        grCoeff.SetPoint(0,0.7,etaSize[index] * phiSize[index] * sumNoiseTower_lin)
    colour = 46 if i==10 else (ROOT.kOrange if i==5 else i)
    prepare_graph(grMeasured, "clusterDependence_eta%gto%g" % (hPileup[index].GetBinLowEdge(i), hPileup[index].GetBinLowEdge(i+1)), "Pileup noise per cluster for #eta %g- %g; cluster size (#Delta#eta#times#Delta#varphi); Pileup noise [GeV] " % (hPileup[index].GetBinLowEdge(i), hPileup[index].GetBinLowEdge(i+1)), colour)
    prepare_graph(grLinear, "linearSumNoisePerCell_clusterDependence_eta%gto%g" % (hPileup[index].GetBinLowEdge(i), hPileup[index].GetBinLowEdge(i+1)), "Pileup noise per cluster for #eta %g- %g; cluster size (#Delta#eta#times#Delta#varphi) calculated from linear sum of cell noise; Pileup noise [GeV] " % (hPileup[index].GetBinLowEdge(i), hPileup[index].GetBinLowEdge(i+1)), colour)
    prepare_graph(grSqrt, "sqrtSumNoisePerCell_clusterDependence_eta%gto%g" % (hPileup[index].GetBinLowEdge(i), hPileup[index].GetBinLowEdge(i+1)), "Pileup noise per cluster for #eta %g- %g; cluster size (#Delta#eta#times#Delta#varphi) calculated from quadratic sum of cell noise; Pileup noise [GeV] " % (hPileup[index].GetBinLowEdge(i), hPileup[index].GetBinLowEdge(i+1)), colour)
    grLinear.SetMarkerStyle(20)
    grSqrt.SetMarkerStyle(24)
    grLinear.SetMarkerSize(0.7)
    grSqrt.SetMarkerSize(0.7)
    grMeasured.SetMarkerSize(1.2)
    if i<17:
        fitLin = ROOT.TF1("fitLin"+str(i),"[0]*pow(x,[1])",1,500)
        fitSq = ROOT.TF1("fitSq"+str(i),"[0]*pow(x,[1])",1,500)
        fitMeas = ROOT.TF1("fitMeas"+str(i),"[0]*pow(x,[1])",1,500)
        results = []
        results.append(grLinear.Fit(fitLin,"S"))
        results.append(grSqrt.Fit(fitSq,"S"))
        results.append(grMeasured.Fit(fitMeas,"S"))
        for iFit in range(0,3):
            for jFit in range(0,2):
                fits[2*iFit+jFit].SetPoint(i-1, 0.5*(hPileup[index].GetBinLowEdge(i) + hPileup[index].GetBinLowEdge(i+1)), results[iFit].Get().Parameter(jFit))
                fits[2*iFit+jFit].SetPointError(i-1, hPileup[index].GetBinWidth(i)/2., results[iFit].Get().Error(jFit))
        all_graphs.append(grMeasured)
        all_graphs_lin.append(grLinear)
        all_graphs_sqrt.append(grSqrt)


#Draw pileup per layer
canv = prepare_divided_canvas('Minimum bias event in ECal - noise per cluster', 'per_cluster', nClusters )
for i in xrange(nClusters):
    canv.cd(i+1)
    draw_1histogram(hPileup[i], hPileup[i].GetXaxis().GetTitle(), hPileup[i].GetYaxis().GetTitle())
    hPileup[i].Draw("ep")
canv1 = prepare_divided_canvas('Minimum bias event in ECal - noise per layer', 'per_cell', nlayers )
for i in xrange(nlayers):
    canv1.cd(i+1)
    draw_1histogram(hPileupCell[i], hPileupCell[i].GetXaxis().GetTitle(), hPileupCell[i].GetYaxis().GetTitle())
    hPileupCell[i].Draw("ep")
canv2 = prepare_divided_canvas("Pileup noise per cluster", "all_clusters", 16)
canv4 = prepare_divided_canvas("Coefficients", "all_coefficients", 16)
cluster_sizes = []
for  index in xrange(nClusters):
    cluster_sizes.append(etaSize[index] * phiSize[index] * nlayers)
all_graphs_coeff_fit = []
all_graphs_coeff = []
for i in range(1, 17):
    canv2.cd(i)
    all_graphs[i-1].Draw("aep")
    all_graphs[i-1].GetYaxis().SetRangeUser(0,10)
    all_graphs_lin[i-1].Draw("samepc")
    all_graphs_sqrt[i-1].Draw("samepc")
    # fitLin =  all_graphs_lin[i-1].Fit("pol1","S")
    # fitSqrt =  all_graphs_sqrt[i-1].Fit("pol1","S")
    # fitClu =  all_graphs[i-1].Fit("pol1","S")
    # alin = fitLin.Get().Parameter(0)
    # blin = fitLin.Get().Parameter(1)
    # asq = fitSqrt.Get().Parameter(0)
    # bsq = fitSqrt.Get().Parameter(1)
    # acl = fitClu.Get().Parameter(0)
    # bcl = fitClu.Get().Parameter(1)
    # coefficient = 0
    # grCoefficientFit = ROOT.TGraphErrors()
    # for index, clu_size in enumerate(cluster_sizes):
    #     B = 2.*(alin - asq) + 2. * clu_size * (blin - bsq)
    #     A = - alin + 2. * asq + clu_size * (- blin + 2. * bsq)
    #     coefficient = (acl + bcl * clu_size - A) / B
    #     print ("eta = ", i*0.1, "  cluster size ", clu_size, "  coefficient ", coefficient, "  offset ", (acl - A) / B, "  slope ", bcl / B )
    #     grCoefficientFit.SetPoint(index, clu_size, coefficient)
    # colour = 46 if i==10 else (ROOT.kOrange if i==5 else i)
    # prepare_graph(grCoefficientFit, "coefficient_clusterDependence_eta%gto%g" % (hPileup[index].GetBinLowEdge(i), hPileup[index].GetBinLowEdge(i+1)), "Coefficient for #eta %g- %g; cluster size (#cells); Coefficient" % (hPileup[index].GetBinLowEdge(i), hPileup[index].GetBinLowEdge(i+1)), colour)
    # grCoefficientFit.SetMarkerStyle(22)
    # grCoefficientFit.SetMarkerSize(1.5)
    # all_graphs_coeff_fit.append(grCoefficientFit)
    # canv4.cd(i)
    # grCoefficientFit.Draw("aep")
    # fitfnc = ROOT.TF1("fitfnc"+str(i),"[0]+[1]/x", 0, 2000)
    # fitCoeff = all_graphs_coeff_fit[i-1].Fit("fitfnc"+str(i),"S")
    # acoeff = fitCoeff.Get().Parameter(0)
    # bcoeff = fitCoeff.Get().Parameter(1)
    # grCoefficient = ROOT.TGraphErrors()
    # for index, clu_size in enumerate(cluster_sizes):
    #     sumNoiseTower_coeff = 0
    #     errNoiseTower_coeff = 0
    #     coefficient = 0.7 #acoeff + bcoeff / clu_size
    #     for iLayer in xrange(nlayers):
    #         sumNoiseTower_coeff += (hPileupCell[iLayer].GetBinContent(i)**(1./coefficient))
    #         errNoiseTower_coeff += (hPileupCell[iLayer].GetBinError(i)**(1./coefficient))
    #     print("++++",sumNoiseTower_coeff, errNoiseTower_coeff)
    #     sumNoiseTower_coeff = sumNoiseTower_coeff**coefficient
    #     errNoiseTower_coeff = errNoiseTower_coeff**coefficient
    #     print("++++",sumNoiseTower_coeff, errNoiseTower_coeff)
    #     grCoefficient.SetPoint(index, clu_size, pow(clu_size * 1./nlayers, coefficient) * sumNoiseTower_coeff)
    #     grCoefficient.SetPointError(index, 0, pow(clu_size * 1./nlayers, coefficient) * errNoiseTower_coeff )
    # colour = 46 if i==10 else (ROOT.kOrange if i==5 else i)
    # prepare_graph(grCoefficient, "coeffSumNoisePerCell_clusterDependence_eta%gto%g" % (hPileup[index].GetBinLowEdge(i), hPileup[index].GetBinLowEdge(i+1)), "Pileup noise per cluster for #eta %g- %g; cluster size (#cells) calculated from sum of cell noise with coefficient; Pileup noise [GeV] " % (hPileup[index].GetBinLowEdge(i), hPileup[index].GetBinLowEdge(i+1)), colour)
    # grCoefficient.SetMarkerStyle(22)
    # grCoefficient.SetMarkerSize(1.5)
    # all_graphs_coeff.append(grCoefficient)
    # canv2.cd(i)
    # grCoefficient.Draw("sameep")

if outname:
    canv.Print(outname+".gif")
    canv2.Print(outname+"_summary.gif")
    canv4.Print(outname+"_coefficients.gif")
    canv1.Print(outname+"_percell.gif")
    plots = TFile(outname+".root","RECREATE")
else:
    if args.addElecNoise:
        suffix = "_inclElecNoise"
    else:
        suffix=""
    canv.Print("EstimatePileup_Cluster_mu"+str(mu)+"_"+str(num_good_files)+"files"+suffix+".gif")
    canv2.Print("EstimatePileup_ClusterSummary_mu"+str(mu)+"_"+str(num_good_files)+"files"+suffix+".gif")
    canv4.Print("EstimatePileup_Coefficients_mu"+str(mu)+"_"+str(num_good_files)+"files"+suffix+".gif")
    canv1.Print("EstimatePileup_Cell_mu"+str(mu)+"_"+str(num_good_files)+"files"+suffix+".gif")
    plots = TFile("EstimatePileup_Cluster_mu"+str(mu)+"_"+str(num_good_files)+"files"+suffix+".root","RECREATE")

canv3 = prepare_divided_canvas("Pileup noise per cluster LOG scale", "all_clusters_log", 16)
for i in range(1, 17):
    pad = canv3.cd(i)
    pad.SetLogy()
    all_graphs[i-1].Draw("aep")
    # all_graphs[i-1].GetYaxis().SetRangeUser(0.01,50)
    all_graphs[i-1].GetYaxis().SetRangeUser(0.1,100)
    all_graphs_lin[i-1].Draw("samepc")
    all_graphs_sqrt[i-1].Draw("samepc")
if outname:
    canv3.Print(outname+"_summaryLOG.gif")
else:
    canv3.Print("EstimatePileup_ClusterSummaryLOG_mu"+str(mu)+"_"+str(num_good_files)+"files"+suffix+".gif")

canv4 = prepare_divided_canvas("Pileup noise per cluster ALL etas LOG scale", "all_clusters_all_etas_log", 1)
pad = canv4.cd()
pad.SetLogy()
for i in range(1, 16):
    if i==1:
         all_graphs[i-1].Draw("aep")
         all_graphs[i-1].SetTitle("Pileup noise per cluster")
    else:
         all_graphs[i-1].Draw("sameep")
    # all_graphs[i-1].GetYaxis().SetRangeUser(0.01,50)
    all_graphs[i-1].GetYaxis().SetRangeUser(0.1,100)
    all_graphs_lin[i-1].Draw("samepc")
    all_graphs_sqrt[i-1].Draw("samepc")
if outname:
    canv4.Print(outname+"_summaryALLLOG.gif")
else:
    canv4.Print("EstimatePileup_ClusterSummaryAllLOG_mu"+str(mu)+"_"+str(num_good_files)+"files"+suffix+".gif")

canv5 = prepare_long_canvas("Pileup noise per cluster fit params", "fit_params", 2)
for j in range(0,2):
    canv5.cd(j+1)
    min = 0
    max = 0
    for i in range(0,3):
        prepare_graph(fits[2*i+j], "FitToClusterDependence_"+fitNamesParTypes[i]+"_p"+str(j), "Fit to pileup noise per cluster ("+fitNamesExplanation[i]+") + p_{"+str(j)+"};|#eta| ; p_{"+str(j)+"}", fitColour[i])
        fits[2*i+j].SetMarkerStyle(fitStyle[j])
        fits[2*i+j].SetMarkerSize(1.)
        if i==0:
            fits[2*i+j].Draw("aep")
            fits[2*i+j].SetTitle("Fit to pileup noise per cluster")
        else:
            fits[2*i+j].Draw("sameep")
        min = fits[2*i+j].GetYaxis().GetXmin() if min > fits[2*i+j].GetYaxis().GetXmin() else min
        max = fits[2*i+j].GetYaxis().GetXmax() if max < fits[2*i+j].GetYaxis().GetXmax() else max
    fits[j].GetYaxis().SetRangeUser(min, max)
if outname:
    canv5.Print(outname+"_fitParams.gif")
else:
    canv5.Print("EstimatePileup_fitParams_mu"+str(mu)+"_"+str(num_good_files)+"files"+suffix+".gif")

# translate fits to TH1F
fits_hist = []
for fit in fits:
    hist = TH1F("hist"+fit.GetName(), fit.GetTitle(), nbins, 0, etaMax)
    yVal = fit.GetY()
    for i in range(0,fit.GetN()):
        hist.SetBinContent(i+1, yVal[i])
    fits_hist.append(hist)
fits_divided = []
for j in range (0,2):
    fits_divided.append(ROOT.TGraphErrors())
    fits_divided[j].SetName("correctionFactorFromSqrtToMeasured_p"+str(j))
    fits_divided[j].SetTitle("correctionFactorFromSqrtToMeasured_p"+str(j))
    print  fits[2*1+j].GetName(), "=? SQRT p",j
    print  fits[2*2+j].GetName(), "=? meas p",j
    xValues_sqrt = fits[2*1+j].GetX()
    xValues_meas = fits[2*2+j].GetX()
    yValues_sqrt = fits[2*1+j].GetY()
    yValues_meas = fits[2*2+j].GetY()
    yErrors_sqrt = fits[2*1+j].GetEY()
    yErrors_meas = fits[2*2+j].GetEY()
    for i in range(0,fits[2*1+j].GetN()):
        if  yValues_sqrt[i] > 0:
            if j==0:
                # multiplicative factors
                fits_divided[j].SetPoint(i,  xValues_meas[i], yValues_meas[i] / yValues_sqrt[i])
                print  xValues_meas[i], "  :  ", yValues_meas[i],"/", yValues_sqrt[i], "=", yValues_meas[i] / yValues_sqrt[i]
            if j==1:
                # power, need to subtract
                fits_divided[j].SetPoint(i,  xValues_meas[i], yValues_meas[i] - yValues_sqrt[i])
                print  xValues_meas[i], "  :  ", yValues_meas[i],"/", yValues_sqrt[i], "=", yValues_meas[i] - yValues_sqrt[i]
# fits_divided[j].SetPointError(i+1,  xValues_meas[i],  yErrors_meas[i] / yErrors_sqrt[i])
fits_hist_divided = []
for fit in fits_divided:
    print "saving HIST++++++++++", fit.GetName()
    hist = TH1F("hist"+fit.GetName(), fit.GetTitle(), nbins, 0, etaMax)
    yVal = fit.GetY()
    for i in range(0,fit.GetN()):
        hist.SetBinContent(i+1, yVal[i])
    fits_hist_divided.append(hist)

plots.cd()
for hset in [hPileup, hPileupCell, all_graphs, all_graphs_lin, all_graphs_sqrt, fits, fits_hist, fits_divided, fits_hist_divided
             ]:
    for h in hset:
        h.Write()
if args.addElecNoise:
    for h in hElecNoiseInLayer:
        h.Write()
plots.Close()

raw_input("Press ENTER to exit")
