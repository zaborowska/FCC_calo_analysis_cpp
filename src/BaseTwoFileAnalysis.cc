#include "BaseTwoFileAnalysis.h"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

// STL
#include <vector>
#include <iostream>

BaseTwoFileAnalysis::BaseTwoFileAnalysis() {
  TH1::AddDirectory(kFALSE);
}

BaseTwoFileAnalysis::~BaseTwoFileAnalysis() {
  Delete_histos();
}

void BaseTwoFileAnalysis::loop(const std::string& aFilenameSim, const std::string& aFilenameRec, bool aVerbose) {
  //Reset histograms
  Reset_histos();

  //Open file in the reader - with particles
  auto readerSim = podio::ROOTReader();
  auto storeSim = podio::EventStore();
  try {
    readerSim.openFile(aFilenameSim);
    std::cout << "CaloAnalysis_recoMonitor opening file with simulation output " << aFilenameSim << std::endl;
  }
  catch(std::runtime_error& err) {
    std::cerr<<err.what()<<". Quitting."<<std::endl;
    exit(1);
  }
  storeSim.setReader(&readerSim);
  //Open file in the reader - with reconstructed clusters
  auto readerRec = podio::ROOTReader();
  auto storeRec = podio::EventStore();
  try {
    readerRec.openFile(aFilenameRec);
    std::cout << "CaloAnalysis_recoMonitor opening file with reconstruction output " << aFilenameRec << std::endl;
  }
  catch(std::runtime_error& err) {
    std::cerr<<err.what()<<". Quitting."<<std::endl;
    exit(1);
  }
  storeRec.setReader(&readerRec);

  //Loop over all events
  unsigned nEvents = readerRec.getEntries();
  std::cout << "Number of events: " << nEvents << std::endl;
  for(unsigned i=0; i<nEvents; ++i) {
    if(i%1000==0) std::cout<<"reading event "<<i<<std::endl;
    if(i>11) aVerbose = false;

    processEvent(storeSim, storeRec, i, aVerbose);

    storeSim.clear();
    storeRec.clear();
    readerSim.endOfEvent();
    readerRec.endOfEvent();
  }

  finishLoop(nEvents, aVerbose);

  std::cout << "End of loop" << std::endl;
  return;
}

void BaseTwoFileAnalysis::Reset_histos() {
  for (auto iterator=m_histograms.begin(); iterator<m_histograms.end(); iterator++) {
    (*iterator)->Reset();
    (*iterator)->Sumw2();
  }
}

void BaseTwoFileAnalysis::Delete_histos() {
  for (auto iterator=m_histograms.begin(); iterator<m_histograms.end(); iterator++) {
    delete (*iterator);
  }
  m_histograms.clear();
}
