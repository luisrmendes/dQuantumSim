#include "QubitLayer.h"
#include "matrix.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	printf("\n");

	// initialize a qubit state vector, specifying number of qubits
	QubitLayer qL(3);
	
	// Apply pauliX gate to the first qubit
	qL.pauliX(1);
	qL.hadamard(1);

	qL.measure();

	return 0;
}