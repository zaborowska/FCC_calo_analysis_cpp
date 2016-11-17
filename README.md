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
python scripts/calo_init.py <INPUT_FILE_NAME_CONTAINING_*_CHARACTER>.root -r -e 100
~~~

where `-r` indicates that a regex should be used, substituting the `*` character in the input file name with the energy of the particle (specified in argument `-e`).



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

