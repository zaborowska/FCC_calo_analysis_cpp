from draw_functions import prepare_double_canvas, prepare_second_graph, prepare_graph, prepare_divided_canvas
if __name__=="__main__":
    import sys
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-t","--title", default="Title", help="Title", type=str)
    parser.add_argument("-n","--histogramName", default="etaEnWeightedVsEtaTrueLayer2", help="Name of the histogram containing the energy distribution", type = str)
    parser.add_argument("-max","--axisMax", help="Maximum of the axis", type = float)
    parser.add_argument("-min","--axisMin", help="Minimum of the axis", type = float)
    parser.add_argument("--style", help="Draw style", type = str, default = "box")
    parser.add_argument("--rebin", help="Rebin", type = int, nargs='+')
    parser.add_argument("--gaus", help="Fit gaus", action='store_true')
    parser.add_argument("--gausTwoSigma", help="Fit gaus", action='store_true')
    parser.add_argument("--out", help="Addition to the output name", type = str)
    parser.add_argument("-i","--input", help="Input file", type = str, nargs='+',required = True)
    args = parser.parse_args()
    hname = args.histogramName
    title = args.title
    axisMin = args.axisMin if '-min' in sys.argv or '--axisMin' in sys.argv else 0
    axisMax = args.axisMax
    draw = args.style
    inputName = args.input
    if args.rebin:
        if len(args.rebin) == 1:
            rebin = args.rebin * len(inputName)
        elif len(args.rebin) == len(inputName):
            rebin = args.rebin
        else:
            print("ERROR, length of rebin should be either 1 or same as number of input files")
            exit()
    import ROOT
    ROOT.gStyle.SetOptStat(0000)
    # ROOT.gStyle.SetOptFit(111)
    factor = 2
    cRes, padRes, padLin = prepare_double_canvas("eta","Pseudorapidity", factor)
    resName = ""
    energyName = ""
    layerName = ""
    layerColour = 9
    layerColours = [1, ROOT.kAzure-3, ROOT.kRed-3, ROOT.kGreen+2, ROOT.kOrange+5, ROOT.kGray+2, ROOT.kViolet-5, ROOT.kYellow-6]
    if hname.find('Layer') != -1:
        layerName = str(int(hname[(hname.find('Layer')+5):])+1) + ". layer"
        layerColour = int(hname[(hname.find('Layer')+5):])
    for ifile,inname in enumerate(inputName):
        current_name = inname.split('/')[-1][:-5] + '_' + hname
        resName = ""
        for name in current_name.split('_'):
            if name.find('weight') != -1:
                weight = float(name[6:])
            else:
                resName += name
                resName += "_"
            if name.find('GeV') != -1:
                energyName = float(name[:-3])
        file = ROOT.TFile(inname,"READ")
        hist = file.Get(hname)
        hist.SetTitle(";#eta_{MC};#eta_{rec}")
        padRes.cd()
        hist.Draw(draw)
        if args.rebin:
            hist.Rebin(rebin[ifile])
        prepare_graph(hist, "", ";#eta_{MC};#eta_{rec}",layerColours[layerColour])
        hDiff = ROOT.TH1F("h_diff","Diff eta", hist.GetNbinsX(), hist.GetXaxis().GetBinCenter(1), hist.GetXaxis().GetBinCenter(hist.GetNbinsX()))
        prepare_second_graph(hDiff, hist, "", ";#eta_{MC};#eta_{rec}", factor)
        # hDiff.GetXaxis().SetTitle("#eta_{MC}")
        hDiff.SetTitle("weight "+str(weight)+";;#Delta#eta")
        if hname.find("EnWeighted") != -1:
            hDiff.SetTitle("energy weighted")
        ii = 0
        for i in range(1, hist.GetNbinsX()):
            hProject = hist.ProjectionY( "hprojection"+str(i), i, i,"e")
            if hProject.GetEntries() > 0:
                hDiff.SetBinContent(i,hProject.GetMean() - hProject.GetBinCenter(i))
                # print(i, "   ", hProject.GetMean()," - ",hProject.GetBinCenter(i))
                hDiff.SetBinError(i,hProject.GetRMS())
                ii = ii + 1
        hist.GetXaxis().SetRangeUser(-0.016,0.016)
        hist.GetYaxis().SetRangeUser(-0.016,0.016)
        padLin.cd()
        hDiff.GetXaxis().SetRangeUser(-0.016, 0.016)
        hDiff.GetYaxis().SetRangeUser(-0.0021, 0.0021)
        hDiff.Draw("pe")
        fit = ROOT.TF1("fit","pol0",-0.02,0.02)
        fit.FixParameter(0,0)
        hDiff.Fit("fit")
        cRes.Update()
        if hname.find("EnWeighted") != -1:
            print(hname.find("EnWeighted"))
            print(hname)
            cRes.SaveAs(resName[:-1] + ".png")
            cRes.SaveAs(resName[:-1] + ".pdf")
        else:
            print(hname)
            cRes.SaveAs(resName[:-1] + "_weight" + str(weight) + ".png")
            cRes.SaveAs(resName[:-1] + "_weight" + str(weight) + ".pdf")
#    input("enter")
