import calo_init
calo_init.add_defaults()
## add arguments relevant only for that script
calo_init.parser.add_argument("--clusterColl", help="Name of the clusters collection (fcc::CaloClusterCollection)", type = str)
calo_init.parser.add_argument("--positionColl", help="Name of the positioned cells collection (fcc::PositionedCaloHitCollection)", type = str)
calo_init.parser.add_argument("--cellColl", help="Name of the cells collection (fcc::CaloHitCollection)", type = str)
calo_init.parser.add_argument("--windowSeed", help="Size of the window used for seeding [eta,phi]", type = int, nargs=2)
calo_init.parser.add_argument("--windowPos", help="Size of the window used for berycentre coalculation [eta,phi]", type = int, nargs=2)
calo_init.parser.add_argument("--windowDupl", help="Size of the window used for duplicate removal [eta,phi]", type = int, nargs=2)
calo_init.parser.add_argument("--windowFin", help="Size of the final window used [eta,phi]", type = int, nargs=2)
calo_init.parser.add_argument("--dEta", help="Size of the tower in eta", type = float, nargs=2)
calo_init.parser.add_argument("--maxEta", help="Maximum eta", type = float)
group = calo_init.parser.add_mutually_exclusive_group()
group.add_argument("--dPhi", help="Size of the tower in phi", type = float)
group.add_argument("--numPhi", help="Number of the towers in phi", type = int)
calo_init.parser.add_argument("--zoom", help="How many bins around centre should be visible", type = int, nargs=2)
calo_init.parser.add_argument("--event", help="Number of an event to draw", type = int)
calo_init.parse_args()
calo_init.print_config()

from math import pi, floor
# set of default parameters
eventToDraw = 0
maxEta = 1.68024
nPhi = 704 # artificially increase by 1 (odd number) - to make plots look OK
dEta = 0.01
nEta = int(2*maxEta/dEta + 1)
dPhi = 2*pi/nPhi
nameClusterCollection = "CaloClusters"
namePositionedHitsCollection = "caloCellsPositions"
nameCellsCollection = "ECalBarrelCells"
zoomEta = 31
zoomPhi = 31
etaWindowSeed = 7
phiWindowSeed = 15
etaWindowPos = 3
phiWindowPos = 11
etaWindowDupl = 5
phiWindowDupl = 11
etaWindowFin = 7
phiWindowFin = 19
# get parameters if passed from command line
if calo_init.args.clusterColl:
    nameClusterCollection = calo_init.args.clusterColl
if calo_init.args.cellColl:
    nameCellsCollection = calo_init.args.cellColl
if calo_init.args.positionColl:
    namePositionedHitsCollection = calo_init.args.positionColl
if calo_init.args.maxEta:
    maxEta = calo_init.args.maxEta
if calo_init.args.numPhi:
    nPhi = calo_init.args.numPhi
    dPhi = 2*pi/nPhi
if calo_init.args.dPhi:
    dPhi = calo_init.args.dPhi
    nPhi = int(dPhi*2*pi)
if calo_init.args.windowSeed:
    etaWindowSeed = calo_init.args.windowSeed[0]
    phiWindowSeed = calo_init.args.windowSeed[1]
if calo_init.args.windowPos:
    etaWindowPos = calo_init.args.windowPos[0]
    phiWindowPos = calo_init.args.windowPos[1]
if calo_init.args.windowDupl:
    etaWindowDupl = calo_init.args.windowDupl[0]
    phiWindowDupl = calo_init.args.windowDupl[1]
if calo_init.args.windowFin:
    etaWindowFin = calo_init.args.windowFin[0]
    phiWindowFin = calo_init.args.windowFin[1]
if calo_init.args.zoom:
    zoomEta = calo_init.args.zoom[0]
    zoomPhi = calo_init.args.zoom[1]
if calo_init.args.event:
    eventToDraw = calo_init.args.event

from ROOT import gSystem
gSystem.Load("./libCaloAnalysis.so")
from ROOT import ReconstructionExampleWithCells, TCanvas, TFile, gStyle, gPad, kGreen, kRed, kBlue, TColor, kBlack, TGraphErrors, kMagenta
from draw_functions import *

# use this script for multiple files
# gStyle.SetPalette(56) # kInvertedDarkBodyRadiator
gStyle.SetPalette(73) # kCMYK

for ifile, filename in enumerate(calo_init.filenamesIn):
    energy = calo_init.energy(ifile)
    print "Energy of the initial particle: " + str(energy)
    print "File with reconstruction results: " + filename
    analysis = ReconstructionExampleWithCells(nameClusterCollection,
                                              nameCellsCollection,
                                              eventToDraw,
                                              energy,
                                              maxEta, # max eta
                                              nEta, # number of bins in eta
                                              nPhi, # number of bins in phi
                                              8,
                                              dEta, # tower size in eta
                                              dPhi, # tower size in phi
                                              "system:4,cryo:1,type:3,subtype:3,layer:8,eta:9,phi:10", # cell encoding
                                              -1.68024, # eta offset
                                              -pi+pi/704.) # phi offset
    analysis.loop(filename, calo_init.verbose)
    # retrieve histograms to draw them
    hist = []
    hist.append(analysis.hAllCellEnergy)
    hist.append(analysis.hClusterEnergy)
    hist.append(analysis.hClusterCellEnergy)
    print analysis.hClusterEnergy.GetEntries()
    print analysis.hClusterCellEnergy.GetEntries()
    ## Calculate some parameters
    enTotal = hist[0].Integral()
    enCluster = hist[1].Integral()
    meanEta = hist[1].ProjectionX().GetMean()
    meanPhi = hist[1].ProjectionY().GetMean()
    meanEtaCentre = hist[1].ProjectionX().GetBinCenter(hist[1].ProjectionX().FindBin(meanEta))
    meanPhiCentre = hist[1].ProjectionY().GetBinCenter(hist[1].ProjectionY().FindBin(meanPhi))
    print "eta = ", meanEta
    print "phi = ", meanPhi
    print "eta = ", meanEtaCentre
    print "phi = ", meanPhiCentre
    hist[2].SetMinimum(0)

    ## Set drawing options
    for h in hist:
        h.GetYaxis().SetRangeUser(meanPhiCentre - (floor(zoomPhi/2) ) * dPhi,meanPhiCentre + (floor(zoomPhi/2) ) * dPhi)
        h.GetXaxis().SetRangeUser(meanEtaCentre - (floor(zoomEta/2) ) * dEta,meanEtaCentre + (floor(zoomEta/2) ) * dEta)
        h.GetXaxis().SetNdivisions(zoomEta/2)
        h.GetYaxis().SetNdivisions(zoomPhi)
        h.GetXaxis().SetLabelSize(0.02)
        h.GetYaxis().SetLabelSize(0.02)

    canvLog = TCanvas('ECal_log_map_e'+str(energy)+'GeV', 'ECal', 1600, 1400 )
    canv = TCanvas('ECal_map_e'+str(energy)+'GeV', 'ECal', 1600, 1400 )
    canv.Divide(2,2)
    pad = canv.cd(1)
    pad.SetGrid()
    draw_hist2d(hist[0])
    pad = canv.cd(2)
    pad.SetLogz()
    pad.SetGrid()
    draw_hist2d(hist[0])
    draw_text(["energy: "+str(round(enTotal,2))+" GeV"])
    pad2 = canv.cd(3)
    draw_hist2d(hist[1])
    pad2.SetGrid()
    draw_text(["energy: "+str(round(hist[1].Integral(),2))+" GeV",
               "           "+str(round(hist[1].Integral()/enTotal*100,1))+" %"])
    pad = canv.cd(4)
    pad.SetGrid()
    draw_hist2d(hist[2])
    draw_text(["energy: "+str(round(hist[2].Integral(),2))+" GeV",
               "           "+str(round(hist[2].Integral()/enTotal*100,1))+" %"])
    for ipad in range(2,5):
        canv.cd(ipad)
        draw_rectangle([meanEta-etaWindowSeed/2.*dEta, meanPhi-phiWindowSeed/2.*dPhi],
                       [meanEta+etaWindowSeed/2.*dEta, meanPhi+phiWindowSeed/2.*dPhi], kBlack, 2)
        draw_rectangle([meanEta-etaWindowDupl/2.*dEta, meanPhi-phiWindowDupl/2.*dPhi],
                       [meanEta+etaWindowDupl/2.*dEta, meanPhi+phiWindowDupl/2.*dPhi], kBlue, 2)
        draw_rectangle([meanEta-etaWindowPos/2.*dEta, meanPhi-phiWindowPos/2.*dPhi],
                       [meanEta+etaWindowPos/2.*dEta, meanPhi+phiWindowPos/2.*dPhi], kGreen, 2)
        draw_rectangle([meanEta-etaWindowFin/2.*dEta, meanPhi-phiWindowFin/2.*dPhi],
                       [meanEta+etaWindowFin/2.*dEta, meanPhi+phiWindowFin/2.*dPhi], kRed, 5)

    canvLog.SetRightMargin(0.15)
    ROOT.gStyle.SetOptStat(0000)
    pad = canvLog.cd()
    pad.SetLogz()
    pad.SetGrid()
    draw_hist2d(hist[0])
    # draw_text(["energy: "+str(round(enCluster,2))+" GeV ("+str(round(enCluster / enTotal,1))+" %)"])
    hist[0].SetTitle("energy: "+str(round(enCluster,2))+" GeV ("+str(round(enCluster / enTotal * 100.,1))+" %)")
    canvLog.cd()
    draw_rectangle([meanEtaCentre-etaWindowSeed/2.*dEta, meanPhiCentre-phiWindowSeed/2.*dPhi],
                   [meanEtaCentre+etaWindowSeed/2.*dEta, meanPhiCentre+phiWindowSeed/2.*dPhi], kBlack, 2)
    draw_rectangle([meanEtaCentre-etaWindowDupl/2.*dEta, meanPhiCentre-phiWindowDupl/2.*dPhi],
                   [meanEtaCentre+etaWindowDupl/2.*dEta, meanPhiCentre+phiWindowDupl/2.*dPhi], kBlue, 2)
    draw_rectangle([meanEtaCentre-etaWindowPos/2.*dEta, meanPhiCentre-phiWindowPos/2.*dPhi],
                   [meanEtaCentre+etaWindowPos/2.*dEta, meanPhiCentre+phiWindowPos/2.*dPhi], kGreen, 2)
    draw_rectangle([meanEtaCentre-etaWindowFin/2.*dEta, meanPhiCentre-phiWindowFin/2.*dPhi],
                   [meanEtaCentre+etaWindowFin/2.*dEta, meanPhiCentre+phiWindowFin/2.*dPhi], kRed, 5)
    if calo_init.output(ifile):
        # canv.Print(calo_init.output(ifile)+'.root')
        # canv.Print(calo_init.output(ifile)+'.png')
        canvLog.Print(calo_init.output(ifile)+'.root')
    else:
        # canv.Print('ECal_map_e'+str(energy)+'GeV.png')
        canvLog.Print('ECal_log_map_e'+str(energy)+'GeV_event_'+str(eventToDraw)+'_window_'+str(etaWindowFin)+'x'+str(phiWindowFin)+'.png')

    canvLayers = prepare_divided_canvas("Layers", "Layers", 8)
    for lay in range(0,8):
        pad = canvLayers.cd(lay+1)
        pad.SetLogz()
        pad.SetGrid()
        draw_hist2d(analysis.hLayerEnergy[lay])
        draw_rectangle([meanEtaCentre-etaWindowSeed/2.*dEta, meanPhiCentre-phiWindowSeed/2.*dPhi],
                       [meanEtaCentre+etaWindowSeed/2.*dEta, meanPhiCentre+phiWindowSeed/2.*dPhi], kBlack, 2)
        draw_rectangle([meanEtaCentre-etaWindowDupl/2.*dEta, meanPhiCentre-phiWindowDupl/2.*dPhi],
                       [meanEtaCentre+etaWindowDupl/2.*dEta, meanPhiCentre+phiWindowDupl/2.*dPhi], kBlue, 2)
        draw_rectangle([meanEtaCentre-etaWindowPos/2.*dEta, meanPhiCentre-phiWindowPos/2.*dPhi],
                       [meanEtaCentre+etaWindowPos/2.*dEta, meanPhiCentre+phiWindowPos/2.*dPhi], kGreen, 2)
        draw_rectangle([meanEtaCentre-etaWindowFin/2.*dEta, meanPhiCentre-phiWindowFin/2.*dPhi],
                       [meanEtaCentre+etaWindowFin/2.*dEta, meanPhiCentre+phiWindowFin/2.*dPhi], kRed, 5)
        analysis.hLayerEnergy[lay].GetYaxis().SetRangeUser(meanPhiCentre - (floor(zoomPhi/2) ) * dPhi,meanPhiCentre + (floor(zoomPhi/2) ) * dPhi)
        analysis.hLayerEnergy[lay].GetXaxis().SetRangeUser(meanEtaCentre - (floor(zoomEta/2) ) * dEta,meanEtaCentre + (floor(zoomEta/2) ) * dEta)
        analysis.hLayerEnergy[lay].GetXaxis().SetNdivisions(zoomEta/2)
        analysis.hLayerEnergy[lay].GetYaxis().SetNdivisions(zoomPhi)
        analysis.hLayerEnergy[lay].GetXaxis().SetLabelSize(0.02)
        analysis.hLayerEnergy[lay].GetYaxis().SetLabelSize(0.02)
        analysis.hLayerEnergy[lay].GetZaxis().SetRangeUser(0,0.1)

    if calo_init.output(ifile):
        canvLayers.Print(calo_init.output(ifile)+'_layers.root')
        canvLayers.Print(calo_init.output(ifile)+'_layers.png')
        canvLayers.Print(calo_init.output(ifile)+'_layers.root')
    else:
        canvLog.Print('ECal_log_map_layers_e'+str(energy)+'GeV_event_'+str(eventToDraw)+'_window_'+str(etaWindowFin)+'x'+str(phiWindowFin)+'.png')

    canvLayerContainment = prepare_divided_canvas("LayersContainment", "Layers containment", 8)
    for lay in range(0,8):
        pad = canvLayerContainment.cd(lay+1)
        analysis.hLayerContainment[lay].Draw()

    hists_cont = [analysis.hContainment, analysis.hContainmentPercent, analysis.hLayerContainment95, analysis.hLayerContainment90, analysis.hLayerContainment85]
    canvTotalContainment = prepare_divided_canvas("LayersTotalContainment", "Layers total containment", len(hists_cont) + 3)
    for h in range(0,len(hists_cont)):
        pad = canvTotalContainment.cd(h+1)
        hists_cont[h].Draw()

    from math import fabs
    canvLayersPercent = prepare_divided_canvas("Layers percent cont", "Layers cont", 8)
    layerContainment90 = TGraphErrors()
    layerContainment95 = TGraphErrors()
    layerContainment85 = TGraphErrors()
    containments = [0.95, 0.9, 0.85]
    graphs = [layerContainment95, layerContainment90, layerContainment85]
    for lay in range(0,8):
        pad = canvLayersPercent.cd(lay+1)
        analysis.hLayerContainmentPercent[lay].Draw()
        fun = ROOT.TF1("fitContainmentLayer"+str(lay),"[0]*x/(1+[1]*x)",0.001, 0.5, 2)
        result = analysis.hLayerContainmentPercent[lay].Fit(fun, "RS")
        p0 = result.Get().Parameter(0)
        p1 = result.Get().Parameter(1)
        dp0 = result.Get().ParError(0)
        dp1 = result.Get().ParError(1)
        for icon, contain in enumerate(containments):
            rrange = 1. / (p0/contain - p1)
            drange = fabs(-1. / (p0/contain - p1)**2 ) * (fabs(1/contain) * dp0 + dp1)
            graphs[icon].SetPoint(lay,lay, rrange)
            graphs[icon].SetPointError(lay,0.5, drange)
    for icon, g in enumerate(graphs):
        pad = canvTotalContainment.cd(len(hists_cont) + icon)
        prepare_graph(g, "containment"+str(containments[icon]), str(containments[icon]*100)+"% containment in layer; #layer; #Delta R")
        g.Draw()

    for lay in range(0,8):
        pad = canvLayers.cd(lay+1)
        draw_ellipse([meanEtaCentre, meanPhiCentre],
                       [layerContainment90.GetY()[lay]] * 2, kMagenta, 3)
    canvLayers.Update()

raw_input("")
