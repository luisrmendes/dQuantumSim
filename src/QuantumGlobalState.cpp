#include "QuantumGlobalState.h"
#include <vector>

using namespace std;

void QuantumGlobalState::printState() const
{
	std::cout << '|';
	for(size_t i = this->qGS.size(); i > 0; i--) {
		std::cout << this->qGS[i - 1];
	}
	std::cout << ">\n";
}

QuantumGlobalState::QuantumGlobalState(size_t length)
{
	for(std::size_t i = 0; i < length; ++i) {
		qGS.push_back(0);
	}
}