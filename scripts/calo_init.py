import argparse
import re

parser = argparse.ArgumentParser()
def add_defaults():
    ## Obligatory arguments
    parser.add_argument("inputFile", help="Input file name for analysis", type = str)
    parser.add_argument("energy", help="Energy of the particle [GeV]", type=int, nargs='+')
    ## Optional arguments
    parser.add_argument('-r', "--inputFileRegex", action='store_true', help='Parse inputFile and insert energy in place of \'*\' character')
    parser.add_argument("-o","--output", help="Output file name", type = str)
    parser.add_argument('-v', "--verbose", action='store_true', help='Verbose')

def parse_args():
    global args
    args = parser.parse_args()
    global filenames
    filenameIn = args.inputFile
    global verbose
    verbose = args.verbose
    global filenameOut
    filenameOut = args.output
    global energies
    energies = args.energy
    global regex
    regex = args.inputFileRegex
    if regex:
        pattern = re.compile('\*')
        if pattern.search(filenameIn):
            filenames = []
            for en in energies:
                filenames.append( re.sub(pattern,str(en),filenameIn) )
        else:
            print("Character '*' not found in the input file name.")
            print("Either include '*' so it can be substituted with the energy or do not use '-r/--inputFileRegex' option")
            exit()
    else:
        filenames = [filenameIn]

def print_config():
    for name in filenames:
        print("Input file: "+ name)
    if filenameOut:
        print("Output file: " + filenameOut)
    if energies:
        print("Energy of initial particles: " + str(energies) + " GeV")

if __name__ == "__main__":
    add_defaults()
    parse_args()
    print_config()
