#include "QubitStateVector.h"
#include "matrixOperations.h"
#include <array>
#include <cmath>

using namespace std;

void QubitStateVector::hadamard(int targetQubit)
{
	complex<double> unitary[] = {
		1 / sqrt(2), 1 / sqrt(2), 1 / sqrt(2), -1 / sqrt(2)};

	this->state = matrixMultiplication(this->state, unitary, 2);
}

void QubitStateVector::pauliZ(int targetQubit)
{
	complex<double> unitary[] = {1, 0, 0, -1};

	this->state = matrixMultiplication(this->state, unitary, 2);
}

void QubitStateVector::pauliY(int targetQubit)
{
	complex<double> unitary[] = {0, 1i, 1i, 0};

	this->state = matrixMultiplication(this->state, unitary, 2);
}

void QubitStateVector::pauliX(int targetQubit)
{
	complex<double> unitary[] = {0, 1, 1, 0};

	// construct the unitary matrix with the identity of the other qubits
	// targetQubit = 3
	complex<double> identity[] = {1, 0, 0, 1};
	complex<double> identity2[] = {1, 0, 0, 1};

	this->state = matrixMultiplication(this->state, unitary, 2);
}

void QubitStateVector::controlledNot()
{
	complex<double> unitary[]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0};

	this->state = matrixMultiplication(this->state, unitary, 4);
}

// void printState()
// {
// 	cout << "State of qubit:" << endl;
// 	for(unsigned int i = 0; i < this->state.size(); i++) {
// 		cout << noshowpos << state[i].real();
// 		if(state[i].imag() != 0)
// 			cout << showpos << state[i].imag() << "i";
// 		cout << endl;
// 	}
// 	cout << endl;
// }

void QubitStateVector::printState()
{
	cout << "State of entangled qubits:" << endl;
	for(unsigned int i = 0; i < this->state.size(); i++) {
		cout << noshowpos << state[i].real();
		if(state[i].imag() != 0)
			cout << showpos << state[i].imag() << "i";
		cout << endl;
	}
	cout << endl;
}

QubitStateVector::QubitStateVector(int numQubits)
{
	array<complex<double>, 2> identity = {1, 0};
	vector<complex<double>> result = {1, 0};

	for(size_t i = 1; i < numQubits; i++) {
		result = tensorMultiplication(result, identity);
	}
	this->state = result;
}
