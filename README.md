FCC_calo_analysis_cpp
=====================

C++ reader of calorimeter hits based on https://github.com/HEP-FCC/analysis-cpp.

Initialization
--------------
- fcc-edm and podio has to be compiled
- Have to init FCCSW, fcc-edm and podio + FCC_analysis_cpp

  source setup.sh

Notes:
- problem in initialization of fcc-edm and podio  (seems to be running fine)

  dirname: missing operand
 

Make
-----
	mkdir build
	cd build
	cmake -DCMAKE_INSTALL_PREFIX=../install ..
	make -j 4 install

Run
-----
- Default example (from analysis-cpp):
   
   reads example.root (produced with ${FCCEDM}/bin/fccedm-write)
   
   ./install/bin/fccanalysiscpp-read    
    python: python -i example-lib/test_macro.py	

- My calo hit reader (example/read-calo.cc) 

   needs output-calo.root with ECal + HCal hits

   creates output-calo-hits.root with histograms total hit energy in ECAL and in HCAL
   
   ./install/bin/read-calo


