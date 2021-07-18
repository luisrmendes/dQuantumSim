#include "Qubit.h"
#include "matrix.h"
#include "operations.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	printf("\n");

	// Initialize both qubits as |0>
	Qubit q1(1, {1, 0});
	Qubit q2(2, {1, 0});

	// q1.printState();
	// q1.hadamard();
	MultiQubit entangledQubit = Qubit::tensorMultiplication(q1, q2);

	entangledQubit.printState();

	return 0;
}