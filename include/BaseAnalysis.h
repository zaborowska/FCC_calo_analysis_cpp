#ifndef BASEANALYSIS_H
#define BASEANALYSIS_H

#include "TObject.h"
#include "TH1.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class BaseAnalysis {
public:
  BaseAnalysis();
  virtual ~BaseAnalysis();

  void loop(const std::string& aFilename, bool aVerbose = false);  //Open the file in the reader and loop through the events

  virtual void processEvent(podio::EventStore& aStore, bool aVerbose = false) = 0;
protected:
  std::vector<TH1*> hVector;
private:
  virtual void Delete_histos();
  virtual void Reset_histos();
};

#endif /* BASEANALYSIS_H */
