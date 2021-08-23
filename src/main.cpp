#include "QubitLayer.h"
#include "StateAccess.h"
#include "matrix.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	printf("\n");

	// initialize a qubit state vector, specifying number of qubits

	QubitLayer qL(3);

	// Apply pauliX gate to the first qubit
	qL.pauliX(2);
	qL.pauliX(1);
	qL.pauliZ(0);
	qL.pauliY(1);
	qL.pauliY(0);
	qL.pauliY(2);
	qL.pauliY(0);
	

	// qL.hadamard(1);
	// qL.hadamard(1);

	// qL.pauliX(1);
	// qL.hadamard(2);
	// qL.pauliZ(1);

	// qL.printStateVector();
	qL.measure();

	return 0;
}