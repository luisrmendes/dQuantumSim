#include "Qubit.h"
#include <iostream>

using namespace std;

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

Qubit::Qubit(int id) { this->id = id; }