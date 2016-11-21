FCC_calo_analysis
=====================

Analysis tools for calorimetry:
 - hits/cells/cluster read from EDM in c++
 - setup and results of analysis (drawing etc.) handled from python

# Initialization

Dependencies:
 - ROOT
 - PODIO
 - fcc-edm
taken from the FCC software stack:

~~~{.sh}
source init.sh
~~~

# Build

~~~{.sh}
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=../install ..
make -j 8 install
cd ..
~~~

# Run analysis

## Common: configuration

To see the help message:

~~~{.sh}
python scripts/calo_init.py --help
~~~

The required argument is the input file name and the energy (vector):

~~~{.sh}
python scripts/calo_init.py <INPUT_FILE_NAME>.root [energy]
~~~

User may also specify any of the optional arguments.

In particular, if user wants to analyse multiple files, for different energies of the initial particle:

~~~{.sh}
python scripts/calo_init.py <INPUT_FILE_NAME_CONTAINING_*_CHARACTER>.root 10 20 50 100 -r
~~~

where `-r` indicates that a regex should be used, substituting the `*` character in the input file name with the energy of the particle.

For instance,
~~~{.sh}
python scripts/calo_init.py 'output_electron_*GeV.root' 10 100 -r
~~~
results in:
~~~{.sh}
Input file: output_electron_10GeV.root
Input file: output_electron_100GeV.root
Energy of initial particle: [10, 100] GeV
~~~


## Example: CaloAnalysis_simple (using HistogramClass)

Required arguments:
 - input file name
 - energy
 - sf

~~~{.sh}
cd scripts
python test_macro_simple.py <INPUT_FILE_NAME>.root 50 5.4
~~~


## Shower profiles: CaloAnalysis_profiles (using HistogramClass_profiles)

Required arguments:
 - input file name
 - energy
 - sf

Analysis is done for one file.

~~~{.sh}
cd scripts
python test_macro_profiles.py <INPUT_FILE_NAME>.root
~~~

## Cells: CaloAnalysis_cells (using HistogramClass_cells)

Required arguments:
 - input file name
 - energy
 - sf

Analysis is done for one file.

~~~{.sh}
cd scripts
python test_macro_cells.py <INPUT_FILE_NAME>.root
~~~


## Reconstruction preview: CaloAnalysis_recoExample (using HistogramClass_recoExample)

Preview of the reconstructed cluster.
The first plot contains all towers, the second the centre of the cluster and the third one all the cells associated to the cluster.
Additionally the windows used in the reconstrucion are drawn.

Required arguments:
 - input file name
 - energy

Analysis is done for all the files (all energies), but for one event from each (default: first event).

~~~{.sh}
cd scripts
python scripts/plot_recoExample.py <INPUT_FILE_NAME>.root ENERGY
~~~

Input file name and the energy are required.

List of additional options:
~~~{.sh}
  -r, --inputFileRegex  Parse inputFile and insert energy in place of '*'
                        character
  -o OUTPUT, --output OUTPUT
                        Output file name
  -v, --verbose         Verbose
  --windowSeed WINDOWSEED WINDOWSEED
                        Size of the window used for seeding [eta,phi]
  --windowPos WINDOWPOS WINDOWPOS
                        Size of the window used for berycentre coalculation
                        [eta,phi]
  --windowDupl WINDOWDUPL WINDOWDUPL
                        Size of the window used for duplicate removal
                        [eta,phi]
  --dEta DETA DETA      Size of the tower in eta
  --maxEta MAXETA       Maximum eta
  --dPhi DPHI           Size of the tower in phi
  --numPhi NUMPHI       Number of the towers in phi
  --zoom ZOOM ZOOM      How many bins around centre should be visible
  --event EVENT         Number of an event to draw
~~~

## Reconstruction monitor: CaloAnalysis_recoMonitor (using HistogramClass_recoMonitor)

Monitor plots of the reconstruction procedure.
The plots are created based on the clusters and generated particles (for single particle events only).

~~~{.sh}
cd scripts
python scripts/plot_recoExample.py <INPUT_FILE_NAME>.root ENERGY <INPUT_FILE_WITH_MC_INFORMATION>.root
~~~

The plots are saved to root file and in png. The first row includes the energy distribution, energy distribution as a function of phi, number of reconstructed clusters and the number of reconstructed clusters as a function of phi.
The second row contains information about any reconstructed duplicates (difference of energy, eta, phi and R=sqrt(eta^2+phi^2) with respect to the most energetic cluster).
The last row contains comparison to the MC particle: difference in eta distribution, difference in eta as a function of eta, difference in phi distribution and difference in phi as a function of phi.

List of additional options:
~~~{.sh}
  -r, --inputFileRegex  Parse inputFile and insert energy in place of '*'
                        character
  -o OUTPUT, --output OUTPUT
                        Output file name
  -v, --verbose         Verbose
  --dEta DETA DETA      Size of the tower in eta
  --maxEta MAXETA       Maximum eta
  --dPhi DPHI           Size of the tower in phi
  --numPhi NUMPHI       Number of the towers in phi
~~~