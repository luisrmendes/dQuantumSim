#include "Qubit.h"
#include "aux.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	printf("\n");
	cout << "Hello simulator!" << endl;

	Qubit q1(1);

	q1.printState();

	return 0;
}