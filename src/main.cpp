#include "QubitLayer.h"
#include "StateAccess.h"
#include "matrix.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	printf("\n");

	// specify number of qubits, also in QubitLayer.h
	QubitLayer qL(3);

	// Apply pauliX gate to the first qubit
	
	qL.pauliY(2);
	qL.hadamard(2);
	qL.pauliY(0);
	qL.hadamard(1);
	
	qL.hadamard(0);
	qL.hadamard(1);

	qL.measure();

	return 0;
}