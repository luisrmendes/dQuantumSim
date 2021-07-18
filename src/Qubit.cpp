#include "Qubit.h"
#include <iostream>

using namespace std;

void Qubit::hadamard()
{
	std::complex<double> mat[] = {
		1 / sqrt(2), 1 / sqrt(2), 1 / sqrt(2), -1 / sqrt(2)};

	this->state = gateOperation(this->state, mat);
}

void Qubit::pauli_Z()
{
	using namespace std::complex_literals;

	std::complex<double> mat[] = {1, 0, 0, -1};

	this->state = gateOperation(this->state, mat);
}

void Qubit::pauli_Y()
{
	using namespace std::complex_literals;

	std::complex<double> mat[] = {0, 1i, 1i, 0};

	this->state = gateOperation(this->state, mat);
}

void Qubit::pauli_X()
{
	using namespace std::complex_literals;

	std::complex<double> mat[] = {0, 1, 1, 0};

	this->state = gateOperation(this->state, mat);
}

void Qubit::printState()
{
	cout << "State of qubit " << this->id << ":" << endl;
	for(unsigned int i = 0; i < this->state.size(); i++) {
		cout << noshowpos << state[i].real();
		if(state[i].imag() != 0)
			cout << showpos << state[i].imag() << "i";
		cout << endl;
	}
}
