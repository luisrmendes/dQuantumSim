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
	q1.hadamard();
	q1.printState();

	return 0;
}