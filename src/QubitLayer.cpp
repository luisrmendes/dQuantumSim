#include "QubitLayer.h"

using namespace std;

void QubitLayer::hadamard(int targetQubit)
{
	// calculate jump
	size_t jump = pow(2, qubitCount - targetQubit) * 2;

	for(size_t i = 0; i < this->states.size(); i += jump) {
		this->states[i + 1] = this->states[i].real() * (1 / sqrt(2));
	}

	updateStates();
}

// TODO
void QubitLayer::pauliZ(int targetQubit) { return; }

// TODO
void QubitLayer::pauliY(int targetQubit) { return; }

void QubitLayer::pauliX(int targetQubit)
{
	// calculate jump
	size_t jump = pow(2, qubitCount - targetQubit) * 2;

	for(size_t i = 0; i < this->states.size(); i += jump) {
		this->states[i].real() == 0 ? this->states[i + 1].real(1)
									: this->states[i + 1].real(0);
	}

	updateStates();
}

void QubitLayer::measure()
{
	for(size_t i = 0; i < this->states.size(); i += 2) {
		float result = pow(abs(this->states[i]), 2); // not sure...

		// TODO: proper string formatter
		cout << "|" << bitset<3>(i / 2) << "> -> " << result << endl;
	}
}

void QubitLayer::updateStates()
{
	for(size_t i = 0; i < this->states.size(); i += 2) {
		this->states[i] = this->states[i + 1];
		this->states[i + 1] = {0, 0};
	}
}

void QubitLayer::printStateVector()
{
	for(auto it = this->states.begin(); it != this->states.end(); ++it) {
		cout << *it << endl;
	}

	cout << "State vector size: " << this->states.size() << endl;
}

QubitLayer::QubitLayer(int qubitCount)
{
	this->qubitCount = qubitCount;
	// calculate total size
	size_t v_size = 2 * pow(2, qubitCount);

	// populate vector with all (0,0)
	size_t i = 0;
	while(i < v_size) {
		this->states.push_back(0);
		++i;
	}
}