FCC_calo_analysis_cpp
=====================

C++ reader of calorimeter hits based on https://github.com/HEP-FCC/analysis-cpp

# Initialization

- fcc-edm and podio has to be compiled
- Have to init FCCSW, fcc-edm and podio + FCC_analysis_cpp

~~~{.sh}
source setup.sh
~~~

#Make

~~~{.sh}
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=../install ..
make -j 8 install
~~~

# Run analysis

Baseline: CaloAnalysis_simple.cc/h, HistogramClass.cc/h
~~~{.sh}
cd ../caloAnalysis
python test_macro_simple.py
~~~

