#include "QubitStateVector.h"
#include "matrix.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	printf("\n");

	// Initialize both qubits as |0>
	QubitStateVector qsv(1);

	qsv.printState();
	qsv.pauliY(0);
	qsv.printState();

	return 0;
}