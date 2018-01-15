from ROOT import TH1F, TCanvas, TLegend, TFile, gStyle, gPad
import itertools

#Options:
#Traces 1: traces from first 2 layers go in the front, 5 layers to the back
#Traces 2 (default): traces from first 3 layers go in the front, 5 layers to the back
#Impedance: 50 (default) or 33 Ohm
#Shields width: 2 (default) or 4 - how many times are shield larger than gap hs 

flagTraces = 'traces2'
flagImpedance = '50'
flagsShieldsWidth = 2

#Check the flags
if flagTraces!='traces1' and flagTraces!='traces2':
    print "WARNING: Trace option ", flagTraces, "not know, setting to default (traces2)"
    flagTraces = 'traces2'

if flagImpedance!='50' and flagImpedance!='33':
    print "WARNING: thickness hs for impedance ", flagImpedance, " Ohm not know, setting to default (50 Ohm)"
    flagImpedance = '50'

filename = "ecalBarrel_"+flagImpedance+"Ohm_"+flagTraces+"_"+str(flagsShieldsWidth)+"shieldWidth"
fIn = TFile("capacitances_"+filename+".root","r")

fSaveAll = TFile("elecNoise_"+filename+".root","RECREATE")
fSave = TFile("elecNoise_ecalBarrel.root","RECREATE")

nLayers = 8

SFatlas = 0.18
SFfcc = [0.12125, 0.14283, 0.16354, 0.17662, 0.18867, 0.19890, 0.20637, 0.20802]
noisePerCapacitance = 0.0397372130805
print "noise per capacitance in ATLAS ", noisePerCapacitance, " MeV/pF"
GeV = 1000.

gStyle.SetOptStat(0)

#TH1 with capacitances
hCapShield = []
hCapTrace = []
hCapDetector = []
hCapTotal = []
# electronic noise histograms
h_elecNoise_fcc = [] # default total noise shield + detector capacitance (without trace capacitance) -> to be used in FCCSW as noise estimation
h_elecNoise_all = [] # total noise shield + trace + detector capacitance
h_elecNoise_withoutTraceCap = [] # total noise without trace capacitance (as in ATLAS - trace cap. can be neglected): from shield + detector capacitance
h_elecNoise_shield = []
h_elecNoise_trace = []
h_elecNoise_detector = []

#Read graphs from files
for i in range (0, nLayers):  
    nameShield = "hCapacitance_shields"+str(i)
    hCapShield.append(fIn.Get(nameShield))
    nameTrace = "hCapacitance_traces"+str(i)
    hCapTrace.append(fIn.Get(nameTrace))
    nameDetector = "hCapacitance_detector"+str(i)
    hCapDetector.append(fIn.Get(nameDetector))

index = 0    
nbins = hCapShield[index].GetNbinsX()
etaMax = hCapShield[index].GetXaxis().GetBinUpEdge(nbins)
print "number of bins ", nbins, ", etaMax ", etaMax

maximumCap = 0.
maximumNoiseAll = 0.
maximumNoise = 0.

for i in range (0, nLayers):  
    #Prepare electronic noise histograms    
    h_elecNoise_fcc.append( TH1F() )
    h_elecNoise_fcc[i].SetLineWidth(3)
    h_elecNoise_fcc[i].SetLineColor(i+1)
    h_elecNoise_fcc[i].SetBins(nbins, 0., etaMax)
    h_elecNoise_fcc[i].SetTitle("Default electronic noise - shield + detector capacitance; |#eta|; Electronic noise [GeV]")
    h_elecNoise_fcc[i].SetName("h_elecNoise_fcc_"+str(i+1))

    h_elecNoise_all.append( TH1F() )
    h_elecNoise_all[i].SetLineWidth(3)
    h_elecNoise_all[i].SetLineColor(i+1)
    h_elecNoise_all[i].SetBins(nbins, 0., etaMax)
    h_elecNoise_all[i].SetTitle("Total electronic noise - shield + trace + detector capacitance; |#eta|; Electronic noise [GeV]")
    h_elecNoise_all[i].SetName("h_elecNoise_all_"+str(i+1))
    
    h_elecNoise_withoutTraceCap.append( TH1F() )
    h_elecNoise_withoutTraceCap[i].SetLineWidth(3)
    h_elecNoise_withoutTraceCap[i].SetLineColor(i+1)
    h_elecNoise_withoutTraceCap[i].SetBins(nbins, 0., etaMax)
    h_elecNoise_withoutTraceCap[i].SetTitle("Electronic noise without trace capacitance; |#eta|; Electronic noise [GeV]")
    h_elecNoise_withoutTraceCap[i].SetName("h_elecNoise_withoutTraceCap_"+str(i+1))

    h_elecNoise_shield.append( TH1F() )
    h_elecNoise_shield[i].SetLineWidth(3)
    h_elecNoise_shield[i].SetLineColor(i+1)
    h_elecNoise_shield[i].SetBins(nbins, 0., etaMax)
    h_elecNoise_shield[i].SetTitle("Electronic noise - shields; |#eta|; Electronic noise [GeV]")
    h_elecNoise_shield[i].SetName("h_elecNoise_shield_"+str(i+1))

    h_elecNoise_trace.append( TH1F() )
    h_elecNoise_trace[i].SetLineWidth(3)
    h_elecNoise_trace[i].SetLineColor(i+1)
    h_elecNoise_trace[i].SetBins(nbins, 0., etaMax)
    h_elecNoise_trace[i].SetTitle("Electronic noise -traces; |#eta|; Electronic noise [GeV]")
    h_elecNoise_trace[i].SetName("h_elecNoise_trace_"+str(i+1))

    h_elecNoise_detector.append( TH1F() )
    h_elecNoise_detector[i].SetLineWidth(3)
    h_elecNoise_detector[i].SetLineColor(i+1)
    h_elecNoise_detector[i].SetBins(nbins, 0., etaMax)
    h_elecNoise_detector[i].SetTitle("Electronic noise - detector; |#eta|; Electronic noise [GeV]")
    h_elecNoise_detector[i].SetName("h_elecNoise_detector_"+str(i+1))

    #Total capacitance plot (shield + trace + detector)
    hCapTotal.append( TH1F() )
    hCapTotal[i].SetBins(nbins, 0, etaMax)
    hCapTotal[i].SetLineColor(i+1)
    hCapTotal[i].SetLineWidth(3)
    hCapTotal[i].SetTitle("Total capacitance; |#eta|; Capacitance [pF]")
    hCapTotal[i].SetName("hCapacitance"+str(i))

    #Correct for different SF in ATLAS and FCC
    noiseConversionFactor = noisePerCapacitance * SFatlas / SFfcc[i]
    for ibin in range(0, nbins+1):
        capShield = hCapShield[i].GetBinContent(ibin)
        capTrace = hCapTrace[i].GetBinContent(ibin)
        capDetector = hCapDetector[i].GetBinContent(ibin)
        #total capacitance
        hCapTotal[i].SetBinContent( ibin, capShield + capTrace + capDetector )
        #noise
        noiseAll = noiseConversionFactor * ( capShield + capTrace + capDetector ) / GeV
        noiseWithoutTrace = noiseConversionFactor * ( capShield + capDetector ) / GeV
        noiseShield = noiseConversionFactor * capShield / GeV
        noiseTrace = noiseConversionFactor * capTrace / GeV
        noiseDetector = noiseConversionFactor * capDetector / GeV
        #find maximum for drawing of histograms
        if noiseAll>maximumNoiseAll:
            maximumNoiseAll = noiseAll
        if noiseWithoutTrace>maximumNoise:
            maximumNoise = noiseWithoutTrace
        if (capShield + capTrace + capDetector)>maximumCap:
            maximumCap = (capShield + capTrace + capDetector)
            
        #fill histogram
        #default: without traces
        h_elecNoise_fcc[i].SetBinContent(ibin, noiseWithoutTrace)
        h_elecNoise_all[i].SetBinContent(ibin, noiseAll)
        h_elecNoise_withoutTraceCap[i].SetBinContent(ibin, noiseWithoutTrace)
        h_elecNoise_shield[i].SetBinContent(ibin, noiseShield)
        h_elecNoise_trace[i].SetBinContent(ibin, noiseTrace)
        h_elecNoise_detector[i].SetBinContent(ibin, noiseDetector)
        if ibin==1:
            print "layer %d" %i, "eta==0: capacitance %.0f pF," %( capShield + capTrace + capDetector ), "total elec. noise %.4f GeV" %noiseAll, "elec. noise without trace cap. %.4f GeV" %noiseWithoutTrace

#print maximumCap, maximumNoiseAll, maximumNoise

cCapacitance = TCanvas("cCapacitance","Capacitance per cell",800,600)
cCapacitance.cd()
legend = TLegend(0.1,0.5,0.3,0.9)
legend.SetHeader("Longitudinal layers")
for i, h in enumerate(hCapTotal):
    h.SetMinimum(0)
    h.SetMaximum(maximumCap)
    h.GetYaxis().SetTitleOffset(1.4)
    if i == 0:
        h.Draw("")
    else:
        h.Draw("same")
    legend.AddEntry(h,str(i)+" layer","l")


# Prepare "nice" plots & save all capacitances + noise
fSaveAll.cd()

for h in hCapTotal:
    h.SetMinimum(0.)
    h.SetMaximum(maximumCap*1.2)
    h.GetYaxis().SetTitleOffset(1.4)
    h.Write()

legend.Draw()
cCapacitance.Update()
cCapacitance.Write()

for h in h_elecNoise_all:
    h.SetMinimum(0.)
    h.SetMaximum(maximumNoiseAll*1.2)
    h.GetYaxis().SetTitleOffset(1.4)
    h.Write()

for h in itertools.chain(h_elecNoise_fcc, h_elecNoise_withoutTraceCap, h_elecNoise_shield, h_elecNoise_trace, h_elecNoise_detector):
    h.SetMinimum(0.)
    h.SetMaximum(maximumNoise*1.2)
    h.GetYaxis().SetTitleOffset(1.4)
    h.Write()

cNoise = TCanvas("cNoise","Electronic noise per cell",800,600)
cNoise.cd()
for i, h in enumerate(h_elecNoise_fcc):
    if i == 0:
        h.Draw("")
    else:
        h.Draw("same")

legend.Draw()
cNoise.Update()
cNoise.Write()

cNoiseWithoutTrace = TCanvas("cNoiseWithoutTrace","Electronic noise without trace cap. per cell",800,600)
cNoiseWithoutTrace.cd()
for i, h in enumerate(h_elecNoise_withoutTraceCap):
    if i == 0:
        h.Draw("")
    else:
        h.Draw("same")

legend.Draw()
cNoiseWithoutTrace.Update()
cNoiseWithoutTrace.Write()

legendP = TLegend(0.1,0.6,0.43,0.9)
legendP.SetHeader("Capacitance")

cCapParts = TCanvas("cCapParts","",1200,1000)
cCapParts.Divide(3,3)    
for i in range (0, nLayers):
    cCapParts.cd(i+1)
    if i<2:
        hCapTotal[i].SetMaximum(400)
    hCapTotal[i].SetTitle("Layer "+str(i))
    hCapTotal[i].GetXaxis().SetTitleSize(0.045)
    hCapTotal[i].GetYaxis().SetTitleSize(0.045)
    hCapTotal[i].GetYaxis().SetTitleOffset(1.15)
    hCapTotal[i].SetLineColor(1)
    hCapShield[i].SetLineColor(2)
    hCapTrace[i].SetLineColor(3)
    hCapDetector[i].SetLineColor(4)
    hCapTotal[i].Draw()
    hCapShield[i].Draw("same")
    hCapTrace[i].Draw("same")
    hCapDetector[i].Draw("same")
    gPad.Update()
    if i==0:
        legendP.AddEntry(hCapTotal[i],"total cap.","l")
        legendP.AddEntry(hCapShield[i],"shield cap.","l")
        legendP.AddEntry(hCapTrace[i],"trace cap.","l")
        legendP.AddEntry(hCapDetector[i],"detector cap.","l")
    legendP.Draw()

cCapParts.Write()

#Save final noise plot (to be used in FCCSW)
fSave.cd()
for h in h_elecNoise_fcc:
    h.Write()

closeInput = raw_input("Press ENTER to exit")
