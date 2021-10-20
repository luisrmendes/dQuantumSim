#ifndef QUANTUMGLOBALSTATE_H
#define QUANTUMGLOBALSTATE_H

#include <vector>

class QuantumGlobalState
{
  private:
	  std::vector<unsigned int> quantumGlobalState;

  public:
    QuantumGlobalState(unsigned int length);
};

#endif