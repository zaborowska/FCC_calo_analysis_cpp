from ROOT import TH1F, TCanvas, TLegend, TFile, gStyle, gPad
import itertools

#Options:
#Impedance: 50 (default) or 33 Ohm
#Shields width: 2 (default) or 4 - how many times are shield larger than gap hs 

flagImpedance = '33'
flagsShieldsWidth = 2

#Check the flags
if flagImpedance!='50' and flagImpedance!='33':
    print "WARNING: thickness hs for impedance ", flagImpedance, " Ohm not know, setting to default (50 Ohm)"
    flagImpedance = '50'

filename = "emec_"+flagImpedance+"Ohm_"+str(flagsShieldsWidth)+"shieldWidth"
fIn = TFile("capacitances_"+filename+".root","r")

Nplanes = 156

emecLayersToMerge = [2] + [2] + [4]*38
#emecLayersToMerge = [26]*6
Nlayers = len(emecLayersToMerge)
print "Number of layers in EMEC: ", Nlayers
indexLayersToMerge = []
sumOfLayers = 0
for i in range(0, Nlayers):
    indexLayersToMerge.append(sumOfLayers + emecLayersToMerge[i])
    sumOfLayers += emecLayersToMerge[i]
print emecLayersToMerge, indexLayersToMerge
    
fSaveAll = TFile("elecNoise_"+filename+"_"+str(Nlayers)+"layers.root","RECREATE")
fSave = TFile("elecNoise_emec_"+str(Nlayers)+"layers.root","RECREATE")

    
SFatlas = 0.18
SFfcc = 0.072
noisePerCapacitance = 0.0397372130805
print "noise per capacitance ATLAS ", noisePerCapacitance, " MeV/pF"
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
for i in range (0, Nplanes):  
    nameShield = "hCapacitance_shields"+str(i)
    hCapShield.append(fIn.Get(nameShield))
    nameTrace = "hCapacitance_traces"+str(i)
    hCapTrace.append(fIn.Get(nameTrace))
    nameDetector = "hCapacitance_detector"+str(i)
    hCapDetector.append(fIn.Get(nameDetector))

index0 = 0    
nbins = hCapShield[index0].GetNbinsX()
etaMin = hCapShield[index0].GetXaxis().GetBinLowEdge(1)
etaMax = 2.5
print "number of bins ", nbins,  ", etaMin ", etaMin, ", etaMax ", etaMax

maximumCap = 0.
maximumNoiseAll = 0.
maximumNoise = 0.

for i in range (0, Nlayers):  
    #Prepare electronic noise histograms    
    h_elecNoise_fcc.append( TH1F() )
    h_elecNoise_fcc[i].SetLineWidth(3)
    #h_elecNoise_fcc[i].SetLineColor(i+1)
    h_elecNoise_fcc[i].SetBins(nbins, etaMin, etaMax)
    h_elecNoise_fcc[i].SetTitle("Default electronic noise - shield + detector capacitance; |#eta|; Electronic noise [GeV]")
    h_elecNoise_fcc[i].SetName("h_elecNoise_fcc_"+str(i+1))

    h_elecNoise_all.append( TH1F() )
    h_elecNoise_all[i].SetLineWidth(3)
    #h_elecNoise_all[i].SetLineColor(i+1)
    h_elecNoise_all[i].SetBins(nbins, etaMin, etaMax)
    h_elecNoise_all[i].SetTitle("Total electronic noise - shield + trace + detector capacitance; |#eta|; Electronic noise [GeV]")
    h_elecNoise_all[i].SetName("h_elecNoise_all_"+str(i+1))
    
    h_elecNoise_withoutTraceCap.append( TH1F() )
    h_elecNoise_withoutTraceCap[i].SetLineWidth(3)
    #h_elecNoise_withoutTraceCap[i].SetLineColor(i+1)
    h_elecNoise_withoutTraceCap[i].SetBins(nbins, etaMin, etaMax)
    h_elecNoise_withoutTraceCap[i].SetTitle("Electronic noise without trace capacitance; |#eta|; Electronic noise [GeV]")
    h_elecNoise_withoutTraceCap[i].SetName("h_elecNoise_withoutTraceCap_"+str(i+1))

    h_elecNoise_shield.append( TH1F() )
    h_elecNoise_shield[i].SetLineWidth(3)
    #h_elecNoise_shield[i].SetLineColor(i+1)
    h_elecNoise_shield[i].SetBins(nbins, etaMin, etaMax)
    h_elecNoise_shield[i].SetTitle("Electronic noise - shields; |#eta|; Electronic noise [GeV]")
    h_elecNoise_shield[i].SetName("h_elecNoise_shield_"+str(i+1))

    h_elecNoise_trace.append( TH1F() )
    h_elecNoise_trace[i].SetLineWidth(3)
    #h_elecNoise_trace[i].SetLineColor(i+1)
    h_elecNoise_trace[i].SetBins(nbins,etaMin, etaMax)
    h_elecNoise_trace[i].SetTitle("Electronic noise -traces; |#eta|; Electronic noise [GeV]")
    h_elecNoise_trace[i].SetName("h_elecNoise_trace_"+str(i+1))
    
    h_elecNoise_detector.append( TH1F() )
    h_elecNoise_detector[i].SetLineWidth(3)
    #h_elecNoise_detector[i].SetLineColor(i+1)
    h_elecNoise_detector[i].SetBins(nbins, etaMin, etaMax)
    h_elecNoise_detector[i].SetTitle("Electronic noise - detector; |#eta|; Electronic noise [GeV]")
    h_elecNoise_detector[i].SetName("h_elecNoise_detector_"+str(i+1))

    #Total capacitance plot (shield + trace + detector)
    hCapTotal.append( TH1F() )
    hCapTotal[i].SetBins(nbins, etaMin, etaMax)
    #hCapTotal[i].SetLineColor(i+1)
    hCapTotal[i].SetLineWidth(3)
    hCapTotal[i].SetTitle("Total capacitance; |#eta|; Capacitance [pF]")
    hCapTotal[i].SetName("hCapacitance"+str(i))

indexLayer = 0
capShield = [0]*(nbins+1)
capTrace = [0]*(nbins+1)
capDetector = [0]*(nbins+1)

#Correct for different SF in ATLAS and FCC
noiseConversionFactor = noisePerCapacitance * SFatlas / SFfcc

for ii in range (0, Nplanes):
    if ii < indexLayersToMerge[indexLayer]:
        for ibin in range(0, nbins+1):
            capShield[ibin] += hCapShield[ii].GetBinContent(ibin)
            capTrace[ibin] += hCapTrace[ii].GetBinContent(ibin)
            capDetector[ibin] += hCapDetector[ii].GetBinContent(ibin)
        #print ii, indexLayer
    else:
        #print "Fill ", ii, indexLayer
        for ibin in range(0, nbins+1):
            #total capacitance
            hCapTotal[indexLayer].SetBinContent( ibin, capShield[ibin] + capTrace[ibin] + capDetector[ibin] )
            #noise
            noiseAll = noiseConversionFactor * ( capShield[ibin] + capTrace[ibin] + capDetector[ibin] ) / GeV
            noiseWithoutTrace = noiseConversionFactor * ( capShield[ibin] + capDetector[ibin] ) / GeV
            noiseShield = noiseConversionFactor * capShield[ibin] / GeV
            noiseTrace = noiseConversionFactor * capTrace[ibin] / GeV
            noiseDetector = noiseConversionFactor * capDetector[ibin] / GeV
            #find maximum for drawing of histograms
            if noiseAll>maximumNoiseAll:
                maximumNoiseAll = noiseAll
            if noiseWithoutTrace>maximumNoise:
                maximumNoise = noiseWithoutTrace
            if (capShield[ibin] + capTrace[ibin] + capDetector[ibin])>maximumCap:
                maximumCap = (capShield[ibin] + capTrace[ibin] + capDetector[ibin])
            #fill histogram
            #default: without traces
            h_elecNoise_fcc[indexLayer].SetBinContent(ibin, noiseWithoutTrace)
            h_elecNoise_all[indexLayer].SetBinContent(ibin, noiseAll)
            h_elecNoise_withoutTraceCap[indexLayer].SetBinContent(ibin, noiseWithoutTrace)
            h_elecNoise_shield[indexLayer].SetBinContent(ibin, noiseShield)
            h_elecNoise_trace[indexLayer].SetBinContent(ibin, noiseTrace)
            h_elecNoise_detector[indexLayer].SetBinContent(ibin, noiseDetector)
            capShield[ibin] = hCapShield[ii].GetBinContent(ibin)
            capTrace[ibin] =  hCapTrace[ii].GetBinContent(ibin)
            capDetector[ibin] = hCapDetector[ii].GetBinContent(ibin)
            #if ibin==1:
            #   print "layer %d" %i, "eta==0: capacitance %.0f pF," %( capShield[ibin] + capTrace[ibin] + capDetector[ibin] ), "total elec. noise %.4f GeV" %noiseAll, "elec. noise without trace cap. %.4f GeV" %noiseWithoutTrace
        indexLayer += 1
        #print indexLayer, ii
  
#Fill histograms in last layer
for ibin in range(0, nbins+1):
    #total capacitance
    hCapTotal[indexLayer].SetBinContent( ibin, capShield[ibin] + capTrace[ibin] + capDetector[ibin] )
    #noise
    noiseAll = noiseConversionFactor * ( capShield[ibin] + capTrace[ibin] + capDetector[ibin] ) / GeV
    noiseWithoutTrace = noiseConversionFactor * ( capShield[ibin] + capDetector[ibin] ) / GeV
    noiseShield = noiseConversionFactor * capShield[ibin] / GeV
    noiseTrace = noiseConversionFactor * capTrace[ibin] / GeV
    noiseDetector = noiseConversionFactor * capDetector[ibin] / GeV
    #find maximum for drawing of histograms
    if noiseAll>maximumNoiseAll:
        maximumNoiseAll = noiseAll
    if noiseWithoutTrace>maximumNoise:
        maximumNoise = noiseWithoutTrace
    if (capShield[ibin] + capTrace[ibin] + capDetector[ibin])>maximumCap:
        maximumCap = (capShield[ibin] + capTrace[ibin] + capDetector[ibin])
    #fill histogram
    #default: without traces
    h_elecNoise_fcc[indexLayer].SetBinContent(ibin, noiseWithoutTrace)
    h_elecNoise_all[indexLayer].SetBinContent(ibin, noiseAll)
    h_elecNoise_withoutTraceCap[indexLayer].SetBinContent(ibin, noiseWithoutTrace)
    h_elecNoise_shield[indexLayer].SetBinContent(ibin, noiseShield)
    h_elecNoise_trace[indexLayer].SetBinContent(ibin, noiseTrace)
    h_elecNoise_detector[indexLayer].SetBinContent(ibin, noiseDetector)
    
#print maximumCap, maximumNoiseAll, maximumNoise

# Prepare "nice" plots & save all capacitances + noise
fSaveAll.cd()
for h in hCapTotal:
    h.SetMinimum(0.)
    h.SetMaximum(maximumCap*1.2)
    h.GetYaxis().SetTitleOffset(1.4)
    h.Write()
for h in  h_elecNoise_all:
    h.SetMinimum(0.)
    h.SetMaximum(maximumNoiseAll*1.2)
    h.GetYaxis().SetTitleOffset(1.4)
    h.Write()

    
for h in itertools.chain(h_elecNoise_fcc, h_elecNoise_withoutTraceCap, h_elecNoise_shield, h_elecNoise_trace, 
h_elecNoise_detector):
    h.SetMinimum(0.)
    h.SetMaximum(maximumNoise*1.2)
    h.GetYaxis().SetTitleOffset(1.4)
    h.Write()

#Final noise for FCCSW
fSave.cd()
for h in h_elecNoise_fcc:
    h.Write()

closeInput = raw_input("Press ENTER to exit")
