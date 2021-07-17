#include "Qubit.h"
#include <iostream>

using namespace std;

void Qubit::pauli_Z()
{
    double mat[] = {
		1, 0,
		0, -1
	};

    this->state = gateOperation(this->state, mat);
}

void Qubit::pauli_Y()
{
	using namespace std::complex_literals;

    std::variant<double, std::complex<double>> mat[] = {
		0, 1i,
		1i, 0
	};

    this->state = gateOperation(this->state, mat);
}

void Qubit::pauli_X()
{
    double mat[] = {
		0, 1,
		1, 0
	};

    this->state = gateOperation(this->state, mat);
}

void Qubit::printState()
{
	cout << "State of qubit " << this->id << ":" << endl;
	for(unsigned int i=0; i<this->state.size(); i++) {
		cout << state[i] << endl;
	}
	cout << endl;
}
