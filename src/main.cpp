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
	qL.pauliX(0);
	qL.pauliX(1);

	qL.controlledZ(0, 1);

	qL.measure();

	return 0;
}