#include "BaseAnalysis.h"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

// STL
#include <vector>
#include <iostream>

BaseAnalysis::BaseAnalysis() {}

BaseAnalysis::~BaseAnalysis() {
  Delete_histos();
}

void BaseAnalysis::loop(const std::string& aFilename, bool aVerbose) {
  //Reset histograms
  Reset_histos();

  //Open file in the reader
  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  try {
    reader.openFile(aFilename);
    std::cout << "BaseAnalysis opening file " << aFilename << std::endl;
  }
  catch(std::runtime_error& err) {
    std::cerr<<err.what()<<". Quitting."<<std::endl;
    exit(1);
  }
  store.setReader(&reader);

  //Loop over all events
  unsigned nEvents = reader.getEntries();
  std::cout << "Number of events: " << nEvents << std::endl;
  for(unsigned i=0; i<nEvents; ++i) {
    if(i%1000==0) std::cout<<"reading event "<<i<<std::endl;
    if(i>11) aVerbose = false;

    processEvent(store, aVerbose);

    store.clear();
    reader.endOfEvent();
  }

  std::cout << "End of loop" << std::endl;

  return;
}

void BaseAnalysis::Reset_histos() {
  std::cout<<"RESET"<<std::endl;
  for (auto iterator=hVector.begin(); iterator<hVector.end(); iterator++) {
    (*iterator)->Reset();
    (*iterator)->Sumw2();
  }
}

void BaseAnalysis::Delete_histos() {
  std::cout<<"DELETE"<<std::endl;
  for (auto iterator=hVector.begin(); iterator<hVector.end(); iterator++) {
    delete (*iterator);
  }
  hVector.clear();
}
