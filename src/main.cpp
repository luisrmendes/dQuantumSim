#include "Qubit.h"
#include "utils.h"
#include "operations.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	printf("\n");
	cout << "Hello simulator!" << endl;

	Qubit q1(1);
	Qubit q2(2);

	q1.printState();
	

	// TODO: Aplicar operacoes de gates no qubit

	// generateRandomMatrix(matA, dim, 1, 3, 2);

	// printMatrix(matA, dim);
	// tensorMultiplication

	return 0;
}