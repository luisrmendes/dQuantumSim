#include "Qubit.h"

using namespace std;

void Qubit::hadamard()
{
	std::complex<double> mat[] = {
		1 / sqrt(2), 1 / sqrt(2), 1 / sqrt(2), -1 / sqrt(2)};

	this->state = matrixMultiplication(this->state, mat, 2);
}

void Qubit::pauli_Z()
{
	std::complex<double> mat[] = {1, 0, 0, -1};

	this->state = matrixMultiplication(this->state, mat, 2);
}

void Qubit::pauli_Y()
{
	std::complex<double> mat[] = {0, 1i, 1i, 0};

	this->state = matrixMultiplication(this->state, mat, 2);
}

void Qubit::pauli_X()
{
	std::complex<double> mat[] = {0, 1, 1, 0};

	this->state = matrixMultiplication(this->state, mat, 2);
}

void Qubit::printState()
{
	cout << "State of qubit:" << endl;
	for(unsigned int i = 0; i < this->state.size(); i++) {
		cout << noshowpos << state[i].real();
		if(state[i].imag() != 0)
			cout << showpos << state[i].imag() << "i";
		cout << endl;
	}
	cout << endl;
}
