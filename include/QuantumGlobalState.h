#ifndef QUANTUMGLOBALSTATE_H
#define QUANTUMGLOBALSTATE_H

#include <iostream>
#include <vector>

class QuantumGlobalState
{
  private:
	std::vector<bool> qGS;

  public:
	QuantumGlobalState(size_t length);
	void flip(int index) { qGS[index] = !qGS[index]; }
	void printState() const;

	// overload + operator (size_t + QuantumGlobalState)

	// convert to a real unsigned long long
};

// QuantumGlobalState qGS(10);
// qGS.printState();
// qGS.flip(9);
// qGS.printState();

#endif