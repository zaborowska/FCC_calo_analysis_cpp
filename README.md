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

## Example: CaloAnalysis_simple (using HistogramClass)

~~~{.sh}
cd scripts
python test_macro_simple.py
~~~

## Shower profiles: CaloAnalysis_profiles (using HistogramClass_profiles)

~~~{.sh}
cd scripts
python test_macro_profiles.py
~~~

## Cells: CaloAnalysis_cells (using HistogramClass_cells)

~~~{.sh}
cd scripts
python test_macro_cells.py
~~~

