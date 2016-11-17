import argparse
import re
def parse_args():
    parser = argparse.ArgumentParser()
    ## Obligatory arguments
    parser.add_argument("inputFile", help="Input file name", type = str)
    ## Optional arguments
    parser.add_argument("-e","--energy", help="Energy of the particle [GeV]", type = int)
    parser.add_argument('-r', "--inputFileRegex", action='store_true', help='Parse inputFile and insert energy in place of \'*\' character')
    parser.add_argument("-o","--output", help="Output file name", type = str)
    parser.add_argument("--sf", help="SF", type = float)
    args = parser.parse_args()
    global filenameIn
    filenameIn = args.inputFile
    global filenameOut
    filenameOut = args.output
    global energy
    energy = args.energy
    global regex
    regex = args.inputFileRegex
    if regex:
        pattern = re.compile('\*')
        if pattern.search(filenameIn):
            if energy:
                filenameIn = re.sub(pattern,str(energy),filenameIn)
            else:
                print("Specify energy of the initial particle to insert it in the input file name")
                exit()
        else:
            print("Character '*' not found in the input file name.")
            print("Either include '*' so it can be substituted with the energy or do not use '-r/--inputFileRegex' option")
            exit()
    global sf
    sf = args.sf

def print_config():
    print("Input file: " + filenameIn)
    if filenameOut:
        print("Output file: " + filenameOut)
    if energy:
        print("Energy of initial particle: " + str(energy) + " GeV")
    if sf:
        print("SF: " + str(sf))

if __name__ == "__main__":
    parse_args()
    print_config()
