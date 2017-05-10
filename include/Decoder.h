#ifndef DECODER_H
#define DECODER_H

#include <string>
#include <map>

struct Field {
  std::string name;
  long long mask;
  uint offset;
  uint width;
  int minVal;
  int maxVal;
  bool isSigned;
};

class Decoder {
public:
  Decoder(const std::string& aDescr);
  unsigned long long operator[](const std::string& aName) const;
  unsigned long long value(const std::string& aName) const;
  unsigned long long value(const std::string& aName, long long aVal) const;
  double segmentationPosition(long long aVal, double aSize, double aOffset) const;
  void setValue(long long aVal);
private:
  std::string m_description;
  std::map<std::string, Field> m_fields;
  unsigned long long m_currentValue;
};
#endif /* DECODER_H */
