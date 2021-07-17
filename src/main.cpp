#include "Qubit.h"
#include "matrix.h"
#include "operations.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	printf("\n");

	Qubit q1(1);
	Qubit q2(2);

	q1.printState();
	// vector<double> result = tensorMultiplication(q1.getState(), q2.getState());
	
	// for(unsigned int i=0; i<result.size(); i++) {
	// 	cout << result[i] << endl;
	// }
	// cout << endl;
	

	// TODO: Aplicar operacoes de gates no qubit

	// generateRandomMatrix(matA, dim, 1, 3, 2);

	// printMatrix(matA, dim);
	q1.pauli_X();
	q1.printState();

	return 0;
}