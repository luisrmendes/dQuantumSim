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

	// qL.printStates();

	qL.measure();

	return 0;
}