from ROOT import TH1F, TCanvas, TLegend, TFile, Double, gStyle
from math import ceil, sin, cos, atan, exp, log, tan, pi, pow

#Options:
#Impedance: 50 (default) or 33 Ohm
#Shields width: 2 (default) or 4 - how many times are shield larger than gap hs 

flagImpedance = '33'
flagsShieldsWidth = 2

#Check the flags
if flagImpedance!='50' and flagImpedance!='33':
    print "WARNING: thickness hs for impedance ", flagImpedance, " Ohm not know, setting to default (33 Ohm)"
    flagImpedance = '33'

print "Calculating capacitances for this setup:"
print "Impedance, ", flagImpedance, " Ohm , shield width -", flagsShieldsWidth, " * hs" 
    
filename = "capacitances_emec_"+flagImpedance+"Ohm_"+str(flagsShieldsWidth)+"shieldWidth.root"

#EMEC geometry
rmax = 2550.
z_1stpcb = 5440.
zmax = 6020.
pcbThickness = 1.2 #mm
pcbThicknessNoHV = 1.0 #mm (0.1 mm between PCB inside & HV layer on both sides)
passiveThickness = 1.5 #mm
activeThickness = 1.0 #mm (double of the LAr width)
z_period  = activeThickness + pcbThickness + passiveThickness # 2* LAr gap + PCB thickness + absorber
deltaEta = 0.01
deltaPhi = 0.01
maxEta = 2.5
minEta = 0.
Nplanes = 156

minEta = []
numEta = []
numEtaFront = []
numEtaBack = []
zlayer = []
rmin = []

#constants:
# distance from signal trace to shield (HS) - from impedance vs. trace width vs. distance to ground layer 2D plot (Z = 50 Ohm)
# trace width (W) - min value
# trace thickness (T) - min value
# distance from shield to the edge of PCB
#http://www.analog.com/media/en/training-seminars/design-handbooks/Basic-Linear-Design/Chapter12.pdf, page 40
#signal trace
hs = 0
if flagImpedance=='50':
    hs=0.17 # for 50 Ohm
else:    
    hs = 0.09 # for 33 Ohm
w=0.127
t=0.035
#shield
nmult_ws = flagsShieldsWidth
# how many times are shield larger than gap hs (consider 2 - default and 4 - extreme case)

ws = nmult_ws*hs
hm = (pcbThicknessNoHV - 5*t - 2*hs)/2.   

epsilonR = 4.0 # PCB

#conversion factor: 1 inch = 25/4 mm
inch2mm = 25.4

#multiplicative factor for each PCB
# factor 2 for traces, detector area
nmult = 2

#Detector area calculation
epsilonRLAr = 1.5 # LAr at 88 K
epsilon0 = 8.854/1000. #pF/mm

#EMEC eta ranges per disk
for i in range (0, Nplanes):
    zlayer.append( z_1stpcb + i*z_period )
    rmin.append( zlayer[i] * tan(2*atan(exp(-(maxEta)))) )
    etaminLayer = -log(tan(0.5*atan(rmax/zlayer[i])))
    minEta.append( etaminLayer )
    numEta.append( int(ceil((maxEta-minEta[i])/deltaEta)) )
    numEtaFront.append( numEta[i] / 4 )
    numEtaBack.append( numEta[i] - numEtaFront[i] )
    print "disc ", i, " z ", zlayer[i], "eta_min ", minEta[i], " num. of eta bins ", numEta[i]

layerThickness = []
tracesPerLayer = []
tracesLength0 = []
for j in range (0, Nplanes):
    layerThickness_disk = []
    for i in range (0, numEta[j]):
        layerThickness_disk.append( zlayer[j] * ( tan(2*atan(exp(-(minEta[j]+i*deltaEta)))) - tan(2*atan(exp(-(minEta[j]+(i+1)*deltaEta)))) ) ) 
    layerThickness.append(layerThickness_disk)
#print (layerThickness)

for j in range (0, Nplanes):
    #print "plane ", j
    tracesPerLayer_disk = []
    tracesLength0_disk = []
    for i in range (0, numEta[j]):
        if i<(numEtaFront[j]):
            tracesPerLayer_disk.append(numEtaFront[j]-1-i)
            sumTraceLength = 0
            index = i
            while index>0:
                sumTraceLength += layerThickness[j][index-1]
                index -= 1
            tracesLength0_disk.append(sumTraceLength)
        else:
            tracesPerLayer_disk.append(i-numEtaFront[j])
            sumTraceLength = 0
            index = i
            while index<(numEta[j]-1):
                sumTraceLength += layerThickness[j][index]
                index += 1
            tracesLength0_disk.append(sumTraceLength)
    tracesPerLayer.append(tracesPerLayer_disk)
    tracesLength0.append(tracesLength0_disk)
#print tracesPerLayer
#print tracesLength0  

#1st PCB
index1st = 0 # same binning for all planes (1st PCB)
numEta1st = numEta[index1st]
minEta1st = minEta[index1st] 

hCapTrace = []
hCapShield = []
hCapDetector = []
for i in range (0, Nplanes):
    #traces 
    hCapTrace.append(TH1F())
    hCapTrace[i].SetBins(numEta1st, minEta1st, maxEta)
    hCapTrace[i].SetLineWidth(2)
    hCapTrace[i].SetTitle("Capacitance of traces; |#eta|; Capacitance [pF]")
    hCapTrace[i].SetName("hCapacitance_traces"+str(i))
    #shields
    hCapShield.append(TH1F())
    hCapShield[i].SetBins(numEta1st, minEta1st, maxEta)
    hCapShield[i].SetLineWidth(2)
    hCapShield[i].SetTitle("Capacitance of shields; |#eta|; Capacitance [pF]")
    hCapShield[i].SetName("hCapacitance_shields"+str(i))
    #area
    hCapDetector.append(TH1F())
    hCapDetector[i].SetBins(numEta1st, minEta1st, maxEta)
    hCapDetector[i].SetLineWidth(2)
    hCapDetector[i].SetTitle("Capacitance of detector area; |#eta|; Capacitance [pF]")
    hCapDetector[i].SetName("hCapacitance_detector"+str(i))

for ii in range (0, Nplanes):
    #only half shields / detector area in the 1st layer
    if ii == 0:
        ncorrection = 0.5
    else:
        ncorrection = 1
    rmin1 = rmin[ii]
    for i in range (0, numEta[ii]):
        #Trace capacitance (stripline)
        traceLength = tracesLength0[ii][i]
        logStripline = log(3.1 * hs / (0.8 * w + t))
        capacitanceTrace = ncorrection * nmult * 1 / inch2mm * 1.41 * epsilonR / logStripline * traceLength
        hCapTrace[ii].SetBinContent(numEta1st - numEta[ii] + i + 1, capacitanceTrace)
     
        #Shield capacitance (microstrip)
        shieldLength = tracesPerLayer[ii][i] * layerThickness[ii][i]
        logMicrostrip = log(5.98 * hm / (0.8 * ws + t))
        capacitanceShield = 2 * ncorrection * nmult * 1 / inch2mm * 0.67 * (epsilonR + 1.41) / logMicrostrip * shieldLength
        hCapShield[ii].SetBinContent(numEta1st - numEta[ii] + i + 1, capacitanceShield)
      
        #Detector area (C = epsilon*A/d)
        rmin2 = rmin1 + layerThickness[ii][i]
        area = deltaPhi/2. * ( pow(rmin2,2) - pow(rmin1,2) )  #mm2
        #print rmin1, rmin2, pow(rmin2,2) - pow(rmin1,2), area
        rmin1 = rmin2
        distance = activeThickness / 2.
        capacitanceDetector = ncorrection * nmult * epsilon0 * epsilonRLAr * area / distance
        hCapDetector[ii].SetBinContent(numEta1st - numEta[ii] + i + 1, capacitanceDetector)
        #print "eta bin %d" %i, "capacitanceTrace %.0f mm," %capacitanceTrace, "capacitanceShield %.0f pF" %capacitanceShield, "capacitanceArea %.2f pF" %capacitanceArea

plots = TFile(filename,"RECREATE")

for i in range (0, Nplanes):
    hCapTrace[i].Write()
    hCapShield[i].Write()
    hCapDetector[i].Write()
    
closeInput = raw_input("Press ENTER to exit")
