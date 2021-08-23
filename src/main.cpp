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
	qL.pauliX(0);

	// qL.hadamard(1);
	// qL.hadamard(1);

	// qL.pauliX(1);
	// qL.hadamard(2);
	// qL.pauliZ(1);

	// qL.printStateVector();
	qL.measure();

	bitset<numQubits> state = 2;
	cout << state << endl;
	state.flip(1);
	cout << state << endl;

	return 0;
}