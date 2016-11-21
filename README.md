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

In particular, if user wants to analyse multiple files:

~~~{.sh}
python scripts/calo_init.py <INPUT_FILE_NAME_CONTAINING_?_CHARACTER>.root ENERGY -r [LIST_OF_SUBSTITUTIONS]
~~~

where `-r` indicates that a regex should be used, substituting the `?` character in the input (or output) file name with the elements of the list of substitutions.

For instance,
~~~{.sh}
python scripts/calo_init.py 'output_10GeV_?.root' 10 -r firstPart secondPart
~~~

results in:
~~~{.sh}
Input file: output_10GeV_firstPart.root
Input file: output_10GeV_secondPart.root
Energy of initial particle: [10] GeV
~~~

If instead of of a list, "energy" is used after "-r" option, the '?' character gets substituted with the energy of the particles.

For instance,
~~~{.sh}
python scripts/calo_init.py 'output_?GeV.root' 10 50 100 -r energy
~~~

results in:
~~~{.sh}
Input file: output_10GeV.root
Input file: output_50GeV.root
Input file: output_100GeV.root
Energy of initial particle: [10, 50, 100] GeV
~~~


## Example: CaloAnalysis_simple (using HistogramClass)

Required arguments:
 - input file name
 - energy
 - sf

~~~{.sh}
cd scripts
python test_macro_simple.py <INPUT_FILE_NAME>.root ENERGY SF
~~~

## Shower profiles: CaloAnalysis_profiles (using HistogramClass_profiles)

Required arguments:
 - input file name
 - energy
 - sf

Analysis is done for one file.

~~~{.sh}
cd scripts
python test_macro_profiles.py <INPUT_FILE_NAME>.root ENERGY SF
~~~

## Cells: CaloAnalysis_cells (using HistogramClass_cells)

Required arguments:
 - input file name
 - energy
 - sf

Analysis is done for one file.

~~~{.sh}
cd scripts
python test_macro_cells.py <INPUT_FILE_NAME>.root ENERGY SF
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
   -r REGEX [REGEX ...], --regex REGEX [REGEX ...]
                        String to insert in place of '?' character in file
                        names ("energy" inserts the values of energies)
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
   -r REGEX [REGEX ...], --regex REGEX [REGEX ...]
                        String to insert in place of '?' character in file
                        names ("energy" inserts the values of energies)
  -o OUTPUT, --output OUTPUT
                        Output file name
  -v, --verbose         Verbose
  --dEta DETA DETA      Size of the tower in eta
  --maxEta MAXETA       Maximum eta
  --dPhi DPHI           Size of the tower in phi
  --numPhi NUMPHI       Number of the towers in phi
~~~