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

The required argument is the input file name:

~~~{.sh}
python scripts/calo_init.py <INPUT_FILE_NAME>.root
~~~

User may also specify any of the optional arguments.

In particular, if user wants to analyse multiple files, for different energies of the initial particle:

~~~{.sh}
python scripts/calo_init.py <INPUT_FILE_NAME_CONTAINING_*_CHARACTER>.root -r -e 10 20 50 100
~~~

where `-r` indicates that a regex should be used, substituting the `*` character in the input file name with the energy of the particle (specified in argument `-e`).

For instance,
~~~{.sh}
python scripts/calo_init.py 'output_electron_*GeV.root' -r -e 10 100
~~~
results in:
~~~{.sh}
Input file: output_electron_10GeV.root
Input file: output_electron_100GeV.root
Energy of initial particle: [10, 100] GeV
~~~


## Example: CaloAnalysis_simple (using HistogramClass)

~~~{.sh}
cd scripts
python test_macro_simple.py <INPUT_FILE_NAME>.root -e 50 --sf 5.4
~~~


## Shower profiles: CaloAnalysis_profiles (using HistogramClass_profiles)

~~~{.sh}
cd scripts
python test_macro_profiles.py <INPUT_FILE_NAME>.root
~~~

## Cells: CaloAnalysis_cells (using HistogramClass_cells)

~~~{.sh}
cd scripts
python test_macro_cells.py <INPUT_FILE_NAME>.root
~~~


## Reconstruction preview: CaloAnalysis_recoExample (using HistogramClass_recoExample)

Preview of the reconstructed cluster.
The first plot contains all towers, the second the centre of the cluster and the third one all the cells associated to the cluster.
Additionally the windows used in the reconstrucion are drawn.

~~~{.sh}
cd scripts
python scripts/plot_recoExample.py <INPUT_FILE_NAME>.root -e ENERGY
~~~

Input file name and the energy are required.

List of additional options:
~~~{.sh}
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
