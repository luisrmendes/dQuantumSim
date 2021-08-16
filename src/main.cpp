#include "QubitLayer.h"
#include "matrix.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	printf("\n");

	// TODO: initialize a qubit state vector, specifying number of qubits
	QubitLayer qL(5);

	qL.printStates();

	// TODO: add Pauli X

	return 0;
}