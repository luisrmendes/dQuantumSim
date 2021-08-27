#include "QubitLayer.h"
#include "StateAccess.h"
#include "matrix.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	printf("\n");

	// Specify number of qubits, also in QubitLayer.h

	QubitLayer qL(3);

	// init
	qL.hadamard(0);
	qL.hadamard(1);
	qL.hadamard(2);

	// oracle
	qL.pauliX(0);
	qL.pauliX(1);
	qL.toffoli(0, 1, 2);
	qL.pauliX(0);
	qL.pauliX(1);

	// grover diffusion
	qL.hadamard(0);
	qL.hadamard(1);
	qL.hadamard(2);
	qL.pauliX(0);
	qL.pauliX(1);
	qL.controlledZ(0, 1);
	qL.pauliX(0);
	qL.pauliX(1);
	qL.hadamard(0);
	qL.hadamard(1);

	qL.measure();

	/**
	 * PHASE ORACLE
	 */
	// QubitLayer qL(2);

	// // init
	// qL.hadamard(0);
	// qL.hadamard(1);

	// // oracle
	// qL.pauliX(0);
	// qL.pauliX(1);
	// qL.controlledZ(0, 1);
	// qL.pauliX(0);
	// qL.pauliX(1);

	// // grover diffusion
	// qL.hadamard(0);
	// qL.hadamard(1);
	// qL.pauliX(0);
	// qL.pauliX(1);
	// qL.controlledZ(0, 1);
	// qL.pauliX(0);
	// qL.pauliX(1);
	// qL.hadamard(0);
	// qL.hadamard(1);

	// qL.measure();

	return 0;
}